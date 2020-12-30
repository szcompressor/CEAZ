#ifndef H_HUFFMAN_ENCODER_H
#define H_HUFFMAN_ENCODER_H

#include "constants.h"
#include <bitset>

void ParallelEncoder(hls::stream<CodeT> quant_code_stream[kNumHists], uint32_t hist0[1024], uint32_t hist1[1024], uint32_t hist2[1024], uint32_t hist3[1024], uint32_t hist4[1024], 
    uint32_t hist5[1024], uint32_t hist6[1024], uint32_t hist7[1024], uint32_t hist8[1024], uint32_t hist9[1024], uint32_t hist10[1024], uint32_t hist11[1024], 
    uint32_t hist12[1024], uint32_t hist13[1024], uint32_t hist14[1024], uint32_t hist15[1024], hls::stream<uint32_t> huff_encoder_stream[kNumHists]);

#endif