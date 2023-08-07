import wave
import struct

def pcm_to_wav(pcm_file, wav_file, sample_width, sample_rate, num_channels):
    # 打开 PCM 文件
    pcm = open(pcm_file, 'rb')
    
    # 创建 WAV 文件
    wav = wave.open(wav_file, 'wb')
    wav.setsampwidth(sample_width)
    wav.setframerate(sample_rate)
    wav.setnchannels(num_channels)
    
    # 写入数据
    wav.writeframes(pcm.read())
    
    # 关闭文件
    pcm.close()
    wav.close()

def wav_to_pcm(input_file, output_file):
    try:
        with wave.open(input_file, 'rb') as wav_in:
            params = wav_in.getparams()
            frames = wav_in.readframes(params.nframes)

            pcm_data = struct.unpack_from('<{}h'.format(params.nframes), frames)
            pcm_data = struct.pack('{}h'.format(params.nframes), *pcm_data)

            with open(output_file, 'wb') as pcm_out:
                pcm_out.write(pcm_data)

        print("WAV to PCM conversion successful.")
    except Exception as e:
        print("Error converting WAV to PCM: ", str(e))

# # 指定 PCM 文件路径
# pcm_file = 'output.pcm'
# filtered_pcm_file = 'filtered_output.pcm'

# # 指定 WAV 文件路径
# wav_file = 'output.wav'
# filtered_wav_file = 'filtered_output.wav'

# # 指定 PCM 文件参数
# sample_width = 2  # 采样宽度（字节数）
# sample_rate = 16000  # 采样率
# num_channels = 1  # 声道数

# # 转换 PCM 到 WAV
# pcm_to_wav(pcm_file, wav_file, sample_width, sample_rate, num_channels)
# pcm_to_wav('noise.pcm', 'noise.wav', sample_width, sample_rate, num_channels)

# # 转换 WAV 到 PCM
# # wav_to_pcm(filtered_wav_file, filtered_pcm_file)