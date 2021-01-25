#ifndef H_HUFFMAN_ENCODER_H
#define H_HUFFMAN_ENCODER_H

#include "constants.h"
#include <bitset>

void ParallelEncoder(hls::stream<CodeT> quant_code_stream[kNumHists], Codeword hist0[1024], Codeword hist1[1024], Codeword hist2[1024], Codeword hist3[1024], Codeword hist4[1024], 
    Codeword hist5[1024], Codeword hist6[1024], Codeword hist7[1024], Codeword hist8[1024], Codeword hist9[1024], Codeword hist10[1024], Codeword hist11[1024], 
    Codeword hist12[1024], Codeword hist13[1024], Codeword hist14[1024], Codeword hist15[1024], hls::stream<Codeword> huff_encoder_stream[kNumHists]);

#endif