from moudule import pcm2wav
from moudule import filter
import soundfile as sf

# 指定 PCM 文件路径
pcm_file = 'audioFile/pcmFile/output.pcm'
noise_pcm_file = 'audioFile/pcmFile/noise.pcm'
filtered_pcm_file = 'audioFile/pcmFile/filtered_output.pcm'

# 指定 WAV 文件路径
wav_file = 'audioFile/wavFile/output.wav'
noise_wav_file = 'audioFile/wavFile/noise.wav'
filtered_wav_file = 'audioFile/wavFile/filtered_output.wav'

# 指定 PCM 文件参数
sample_width = 2  # 采样宽度（字节数）
sample_rate = 16000  # 采样率
num_channels = 1  # 声道数

pcm2wav.pcm_to_wav(pcm_file, wav_file, sample_width, sample_rate, num_channels)
pcm2wav.pcm_to_wav(noise_pcm_file, noise_wav_file, sample_width, sample_rate, num_channels)

audio, sr = sf.read(wav_file)
noise_audio, noise_sr = sf.read(noise_wav_file)

# 设置小波变换参数
wavelet = 'db10'  # 选择小波基函数
level = 3  # 分解层数

# 执行小波变换
output_audio = filter.wavelet_transform(audio, wavelet, level)

# output_audio = lms_adaptive_filter(audio, noise_audio, 100, 0.01, len(audio))


output_audio = filter.spectral_subtraction(output_audio, noise_audio, 1)


sf.write(filtered_wav_file, output_audio, sr)


pcm2wav.wav_to_pcm(filtered_wav_file, filtered_pcm_file)