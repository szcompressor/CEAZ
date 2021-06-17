#include "histogram.h"

void HistogramMap(hls::stream<CodeT>& quant_code_stream, uint32_t hist[kHistSize]) {

#pragma HLS DEPENDENCE variable=hist intra RAW false

    for(uint32_t i = 0; i < kHistSize; i++) {
    #pragma HLS PIPELINE II=1 rewind        
        hist[i] = 0;
    }

    CodeT old = quant_code_stream.read();
    uint32_t acc = 1;
    for(uint32_t i = 1; i < kInSize; i++) {
    #pragma HLS PIPELINE II=1
        CodeT val = quant_code_stream.read();
        if(old == val) {
            acc = acc + 1;
        } else {
            hist[old] = acc;
            acc = hist[val] + 1;
        }
        old = val;
    }

    hist[old] = acc;
}

void HistogramReduce(uint32_t hist0[kHistSize], uint32_t hist1[kHistSize], uint32_t hist2[kHistSize], uint32_t hist3[kHistSize], uint32_t hist4[kHistSize], 
    uint32_t hist5[kHistSize], uint32_t hist6[kHistSize], uint32_t hist7[kHistSize], uint32_t hist8[kHistSize], uint32_t hist9[kHistSize], uint32_t hist10[kHistSize],
    uint32_t hist11[kHistSize], uint32_t hist12[kHistSize], uint32_t hist13[kHistSize], uint32_t hist14[kHistSize], uint32_t hist15[kHistSize], Symbol freq[INPUT_SYMBOL_SIZE] /* hls::stream<uint32_t>& freq_stream */) {
    // std::ofstream o_file0;
    // o_file0.open("C:\\Users\\Bizon\\Desktop\\all_hls_projects\\fixed_table\\inter_data\\code_freq.txt");
    uint32_t freq_reg = 0;

    // uint32_t test_arr [kHistSize] = {3, 1, 2, 5, 5, 1, 5, 7, 6, 0};
    // uint32_t test_arr [kHistSize] = {1, 1, 2, 1, 2, 2, 3, 2, 2, 2, 4, 4, 7, 1, 1, 1};

    for(uint16_t i = 0; i < kHistSize; i++) {
    #pragma HLS PIPELINE II=1 rewind
        
        freq_reg = (hist0[i] + hist1[i] + hist2[i] + hist3[i] + hist4[i] + hist5[i] + hist6[i] + hist7[i] + hist8[i] + hist9[i] + hist10[i] + hist11[i] + hist12[i]+ hist13[i] + hist14[i] + hist15[i]);
        freq[i].value = i;
        freq[i].frequency = freq_reg;

        // freq_stream << freq_reg;
        // freq_stream << test_arr[i];
        // o_file0 << freq_reg << "\n";
    }
    // o_file0.close();
}

void QuantCodeFrequency(hls::stream<CodeT> quant_code_stream0[kNumHists], hls::stream<CodeT> quant_code_stream1[kNumHists], Symbol freq[INPUT_SYMBOL_SIZE]){

#pragma HLS DATAFLOW

    uint32_t hist0[kHistSize];
    uint32_t hist1[kHistSize];
    uint32_t hist2[kHistSize];
    uint32_t hist3[kHistSize];
    uint32_t hist4[kHistSize];
    uint32_t hist5[kHistSize];
    uint32_t hist6[kHistSize];
    uint32_t hist7[kHistSize];
    uint32_t hist8[kHistSize];
    uint32_t hist9[kHistSize];
    uint32_t hist10[kHistSize];
    uint32_t hist11[kHistSize];
    uint32_t hist12[kHistSize];
    uint32_t hist13[kHistSize];
    uint32_t hist14[kHistSize];
    uint32_t hist15[kHistSize];

    hls::stream<CodeT> quant_code_stream2[kNumHists];
    CodeT quant_code_reg[kNumHists];

    // #pragma HLS BIND_STORAGE variable=hist0 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist1 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist2 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist3 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist4 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist5 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist6 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist7 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist8 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist9 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist10 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist11 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist12 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist13 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist14 type=RAM_2P impl=BRAM latency=1 
    // #pragma HLS BIND_STORAGE variable=hist15 type=RAM_2P impl=BRAM latency=1 

    #pragma HLS BIND_STORAGE variable=hist0 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist1 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist2 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist3 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist4 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist5 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist6 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist7 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist8 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist9 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist10 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist11 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist12 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist13 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist14 type=RAM_2P latency=1 
    #pragma HLS BIND_STORAGE variable=hist15 type=RAM_2P latency=1 

    #pragma HLS STREAM variable = quant_code_stream2 depth = 32
    #pragma HLS ARRAY_PARTITION variable = quant_code_reg dim = 1 complete

    for(uint16_t i1 = 0; i1 < kInSize; i1++) {
    #pragma HLS PIPELINE II=1 rewind

        for (uint8_t i0 = 0; i0 < kNumHists; i0++) {
        #pragma HLS UNROLL
        quant_code_reg[i0] = quant_code_stream0[i0].read();
        quant_code_stream1[i0].write(quant_code_reg[i0]);
        quant_code_stream2[i0].write(quant_code_reg[i0]);
        }
    }

    HistogramMap(quant_code_stream2[0], hist0);
    HistogramMap(quant_code_stream2[1], hist1);
    HistogramMap(quant_code_stream2[2], hist2);
    HistogramMap(quant_code_stream2[3], hist3);
    HistogramMap(quant_code_stream2[4], hist4);
    HistogramMap(quant_code_stream2[5], hist5);
    HistogramMap(quant_code_stream2[6], hist6);
    HistogramMap(quant_code_stream2[7], hist7);
    HistogramMap(quant_code_stream2[8], hist8);
    HistogramMap(quant_code_stream2[9], hist9);
    HistogramMap(quant_code_stream2[10], hist10);
    HistogramMap(quant_code_stream2[11], hist11);
    HistogramMap(quant_code_stream2[12], hist12);
    HistogramMap(quant_code_stream2[13], hist13);
    HistogramMap(quant_code_stream2[14], hist14);
    HistogramMap(quant_code_stream2[15], hist15);

    HistogramReduce(hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15, freq);
}
