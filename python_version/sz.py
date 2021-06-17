# 
# Python version of SZ compression algorithm
# 
# Usage: python sz.py input_file output_file
# 

import sys
import os
import numpy as np
import math
import contextlib
import collections
import huffmancoding
import utils
import random

# interact --gres=gpu:v100-16:1 --partition GPU-shared -t 00:30:00
# /jet/home/czhang82/cusz/script/sh.parse-nvprof-log
# nvprof /jet/home/czhang82/cusz/bin/cusz -t f32 -m r2r -e 1e-4 -i /jet/home/czhang82/p_data/CESM_AEROD_v_1_1800_3600.dat_16_16.bin -l 16,16 -z

# Command line main application function.
def main(args):
    # Handle command line arguments
    if len(args) != 2:
        sys.exit("Usage: python huffman-compress.py input_file output_file")
    input_file, output_file = args
    file_path = 'C:\\Users\\Bizon\\Desktop\\all_hls_projects\\s1\\sample-cesm-CLDHGH'

    # valid_size = 512
    # symbol_freqs = [0] * 1024
    # symbol_freqs[0:valid_size] = range(1, valid_size + 1)
    # utils.save_code_freq(symbol_freqs, 'xx')

    # HACC
    # dataset = 'HACC'
    # num_dims = 1
    # dim0 = 280953867
    # dim1 = 0
    # dim2 = 0
    # data_type = np.float32
    # ext = '.f32'
    # file_dir = 'C:\\Users\\Bizon\\Desktop\\chengming\\data_com\\HACC\\280953867\\'
    # # file_name = 'vx.f32'
    # # file_path = 'C:\\Users\\Bizon\\Desktop\\data_com\\HACC\\280953867\\vx.f32'
    # # utils.split_file(file_path, 'dat', data_type, 2)
    # load_eb = 0
    # eb_dict_obj = 'HACC_eb_dict_10.5'
    # all_freq_obj = 'HACC_all_freq_10.5'

    # Hurricane CLOUD
    # dataset = 'Hurr'
    # num_dims = 3
    # dim0 = 500
    # dim1 = 500
    # dim2 = 100
    # data_type = np.float32
    # ext = '.bin'
    # # step = '01'
    # # file_name = f'CLOUDf{step}.bin'
    # file_dir = 'C:\\Users\\Bizon\\Desktop\\data_com\\HURR\\48\\'
    # load_eb = 0
    # eb_dict_obj = 'Hurr_eb_dict_10.5'
    # all_freq_obj = 'Hurr_all_freq_10.5'
    #
    # # sian: uf48.bin.f32  eb 0.1 2 5
    # ext = '.f32'
    # file_dir = 'C:\\Users\\Bizon\\Desktop\\data_com\\Hurr_100x500x500\\'

    # Brown
    # dataset = 'Brown'
    # num_dims = 1
    # dim0 = 33554433
    # dim1 = 0
    # dim2 = 0
    # data_type = np.float64
    # ext = '.dat'
    # file_dir = 'C:\\Users\\Bizon\\Desktop\\chengming\\data_com\\Brown\\'
    # # file_path = 'C:\\Users\\Bizon\\Desktop\\data_com\\Brown\\sample_r_B_0.5_26.dat'
    # # utils.split_file(file_path, 'dat', data_type, 2)
    # load_eb = 0
    # eb_dict_obj = 'Brown_eb_dict_21'
    # all_freq_obj = 'Brown_all_freq_21'

    # NWChem
    # dataset = 'NWChem'
    # num_dims = 1
    # dim0 = 539016059
    # dim1 = 0
    # dim2 = 0
    # data_type = np.float64
    # ext = '.d64'
    # file_dir = 'C:\\Users\\Bizon\\Desktop\\chengming\\data_com\\SDRBENCH-NWChem-dataset\\fields\\'
    # load_eb = 0
    # eb_dict_obj = 'NWChem_eb_dict_21'
    # all_freq_obj = 'NWChem_all_freq_21'
    # #utils.split_NWChem(file_dir, ext, data_type, 3)

    # CESM
    # dataset = 'CESM'
    # num_dims = 2
    # dim0 = 3600
    # dim1 = 1800
    # dim2 = 0
    # data_type = np.float32
    # ext = '.dat'
    # file_dir = 'C:\\Users\\Bizon\\Desktop\\chengming\\data_com\\CESM\\1800x3600\\'
    # load_eb = 0
    # eb_dict_obj = 'CESM_eb_dict_10.5'
    # all_freq_obj = 'CESM_all_freq_10.5'
    # files = ['AEROD_v_1_1800_3600.dat', 'CLDHGH_1_1800_3600.dat', 'CLDLOW_1_1800_3600.dat', 'CLDMED_1_1800_3600.dat', 'CLDTOT_1_1800_3600.dat']

    # S3D
    dataset = 'S3D'
    num_dims = 1
    dim0 = 125000000
    dim1 = 0
    dim2 = 0
    data_type = np.float64
    ext = '.mpi'
    file_dir = 'C:\\Users\\Bizon\\Desktop\\chengming\\data_com\\S3D\\fields\\'
    # file_dir = 'C:\\Users\\Bizon\\Desktop\\data_com\\S3D\\'
    # file_name = 'stat_planar.1.1000E-03.field.mpi'
    # file_path = f'{file_dir}{file_name}'
    # utils.split_file(file_path, ext, data_type, 11)
    load_eb = 0
    eb_dict_obj = 'S3D_eb_dict_21'
    all_freq_obj = 'S3D_all_freq_21'

    # common part
    files = utils.get_file_l(file_dir, ext)
    eb_dict = utils.load_obj(eb_dict_obj) if load_eb == 1 else {}
    all_freqs = [[0] * 1024 for _ in range(len(files))]
    # ave_freq = utils.calc_ave_freq(all_freq_obj, 1)
    # all_freqs[0][:] = ave_freq
    for i, file_name in enumerate(files):
        # print(file_name)
        # if i != 0:
        #     continue

        file_path = f'{file_dir}{file_name}'
        # utils.save_bin_file(dataset, file_name, file_path, data_type, 2, dim0, dim1, 0, 256, 256, 0)
        range_v = utils.get_one_range(file_path, data_type, ext)
        print(f'\ndata range is {range_v}')
        abs_eb = 0.0001
        rel_eb = abs_eb * range_v
        if load_eb == 1:
            rel_eb = eb_dict[file_name]
        else:
            eb_dict[file_name] = rel_eb

        raw_arr, outlier = sz_compress(dataset, file_path, file_name, num_dims, dim0, dim1, dim2, 20, 0, 0, 512, abs_eb, rel_eb, data_type, eb_dict, i, all_freqs)
        # x_data = sz_decompress(file_path, file_name, outlier, num_dims, dim0, dim1, dim2, 1, 1, 1, 512, rel_eb)

    # utils.save_obj(eb_dict, eb_dict_obj)
    # utils.save_obj(all_freqs, all_freq_obj)

    # f_comp = open('./inter_data/over_eb.txt', "w")
    # for j in range(dim0 * dim1 * dim2):
    #     if abs(raw_arr[j] - x_data[j]) > rel_eb:
    #         f_comp.write(f'{j} {raw_arr[j]} {x_data[j]}\n')
    # f_comp.close()

    # rmse, nrmse, psnr = measure_metrics(raw_arr, x_data)
    # print(f'rmse is {rmse}, nrmse is {nrmse}, psnr is {psnr}')


