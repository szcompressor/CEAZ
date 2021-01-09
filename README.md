FPGA-Accelerated Error-Bounded Lossy Compression
---

Nowadays, many different tasks such as artificial intelligence, deep learning, graph analysis, and experimental analysis applications need to be simultaneously executed and managed along with the main simulation tasks in the supercomputer, all of which often generate huge amounts of scientific data that must be transferred for in situ processing or post analysis. 

To alleviate the network traffic and storage overhead, data reduction is necessarily needed by HPC in leadership computing facilities or even edge computing in experimental and observational facilities. During the past five years, [SZ compression algorithm](http://szcompressor.org/) has gained much attention as a powerful data reduction technique because of its high reduction capability. However, it suffers from low throughput and high resource utilization, which impedes its adoption in many scenarios that require high-rate streaming data or use low-power embedded processors. 

FPGA, featuring the capabilities of configurability, high throughput, low latency, and high energy efficiency, can provide a potentially good solution to these issues. This project is to optimize and implement an FPGA-enhanced lossy compression for better scientific data management, which can be potentially integrated into [computational storage devices](https://www.snia.org/computational) such as Eideticom’s [NoLoad® Computational Storage Processor](https://www.eideticom.com/uploads/images/NoLoad_U2_Computational_Storage_Product_Brief.pdf). 

Preliminary result has been published in [PPoPP'20](https://dl.acm.org/doi/10.1145/3332466.3374525).

This project is supported by [Xilinx](https://www.xilinx.com/) and [UChicago Argonne](https://www.uchicagoargonnellc.org). 

(C) 2020 by Washington State University and Argonne National Laboratory. See [COPYRIGHT](https://github.com/szcompressor/SZ_HLS/blob/main/LICENSE) in top-level directory.

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

Developers: Chengming Zhang, Jiannan Tian, Dingwen Tao

Contributors (alphabetic): Franck Cappello, Sheng Di, Xin Liang
