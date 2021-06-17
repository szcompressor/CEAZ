#ifndef H_HUFFMAN_ENCODER_H
#define H_HUFFMAN_ENCODER_H

#include "constants.h"
#include <bitset>

void ParallelEncoder(hls::stream<CodeT> quant_code_stream[kNumHists], hls::stream<ap_uint<kQuaVecWidth> >& qua_code_vector_stream, hls::stream<Codeword_struct> huff_encoder_stream[kNumHists]);

#endif