#ifndef H_DUAL_QUANT_H
#define H_DUAL_QUANT_H

#include "constants.h"
#include "hls_math.h"

/********************************************************************************/
// MACROS
#define ABS(X) ( (X) > 0 ? (X) : -(X) )

/********************************************************************************/
// DATA TYPES
#ifdef DOUBLE
typedef double InputDataT;
#else
typedef float InputDataT; 
#endif

const double kErrorBound = ebs_l4[EBx2_r];
const uint32_t kRadius = dims_l16[RADIUS];

namespace dual {
template <typename T, typename Q>
void lorenzo_2d_1l_stream(hls::stream<ap_uint<kMemWidth> >& mem_row, hls::stream<CodeT> code_stream[kNumHists], hls::stream<ap_uint<kQuaVecWidth> >& qua_code_vector_stream, uint16_t eng_blks, uint8_t call_idx) {

    uint32_t d_raw[kNumDataPerRow];
    float d_reg[kNumDataPerRow];
    double pre_qua_reg[kNumDataPerRow];
    ap_uint<kQuaVecWidth> qua_code_vector_reg = 0; 

    Q pre_qua_buf0[kBlkSize];
    Q pre_qua_buf1[kNumDataPerRow + 1];
    Q pre_qua_buf2[kNumDataPerRow + 1];
    Q pred[kNumDataPerRow]; 
    Q post_err[kNumDataPerRow];
    Q quantizable[kNumDataPerRow];
    CodeT code_reg[kNumDataPerRow];

    #pragma HLS ARRAY_PARTITION variable = d_raw dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = d_reg dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = pre_qua_reg dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = pre_qua_buf1 dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = pre_qua_buf2 dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = pred dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = post_err dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = quantizable dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = code_reg dim = 1 complete
    #pragma HLS ARRAY_PARTITION variable = pre_qua_buf0 dim = 1 complete
    // #pragma HLS RESOURCE variable=pre_qua_buf0 core=XPM_MEMORY uram

    // std::ofstream o_file0, o_file1, o_file2;
    // std::string f_name0 = "C:\\Users\\Bizon\\Desktop\\sz_hls4\\inter_data\\ori_data_" + std::to_string(call_idx) + ".txt";
    // std::string f_name1 = "C:\\Users\\Bizon\\Desktop\\sz_hls4\\inter_data\\code_" + std::to_string(call_idx) + ".txt";
    // std::string f_name2 = "C:\\Users\\Bizon\\Desktop\\sz_hls4\\inter_data\\pre_quant_" + std::to_string(call_idx) + ".txt";

    // o_file0.open(f_name0);
    // o_file1.open(f_name1);
    // o_file2.open(f_name2);

    for(uint8_t i0 = 0; i0 <= kNumDataPerRow; i0++) {
    #pragma HLS UNROLL  
        pre_qua_buf0[i0] = 0;
        pre_qua_buf1[i0] = 0;
        pre_qua_buf2[i0] = 0;
    }

dual_loop:  
    for (uint16_t i2 = 0; i2 < eng_blks; i2++) {
        // prequantization and postquantization
        for (uint16_t i1 = 0; i1 < kRowsPerBlk; i1++) {
        #pragma HLS PIPELINE II = 1 rewind

            ap_uint<kMemWidth> row_buf = mem_row.read();

            for (uint8_t i0 = 0; i0 < kNumDataPerRow; i0++) {
            #pragma HLS UNROLL   
                d_raw[i0] = row_buf.range(kDataWidth * (i0 + 1) - 1, kDataWidth * i0);
                d_reg[i0] = *(float*)&(d_raw[i0]);
                pre_qua_reg[i0] = d_reg[i0] * kErrorBound;
                pre_qua_buf2[i0 + 1] = hls::floor(pre_qua_reg[i0]);

                if (i2 == 0) {
                    pre_qua_buf1[i0 + 1] = 0;
                } else {
                    pre_qua_buf1[i0 + 1] = pre_qua_buf0[i1 * kNumDataPerRow + i0];
                }
                pre_qua_buf0[i1 * kNumDataPerRow + i0] = pre_qua_buf2[i0 + 1];

                pred[i0] = pre_qua_buf2[i0] + pre_qua_buf1[i0 + 1] - pre_qua_buf1[i0];
                post_err[i0] = pre_qua_buf2[i0 + 1] - pred[i0];
                quantizable[i0] = hls::abs(post_err[i0]) < kRadius;
                code_reg[i0] = post_err[i0] + kRadius;
                // data[id] = (1 - quantizable[i0]) * pre_qua_buf2[i0+1];  // data array as outlier
                code_reg[i0] = quantizable[i0] * code_reg[i0];
                code_stream[i0] << code_reg[i0];
                qua_code_vector_reg.range(kDualCodeWidth * (i0 + 1) - 1, kDualCodeWidth * i0) = code_reg[i0];
            }

            if (i2 * kRowsPerBlk + i1 < kHuffRows) {
                qua_code_vector_stream << qua_code_vector_reg;
            }
            
            if (i1 == kRowsPerBlk - 1) {
                pre_qua_buf1[0] = 0;
                pre_qua_buf2[0] = 0;

            } else {
                pre_qua_buf1[0] = pre_qua_buf1[kNumDataPerRow];
                pre_qua_buf2[0] = pre_qua_buf2[kNumDataPerRow];
            }

            // if (i2 * kRowsPerBlk + i1 < kHuffRows) {
            //     for (uint8_t i0 = 0; i0 < kNumDataPerRow; i0++) {
            //         o_file0 << std::setprecision(14) << d_reg[i0] << "\n";
            //         o_file1 << code_reg[i0] << "\n";
            //         o_file2 << std::setprecision(14) << pre_qua_reg[i0] << "\n";

            //         if ((i2 * kRowsPerBlk + i1) * kNumDataPerRow + i0 == 1395) {  
            //             std::cout << pre_qua_buf2[i0] << " " << pre_qua_buf1[i0+1] << " " << pre_qua_buf1[i0] << " " << pre_qua_buf2[i0 + 1] << std::endl;
            //         } 
            //     }
            // }
        }
    }
    // o_file0.close();
    // o_file1.close();
    // o_file2.close();

// dual_loop:
//     for (uint16_t i1 = 0; i1 < eng_blks; i1++) {
//         // prequantization and postquantization
//         for (uint16_t i0 = 0; i0 < kBlkSize; i0++) {
//         #pragma HLS PIPELINE II = 1
//         	uint32_t g_i = i1 * kBlkSize + i0;
//             uint32_t d_raw = data[g_i];
//             float d_reg = *(float*)&d_raw;
//             // d_reg = *(InputDataT*)(&d_raw);
//             // d_reg = *reinterpret_cast<InputDataT*>(&d_raw);

//             double pre_qua_reg = d_reg * kErrorBound;
//             // pre_qua_buf1[i0 + 1] = hls::round(pre_qua_reg);
//             // pre_qua_buf1[i0 + 1] = hls::ceil(pre_qua_reg);
            
//             pre_qua_buf0[i0 + 1] = pre_qua_buf1[i0 + 1];
//             pre_qua_buf1[i0 + 1] = hls::floor(pre_qua_reg);
//             Q pred = pre_qua_buf1[i0] + pre_qua_buf0[i0 + 1] - pre_qua_buf0[i0];
//             Q post_err = pre_qua_buf1[i0 + 1] - pred;
//             uint8_t quantizable = hls::abs(post_err) < kRadius;
//             CodeT code_reg = post_err + kRadius;
//             // data[id] = (1 - quantizable) * pre_qua_buf1[i0+1];  // data array as outlier
//             code_reg = quantizable * code_reg;
//             // code[g_i] = code_reg;
//             code << code_reg;

//             // if (i1 == 4) 
//             // {   
//             //     o_file0 << std::setprecision(14) << d_reg << "\n";
//             //     o_file1 << code_reg << "\n";
//             //     o_file2 << std::setprecision(14) << pre_qua_reg << "\n";

//             //     if (i0 == 1395)
//             //     {
//             //         std::cout << pre_qua_buf1[i0] << " " << pre_qua_buf0[i0+1] << " " << pre_qua_buf0[i0] << " " << pre_qua_buf1[i0 + 1] << std::endl;
//             //     }
//             // }
//             // o_file1 << code_reg << "\n";
//         }
//     }
//     // o_file0.close();
//     // o_file1.close();
//     // o_file2.close();

}

}

#endif //H_DUAL_QUANT_H