def measure_metrics(ori_arr, de_arr):
    ori_arr = np.array(ori_arr)
    de_arr = np.array(de_arr)
    arr_len = len(ori_arr)
    ori_max = np.max(ori_arr)
    ori_min = np.min(ori_arr)
    root_mean_squared_error = np.sqrt((1 / arr_len) * np.sum(np.square(ori_arr - de_arr)))
    normalized_root_mean_squared_error = root_mean_squared_error / (ori_max - ori_min)
    peak_signal_to_noise_ratio = 20 * np.log10((ori_max - ori_min) / root_mean_squared_error)
    return root_mean_squared_error, normalized_root_mean_squared_error, peak_signal_to_noise_ratio


def config_sz(dim0, dim1, dim2, num_blks0, num_blks1, num_blks2, radius, eb):
    dims = [dim0, dim1, dim2, num_blks0, num_blks1, num_blks2]  # 5

    def calc_base_addr(dim, num_blks, dims):
        if dim == 0:
            dims.append([])
            dims.append([])
            return

        per_blk = [0] * num_blks
        base_addr = [0] * num_blks
        ave_rows = (dim - 1) // num_blks + 1
        total_rows = 0
        for b in range(num_blks):
            base_addr[b] = total_rows
            if total_rows + ave_rows <= dim:
                per_blk[b] = ave_rows
                total_rows += ave_rows
            else:
                per_blk[b] = dim - total_rows

        dims.append(per_blk)
        dims.append(base_addr)

    calc_base_addr(dim0, num_blks0, dims)  # 6, 7
    calc_base_addr(dim1, num_blks1, dims)  # 8, 9
    calc_base_addr(dim2, num_blks2, dims)  # 10, 11
    dims.append(radius)  # 12

    eb_variants = [0.0] * 4
    eb_variants[0] = eb
    eb_variants[1] = 1 / eb
    eb_variants[2] = eb * 2
    eb_variants[3] = 1 / (eb * 2)

    return dims, eb_variants


