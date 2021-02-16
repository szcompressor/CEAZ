# 
# Python version of SZ compression algorithm
# 
# Usage: python sz.py input_file output_file
# 

import contextlib
import math
import sys
import numpy as np

import huffmancoding


# Command line main application function.
def main(args):
    # Handle command line arguments
    if len(args) != 2:
        sys.exit("Usage: python huffman-compress.py input_file output_file")
    input_file, output_file = args

    # Read input file once to compute symbol frequencies.
    # The resulting generated code is optimal for static Huffman coding and also canonical.
    bin_file = 'C:\\Users\\Bizon\\Desktop\\s1\\sample-cesm-CLDHGH'
    output_file = 'sample-cesm-CLDHGH.sz'
    # save_bin_file(bin_file, 1024, 64)
    code_freq, dual_quant_code = dual_quant(bin_file, 1024, 64)

    test_arr = [3, 1, 2, 5, 5, 1, 5, 7, 6, 0]
    test_freq = [0] * 1024
    test_freq[0:10] = test_arr[:]

    # freqs = get_frequencies(input_file)
    # freqs.increment(256)  # EOF symbol gets a frequency of 1
    freqs = huffmancoding.FrequencyTable(code_freq)
    code = freqs.build_code_tree()

    # Replace code tree with canonical one. For each symbol,
    # the code value may change but the code length stays the same.
    canoncode = huffmancoding.CanonicalCode(tree=code, symbollimit=freqs.get_symbol_limit())
    code = canoncode.to_code_tree()
    codes = code.codes

    f = open("./inter_data/codeword_refer.txt", "w")
    for c in codes:
        c_str = '{}'.format(c)
        f.write(c_str)
        f.write('\n')
    f.close()

    # Read input file again, compress with Huffman coding, and write output file
    # with open(input_file, "rb") as inp, \
    #         contextlib.closing(huffmancoding.BitOutputStream(open(output_file, "wb"))) as bit_out:
    #     write_code_len_table(bit_out, canoncode)
    #     compress(code, inp, bit_out)

    with contextlib.closing(huffmancoding.BitOutputStream(open(output_file, "wb"))) as bit_out:
        write_code_len_table(bit_out, canoncode)
        compress(code, dual_quant_code, 1024, 64, bit_out)


def round_half_up(n, decimals=0):
    multiplier = 10 ** decimals
    half_up = math.floor(n * multiplier + 0.5) / multiplier
    # return int(half_up)
    return half_up


def save_bin_file(file, dim0, dim1):
    dims_l16 = [3600, 1800, 1, 1, 225, 113, 1, 1, 2, 0, 0, 0, 6480000, 1024, 512, 0]
    raw_arr = np.fromfile(file, dtype=np.float32, count=-1)
    ori_arr = np.reshape(raw_arr, (dims_l16[1], dims_l16[0]))
    n_arr = ori_arr[0:dim1, 0:dim0]
    n_arr = n_arr.astype('float32')
    print(np.shape(n_arr))
    bin_name = 'C:\\Users\\Bizon\\Desktop\\sz_hls3\\{}_{}.bin'.format(len(n_arr), len(n_arr[0]))
    print('save file: {}:'.format(bin_name))
    n_arr.tofile(bin_name)


