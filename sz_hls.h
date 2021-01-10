#ifndef H_SZ_HLS_H
#define H_SZ_HLS_H

#include "dual_quant.h"
#include "huffman_codebook.h"
#include "huffman_encoder.h"

void sz_hls(ap_uint<kMemWidth>* in_data, ap_uint<kOutWidth>* out_data);

#endif //H_SZ_HLS_H
