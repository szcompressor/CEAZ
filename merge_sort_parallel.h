const static int SIZE = 1024;
const static int STAGES = 10;
#include "huffman_codebook.h"

// typedef float DTYPE;

typedef Symbol DTYPE;

//struct DTYPE {
//    ap_uint<kSymbolBits> value;
//    Frequency frequency;
//};

extern void merge_sort_parallel(DTYPE A[SIZE], DTYPE B[SIZE]);