def dual_quant(filepath, dim0, dim1):
    # dims_l16
    DIM0 = 0
    DIM1 = 1
    DIM2 = 2
    DIM3 = 3
    nBLK0 = 4
    nBLK1 = 5
    nBLK2 = 6
    nBLK3 = 7
    nDIM = 8
    LEN = 12
    CAP = 13
    RADIUS = 14

    # ebs_l4
    EB = 0
    EBr = 1
    EBx2 = 2
    EBx2_r = 3

    dims_l16 = [3600, 1800, 1, 1, 225, 113, 1, 1, 2, 0, 0, 0, 6480000, 1024, 512, 0]
    ebs_l4 = [0.000089401054382324226, 11185.550404400077, 0.00017880210876464845, 5592.7752022000386]
    raw_arr = np.fromfile(filepath, dtype=np.float32, count=-1)
    ori_arr = np.reshape(raw_arr, (dims_l16[DIM1], dims_l16[DIM0]))

    num_blks = 1
    rows_per_blk = [0] * num_blks
    base_addr = [0] * num_blks
    ave_rows = (dim1 - 1) // num_blks + 1
    total_rows = 0
    for b in range(num_blks):
        base_addr[b] = total_rows
        if total_rows + ave_rows <= dim1:
            rows_per_blk[b] = ave_rows
            total_rows += ave_rows
        else:
            rows_per_blk[b] = dim1 - total_rows

    dual_quant_code = [[0] * dim0] * dim1
    code_freq = [0] * 1024
    f2 = open("./inter_data/code_freq_refer.txt", "w")

    for i3 in range(0, num_blks):
        pre_qua_buf0 = [0] * (dim0 + 1)
        pre_qua_buf1 = [0] * (dim0 + 1)
        f0 = open("./inter_data/ori_data_refer" + str(i3) + ".txt", "w")
        f1 = open("./inter_data/code_refer" + str(i3) + ".txt", "w")

        for i1 in range(0, rows_per_blk[i3]):
            for i0 in range(0, dim0):
                pre_qua_buf0[i0 + 1] = pre_qua_buf1[i0 + 1]
                g_i1 = base_addr[i3] + i1
                pre_qua_reg = ori_arr[g_i1][i0] * ebs_l4[EBx2_r]

                # pre_qua_buf1[i0 + 1] = int(round(pre_qua_reg))
                # pre_qua_buf1[i0 + 1] = int(round_half_up(pre_qua_reg))
                # pre_qua_buf1[i0 + 1] = int(math.ceil(pre_qua_reg))
                pre_qua_buf1[i0 + 1] = int(math.floor(pre_qua_reg))
                pred = pre_qua_buf1[i0] + pre_qua_buf0[i0 + 1] - pre_qua_buf0[i0]
                post_err = pre_qua_buf1[i0 + 1] - pred
                quantizable = abs(post_err) < dims_l16[RADIUS]
                _code = post_err + dims_l16[RADIUS]
                # data[id] = (1 - quantizable) * pre_qua_buf1[i0+1]  // data array as outlier
                code_reg = quantizable * _code
                dual_quant_code[g_i1][i0] = code_reg
                code_freq[code_reg] += 1

                if i1 * 1024 + i0 == 6837:
                    print(i1, i0)
                    print(pre_qua_reg, pre_qua_buf1[i0], pre_qua_buf0[i0 + 1], pre_qua_buf0[i0], pre_qua_buf1[i0 + 1])

                f0.write("{}\n".format(ori_arr[g_i1][i0]))
                f1.write('{}\n'.format(code_reg))

        f0.close()
        f1.close()

    for c in code_freq:
        f2.write('{}\n'.format(c))
    f2.close()

    return code_freq, dual_quant_code


def save_rand_freq(freq_arr):
    f = open("C:\\Users\\Bizon\\Desktop\\s4\\freq_arr.h", "w")
    f.write('#ifndef _FREQ_ARR_H_\n')
    f.write('#define _FREQ_ARR_H_')
    f.write('\n')
    f.write('const uint32_t freq_arr[1024] = {')
    fr_str = ''
    flag = 0
    for fr in freq_arr:
        if flag == 0:
            flag = 1
            fr_str = '{}'.format(fr)
        else:
            fr_str = '{}, {}'.format(fr_str, fr)
    f.write('\n')
    f.write(fr_str)
    f.write('};\n')
    f.write('#endif\n')
    f.close()


# Returns a frequency table based on the bytes in the given file.
# Also contains an extra entry for symbol 256, whose frequency is set to 0.
def get_frequencies(filepath):
    # freqs = huffmancoding.FrequencyTable([0] * 257)
    # with open(filepath, "rb") as input:
    # 	while True:
    # 		b = input.read(1)
    # 		if len(b) == 0:
    # 			break
    # 		freqs.increment(b[0])
    # return freqs
    rand_arr = np.random.randint(100, size=1024)
    # freq_arr = [7, 5, 4, 8, 12, 10, 20]
    freq_arr = rand_arr.tolist()
    freqs = huffmancoding.FrequencyTable(freq_arr)
    save_rand_freq(freq_arr)

    return freqs


def write_code_len_table(bitout, canoncode):
    for i in range(canoncode.get_symbol_limit()):
        val = canoncode.get_code_length(i)
        # For this file format, we only support codes up to 255 bits long
        if val >= 256:
            raise ValueError("The code for a symbol is too long")

        # Write value as 8 bits in big endian
        for j in reversed(range(8)):
            bitout.write((val >> j) & 1)


def compress(code, dual_quant_code, dim0, dim1, bit_out):
    enc = huffmancoding.HuffmanEncoder(bit_out)
    enc.codetree = code

    for i1 in range(0, dim1):
        for i0 in range(0, dim0):
            c = dual_quant_code[i1][i0]
            enc.write(c)
            # enc.write(256)  # EOF


# Main launcher
if __name__ == "__main__":
    main(sys.argv[1:])
