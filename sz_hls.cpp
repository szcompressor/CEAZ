#include "dual_quant.h"
#include "sz_hls.h"

void MemToStream(ap_uint<kMemWidth>* in_data, hls::stream<ap_uint<kMemWidth> >& mem_row) {
    // ap_uint<kMemWidth> mem_buf[kBurst];

    // for (uint32_t i1 = 0; i1 < kInSize / kBurst; i1++) {
    //     for (uint8_t i0 = 0; i0 < kBurst; i0++) {
    //     #pragma HLS PIPELINE II = 1 rewind
    //         mem_buf[i0] = in_data[i1 * kBurst + i0];
    //     }

    //     for (uint8_t i0 = 0; i0 < kBurst; i0++) {
    //     #pragma HLS PIPELINE II = 1 rewind
    //         mem_row << mem_buf[i0];
    //     }
    // }
    ap_uint<kMemWidth> row_reg = 0;

    for (uint32_t i0 = 0; i0 < kInSize; i0++) {
        #pragma HLS PIPELINE II = 1 rewind
        row_reg = in_data[i0];
        mem_row << row_reg;
    }
}

void StreamToMem(hls::stream<Codeword> huff_encoder_stream[kNumHists], ap_uint<kOutWidth>* out_data) {

    uint16_t m_idx = 0;
    ap_uint<kMemWidth> mem_row[kNumHists];
    ap_uint<1> full[kNumHists];
    uint16_t check_full = 0;
    Codeword huff_code[kNumHists];
    ap_uint<kMaxBits> codeword[kNumHists];
    ap_uint<8> code_len[kNumHists];
    uint16_t s_p[kNumHists + 1];
    uint16_t e_p[kNumHists + 1];

    #pragma HLS ARRAY_PARTITION variable = mem_row dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = full dim = 1 complete

    #pragma HLS ARRAY_PARTITION variable = huff_code dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = codeword dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = code_len dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = s_p dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = e_p dim = 1 complete

    for (uint8_t i0 = 0; i0 < kNumHists; i0++) {
    #pragma HLS UNROLL    
        mem_row[i0] = 0;
    }

    s_p[0] = 0;

    for(uint16_t i1 = 0; i1 < kRows; i1++) {
    #pragma HLS PIPELINE II=1 rewind
    // #pragma HLS UNROLL 

        for (uint8_t i0 = 0; i0 < kNumHists; i0++) {
        #pragma HLS UNROLL
        // #pragma HLS PIPELINE II=1 rewind
            huff_code[i0] = huff_encoder_stream[i0].read();
            codeword[i0] = huff_code[i0].codeword;
            code_len[i0] = huff_code[i0].code_length;

            e_p[i0] = s_p[i0] + code_len[i0] -1;
            s_p[i0 + 1] = s_p[i0] + code_len[i0];

            // mem_row.range(e_p[0], s_p[0]) = codeword[0];

            if (e_p[i0] > kMemWidth - 1) {
                full[i0] = 1;
                mem_row[i0] = 0;
            } else if (code_len[i0] > 0) {
                mem_row[i0].range(e_p[i0], s_p[i0]) = codeword[i0];
            }
        }

        check_full = full[0] || full[1] || full[2] || full[3] || full[4] || full[5] || full[6] || full[7] || full[8] || full[9] || full[10] || full[11] || full[12] || full[13] || full[14] || full[15]; 
        if (check_full) {
            for (uint8_t i0 = 0; i0 < kNumHists; i0++) {
            #pragma HLS PIPELINE II=1 rewind
            #pragma HLS UNROLL
//                out_data[m_idx] = mem_row[i0];
//                m_idx += 1;

                out_data[i0] = mem_row[i0];
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

// void ReadQuantCode(ap_uint<kMemWidth>* qua_code_vector_in, hls::stream<CodeT> quant_code_stream[kNumHists]) {

// //    std::ofstream o_file0;
// //    o_file0.open("C:\\Users\\Bizon\\Desktop\\sz_hls4_0\\inter_data\\read_code.txt");

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

void WriteQuantCode(hls::stream<ap_uint<kQuaVecWidth> >& qua_code_vector_stream, ap_uint<kQuaVecWidth> quant_code_buf[kQuantBufSize]) {

    const uint16_t kVectorSize = kBlkSize / kNumHists;

    ap_uint<kQuaVecWidth> qua_code_vector_reg; 

    for (uint16_t i0 = 0; i0 < kHuffRows; i0++) {
        qua_code_vector_reg = qua_code_vector_stream.read();
        quant_code_buf[i0] = qua_code_vector_reg;
    }
}

void ReadQuantCode(ap_uint<kQuaVecWidth> quant_code_buf[kQuantBufSize], hls::stream<CodeT> quant_code_stream[kNumHists]) {

//    std::ofstream o_file0;
//    o_file0.open("C:\\Users\\Bizon\\Desktop\\sz_hls4_0\\inter_data\\read_code.txt");

    for (uint16_t i1 = 0; i1 < kHuffRows; i1++) {
    #pragma HLS PIPELINE II = 1 rewind

        ap_uint<kQuaVecWidth> row_buf = quant_code_buf[i1];

        for (uint8_t i0 = 0; i0 < kNumDataPerRow; i0++) {
        #pragma HLS UNROLL
            quant_code_stream[i0] << row_buf.range(kDualCodeWidth * (i0 + 1) - 1, kDualCodeWidth * i0);

//          o_file0 << row_buf.range(kDualCodeWidth * 1 - 1, 0);
        }
    }
}

void scheduler(hls::stream<ap_uint<kQuaVecWidth> >& qua_code_vector_stream, ap_uint<kQuaVecWidth> quant_code_buf[kQuantBufSize], hls::stream<CodeT> quant_code_stream0[kNumHists], hls::stream<CodeT> quant_code_stream1[kNumHists], 
    Codeword hist0[1024], Codeword hist1[1024], Codeword hist2[1024], Codeword hist3[1024], Codeword hist4[1024], Codeword hist5[1024], Codeword hist6[1024], Codeword hist7[1024], Codeword hist8[1024], Codeword hist9[1024], 
    Codeword hist10[1024], Codeword hist11[1024], Codeword hist12[1024], Codeword hist13[1024], Codeword hist14[1024], Codeword hist15[1024]) {

    hls::stream<uint32_t> freq_stream;
    CodeT quant_code[kNumHists];

    #pragma HLS STREAM variable = freq_stream depth = 32
    #pragma HLS ARRAY_PARTITION variable = quant_code dim = 1 complete

    for (uint16_t i1 = 0; i1 < kBatches; i1++) {

        if (i1 < 1) {
            WriteQuantCode(qua_code_vector_stream, quant_code_buf);
            huf::QuantCodeFrequency(quant_code_stream0, freq_stream);
            huf::HuffConstructTreeStream(freq_stream, hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15);
            ReadQuantCode(quant_code_buf, quant_code_stream1);
        } else {
            for (uint16_t i2 = 0; i2 < kHuffRows; i2++) {
                for (uint8_t i0 = 0; i0 < kNumHists; i0++) {
                    #pragma HLS UNROLL
                    quant_code[i0] = quant_code_stream0[i0].read();
                    quant_code_stream1[i0] << quant_code[i0];
                } 
            }
        }
    }
}

void multi_eng(hls::stream<ap_uint<kMemWidth> >& mem_row, ) {

    uint16_t base_addr [kNumEngs];
    uint16_t eng_stride = kDim1 / kNumEngs;

    for (i0 = 0; i0 < kNumEngs; i0 ++) {
    #pragma HLS UNROLL 
        base_addr[i0] = kRowsPerBlk * eng_stride * i0;
    }

    for (i0 = 0; i0 < kNumEngs; i0 ++) {
    #pragma HLS UNROLL 

        for (i1 = 0; i1 < eng_stride; i1 ++) {
            dual::lorenzo_2d_1l_stream<ap_uint<32>, int16_t>(mem_row, quant_code_stream0, qua_code_vector_stream, kDim1, kCallIdx);

            scheduler(qua_code_vector_stream, quant_code_buf, quant_code_stream0, quant_code_stream1, hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15);

            ParallelEncoder(quant_code_stream1, hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15, huff_encoder_stream);

        }

    }

}

void sz_hls(ap_uint<kMemWidth>* in_data, ap_uint<kOutWidth>* out_data) {

#pragma HLS INTERFACE m_axi port=in_data depth=kInSize
#pragma HLS INTERFACE m_axi port=out_data depth=kOutSize

// #pragma HLS INTERFACE m_axi port=in_data depth=kInSize offset = slave bundle = gmem0
// #pragma HLS INTERFACE m_axi port=out_data depth=kOutSize offset = slave bundle = gmem0
// #pragma HLS INTERFACE s_axilite port=in_data bundle = control
// #pragma HLS INTERFACE s_axilite port=out_data bundle = control

#pragma HLS dataflow
    hls::stream<ap_uint<kMemWidth> > mem_row;
    hls::stream<CodeT> quant_code_stream0[kNumHists];
    hls::stream<CodeT> quant_code_stream1[kNumHists];
    hls::stream<ap_uint<kQuaVecWidth> > qua_code_vector_stream;
    hls::stream<Codeword> huff_encoder_stream[kNumHists];

    Codeword hist0[1024];
    Codeword hist1[1024];
    Codeword hist2[1024];
    Codeword hist3[1024];
    Codeword hist4[1024];
    Codeword hist5[1024];
    Codeword hist6[1024];
    Codeword hist7[1024];
    Codeword hist8[1024];
    Codeword hist9[1024];
    Codeword hist10[1024];
    Codeword hist11[1024];
    Codeword hist12[1024];
    Codeword hist13[1024];
    Codeword hist14[1024];
    Codeword hist15[1024];
    ap_uint<kQuaVecWidth> quant_code_buf[kQuantBufSize];

    #pragma HLS STREAM variable = mem_row depth = 32
    #pragma HLS STREAM variable = quant_code_stream0 depth = 32
    #pragma HLS STREAM variable = quant_code_stream1 depth = 32
    #pragma HLS STREAM variable = qua_code_vector_stream depth = 32
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

    scheduler(qua_code_vector_stream, quant_code_buf, quant_code_stream0, quant_code_stream1, hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15);

    ParallelEncoder(quant_code_stream1, hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15, huff_encoder_stream);

    StreamToMem(huff_encoder_stream, out_data);

    // std::cout << "finish" << std::endl;
}
