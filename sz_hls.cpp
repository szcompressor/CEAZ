#include "dual_quant.h"
#include "sz_hls.h"

template <typename T>
void StreamToMem(hls::stream<T>& in_stream, ap_uint<kOutWidth>* out_data, uint32_t out_size) {
    for (uint32_t i = 0; i < out_size; i++) {
    #pragma HLS PIPELINE II = 1

        if (!in_stream.empty()) {   
            Codeword stream_reg = in_stream.read();
            out_data[i] = stream_reg.codeword;
        }

        // if (!in_stream.empty()) {   
        //     T stream_reg = in_stream.read();
        //     out_data[i] = stream_reg;
        // }

    }
}

void MemToStream(ap_uint<kMemWidth>* in_data, hls::stream<ap_uint<kMemWidth> >& mem_row) {
    ap_uint<kMemWidth> mem_buf[kBurst];

    for (uint32_t i1 = 0; i1 < kInSize / kBurst; i1++) {
        for (uint8_t i0 = 0; i0 < kBurst; i0++) {
        #pragma HLS PIPELINE II = 1
            mem_buf[i0] = in_data[i1 * kBurst + i0];
        }

        for (uint8_t i0 = 0; i0 < kBurst; i0++) {
        #pragma HLS PIPELINE II = 1
            mem_row << mem_buf[i0];
        }
    }
}

// extern "C" {
void sz_hls(ap_uint<kMemWidth>* in_data, ap_uint<kOutWidth>* out_data) {

#pragma HLS INTERFACE m_axi port=in_data depth=kInSize
#pragma HLS INTERFACE m_axi port=out_data depth=kOutSize

#pragma HLS dataflow
    hls::stream<ap_uint<kMemWidth> > mem_row;
    hls::stream<CodeT> quant_code_stream[kNumHists];
    hls::stream<uint32_t> freq_stream;
    hls::stream<Codeword> huff_code_stream;
    #pragma HLS STREAM variable = mem_row depth = kBurst
    #pragma HLS STREAM variable = quant_code_stream depth = 32
    #pragma HLS STREAM variable = freq_stream depth = 32
    #pragma HLS STREAM variable = huff_code_stream depth = 32

    const uint16_t kDim1 = dims_l16[1];
    uint32_t eng_blks[kNumEngs] = {kDim1};
    const uint8_t kCallIdx = 0;

    MemToStream(in_data, mem_row);

	dual::lorenzo_2d_1l_stream<ap_uint<32>, int16_t>(mem_row, quant_code_stream, kDim1, kCallIdx);

    huf::QuantCodeFrequency1(quant_code_stream, freq_stream);

    // huf::QuantCodeFrequency(quant_code_stream, kInSize, eng_blks, freq_stream);

    huf::HuffConstructTreeStream(freq_stream, huff_code_stream);

    StreamToMem<Codeword>(huff_code_stream, out_data, kOutSize);
}

// }
