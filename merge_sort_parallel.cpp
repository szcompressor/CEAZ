#include "merge_sort_parallel.h"
#include "assert.h"

void merge_arrays(DTYPE in[SIZE], int width, DTYPE out[SIZE]) {
  int f1 = 0;
  int f2 = width;
  int i2 = width;
  int i3 = 2*width;
  if(i2 >= SIZE) i2 = SIZE;
  if(i3 >= SIZE) i3 = SIZE;
  DTYPE zero;
  zero.value = 0;
  zero.frequency = 0;
 merge_arrays:
  for (int i = 0; i < SIZE; i++) {
#pragma HLS pipeline II=1
      DTYPE t1 = in[f1];
      DTYPE t2 = (f2 == i3) ? zero : in[f2];
    if(f2 == i3 || (f1 < i2 && t1.frequency <= t2.frequency)) {
	  out[i] = t1;
	  f1++;
	} else {
	  assert(f2 < i3);
	  out[i] = t2;
	  f2++;
	}
	if(f1 == i2 && f2 == i3) {
      f1 = i3;
	  i2 += 2*width;
	  i3 += 2*width;
	  if(i2 >= SIZE) i2 = SIZE;
	  if(i3 >= SIZE) i3 = SIZE;
      f2 = i2;
 	}
  }
}

// void merge_sort_parallel(DTYPE A[SIZE], uint16_t length, DTYPE B[SIZE]) {
// //#pragma HLS dataflow

// 	DTYPE temp[STAGES-1][SIZE];
// #pragma HLS array_partition variable=temp complete dim=1
// // #pragma HLS resource variable=temp core=RAM_2P_BRAM
// // #pragma HLS RESOURCE variable=temp core=XPM_MEMORY uram

// 	uint8_t exp = 0;
// 	uint16_t length_reg = length;
// 	while (length_reg >>= 1) ++exp;

// 	uint16_t width = 1;
//     merge_arrays(A, width, temp[0]);
// 	width *= 2;

// 	for (uint8_t stage = 1; stage < exp; stage++) {	
// #pragma HLS unroll
// #pragma HLS LOOP_TRIPCOUNT max = 8 min = 4 avg = 6
		
// 		merge_arrays(temp[stage-1], width, temp[stage]);
// 		width *= 2;
// 	}

// 	merge_arrays(temp[exp-1], width, B);
// }

void merge_sort_parallel(DTYPE A[SIZE], uint16_t length, DTYPE B[SIZE]) {
//#pragma HLS dataflow

	DTYPE temp[STAGES-1][SIZE];
#pragma HLS array_partition variable=temp complete dim=1
// #pragma HLS resource variable=temp core=RAM_2P_BRAM
// #pragma HLS RESOURCE variable=temp core=XPM_MEMORY uram

	uint8_t exp = 0;
	uint16_t length_reg = length;
	while (length_reg >>= 1) ++exp;

	uint16_t width = 1;
    merge_arrays(A, width, temp[0]);
	width *= 2;

	for (uint8_t stage = 1; stage < STAGES-1; stage++) {	
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max = 8 min = 4 avg = 6
		
		merge_arrays(temp[stage-1], width, temp[stage]);
		width *= 2;
	}

	merge_arrays(temp[STAGES-2], width, B);
}

// void merge_sort_parallel(DTYPE A[SIZE], DTYPE B[SIZE]) {
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