def sz_compress(dataset, file_path, file_name, num_dims, dim0, dim1, dim2, num_blks0, num_blks1, num_blks2, radius, abs_eb, rel_eb, data_type, eb_dict, i, all_freqs):
    print(f'start compress {file_name} .....')

    dims, eb_variants = config_sz(dim0, dim1, dim2, num_blks0, num_blks1, num_blks2, radius, rel_eb)

    raw_arr = np.fromfile(file_path, dtype=data_type, count=-1)
    print(f'data size is {raw_arr.size}')
    # raw_arr = np.fromfile(file_path, dtype='>f', count=-1, offset=0)
    # raw_arr = np.where(raw_arr > 4096, 0.0, raw_arr)
    max_v = np.max(raw_arr)
    min_v = np.min(raw_arr)
    outlier = collections.defaultdict(int)
    # outlier = {}
    std_file = f'./inter_data/std_{dataset}.txt'
    ratio_file = f'./inter_data/ratio_{dataset}.txt'

    if num_dims == 1:
        built_tree = [0] * num_blks0
        built_tree[0] = 0
        num_blks1 = 1
        partial = False
        save_idx = 0
        for b0 in range(num_blks0):
            code_freq, quant_code = c_lorenzo_1d1l(file_path, file_name, raw_arr, outlier, b0, dims, eb_variants, partial, dim0)
            all_freqs[i][:] = code_freq[:]
            # code_freq[:] = all_freqs[0][:]
            # utils.calc_std(code_freq, std_file, b0)

            # if built_tree[b0]:
            #     code_freq_obj = f'{file_name}_{b0}'
            #     utils.save_obj(code_freq, code_freq_obj)
            #     save_idx = b0
            # else:
            #     code_freq_obj = f'{file_name}_{save_idx}'
            #     code_freq = utils.load_obj(code_freq_obj)

            if built_tree[b0] == 1:
                # print('save')
                code_freq_obj = f'{dataset}_code_freq'
                utils.save_obj(code_freq, code_freq_obj)
            else:
                # print('load')
                code_freq_obj = f'{dataset}_code_freq'
                code_freq = utils.load_obj(code_freq_obj)

            codeword_refer = f'./inter_data/{file_name}_codeword_refer_{b0}.txt'
            output_file = f'{file_path}_{b0}_0.sz'
            huffman_compress(code_freq, quant_code, codeword_refer, output_file)

            utils.record_ratio(ratio_file, output_file, len(quant_code), data_type, b0)

    if num_dims == 2:
        ori_arr = np.reshape(raw_arr, (dims[1], dims[0]))
        built_tree = [[0] * num_blks0 for _ in range(num_blks1)]
        built_tree[0][0] = 0
        partial = False
        save_idx = [0, 0]
        for b1 in range(num_blks1):
            for b0 in range(num_blks0):
                code_freq, quant_code = c_lorenzo_2d1l(file_path, file_name, ori_arr, outlier, b0, b1, dims, eb_variants, partial, dim0, dim1)
                # all_freqs[i][:] = code_freq[:]
                # code_freq[:] = all_freqs[0][:]
                # utils.calc_std(code_freq, std_file, b0 + b1)
                # if built_tree[b1][b0]:
                #     code_freq_obj = f'{file_name}_{b0}_{b1}'
                #     utils.save_obj(code_freq, code_freq_obj)
                #     save_idx[:] = [b0, b1]
                # else:
                #     code_freq_obj = f'{file_name}_{save_idx[0]}_{save_idx[1]}'
                #     code_freq = utils.load_obj(code_freq_obj)

                if built_tree[b1][b0] == 1:
                    # print('save')
                    code_freq_obj = f'{dataset}_code_freq'
                    utils.save_obj(code_freq, code_freq_obj)
                else:
                    # print('load')
                    code_freq_obj = f'{dataset}_code_freq'
                    code_freq = utils.load_obj(code_freq_obj)

                codeword_refer = f'./inter_data/{file_name}_codeword_refer_{b0}_{b1}.txt'
                output_file = f'{file_path}_{b0}_{b1}.sz'
                huffman_compress(code_freq, quant_code, codeword_refer, output_file)

                utils.record_ratio(ratio_file, output_file, len(quant_code), data_type, b0 + b1)

    if num_dims == 3:
        # ori_arr = np.reshape(raw_arr, (dims[2], dims[1], dims[0]))
        ori_arr = raw_arr
        built_tree = np.zeros((num_blks2, num_blks1, num_blks0), dtype=int)
        partial = False
        for b2 in range(num_blks2):
            for b1 in range(num_blks1):
                for b0 in range(num_blks0):
                    code_freq, quant_code = c_lorenzo_3d1l(file_path, file_name, ori_arr, outlier, b0, b1, b2, dims, eb_variants, partial, dim0, dim1, dim2)
                    built_tree[b2, b1, b0] = 1

                    codeword_refer = f'./inter_data/{file_name}_codeword_refer_{b0}_{b1}.txt'
                    output_file = f'{file_path}_{b0}_{b1}.sz'
                    huffman_compress(code_freq, quant_code, codeword_refer, output_file)

    o_size = os.stat(file_path).st_size
    c_size = 0
    for b1 in range(num_blks1):
        for b0 in range(num_blks0):
            output_file = f'{file_path}_{b0}_{b1}.sz'
            c_size += os.stat(output_file).st_size 

    # eb * 2, bitrate - 1
    target_ratio = 10.5
    bit_width = 32 if data_type == np.float32 else 64
    compression_ratio = o_size / c_size
    # if compression_ratio > target_ratio:
    #     eb_dict[file_name] = eb_dict[file_name] / (2 ** round(bit_width / target_ratio - bit_width / compression_ratio + 1))
    # else:
    #     eb_dict[file_name] = eb_dict[file_name] * (2 ** round(bit_width / compression_ratio - bit_width / target_ratio + 1))

    # if compression_ratio > target_ratio:
    #     eb_dict[file_name] = eb_dict[file_name] / (2 ** round(bit_width / target_ratio - bit_width / compression_ratio, 4))
    # else:
    #     eb_dict[file_name] = eb_dict[file_name] * (2 ** round(bit_width / compression_ratio - bit_width / target_ratio, 4))

    print(f'o_size is {o_size} bytes, c_size is {c_size} bytes. compression ratio is {compression_ratio}')
    ratio_f = open(f'./inter_data/{dataset}_ratio_{abs_eb}.txt', 'a')
    # ratio_f.write(f'{file_name}  [{min_v} -- {max_v}]  {eb_variants[0]}  {compression_ratio} {bit_width / compression_ratio} \n')
    ratio_f.write(f'{bit_width / compression_ratio} \n')
    ratio_f.close()
    print('finish compression.....')
    return raw_arr, outlier


