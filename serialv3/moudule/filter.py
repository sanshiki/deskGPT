# from scipy.signal import firwin, lfilter
# import soundfile as sf

# # 读取wav文件
# wav_file = 'output.wav'
# audio, sr = sf.read(wav_file)

# # 设计带通滤波器
# low_cutoff_freq = 100  # 低截止频率，单位为Hz
# high_cutoff_freq = 3000  # 高截止频率，单位为Hz
# nyquist_freq = 0.5 * sr  # 奈奎斯特频率
# filter_order = 100  # 滤波器阶数

# # 使用firwin函数设计带通滤波器的系数
# filter_coef = firwin(filter_order, [low_cutoff_freq/nyquist_freq, high_cutoff_freq/nyquist_freq], pass_zero=False)

# # 将音频信号通过滤波器进行滤波
# filtered_audio = lfilter(filter_coef, 1.0, audio)

# # 将滤波后的音频写入新的wav文件
# filtered_file = 'filtered_output.wav'
# sf.write(filtered_file, filtered_audio, sr)

from scipy.signal import firwin, lfilter
import pywt
import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt

def bondpass_filter(audio, sr, low_cutoff_freq, high_cutoff_freq, filter_order):
    nyquist_freq = 0.5 * sr  # 奈奎斯特频率
    filter_coef = firwin(filter_order, [low_cutoff_freq/nyquist_freq, high_cutoff_freq/nyquist_freq], pass_zero=False)
    filtered_audio = lfilter(filter_coef, 1.0, audio)
    return filtered_audio

def wavelet_transform(audio, wavelet, level):
    # 执行小波变换
    coeffs = pywt.wavedec(audio, wavelet, level=level)

    # 获取小波系数
    cA3, cD3, cD2, cD1 = coeffs

    # 设置阈值参数
    threshold = np.std(cD1) * np.sqrt(2 * np.log(len(audio)))

    # 应用软阈值
    cD1_thresholded = pywt.threshold(cD1, threshold, mode='soft')

    # 重构信号
    coeffs_thresholded = cA3, cD3, cD2, cD1_thresholded
    reconstructed_audio = pywt.waverec(coeffs_thresholded, wavelet)

    return reconstructed_audio

def lms_adaptive_filter(input_signal, reference_signal, filter_order, step_size, num_iterations):
    # 初始化滤波器权值
    filter_weights = np.zeros(filter_order)
    
    # 存储滤波器的输出信号
    output_signal = np.zeros_like(input_signal)
    
    # 迭代更新滤波器权值
    for i in range(num_iterations):
        # 获取当前输入信号段
        input_segment = input_signal[i:i+filter_order]
        
        # 使用当前滤波器权值对输入信号进行滤波
        filtered_signal = np.dot(filter_weights, input_segment)
        
        # 计算误差信号（期望输出 - 实际输出）
        if i >= len(reference_signal):
            #从头开始
            error = reference_signal[i-len(reference_signal)] - filtered_signal
        else:
            error = reference_signal[i] - filtered_signal
        
        # 更新滤波器权值
        filter_weights += step_size * error * input_segment
        
        # 存储输出信号
        output_signal[i] = filtered_signal
    
    return output_signal

def noise_sampling(audio, amp_rate, var):
    audio_mean = np.mean(audio)  # 计算音频信号的平均值
    audio_std = np.std(audio)  # 计算音频信号的标准差

    # 创建一个掩码，标记幅值小于平均值amp_rate倍且方差小于给定值var的样本
    mask = (audio < audio_mean * amp_rate) & (np.square(audio - audio_mean) < var)

    # 寻找掩码中连续为True的区域，返回起始索引和长度
    def find_longest_continuous_true(mask):
        max_start = -1
        max_length = 0
        current_start = -1
        current_length = 0

        for i, val in enumerate(mask):
            if val:
                if current_start == -1:
                    current_start = i
                current_length += 1
            else:
                if current_length > max_length:
                    max_start = current_start
                    max_length = current_length
                current_start = -1
                current_length = 0

        if current_length > max_length:
            max_start = current_start
            max_length = current_length

        return max_start, max_length

    start, length = find_longest_continuous_true(mask)  # 获取最长连续True区域的起始索引和长度
    noise = audio[start: start + length]  # 提取噪声样本

    return noise

