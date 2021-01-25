#ifndef H_CONSTANTS_H
#define H_CONSTANTS_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>

#include <stdint.h>
#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"

#include <fstream>

// dims_l16
static const uint8_t DIM0   = 0;
static const uint8_t DIM1   = 1;
static const uint8_t DIM2   = 2;
static const uint8_t DIM3   = 3;
static const uint8_t nBLK0  = 4;
static const uint8_t nBLK1  = 5;
static const uint8_t nBLK2  = 6;
static const uint8_t nBLK3  = 7;
static const uint8_t nDIM   = 8;
static const uint8_t LEN    = 12;
static const uint8_t CAP    = 13;
static const uint8_t RADIUS = 14;

// ebs_l4
static const uint8_t EB     = 0;
static const uint8_t EBr    = 1;
static const uint8_t EBx2   = 2;
static const uint8_t EBx2_r = 3;

static const uint16_t B_1d = 256;
static const uint16_t B_2d = 16;
static const uint16_t B_3d = 8;

static const uint32_t dims_l16[16] = {1024, 64, 1, 1, 225, 113, 1, 1, 2, 0, 0, 0, 6480000, 1024, 512, 0};
static const double ebs_l4[4] = {0.000089401054382324226, 11185.550404400077, 0.00017880210876464845, 5592.7752022000386};
static const uint16_t kMemWidth = 512;
const uint32_t kBurst = 4;
const uint8_t kDataWidth = 32;
const uint16_t kNumDataPerRow = kMemWidth / kDataWidth;
const uint16_t kBlkSize = dims_l16[0];
const uint16_t kRowsPerBlk = kBlkSize / kNumDataPerRow;
const uint32_t kInSize = dims_l16[0] * dims_l16[1] / kNumDataPerRow;

const uint16_t kRows = kInSize;
const uint16_t kHuffRows = 1024; // fix the variable problem

const uint16_t kOutWidth = 512;
const uint32_t kOutSize = kRows * 16;

const uint8_t kNumEngs = 1;
const uint32_t kEngSize = dims_l16[0] * dims_l16[1] / kNumEngs;
const uint16_t kDim0 = dims_l16[0];

const uint8_t kNumHists = 16;

const uint8_t kDualCodeWidth = 16; 
typedef ap_uint<kDualCodeWidth> CodeT;

const uint16_t kQuantBufSize = kHuffRows;
const uint16_t kQuaVecWidth = kNumHists * kDualCodeWidth;

const uint8_t kBatches = 4;

static const uint16_t kMaxBits = 32;
struct Codeword {
    ap_uint<kMaxBits> codeword;
    ap_uint<8> code_length;
};
// #define DOUBLE 1

#endif