def sz_decompress(file_path, file_name, outlier, num_dims, dim0, dim1, dim2, num_blks0, num_blks1, num_blks2, radius, eb):
    print('start decompression.....')
    dims, eb_variants = config_sz(dim0, dim1, dim2, num_blks0, num_blks1, num_blks2, radius, eb)

    if num_dims == 1:
        partial = False
        x_data = [0.0] * dim0
        for b0 in range(num_blks0):
            sz_file = f'{file_path}_{b0}_0.sz'
            de_quant_code = [0] * dims[6][b0]
            decodeword_refer = f'./inter_data/{file_name}_decodeword_refer_{b0}.txt'
            de_freq_file = f'./inter_data/{file_name}_decode_freq_refer_{b0}.txt'
            huffman_decompress(sz_file, de_quant_code, decodeword_refer, de_freq_file)
            x_lorenzo_1d1l(x_data, de_quant_code, outlier, b0, dims, eb_variants, partial, dim0)

    if num_dims == 2:
        partial = False
        x_data = [0.0] * (dim0 * dim1)
        for b1 in range(num_blks1):
            for b0 in range(num_blks0):
                sz_file = f'{file_path}_{b0}_{b1}.sz'
                de_quant_code = [0] * (dims[6][b0] * dims[8][b1])
                decodeword_refer = f'./inter_data/{file_name}_decodeword_refer_{b0}_{b1}.txt'
                de_freq_file = f'./inter_data/{file_name}_decode_freq_refer_{b0}_{b1}.txt'
                huffman_decompress(sz_file, de_quant_code, decodeword_refer, de_freq_file)
                x_lorenzo_2d1l(x_data, de_quant_code, outlier, b0, b1, dims, eb_variants, partial, dim0, dim1)

    if num_dims == 3:
        partial = False
        x_data = [0.0] * (dim0 * dim1 * dim2)
        for b2 in range(num_blks2):    
            for b1 in range(num_blks1):
                for b0 in range(num_blks0):
                    sz_file = f'{file_path}_{b0}_{b1}.sz'
                    de_quant_code = [0] * (dims[6][b0] * dims[8][b1] * dims[10][b2])
                    decodeword_refer = f'./inter_data/{file_name}_decodeword_refer_{b0}_{b1}.txt'
                    de_freq_file = f'./inter_data/{file_name}_decode_freq_refer_{b0}_{b1}.txt'
                    huffman_decompress(sz_file, de_quant_code, decodeword_refer, de_freq_file)
                    x_lorenzo_3d1l(x_data, de_quant_code, outlier, b0, b1, b2, dims, eb_variants, partial, dim0, dim1, dim2)

    print('finish decompression.....')
    return x_data


