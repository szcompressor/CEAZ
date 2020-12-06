#ifndef H_HUFFMAN_ENCODER_H
#define H_HUFFMAN_ENCODER_H

#include "constants.h"
#include <bitset>

const uint16_t kCodeTreeSize = 1024;
static const uint16_t kSymbolSize = 1024;
static const uint8_t kSymbolBits = 16;
static const uint8_t kHistogramBits = 16;
static const uint8_t kLenHistogramSize = 64;
static const uint8_t RADIX = 16;
static const uint8_t BITS_PER_LOOP = 4;
static const uint16_t kMaxBits = 32;

typedef ap_uint<16> CodeT;
typedef uint32_t Frequency;
typedef ap_uint<kHistogramBits> Histogram;
typedef ap_uint<BITS_PER_LOOP> Digit;

struct Symbol {
    ap_uint<kSymbolBits> value;
    Frequency frequency;
};

struct Codeword {
    ap_uint<kMaxBits> codeword;
    ap_uint<8> code_length;
};

namespace huf {
void QuantCodeFrequency0(hls::stream<CodeT> quant_code_stream[kNumEngs], uint32_t quant_code_size, uint32_t eng_blks[kNumEngs],
    hls::stream<uint32_t>& freq_stream);

void QuantCodeFrequency1(hls::stream<CodeT> quant_code_stream[kNumHists], hls::stream<uint32_t>& freq_stream);

void Filter(hls::stream<uint32_t>& freq_stream, Symbol* heap, uint16_t* heap_length);

void RadixSort(Symbol* heap, uint16_t heap_length);

void CreateTree(Symbol* heap, uint16_t num_symbols, ap_uint<kSymbolBits>* parent, ap_uint<kSymbolSize>& left, 
	ap_uint<kSymbolSize>& right, Frequency* frequency);

void ComputeBitsLength(ap_uint<kSymbolBits>* parent, ap_uint<kSymbolSize>& left, ap_uint<kSymbolSize>& right, 
	uint16_t num_symbols, Histogram* length_histogram, Frequency* child_depth);

void CanonizeTree(Symbol* heap, uint16_t num_symbols, Histogram* length_histogram, uint16_t* huff_bits_length, uint16_t tree_depth);

void CreateCodeword(uint16_t* huff_bits_length, Histogram* length_histogram, hls::stream<Codeword>& huff_code_stream);

void HuffConstructTreeStream(hls::stream<Frequency>& freq_stream, hls::stream<Codeword>& huff_code_stream);

}

#endif //H_HUFFMAN_ENCODER_H
