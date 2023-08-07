import wave
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

# 打开 PCM 文件
pcm_file = wave.open('output.pcm', 'rb')

# 读取 PCM 音频数据
sample_width = pcm_file.getsampwidth()
sample_rate = pcm_file.getframerate()
num_frames = pcm_file.getnframes() # 读取音频数据的总帧数
audio_data = pcm_file.readframes(num_frames)

print("params of pcm file:")
print("sample_width:",sample_width)
print("sample_rate:",sample_rate)
print("num_frames:",num_frames)

# 将 PCM 数据转换为 NumPy 数组
audio_signal = np.frombuffer(audio_data, dtype=np.int16)

# 设计滤波器
order = 4  # 滤波器阶数
cutoff_freq = 1000  # 截止频率，可以根据实际情况进行调整
nyquist_freq = 0.5 * sample_rate
normal_cutoff = cutoff_freq / nyquist_freq
b, a = signal.butter(order, normal_cutoff, btype='high', analog=False)

# 应用滤波器
filtered_signal = signal.lfilter(b, a, audio_signal)

# 绘制时域图和频谱图
plt.figure(figsize=(10, 4))

# 绘制时域图
plt.subplot(121)
plt.plot(audio_signal)
plt.title('original signal')
plt.xlabel('sample')
plt.ylabel('amplitude')

# 绘制频谱图
plt.subplot(122)
freq, spectrum = signal.freqz(b, a, fs=sample_rate)
plt.plot(freq, np.abs(spectrum))
plt.title('frequency response')
plt.xlabel('normalized frequency')
plt.ylabel('gain')

plt.tight_layout()
plt.show()

# 保存滤波后的音频
filtered_audio_data = filtered_signal.astype(np.int16)
filtered_pcm_file = wave.open('filtered_recording.pcm', 'wb')
filtered_pcm_file.setsampwidth(sample_width)
filtered_pcm_file.setframerate(sample_rate)
filtered_pcm_file.setnchannels(1)
filtered_pcm_file.writeframes(filtered_audio_data.tobytes())
filtered_pcm_file.writeframes(filtered_audio_data.tobytes())
filtered_pcm_file.close()

# 关闭 PCM 文件
pcm_file.close()