def histogram(arr, out_file_name):
    freq = [0] * 1024
    for x in arr:
        freq[x] += 1

    f = open(out_file_name, "w")
    for q in freq:
        f.write(f'{q} \n')
    f.close()


def huffman_compress(code_freq, quant_code, codeword_refer, output_file):
    code_freq.append(1)
    freq = huffmancoding.FrequencyTable(code_freq)
    code_tree = freq.build_code_tree()
    # Replace code tree with canonical one. For each symbol, the code value may change but the code length stays the same.
    canonical_code = huffmancoding.CanonicalCode(tree=code_tree, symbollimit=freq.get_symbol_limit())
    code_tree = canonical_code.to_code_tree()
    codes = code_tree.codes

    # f0 = open(codeword_refer, "w")
    # for c in codes:
    #     f0.write(f'{c} \n')
    # f0.close()

    with contextlib.closing(huffmancoding.BitOutputStream(open(output_file, "wb"))) as bit_out:
        write_code_len_table(bit_out, canonical_code)
        compress(code_tree, quant_code, bit_out)


def huffman_decompress(sz_file, de_quant_code, decodeword_refer, de_freq_file):

    with open(sz_file, "rb") as sz_file_in:
        bit_in = huffmancoding.BitInputStream(sz_file_in)
        canonical_code = read_code_len_table(bit_in)
        code_tree = canonical_code.to_code_tree()

        f0 = open(decodeword_refer, "w")
        for c in code_tree.codes:
            f0.write(f'{c} \n')
        f0.close()

        decompress(bit_in, code_tree, de_quant_code)
        histogram(de_quant_code, de_freq_file)


