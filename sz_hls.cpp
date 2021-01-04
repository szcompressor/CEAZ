#include "dual_quant.h"
#include "sz_hls.h"

void MemToStream(ap_uint<kMemWidth>* in_data, hls::stream<ap_uint<kMemWidth> >& mem_row) {
    ap_uint<kMemWidth> mem_buf[kBurst];

    for (uint32_t i1 = 0; i1 < kInSize / kBurst; i1++) {
        for (uint8_t i0 = 0; i0 < kBurst; i0++) {
        #pragma HLS PIPELINE II = 1 rewind
            mem_buf[i0] = in_data[i1 * kBurst + i0];
        }

        for (uint8_t i0 = 0; i0 < kBurst; i0++) {
        #pragma HLS PIPELINE II = 1 rewind
            mem_row << mem_buf[i0];
        }
    }
}

void StreamToMem(hls::stream<uint32_t> huff_encoder_stream[kNumHists], ap_uint<kOutWidth>* out_data) {

    uint32_t huff_code = 0;
    for(uint16_t i1 = 0; i1 < kRows; i1++) {
    #pragma HLS PIPELINE II=1 rewind
    // #pragma HLS UNROLL 

        for (uint8_t i0 = 0; i0 < kNumHists; i0++) {
        #pragma HLS UNROLL
        // #pragma HLS PIPELINE II=1 rewind
            huff_code = huff_encoder_stream[i0].read();

            if (i0 == 1) {
                out_data[i1] = huff_code;
            }
        }
    }
}

// void WriteQuantCode(hls::stream<ap_uint<kMemWidth> >& qua_code_vector_stream, ap_uint<kMemWidth>* qua_code_vector_out) {

//     const uint16_t kVectorSize = kBlkSize / kNumHists;

//     ap_uint<kMemWidth> qua_code_vector_reg; 

//     for (uint16_t i0 = 0; i0 < kRows; i0++) {
//         qua_code_vector_reg = qua_code_vector_stream.read();
//         qua_code_vector_out[i0] = qua_code_vector_reg;
//     }
// }

void WriteQuantCode(hls::stream<ap_uint<kMemWidth> >& qua_code_vector_stream, ap_uint<256> quant_code_buf[kQuantBufSize]) {

    const uint16_t kVectorSize = kBlkSize / kNumHists;

    ap_uint<kMemWidth> qua_code_vector_reg; 

    for (uint16_t i0 = 0; i0 < kRows; i0++) {
        qua_code_vector_reg = qua_code_vector_stream.read();
        quant_code_buf[i0] = qua_code_vector_reg;
    }
}

// void ReadQuantCode(ap_uint<kMemWidth>* qua_code_vector_in, hls::stream<CodeT> quant_code_stream[kNumHists]) {

// //    std::ofstream o_file0;
// //    o_file0.open("C:\\Users\\Bizon\\Desktop\\sz_hls2\\inter_data\\read_code.txt");

//     for (uint16_t i1 = 0; i1 < kRows; i1++) {
//     #pragma HLS PIPELINE II = 1 rewind

//         ap_uint<kMemWidth> row_buf = qua_code_vector_in[i1];

//         for (uint8_t i0 = 0; i0 < kNumDataPerRow; i0++) {
//         #pragma HLS UNROLL
//             quant_code_stream[i0] << row_buf.range(kDualCodeWidth * (i0 + 1) - 1, kDualCodeWidth * i0);

// //          o_file0 << row_buf.range(kDualCodeWidth * 1 - 1, 0);
//         }
//     }
// }

void ReadQuantCode(ap_uint<256> quant_code_buf[kQuantBufSize], hls::stream<CodeT> quant_code_stream[kNumHists]) {

//    std::ofstream o_file0;
//    o_file0.open("C:\\Users\\Bizon\\Desktop\\sz_hls2\\inter_data\\read_code.txt");

    for (uint16_t i1 = 0; i1 < kRows; i1++) {
    #pragma HLS PIPELINE II = 1 rewind

        ap_uint<kMemWidth> row_buf = quant_code_buf[i1];

        for (uint8_t i0 = 0; i0 < kNumDataPerRow; i0++) {
        #pragma HLS UNROLL
            quant_code_stream[i0] << row_buf.range(kDualCodeWidth * (i0 + 1) - 1, kDualCodeWidth * i0);

//          o_file0 << row_buf.range(kDualCodeWidth * 1 - 1, 0);
        }
    }
}

