#ifndef H_HUFFMAN_CODEBOOK_H
#define H_HUFFMAN_CODEBOOK_H

#include "constants.h"
#include <bitset>

const uint16_t kCodeTreeSize = 1024;
static const uint16_t kSymbolSize = 1024;
static const uint8_t kSymbolBits = 16;
static const uint8_t kHistogramBits = 16;
static const uint8_t kLenHistogramSize = 64;
static const uint8_t RADIX = 16;
static const uint8_t BITS_PER_LOOP = 4;
// static const uint16_t kMaxBits = 32;

//typedef uint32_t Frequency;
typedef ap_uint<kHistogramBits> Histogram;
typedef ap_uint<BITS_PER_LOOP> Digit;

// struct Symbol {
//     ap_uint<kSymbolBits> value;
//     Frequency frequency;
// };

#endif //H_HUFFMAN_CODEBOOK_H