def c_lorenzo_1d1l(file_path, file_name, ori_arr, outlier, b0, dims, eb_variants, partial, dim0):
    per_blk0 = dims[6][b0]
    base_addr0 = dims[7][b0]
    radius = dims[12]

    if not partial:
        dim0 = per_blk0

    qua_buf = [0] * (dim0 + 1)
    quant_code = [0] * dim0
    code_freq = [1] * 1024
    # pre_quant and post_quant
    for i0 in range(dim0):
        g_i0 = base_addr0 + i0
        qua_buf[i0 + 1] = round(ori_arr[g_i0] * eb_variants[3])
        pred = qua_buf[i0]
        pred_err = qua_buf[i0 + 1] - pred
        quantizable = 1 if abs(pred_err) < radius else 0
        code = pred_err + radius
        code_reg = quantizable * code
        quant_code[i0] = code_reg
        code_freq[code_reg] += 1

        if quantizable == 0:
            outlier[g_i0] = qua_buf[i0 + 1]

    # f2 = open("./inter_data/{}_code_freq_refer_{}.txt".format(file_name, b0), "w")
    # for c in code_freq:
    #     f2.write(f'{c} \n')
    # f2.close()

    return code_freq, quant_code


def c_lorenzo_2d1l(file_path, file_name, ori_arr, outlier, b0, b1, dims, eb_variants, partial, dim0, dim1):
    per_blk0 = dims[6][b0]
    per_blk1 = dims[8][b1]
    base_addr0 = dims[7][b0]
    base_addr1 = dims[9][b1]
    radius = dims[12]
    if not partial:
        dim0 = per_blk0
        dim1 = per_blk1

    # f0 = open("./inter_data/{}_ori_data_refer_{}_{}.txt".format(file_name, b0, b1), "w")
    # f1 = open("./inter_data/{}_code_refer_{}_{}.txt".format(file_name, b0, b1), "w")
    # f2 = open("./inter_data/{}_code_freq_refer_{}_{}.txt".format(file_name, b0, b1), "w")
    # f3 = open("./inter_data/{}_debug1_refer_{}_{}.txt".format(file_name, b0, b1), "w")

    # quant_code = [[0] * dim0 for i in range(dim1)]
    quant_code = [0] * (dim0 * dim1)
    code_freq = [1] * 1024
    qua_buf0 = [0] * (dim0 + 1)
    qua_buf1 = [0] * (dim0 + 1)
    for i1 in range(dim1):
        for i0 in range(dim0):
            qua_buf0[i0 + 1] = qua_buf1[i0 + 1]
            g_i0 = base_addr0 + i0
            g_i1 = base_addr1 + i1
            g_idx = g_i1 * dims[0] + g_i0
            pre_qua = ori_arr[g_i1, g_i0] * eb_variants[3]
            qua_buf1[i0 + 1] = round(pre_qua)
            # qua_buf1[i0 + 1] = round_half_up(pre_qua)
            # qua_buf1[i0 + 1] = math.ceil(pre_qua)
            # qua_buf1[i0 + 1] = math.floor(pre_qua)
            pred = qua_buf1[i0] + qua_buf0[i0 + 1] - qua_buf0[i0]
            pred_err = qua_buf1[i0 + 1] - pred
            quantizable = 1 if abs(pred_err) < radius else 0
            code = pred_err + radius
            code_reg = quantizable * code
            b_idx = i1 * dim0 + i0
            quant_code[b_idx] = code_reg
            code_freq[code_reg] += 1

            # if file_name == 'ODV_dust1_1_1800_3600.dat':
            #     f3.write(f'{ori_arr[g_i1, g_i0]}  {qua_buf1[i0 + 1]} {pred_err} \n')

            if quantizable == 0:
                outlier[g_idx] = qua_buf1[i0 + 1]

    #         if i1 * dim0 + i0 == 6837:
    #             print(i1, i0)
    #             print(pre_qua, qua_buf1[i0], qua_buf0[i0 + 1], qua_buf0[i0], qua_buf1[i0 + 1])
    #
    #         f0.write("{}\n".format(ori_arr[g_i1, g_i0]))
    #         f1.write('{}\n'.format(code_reg))
    #
    # f0.close()
    # f1.close()
    # f3.close()

    # freq_percent = [0.0] * 1024
    # total_size = dim0 * dim1
    # for i, c in enumerate(code_freq):
    #     percent = round(c / total_size * 100)
    #     f2.write(f'{percent} \n')
    #     freq_percent[i] = percent
    # f2.close()
    #
    # utils.plot_dot_line(file_name, range(460, 561), freq_percent[460:561])

    # for c in code_freq:
    #     f2.write(f'{c} \n')
    # f2.close()

    return code_freq, quant_code