void scheduler(hls::stream<ap_uint<kMemWidth> >& qua_code_vector_stream, ap_uint<256> quant_code_buf[kQuantBufSize], hls::stream<CodeT> quant_code_stream1[kNumHists]) {
    for (uint16_t i1 = 0; i1 < 4096; i1++) {

        if (i1 < 1024) {
            WriteQuantCode(qua_code_vector_stream, quant_code_buf);
            huf::QuantCodeFrequency(quant_code_stream0, freq_stream);
            huf::HuffConstructTreeStream(freq_stream, hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15);
            ReadQuantCode(quant_code_buf, quant_code_stream1);
        } else {
            for (uint8_t i0 = 0; i0 < 16; i0++) {
                CodeT quant_code = quant_code_stream0[i0].read();
                quant_code_stream1[i0] << quant_code;
            }
        }
    }
}

void sz_hls(ap_uint<kMemWidth>* in_data, ap_uint<kOutWidth>* out_data, ap_uint<kMemWidth>* qua_code_vector_out, ap_uint<kMemWidth>* qua_code_vector_in) {

#pragma HLS INTERFACE m_axi port=in_data depth=kInSize
#pragma HLS INTERFACE m_axi port=out_data depth=kOutSize

#pragma HLS INTERFACE m_axi port=qua_code_vector_out depth = kInSize 
#pragma HLS INTERFACE m_axi port=qua_code_vector_in depth = kInSize 

// #pragma HLS INTERFACE m_axi port=in_data depth=kInSize offset = slave bundle = gmem0
// #pragma HLS INTERFACE m_axi port=out_data depth=kOutSize offset = slave bundle = gmem0
// #pragma HLS INTERFACE m_axi port=qua_code_vector_out depth = kInSize offset = slave bundle = gmem0 
// #pragma HLS INTERFACE m_axi port=qua_code_vector_in depth = kInSize offset = slave bundle = gmem0
// #pragma HLS INTERFACE s_axilite port=in_data bundle = control
// #pragma HLS INTERFACE s_axilite port=out_data bundle = control
// #pragma HLS INTERFACE s_axilite port=qua_code_vector_out bundle = control
// #pragma HLS INTERFACE s_axilite port=qua_code_vector_in bundle = control

#pragma HLS dataflow
    hls::stream<ap_uint<kMemWidth> > mem_row;
    hls::stream<CodeT> quant_code_stream0[kNumHists];
    hls::stream<CodeT> quant_code_stream1[kNumHists];
    hls::stream<ap_uint<kMemWidth> > qua_code_vector_stream;
    hls::stream<uint32_t> freq_stream;
    hls::stream<uint32_t> huff_encoder_stream[kNumHists];

    uint32_t hist0[1024];
    uint32_t hist1[1024];
    uint32_t hist2[1024];
    uint32_t hist3[1024];
    uint32_t hist4[1024];
    uint32_t hist5[1024];
    uint32_t hist6[1024];
    uint32_t hist7[1024];
    uint32_t hist8[1024];
    uint32_t hist9[1024];
    uint32_t hist10[1024];
    uint32_t hist11[1024];
    uint32_t hist12[1024];
    uint32_t hist13[1024];
    uint32_t hist14[1024];
    uint32_t hist15[1024];
    ap_uint<256> quant_code_buf[kQuantBufSize];

    #pragma HLS STREAM variable = mem_row depth = kBurst
    #pragma HLS STREAM variable = quant_code_stream0 depth = 32
    #pragma HLS STREAM variable = quant_code_stream1 depth = 32
    #pragma HLS STREAM variable = qua_code_vector_stream depth = 32
    #pragma HLS STREAM variable = freq_stream depth = 32
    #pragma HLS STREAM variable = huff_encoder_stream depth = 32

    #pragma HLS RESOURCE variable=hist0 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist1 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist2 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist3 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist4 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist5 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist6 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist7 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist8 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist9 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist10 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist11 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist12 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist13 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist14 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist15 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=quant_code_buf core=XPM_MEMORY uram

    const uint8_t kCallIdx = 0;
    const uint16_t kDim1 = dims_l16[1];
    uint32_t eng_blks[kNumEngs] = {kDim1};

    MemToStream(in_data, mem_row);

	dual::lorenzo_2d_1l_stream<ap_uint<32>, int16_t>(mem_row, quant_code_stream0, qua_code_vector_stream, kDim1, kCallIdx);

    // ParallelEncoder(quant_code_stream1, hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15, huff_encoder_stream);
    ParallelEncoder(quant_code_stream0, hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15, huff_encoder_stream);

    StreamToMem(huff_encoder_stream, out_data);

    // std::cout << "finish" << std::endl;
}
