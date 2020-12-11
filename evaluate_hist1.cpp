#include "constants.h"
typedef uint16_t CodeT;

void MemToStream(ap_uint<kMemWidth>* in_data, hls::stream<CodeT> quant_code_stream[kNumHists]) {

    for (uint32_t i1 = 0; i1 < kInSize; i1++) {
        ap_uint<kMemWidth> row_buf = in_data[i1];
        for (uint8_t i0 = 0; i0 < 16; i0++) {
        // #pragma HLS PIPELINE II = 1
        #pragma HLS UNROLL
            quant_code_stream[i0] << row_buf.range(16 * (i0 + 1) - 1, 16 * i0);
        }
    }
}

void ParallelMap(hls::stream<CodeT> quant_code_stream[kNumHists], uint32_t hist[kNumHists][1024]) {

    #pragma HLS DEPENDENCE variable=hist intra RAW false
    for (uint8_t i1 = 0; i1 < kNumHists; ++i1) {
    #pragma HLS UNROLL
    // #pragma HLS PIPELINE II=1

        // for(uint32_t i0 = 0; i0 < 1024; i0++) {
        // // #pragma HLS PIPELINE II=1
        // #pragma HLS UNROLL  
        //     hist[i1][i0] = 0;
        // }

        // for (uint32_t i0 = 1; i0 < 4096; i0++) {
        //     #pragma HLS PIPELINE II=1
        //     CodeT val = quant_code_stream[i1].read();
        //     hist[i1][val]++;
        // }

        CodeT old = quant_code_stream[i1].read();
        uint32_t acc = 0;
        for(uint32_t i0 = 1; i0 < 4096; i0++) {
        #pragma HLS PIPELINE II=1
            CodeT val = quant_code_stream[i1].read();
            if(old == val) {
                acc = acc + 1;
            } else {
                hist[i1][old] = acc;
                acc = hist[i1][val] + 1;
            }
            old = val;
        }

        hist[i1][old] = acc;
    }
}

void ParallelReduce(uint32_t hist[kNumHists][1024], uint32_t* freq_stream) {
    // std::ofstream o_file0;
    // o_file0.open("C:\\Users\\Bizon\\Desktop\\sz_hls2\\inter_data\\code_freq.txt");

    for(uint16_t i1 = 0; i1 < 1024; i1++) {
    #pragma HLS PIPELINE II=1
    // #pragma HLS UNROLL 

        // uint32_t total_freq = 0;
        freq_stream[i1] = 0;
        for (uint8_t i0 = 0; i0 < kNumHists; ++i0) {
        #pragma HLS UNROLL
        #pragma HLS PIPELINE II=1
            freq_stream[i1] = hist[i0][i1];
        }

        // freq_stream[i1] = total_freq;
        // o_file0 << total_freq << "\n";
    }
}

//Top level function
void QuantCodeFrequency1(ap_uint<kMemWidth>* in_data, uint32_t* freq_stream){

#pragma HLS DATAFLOW
    uint32_t hist[kNumHists][1024];

    hls::stream<CodeT> quant_code_stream[kNumHists];
    #pragma HLS STREAM variable = quant_code_stream depth = 32
    #pragma HLS INTERFACE m_axi port=in_data depth = kInSize
    #pragma HLS INTERFACE m_axi port=freq_stream depth = 1024

    // uint32_t hist0[1024];
    // uint32_t hist1[1024];
    // #pragma HLS ARRAY_PARTITION variable = hist0 dim = 1 complete
    // #pragma HLS ARRAY_PARTITION variable = hist1 dim = 1 complete
    #pragma HLS RESOURCE variable=hist core=RAM_T2P_BRAM 
    #pragma HLS ARRAY_PARTITION variable = hist dim = 1 complete

    // #pragma HLS RESOURCE variable=hist0 core=RAM_T2P_BRAM 
    // #pragma HLS RESOURCE variable=hist1 core=RAM_T2P_BRAM 

    MemToStream(in_data, quant_code_stream);
    ParallelMap(quant_code_stream, hist);
    ParallelReduce(hist, freq_stream);
}