def c_lorenzo_3d1l(file_path, file_name, ori_arr, outlier, b0, b1, b2, dims, eb_variants, partial, dim0, dim1, dim2):
    per_blk0 = dims[6][b0]
    per_blk1 = dims[8][b1]
    per_blk2 = dims[10][b2]

    base_addr0 = dims[7][b0]
    base_addr1 = dims[9][b1]
    base_addr2 = dims[11][b2]

    radius = dims[12]
    if not partial:
        dim0 = per_blk0
        dim1 = per_blk1
        dim2 = per_blk2

    qua_buf = np.zeros((per_blk2+1, per_blk1+1, per_blk0+1), dtype=int)
    quant_code = [0] * (dim0 * dim1 * dim2)
    code_freq = [1] * 1024
    # pre_quant
    for i2 in range(dim2):
        for i1 in range(dim1):
            for i0 in range(dim0):
                g_i2 = base_addr2 + i2
                g_i1 = base_addr1 + i1
                g_i0 = base_addr0 + i0
                g_idx = g_i0 + g_i1 * dims[0] + g_i2 * dims[1] * dims[0]
                qua_buf[i2 + 1, i1 + 1, i0 + 1] = round(ori_arr[g_idx] * eb_variants[3])

    # post_quant
    for i2 in range(dim2):
        for i1 in range(dim1):
            for i0 in range(dim0):
                g_i2 = base_addr2 + i2
                g_i1 = base_addr1 + i1
                g_i0 = base_addr0 + i0
                g_idx = g_i0 + g_i1 * dims[0] + g_i2 * dims[1] * dims[0]

                # + dist=3, - dist=2, + dist=1
                pred = qua_buf[i2, i1, i0] - qua_buf[i2 + 1, i1, i0] - qua_buf[i2, i1 + 1, i0] - qua_buf[i2, i1, i0 + 1] + qua_buf[i2 + 1, i1 + 1, i0] + qua_buf[i2 + 1, i1, i0 + 1] + qua_buf[i2, i1 + 1, i0 + 1]
                pred_err = qua_buf[i2 + 1, i1 + 1, i0 + 1] - pred
                quantizable = 1 if abs(pred_err) < radius else 0
                code = pred_err + radius
                code_reg = quantizable * code
                b_idx = i0 + i1 * dim0 + i2 * dim1 * dim0
                quant_code[b_idx] = code_reg
                code_freq[code_reg] += 1

                if quantizable == 0:
                    outlier[g_idx] = qua_buf[i2 + 1, i1 + 1, i0 + 1]

    f2 = open("./inter_data/{}_code_freq_refer_{}_{}.txt".format(file_name, b0, b1), "w")
    for c in code_freq:
        f2.write('{}\n'.format(c))
    f2.close()

    return code_freq, quant_code


def x_lorenzo_1d1l(x_data, de_quant_code, outlier, b0, dims, eb_variants, partial, dim0):
    per_blk0 = dims[6][b0]
    base_addr0 = dims[7][b0]
    radius = dims[12]

    if not partial:
        dim0 = per_blk0

    qua_buf = [0] * (dim0 + 1)
    for i0 in range(dim0):
        g_i0 = base_addr0 + i0
        pred = qua_buf[i0]
        qua_buf[i0 + 1] = outlier[g_i0] if de_quant_code[i0] == 0 else pred + (de_quant_code[i0] - radius)
        x_data[g_i0] = qua_buf[i0 + 1] * eb_variants[2]


