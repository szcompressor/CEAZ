#include "dual_quant.h"
#include "sz_hls.h"
#include "histogram.h"
#include "huffman_codebook.h"
#include "huffman.h"

void MemToStream(ap_uint<kMemWidth>* in_data, hls::stream<ap_uint<kMemWidth> >& mem_row) {

    ap_uint<kMemWidth> row_reg = 0;

    for (uint32_t i0 = 0; i0 < kInSize; i0++) {
        #pragma HLS PIPELINE II = 1 rewind
        row_reg = in_data[i0];
        // row_reg = 0;
        mem_row << row_reg;
    }
}

void StreamToMem(hls::stream<Codeword_struct> huff_encoder_stream[kNumHists], ap_uint<kOutWidth>* out_data, PackedCodewordAndLength encoding[INPUT_SYMBOL_SIZE]) {

    uint32_t m_idx = 0;
    ap_uint<kMemWidth> mem_row[kNumHists] = {0, 0};
    Codeword_struct huff_code[kNumHists];
    ap_uint<kMaxBits> codeword[kNumHists];
    ap_uint<8> code_len[kNumHists];
    uint16_t e_p[kNumHists + 1] = {0, 0};
    ap_uint<1> full[kNumHists] = {0, 0};
    uint16_t check_full = 0;

    #pragma HLS ARRAY_PARTITION variable = mem_row dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = huff_code dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = codeword dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = code_len dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = e_p dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = full dim = 1 complete

    for (uint8_t i0 = 0; i0 < kNumHists; i0++) {
    #pragma HLS UNROLL    
        mem_row[i0] = 0;
    }

    for(uint16_t i1 = 0; i1 < kInSize; i1++) {
    #pragma HLS PIPELINE II=1 rewind

        for (uint8_t i0 = 0; i0 < kNumHists; i0++) {
        #pragma HLS UNROLL
            huff_code[i0] = huff_encoder_stream[i0].read();
            codeword[i0] = huff_code[i0].codeword;
            code_len[i0] = huff_code[i0].code_length;

            e_p[i0] += code_len[i0];
            if (e_p[i0] > kMemWidth) {
                full[i0] = 1;
                // out_data[m_idx] = mem_row[i0];
                mem_row[i0] = 0;
                e_p[i0] = code_len[i0];
                // m_idx += 1;
            }

            mem_row[i0] = (mem_row[i0] << code_len[i0]) | codeword[i0];

            if (i1 * kNumHists + i0 < INPUT_SYMBOL_SIZE) {
                PackedCodewordAndLength tmp = encoding[i1 * kNumHists + i0]
            }
        }

        out_data[i1] = mem_row[0];
    }
}

void sz_hls(ap_uint<kMemWidth> in_data[kInSize], ap_uint<kOutWidth>* out_data) {

// #pragma HLS INTERFACE m_axi port=in_data depth=kInSize
// #pragma HLS INTERFACE m_axi port=out_data depth=kOutSize

// #pragma HLS INTERFACE m_axi port=in_data depth=kInSize offset = slave bundle = gmem0
// #pragma HLS INTERFACE m_axi port=out_data depth=kOutSize offset = slave bundle = gmem0
// #pragma HLS INTERFACE s_axilite port=in_data bundle = control
// #pragma HLS INTERFACE s_axilite port=out_data bundle = control

#pragma HLS dataflow
    hls::stream<ap_uint<kMemWidth> > mem_row;
    hls::stream<CodeT> quant_code_stream0[kNumHists];
    hls::stream<CodeT> quant_code_stream1[kNumHists];
    hls::stream<ap_uint<kQuaVecWidth> > qua_code_vector_stream;
    hls::stream<Codeword_struct> huff_encoder_stream[kNumHists];

    Symbol freq[INPUT_SYMBOL_SIZE];
    PackedCodewordAndLength encoding[INPUT_SYMBOL_SIZE];

    #pragma HLS STREAM variable = mem_row depth = 32
    #pragma HLS STREAM variable = quant_code_stream0 depth = 32
    #pragma HLS STREAM variable = quant_code_stream1 depth = 32
    #pragma HLS STREAM variable = qua_code_vector_stream depth = 32
    #pragma HLS STREAM variable = huff_encoder_stream depth = 32

    #pragma HLS BIND_STORAGE variable=freq type=RAM_2P impl=BRAM latency=1 
    #pragma HLS BIND_STORAGE variable=encoding type=RAM_2P impl=BRAM latency=1 

    const uint8_t kCallIdx = 0;
    uint32_t eng_blks[kNumEngs] = {kDim1};

    int num_nonzero_symbols = 0;

    MemToStream(in_data, mem_row);

	dual::lorenzo_2d_1l_stream<ap_uint<32>, int16_t>(mem_row, quant_code_stream0, qua_code_vector_stream, kDim1, kCallIdx);

    QuantCodeFrequency(quant_code_stream0, quant_code_stream1, freq);

    huffman_encoding(freq, encoding, &num_nonzero_symbols);

    ParallelEncoder(quant_code_stream1, qua_code_vector_stream, huff_encoder_stream);

    StreamToMem(huff_encoder_stream, out_data, encoding);

    // std::cout << "finish" << std::endl;
}