#谱减法：用输入信号的频谱减去噪声信号的频谱，得到输出信号的频谱
def spectral_subtraction(input_signal, noise_signal, alpha=1):

    # # 生成噪声信号
    # noise_signal = noise_sampling(input_signal, 10, 0.5)

    #如果noise_signal和input_signal长度不一样，则通过对噪声信号随机采样的方式，使得两者长度一致
    if len(noise_signal) < len(input_signal):
        noise_signal = np.tile(noise_signal, len(input_signal) // len(noise_signal) + 1)
        noise_signal = noise_signal[:len(input_signal)]
    elif len(noise_signal) > len(input_signal):
        noise_signal = noise_signal[:len(input_signal)]


    input_fft = np.fft.fft(input_signal)
    noise_fft = np.fft.fft(noise_signal)


    # 计算信噪比
    snr = np.abs(input_fft) / np.abs(noise_fft)
    snr = np.where(snr < 1e-20, 1e-20, snr)

    # 计算输出信号的频谱
    # output_fft = np.where(np.abs(input_fft) > alpha * np.abs(noise_fft), input_fft - alpha * noise_fft, 0)
    output_fft = input_fft - alpha * noise_fft


    # 计算输出信号
    output_signal = np.fft.ifft(output_fft)
    # 化为整型数据
    output_signal = np.real(output_signal)

    # 分别绘制输入信号、噪声信号和输出信号的时域波形
    plt.figure()
    plt.subplot(3, 1, 1)
    # plt.plot(np.abs(input_fft))
    plt.plot(np.abs(input_signal))
    plt.title('Input Signal Spectrum')
    plt.subplot(3, 1, 2)
    plt.plot(np.abs(noise_fft))
    plt.title('Noise Signal Spectrum')
    plt.subplot(3, 1, 3)
    plt.plot(np.abs(output_fft))
    plt.title('Output Signal Spectrum')
    plt.show()
    

    return output_signal


# # 读取wav文件
# wav_file = 'output.wav'
# noise_file = 'noise.wav'
# audio, sr = sf.read(wav_file)
# noise_audio, noise_sr = sf.read(noise_file)

# # 设计带通滤波器
# low_cutoff_freq = 400  # 低截止频率，单位为Hz
# high_cutoff_freq = 6000  # 高截止频率，单位为Hz
# nyquist_freq = 0.5 * sr  # 奈奎斯特频率
# filter_order = 100  # 滤波器阶数

# print("params of filter:")
# print("low_cutoff_freq:",low_cutoff_freq)
# print("high_cutoff_freq:",high_cutoff_freq)
# print("nyquist_freq:",nyquist_freq)
# print("filter_order:",filter_order)

# # 使用firwin函数设计带通滤波器的系数
# filter_coef = firwin(filter_order, [low_cutoff_freq/nyquist_freq, high_cutoff_freq/nyquist_freq], pass_zero=False)

# # 将音频信号通过滤波器进行滤波
# filtered_audio = lfilter(filter_coef, 1.0, audio)

# audio = filtered_audio


# # 设置小波变换参数
# wavelet = 'db10'  # 选择小波基函数
# level = 3  # 分解层数

# # 执行小波变换
# output_audio = wavelet_transform(audio, wavelet, level)

# # output_audio = lms_adaptive_filter(audio, noise_audio, 100, 0.01, len(audio))

# # 取noise_audio为output_audio的前一小部分
# # noise_audio = output_audio[:int(len(output_audio) / 50)]

# output_audio = spectral_subtraction(output_audio, 1)


# 将降噪后的音频写入新的wav文件
# filtered_file = 'filtered_output.wav'
# sf.write(filtered_file, output_audio, sr)