def x_lorenzo_2d1l(x_data, de_quant_code, outlier, b0, b1, dims, eb_variants, partial, dim0, dim1):
    per_blk0 = dims[6][b0]
    per_blk1 = dims[8][b1]
    base_addr0 = dims[7][b0]
    base_addr1 = dims[9][b1]
    radius = dims[12]

    if not partial:
        dim0 = per_blk0
        dim1 = per_blk1

    qua_buf0 = [0] * (dim0 + 1)
    qua_buf1 = [0] * (dim0 + 1)
    for i1 in range(dim1):
        for i0 in range(dim0):
            qua_buf0[i0 + 1] = qua_buf1[i0 + 1]
            g_i0 = base_addr0 + i0
            g_i1 = base_addr1 + i1
            b_idx = i1 * dim0 + i0
            g_idx = g_i1 * dims[0] + g_i0

            pred = qua_buf1[i0] + qua_buf0[i0 + 1] - qua_buf0[i0]
            qua_buf1[i0 + 1] = outlier[g_idx] if de_quant_code[b_idx] == 0 else pred + (de_quant_code[b_idx] - radius)
            x_data[g_idx] = qua_buf1[i0 + 1] * eb_variants[2]


def x_lorenzo_3d1l(x_data, de_quant_code, outlier, b0, b1, b2, dims, eb_variants, partial, dim0, dim1, dim2):

    per_blk0 = dims[6][b0]
    per_blk1 = dims[8][b1]
    per_blk2 = dims[10][b2]

    base_addr0 = dims[7][b0]
    base_addr1 = dims[9][b1]
    base_addr2 = dims[11][b2]

    radius = dims[12]
    if not partial:
        dim0 = per_blk0
        dim1 = per_blk1
        dim2 = per_blk2

    qua_buf = np.zeros((per_blk2+1, per_blk1+1, per_blk0+1), dtype=int)
    for i2 in range(dim2):
        for i1 in range(dim1):
            for i0 in range(dim0):
                g_i2 = base_addr2 + i2
                g_i1 = base_addr1 + i1
                g_i0 = base_addr0 + i0
                b_idx = i0 + i1 * dim0 + i2 * dim1 * dim0
                g_idx = g_i0 + g_i1 * dims[0] + g_i2 * dims[1] * dims[0]

                # + dist=3, - dist=2, + dist=1
                pred = qua_buf[i2, i1, i0] - qua_buf[i2 + 1, i1, i0] - qua_buf[i2, i1 + 1, i0] - qua_buf[i2, i1, i0 + 1] + qua_buf[i2 + 1, i1 + 1, i0] + qua_buf[i2 + 1, i1, i0 + 1] + qua_buf[i2, i1 + 1, i0 + 1]
                qua_buf[i2 + 1, i1 + 1, i0 + 1] = outlier[g_idx] if de_quant_code[b_idx] == 0 else pred + (de_quant_code[b_idx] - radius)
                x_data[g_idx] = qua_buf[i2 + 1, i1 + 1, i0 + 1] * eb_variants[2]
            

def write_code_len_table(bit_out, canonical_code):
    for i in range(canonical_code.get_symbol_limit()):
        val = canonical_code.get_code_length(i)
        # For this file format, we only support codes up to 255 bits long
        if val >= 256:
            raise ValueError("The code for a symbol is too long")

        # Write value as 8 bits in big endian
        for j in reversed(range(8)):
            bit_out.write((val >> j) & 1)


def compress(code_tree, quant_code, bit_out):
    enc = huffmancoding.HuffmanEncoder(bit_out)
    enc.codetree = code_tree

    for q_c in quant_code:
        enc.write(q_c)

    enc.write(1024)  # EOF


def read_code_len_table(bit_in):
    def read_int(n):
        result = 0
        for _ in range(n):
            result = (result << 1) | bit_in.read_no_eof()  # Big endian
        return result

    codelengths = [read_int(8) for _ in range(1025)]
    return huffmancoding.CanonicalCode(codelengths=codelengths)


def decompress(bit_in, code_tree, x_arr):
    dec = huffmancoding.HuffmanDecoder(bit_in)
    dec.codetree = code_tree
    b_idx = 0
    while True:
        symbol = dec.read()
        if symbol == 1024:  # EOF symbol
            break

        x_arr[b_idx] = symbol
        b_idx += 1


# Main launcher
if __name__ == "__main__":
    main(sys.argv[1:])
