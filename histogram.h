#ifndef H_HISTOGRAM_H
#define H_HISTOGRAM_H

#include "constants.h"
#include <bitset>

#include "huffman.h"

void QuantCodeFrequency(hls::stream<CodeT> quant_code_stream0[kNumHists], hls::stream<CodeT> quant_code_stream1[kNumHists], Symbol freq[INPUT_SYMBOL_SIZE]);

#endif //H_HISTOGRAM_H