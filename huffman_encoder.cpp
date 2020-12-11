#include "huffman_encoder.h"

void SingleEncoder(hls::stream<CodeT>& quant_code_stream, uint32_t* huff_codebook, hls::stream<uint32_t>& huff_encoder_stream) {

    CodeT quant_code_reg = quant_code_stream.read();
    huff_encoder_stream << huff_codebook[quant_code_reg];
}

void ParallelEncoder(hls::stream<CodeT> quant_code_stream[kNumHists], uint32_t hist0[1024], uint32_t hist1[1024], uint32_t hist2[1024], uint32_t hist3[1024], uint32_t hist4[1024], 
    uint32_t hist5[1024], uint32_t hist6[1024], uint32_t hist7[1024], uint32_t hist8[1024], uint32_t hist9[1024], uint32_t hist10[1024], uint32_t hist11[1024], 
    uint32_t hist12[1024], uint32_t hist13[1024], uint32_t hist14[1024], uint32_t hist15[1024], hls::stream<uint32_t> huff_encoder_stream[kNumHists]) {

    SingleEncoder(quant_code_stream[0], hist0, huff_encoder_stream[0]);
    SingleEncoder(quant_code_stream[1], hist1, huff_encoder_stream[1]);
    SingleEncoder(quant_code_stream[2], hist2, huff_encoder_stream[2]);
    SingleEncoder(quant_code_stream[3], hist3, huff_encoder_stream[3]);
    SingleEncoder(quant_code_stream[4], hist4, huff_encoder_stream[4]);
    SingleEncoder(quant_code_stream[5], hist5, huff_encoder_stream[5]);
    SingleEncoder(quant_code_stream[6], hist6, huff_encoder_stream[6]);
    SingleEncoder(quant_code_stream[7], hist7, huff_encoder_stream[7]);
    SingleEncoder(quant_code_stream[8], hist8, huff_encoder_stream[8]);
    SingleEncoder(quant_code_stream[9], hist9, huff_encoder_stream[9]);
    SingleEncoder(quant_code_stream[10], hist10, huff_encoder_stream[10]);
    SingleEncoder(quant_code_stream[11], hist11, huff_encoder_stream[11]);
    SingleEncoder(quant_code_stream[12], hist12, huff_encoder_stream[12]);
    SingleEncoder(quant_code_stream[13], hist13, huff_encoder_stream[13]);
    SingleEncoder(quant_code_stream[14], hist14, huff_encoder_stream[14]);
    SingleEncoder(quant_code_stream[15], hist15, huff_encoder_stream[15]);
}
