#include "merge_sort_parallel.h"
#include "assert.h"

void merge_arrays(DTYPE in[SIZE], uint16_t width, uint16_t length, DTYPE out[SIZE]) {
  uint16_t f1 = 0;
  uint16_t f2 = width;
  uint16_t i1 = width;
  uint16_t i2 = 2 * width;
  if(i1 >= length) i1 = length;
  if(i2 >= length) i2 = length;

  DTYPE zero;
  zero.value = 0;
  zero.frequency = 0;

 merge_arrays:
  for (uint16_t i = 0; i < length; i++) {
#pragma HLS LOOP_TRIPCOUNT max = 512 min = 128 avg = 256
#pragma HLS pipeline II=1

      DTYPE t1 = in[f1];
      DTYPE t2 = (f2 == i2) ? zero : in[f2];
    if(f2 == i2 || (f1 < i1 && t1.frequency <= t2.frequency)) {
	  out[i] = t1;
	  f1++;
	} else {
	  assert(f2 < i2);
	  out[i] = t2;
	  f2++;
	}
	if(f1 == i1 && f2 == i2) {
      f1 = i2;
      i1 += 2 * width;
      i2 += 2 * width;
	  if(i1 >= length) i1 = length;
	  if(i2 >= length) i2 = length;
      f2 = i1;
 	}
  }
}

void merge_sort_parallel(DTYPE A[SIZE], uint16_t length, DTYPE B[SIZE]) {
// #pragma HLS dataflow

	DTYPE temp[STAGES-1][SIZE];
	DTYPE B_buf[SIZE];
#pragma HLS array_partition variable=temp complete dim=1

    uint16_t exp = 0;
    uint16_t length_reg = length;
    while (length_reg >>= 1) ++exp;
    length_reg = 1 << exp;
    if (length > length_reg) {
        exp += 1;
        length_reg <<= 1;
    }

	uint16_t width = 1;
	merge_arrays(A, width, length_reg, temp[0]);
	width *= 2;
    uint16_t stages = exp;
	for (uint16_t stage = 1; stage < stages-1; stage++) {
#pragma HLS LOOP_TRIPCOUNT max = 8 min = 6 avg = 6
#pragma HLS unroll
		merge_arrays(temp[stage-1], width, length_reg, temp[stage]);
		width *= 2;
	}

	merge_arrays(temp[stages-2], width, length_reg, B_buf);

	uint16_t pos = 0;
	DTYPE zero;
	zero.value = 0;
	zero.frequency = 0;
	for(uint16_t i = 0; i < SIZE; i++) {
#pragma HLS pipeline II=1
		DTYPE sym = B_buf[i];
	    uint32_t freq = sym.frequency;
	    if (pos < length && freq) {
           B[pos] = sym;
           pos += 1;
	    } else if (pos >= length) {
	        B[pos] = zero;
            pos += 1;
	    }
	}
}

// void merge_sort_parallel(DTYPE A[SIZE], uint16_t length, DTYPE B[SIZE]) {
// #pragma HLS dataflow
// 	DTYPE temp0[SIZE];
// 	DTYPE temp1[SIZE];
// 	DTYPE temp2[SIZE];
// 	DTYPE temp3[SIZE];
// 	DTYPE temp4[SIZE];
// 	DTYPE temp5[SIZE];
// 	DTYPE temp6[SIZE];
// 	DTYPE temp7[SIZE];
// 	DTYPE temp8[SIZE];
// 	// DTYPE temp9[SIZE];

// // #pragma HLS array_partition variable=temp complete dim=1
// // #pragma HLS resource variable=temp core=RAM_2P_BRAM

// // #pragma HLS RESOURCE variable=temp core=XPM_MEMORY uram

//     merge_arrays(A, 1, temp0);
// 	merge_arrays(temp0, 2, temp1);
// 	merge_arrays(temp1, 4, temp2);
// 	merge_arrays(temp2, 8, temp3);
// 	merge_arrays(temp3, 16, temp4);
// 	merge_arrays(temp4, 32, temp5);
// 	merge_arrays(temp5, 64, temp6);
// 	merge_arrays(temp6, 128, temp7);
// 	merge_arrays(temp7, 256, temp8);
// 	merge_arrays(temp8, 512, B);
// }
