#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include "sz_hls.h"

#define GMEM_DWIDTH 512
#define PARALLEL_ENGINE 1
#define GMEM_BURST_SIZE 4
#define GMEM_SIZE (1024 * 64 * 4 / 64)
#define HOST_CHUNK_SIZE (1024 * 64 * 4)

typedef ap_uint<GMEM_DWIDTH> data_t;
const uint8_t kNumBytes = GMEM_DWIDTH / 8;

int main(int argc, char* argv[]) {
  std::string inputFileName = argv[1];
  std::string outputFileName = argv[2];

  // inputFileName = "C:\\Users\\Bizon\\Desktop\\all_hls_projects\\sz_hls4_0\\sample-cesm-CLDHGH";
  inputFileName = "C:\\Users\\Bizon\\Desktop\\all_hls_projects\\sz_hls4_0\\64_1024.bin";

  // File Handling
  // std::fstream inFile;
  // inFile.open(inputFileName.c_str(), std::fstream::binary | std::fstream::in);
  // if (!inFile.is_open()) {
  //     std::cout << "Cannot open the input file!!" << inputFileName << std::endl;
  //     exit(0);
  // }

  std::ifstream inFile;
  inFile.open(inputFileName.c_str(), std::ios::binary | std::ios::in);
  if (!inFile.is_open()) {
      std::cout << "Cannot open the input file!!" << inputFileName << std::endl;
      exit(0);
  }

  std::ofstream outFile;
  outputFileName = "C:\\Users\\Bizon\\Desktop\\all_hls_projects\\fixed_table\\inter_data\\out_data.txt";
  // outFile.open(outputFileName.c_str(), std::fstream::binary | std::fstream::out);
  outFile.open(outputFileName.c_str());

  inFile.seekg(0, std::ios::end); // reaching to end of file
  uint32_t input_size = (uint32_t)inFile.tellg();
  std::cout << "data size: " << input_size << std::endl;
  input_size = GMEM_SIZE * kNumBytes;
  std::cout << "process size: " << input_size << std::endl;

  data_t* source_in = new data_t[GMEM_SIZE];
  data_t* source_out = new data_t[GMEM_SIZE];
  ap_uint<kOutWidth>* out_data = new ap_uint<kOutWidth>[kOutSize];

  for (int i = 0; i < GMEM_SIZE; i++) {
    source_in[i] = 0;
    source_out[i] = 0;
  }

  inFile.seekg(0, std::ios::beg);
  int index = 0;
  for (uint64_t i = 0; i < input_size; i += kNumBytes) {
    data_t x = 0;
    inFile.read((char*)&x, kNumBytes);
    source_in[index++] = x;
  }

  uint32_t blkCalls = (input_size - 1) / HOST_CHUNK_SIZE + 1;
  uint32_t blkSourceStride = HOST_CHUNK_SIZE / kNumBytes;
  uint32_t blkStride = HOST_CHUNK_SIZE / PARALLEL_ENGINE / kNumBytes;
  uint32_t readSize = 0, idxCntr = 0;

  for (uint32_t i = 0; i < blkCalls; i++) {
    uint32_t pSize = HOST_CHUNK_SIZE;
    if (readSize + pSize > input_size) pSize = input_size - readSize;
    readSize += pSize;
    sz_hls(&source_in[blkSourceStride * i], out_data); 
  }

  for (int i = 0; i < kOutSize; i++) {
    //outFile << std::bitset<sizeof(unsigned int)*8>(out_data[i]) << "\n";
    outFile << out_data[i] << "\n";

    if (i < 128) {
      std::cout << out_data[i] << std::endl;    
    }
  } 

  outFile.close();
  inFile.close();
  std::cout << "finish simulation..." << std::endl;
  return 0;
}


