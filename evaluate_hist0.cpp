#include "constants.h"
typedef uint16_t CodeT;

void MemToStream0(ap_uint<kMemWidth>* in_data, hls::stream<CodeT> quant_code_stream[kNumHists]) {

    for (uint32_t i1 = 0; i1 < kInSize; i1++) {
        ap_uint<kMemWidth> row_buf = in_data[i1];
        for (uint8_t i0 = 0; i0 < 16; i0++) {
        // #pragma HLS PIPELINE II = 1 rewind
        #pragma HLS UNROLL
            quant_code_stream[i0] << row_buf.range(16 * (i0 + 1) - 1, 16 * i0);
        }
    }
}

void HistogramMap(hls::stream<CodeT>& quant_code_stream, uint32_t hist[1024]) {

#pragma HLS DEPENDENCE variable=hist intra RAW false

    CodeT old = quant_code_stream.read();
    uint32_t acc = 0;
    for(uint32_t i = 1; i < 4096; i++) {
    #pragma HLS PIPELINE II=1 rewind
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

void HistogramReduce(uint32_t hist0[1024], uint32_t hist1[1024], uint32_t hist2[1024], uint32_t hist3[1024], uint32_t hist4[1024], 
    uint32_t hist5[1024], uint32_t hist6[1024], uint32_t hist7[1024], uint32_t hist8[1024], uint32_t hist9[1024], uint32_t hist10[1024],
    uint32_t hist11[1024], uint32_t hist12[1024], uint32_t hist13[1024], uint32_t hist14[1024], uint32_t hist15[1024], uint32_t* freq_stream) {
    // std::ofstream o_file0;
    // o_file0.open("C:\\Users\\Bizon\\Desktop\\sz_hls1\\inter_data\\code_freq.txt");

    for(uint16_t i = 0; i < 1024; i++) {
    #pragma HLS PIPELINE II=1 rewind
        // freq_stream << (hist0[i] + hist1[i]);
        freq_stream[i] = (hist0[i] + hist1[i] + hist2[i] + hist3[i] + hist4[i] + hist5[i] + hist6[i] + hist7[i] + hist8[i] + hist9[i] + hist10[i] + hist11[i] + hist12[i]+ hist13[i] + hist14[i] + hist15[i]);
        // o_file0 << (hist0[i] + hist1[i]) << "\n";
    }

    // o_file0.close();
}

//Top level function
void QuantCodeFrequency0(ap_uint<kMemWidth>* in_data, uint32_t* freq_stream, uint8_t call_idx){

#pragma HLS DATAFLOW
    uint32_t hist0[1024];
    uint32_t hist1[1024];
    uint32_t hist2[1024];
    uint32_t hist3[1024];
    uint32_t hist4[1024];
    uint32_t hist5[1024];
    uint32_t hist6[1024];
    uint32_t hist7[1024];
    uint32_t hist8[1024];
    uint32_t hist9[1024];
    uint32_t hist10[1024];
    uint32_t hist11[1024];
    uint32_t hist12[1024];
    uint32_t hist13[1024];
    uint32_t hist14[1024];
    uint32_t hist15[1024];

    hls::stream<CodeT> quant_code_stream[kNumHists];
    #pragma HLS STREAM variable = quant_code_stream depth = 32

	// #pragma HLS INTERFACE m_axi port=quant_code_stream0 depth= 32768
	// #pragma HLS INTERFACE m_axi port=quant_code_stream1 depth= 32768

    #pragma HLS INTERFACE m_axi port=in_data depth = kInSize
	#pragma HLS INTERFACE m_axi port=freq_stream depth= 1024

    // #pragma HLS ARRAY_PARTITION variable = hist0 dim = 1 complete
    // #pragma HLS ARRAY_PARTITION variable = hist1 dim = 1 complete
    #pragma HLS RESOURCE variable=hist0 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist1 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist2 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist3 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist4 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist5 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist6 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist7 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist8 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist9 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist10 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist11 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist12 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist13 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist14 core=RAM_T2P_BRAM 
    #pragma HLS RESOURCE variable=hist15 core=RAM_T2P_BRAM 

    MemToStream0(in_data, quant_code_stream);
    HistogramMap(quant_code_stream[0], hist0);
    HistogramMap(quant_code_stream[1], hist1);
    HistogramMap(quant_code_stream[2], hist2);
    HistogramMap(quant_code_stream[3], hist3);
    HistogramMap(quant_code_stream[4], hist4);
    HistogramMap(quant_code_stream[5], hist5);
    HistogramMap(quant_code_stream[6], hist6);
    HistogramMap(quant_code_stream[7], hist7);
    HistogramMap(quant_code_stream[8], hist8);
    HistogramMap(quant_code_stream[9], hist9);
    HistogramMap(quant_code_stream[10], hist10);
    HistogramMap(quant_code_stream[11], hist11);
    HistogramMap(quant_code_stream[12], hist12);
    HistogramMap(quant_code_stream[13], hist13);
    HistogramMap(quant_code_stream[14], hist14);
    HistogramMap(quant_code_stream[15], hist15);

    HistogramReduce(hist0, hist1, hist2, hist3, hist4, hist5, hist6, hist7, hist8, hist9, hist10, hist11, hist12, hist13, hist14, hist15, freq_stream);
}
