#include "huffman_encoder.h"
#include "codeword.h"

void SingleEncoder(hls::stream<CodeT>& quant_code_stream, const Codeword_struct* huff_codebook, hls::stream<Codeword_struct>& huff_encoder_stream) {

    for(uint16_t i1 = 0; i1 < kInSize; i1++) {
    #pragma HLS PIPELINE II=1 rewind
        CodeT quant_code_reg = quant_code_stream.read();
        huff_encoder_stream << huff_codebook[quant_code_reg];
    }
}

void ParallelEncoder(hls::stream<CodeT> quant_code_stream[kNumHists], hls::stream<ap_uint<kQuaVecWidth> >& qua_code_vector_stream, hls::stream<Codeword_struct> huff_encoder_stream[kNumHists]) {
// #pragma HLS DATAFLOW
#pragma HLS allocation function instances=SingleEncoder limit=16

    ap_uint<kQuaVecWidth> qua_code_vector_reg; 
    qua_code_vector_reg = qua_code_vector_stream.read();
    
    SingleEncoder(quant_code_stream[0], tab0, huff_encoder_stream[0]);
    SingleEncoder(quant_code_stream[1], tab1, huff_encoder_stream[1]);
    SingleEncoder(quant_code_stream[2], tab2, huff_encoder_stream[2]);
    SingleEncoder(quant_code_stream[3], tab3, huff_encoder_stream[3]);
    SingleEncoder(quant_code_stream[4], tab4, huff_encoder_stream[4]);
    SingleEncoder(quant_code_stream[5], tab5, huff_encoder_stream[5]);
    SingleEncoder(quant_code_stream[6], tab6, huff_encoder_stream[6]);
    SingleEncoder(quant_code_stream[7], tab7, huff_encoder_stream[7]);
    SingleEncoder(quant_code_stream[8], tab8, huff_encoder_stream[8]);
    SingleEncoder(quant_code_stream[9], tab9, huff_encoder_stream[9]);
    SingleEncoder(quant_code_stream[10], tab10, huff_encoder_stream[10]);
    SingleEncoder(quant_code_stream[11], tab11, huff_encoder_stream[11]);
    SingleEncoder(quant_code_stream[12], tab12, huff_encoder_stream[12]);
    SingleEncoder(quant_code_stream[13], tab13, huff_encoder_stream[13]);
    SingleEncoder(quant_code_stream[14], tab14, huff_encoder_stream[14]);
    SingleEncoder(quant_code_stream[15], tab15, huff_encoder_stream[15]);
}
