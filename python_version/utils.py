import contextlib
import math
import sys
import os
import numpy as np
import pickle
from scipy.integrate import quad
import matplotlib
import matplotlib.pyplot as plt
import huffmancoding


def round_half_up(n, decimals=0):
    multiplier = 10 ** decimals
    half_up = math.floor(n * multiplier + 0.5) / multiplier
    # return int(half_up)
    return half_up


def save_bin_file(dataset, file_name, file_path, data_type, num_dims, ori_dim0, ori_dim1, ori_dim2, crop_dim0, crop_dim1, crop_dim2):
    raw_arr = np.fromfile(file_path, dtype=data_type, count=-1)

    if num_dims == 1:
        p_arr0 = raw_arr[0:crop_dim1]
    elif num_dims == 2:
        ori_arr = np.reshape(raw_arr, (ori_dim1, ori_dim0))
        p_arr0 = ori_arr[0:crop_dim1, 0:crop_dim0]
    elif num_dims == 3:
        ori_arr = np.reshape(raw_arr, (ori_dim2, ori_dim1, ori_dim0))
        p_arr0 = ori_arr[0: crop_dim2, 0:crop_dim1, 0:crop_dim0]

    p_arr0 = p_arr0.astype('float32')
    p_arr1 = p_arr0[:, :]
    print(np.shape(p_arr0))

    bin_name = f'C:\\Users\\Bizon\\Desktop\\p_data\\{dataset}_{file_name}_{crop_dim0}_{crop_dim1}.bin'
    print('save file: {}:'.format(bin_name))
    p_arr0.tofile(bin_name)

    # reshape data for hardware memory
    p_arr1 = np.reshape(p_arr1, (crop_dim1 * crop_dim0 // 16, 16))
    return p_arr1.tolist()


def save_ori_data(data_arr):
    num_rows = len(data_arr)
    num_cols = len(data_arr[0])

    f = open("C:\\Users\\Bizon\\Desktop\\all_hls_projects\\sz_hls4_0\\mem.h", "w")
    f.write('#ifndef _MEM_H_\n')
    f.write('#define _MEM_H_')
    f.write('\n')
    bracket_l = '{'
    bracket_r = '}'

    f.write(f'const float mem[{num_rows}][{num_cols}] = {bracket_l} \n')
    for i in range(num_rows):
        d_str = ''
        for j in range(num_cols):
            if j == 0:
                d_str += f' {bracket_l}{data_arr[i][j]}'
            else:
                d_str += f', {data_arr[i][j]}'
        # f.write('\n')
        f.write(d_str)
        if i < num_rows - 1:
            f.write('}, \n')
        else:
            f.write('} \n')
    f.write('}; \n')

    f.write(f'#pragma HLS ARRAY_PARTITION variable = mem dim = 2 complete \n')
    for i in range(1):
        pragma = f'#pragma HLS RESOURCE variable=mem core=XPM_MEMORY uram \n'
        f.write(pragma)

    f.write('#endif\n')
    f.close()


def save_code_freq(data_arr, file_name):
    arr_len = len(data_arr)

    f = open("C:\\Users\\Bizon\\Desktop\\all_hls_projects\\huff_hls\\symbol_freqs.h", "w")
    # f = open("./inter_data/CLOUD_freq.h", "w")
    # f = open(f"./inter_data/{file_name}", "w")

    f.write('#ifndef _SYMBOL_FREQS_H_\n')
    f.write('#define _SYMBOL_FREQS_H_')
    f.write('\n')
    bracket_l = '{'
    bracket_r = '}'

    f.write(f'const uint32_t symbol_freqs[{arr_len}] = {bracket_l} \n')
    d_str = ''
    for i in range(arr_len):
        if i < arr_len - 1:
            d_str += f' {data_arr[i]},'
        else:
            d_str += f' {data_arr[i]}'

        if i > 0 and i % 63 == 0:
            d_str += '\n'

        # d_str += f'{data_arr[i]} \n'

    f.write(d_str)
    f.write('}; \n')
    f.write('#endif\n')
    f.close()


def save_rand_freq(freq_arr):
    f = open("C:\\Users\\Bizon\\Desktop\\all_hls_projects\\sz_hls4_0\\freq_arr.h", "w")
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


def save_codeword(codewords):
    table_len = len(codewords)
    num_tab = 16
    codeword_len = 1

    f = open("C:\\Users\\Bizon\\Desktop\\all_hls_projects\\fixed_table\\codeword.h", "w")
    f.write('#ifndef _CODEWORD_H_\n')
    f.write('#define _CODEWORD_H_')
    f.write('\n')
    bracket_l = '{'
    bracket_r = '}'

    for i in range(num_tab):
        f.write(f'const Codeword tab{i}[{table_len}] = {bracket_l} \n')
        d_str = ''
        for j in range(table_len):
            if j < table_len - 1:
                d_str += f' {bracket_l}{codewords[j]}, {codeword_len}{bracket_r},'
            else:
                d_str += f' {bracket_l}{codewords[j]}, {codeword_len}{bracket_r}'

            if j > 0 and j % 63 == 0:
                d_str += '\n'

        f.write(d_str)
        f.write('}; \n')

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


def calc_std(arr, file, idx):
    arr_np = np.array(arr)
    # arr_np = arr_np[arr_np != 0]
    arr_np = arr_np[412:612]
    arr_np = np.log10(arr_np)
    mu = np.mean(arr_np)
    sigma = np.std(arr_np)

    sigma = sigma * 10

    std_f = open(file, 'a')
    if idx == 0:
        std_f.write('0\n')
        std_f.write(f'{sigma}\n')
    else:
        std_f.write(f'{sigma}\n')

    std_f.close()
    print(f'std is {sigma} .....')

    return sigma


def record_ratio(ratio_file, sz_file, data_len, data_type, idx):

    bit_width = 32 if data_type == np.float32 else 64
    o_size = (bit_width / 8) * data_len
    c_size = os.stat(sz_file).st_size
    ratio = o_size / c_size
    ratio_f = open(ratio_file, 'a')
    if idx == 0:
        ratio_f.write('0\n')
        ratio_f.write(f'{ratio}\n')
    else:
        ratio_f.write(f'{ratio}\n')

    ratio_f.close()
    print(f'ratio is {ratio} .....')

    return ratio


def plot_dist(arr):
    import numpy as np
    import matplotlib.pyplot as plt
    from scipy.optimize import curve_fit
    import math
    import matplotlib.mlab as mlab
    from scipy.stats import norm

    arr_np = np.array(arr)
    arr_np = arr_np[arr_np != 0]

    mu = np.mean(arr_np)
    sigma = np.std(arr_np)
    num_bins = 400
    n, bins, patches = plt.hist(arr_np, num_bins, density=1, alpha=0.75)
    y = norm.pdf(bins, mu, sigma)

    plt.grid(True)
    plt.plot(bins, y, 'r--')
    plt.xlabel('values')
    plt.ylabel('Probability')
    plt.title(f'Histogram: mu={mu}, sigma={sigma} ')
    plt.show()


def get_all_ranges(path):
    files = os.listdir(path)
    for file in files:
        if not os.path.isdir(file) and file.endswith('.dat'):
            raw_arr = np.fromfile(path + file, dtype=np.float32, count=-1)
            max_v = np.max(raw_arr)
            min_v = np.min(raw_arr)

            # if max_v < 10:
            #     print(f'{file}: max_v is {max_v}, min_v is {min_v}')
            return max_v - min_v


def get_one_range(file, data_type, ext):

    if not os.path.isdir(file) and file.endswith(ext):
        raw_arr = np.fromfile(file, dtype=data_type, count=-1)
        max_v = np.max(raw_arr)
        min_v = np.min(raw_arr)

        return max_v - min_v


def predict_compression_size(code_freq, total_size):
    p1 = code_freq[512] / total_size
    entropy = 0
    for q in code_freq:
        p = q / total_size
        entropy -= p * math.log(p)
    print(f'entropy is {entropy}')

    redundancy = p1 + 0.086
    average_codeword_len = redundancy + entropy
    c_size = int(average_codeword_len * total_size / 8)
    print(f'predicted compression size is {c_size} bytes')

    return c_size


def plot_scatter(fig_name, x_list, y_list):
    plt.figure(f'quant code frequency of {fig_name}')
    ax = plt.gca()
    ax.set_xlabel('code')
    ax.set_ylabel('percent')
    ax.scatter(x_list, y_list, c='r', s=10, alpha=0.5)
    plt.show()


def plot_dot_line(fig_name, x_list, y_list):
    plt.figure(f'quant code frequency of {fig_name}')
    ax = plt.gca()
    ax.set_xlabel('code')
    ax.set_ylabel('percent')
    ax.plot(x_list, y_list, color='green', linewidth=1.5, alpha=0.5, marker='x', markersize=3, markeredgecolor='red')
    matplotlib.pyplot.savefig(f'./inter_data/{fig_name}.jpg')
    # plt.show()


def get_file_l(path, ext):
    files = os.listdir(path)
    file_l = []
    for file in files:
        if not os.path.isdir(file) and file.endswith(ext):
            file_l.append(file)

    return file_l


def save_obj(obj, name):
    with open('./inter_data/' + name + '.pkl', 'wb') as f:
        pickle.dump(obj, f, pickle.HIGHEST_PROTOCOL)


def load_obj(name):
    with open('./inter_data/' + name + '.pkl', 'rb') as f:
        return pickle.load(f)


def calc_ave_freq(name, scale):
    raw_freq = load_obj(name)
    raw_freq = np.array(raw_freq)
    ave_freq = np.round(np.mean(raw_freq, axis=0) * scale) + 1
    return ave_freq


def split_file(file_path, ext, data_type, n):
    raw_arr = np.fromfile(file_path, dtype=data_type, count=-1)
    base_name = file_path[:-len(ext) - 1]
    ave_len = (len(raw_arr) - 1) // n + 1

    for i in range(n):
        if i < n - 1:
            partial_arr = raw_arr[i * ave_len:(i + 1) * ave_len]
        else:
            partial_arr = raw_arr[i * ave_len:]

        partial_file = f'{base_name}_p{i}{ext}'
        partial_arr.tofile(partial_file)


def save_all_freqs(obj_name, file_name):
    freqs = load_obj(obj_name)
    len1 = len(freqs)
    len0 = len(freqs[0])

    f = open(f"./inter_data/{file_name}", "w")
    for i0 in range(len0):
        d_str = ''
        for i1 in range(len1):
            d_str += f'{freqs[i1][i0]} '

        f.write(d_str)
        f.write('\n')

    f.close()


def split_NWChem(file_dir, ext, data_type, n):
    all_arr = np.array([])
    files = get_file_l(file_dir, ext)
    for file in files:
        file_path = f'{file_dir}{file}'
        raw_arr = np.fromfile(file_path, dtype=data_type, count=-1)
        all_arr = np.concatenate((all_arr, raw_arr), axis=0)

    file_path = 'C:\\Users\\Bizon\\Desktop\\data_com\\SDRBENCH-NWChem-dataset\\NWChem' + ext
    base_name = file_path[:-len(ext) - 1]
    ave_len = (all_arr.size - 1) // n + 1

    for i in range(n):
        if i < n - 1:
            partial_arr = all_arr[i * ave_len:(i + 1) * ave_len]
        else:
            partial_arr = all_arr[i * ave_len:]

        partial_file = f'{base_name}_p{i}{ext}'
        partial_arr.tofile(partial_file)
