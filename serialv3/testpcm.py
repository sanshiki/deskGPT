import wave

# 配置音频参数
sample_width = 2  # 采样宽度（字节数）
sample_rate = 44100  # 采样率
channels = 1  # 声道数

# 创建PCM文件
output_file = wave.open('output.pcm', 'wb')
output_file.setsampwidth(sample_width)
output_file.setframerate(sample_rate)
output_file.setnchannels(channels)

# 将音频数据写入PCM文件
output_file.writeframes(byte_data)

# 关闭PCM文件
output_file.close()
