from moudule import iat
from moudule import tts
from moudule import usb_cdc
from moudule import fileIO
from moudule import sampling
from moudule import bytetools
from moudule import datapack
from moudule import chatGPT
from time import sleep,time

import wave
import math
import threading
from queue import Queue

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

# 科大讯飞认证信息
APPID = '6267d3f7'
APISecret = 'ZDRlMDQwMmZjNGJiOTBhYTc5ZWIxY2Ex'
APIKey = '29cd024f42b3faa5e6c15303cf304bc1'
AudioFile = r'demo.pcm'

# 选择麦克风类型
microphone_type = 'atk-mo1053'
# microphone_type = 'inmp441'

# 配置音频参数
sample_width = 2  # 采样宽度（字节数）
sample_rate = 16000  # 采样率
channels = 1  # 声道数

# 缓冲队列
q = Queue()

# usb-cdc
vid = 0x0483
pid = 0x5740

usb = usb_cdc.USB_CDC(vid, pid)

# 数据包
pcDataPack = datapack.dataPack()
    
pcm_file = 'audioFile/pcmFile/output.pcm'
noise_pcm_file = 'audioFile/pcmFile/noise.pcm'
play_pcm_file = 'audioFile/pcmFile/demo.pcm'
play_wav_file = 'audioFile/wavFile/demo.wav'

# usb-cdc线程函数
err_flag = False
record_flag = True
no_voice_flag = False
def usb_cdc_thread():
    global err_flag
    global record_flag

    print("usb_cdc_thread start")

    while record_flag == True and err_flag == False and no_voice_flag == False:
        try:
            pcDataPack.record_enable = True
            pcDataPack.play_enable = False
            pcDataPack.update()
            usb.usb_send_data(pcDataPack.bin) # 发送数据包
            raw_data = usb.usb_read_data(200) # 读取数据
            if raw_data:
                q.put(raw_data)

        except Exception as e:
            print(e)
            err_flag = True

    # pcDataPack.record_enable = True
    # pcDataPack.play_enable = False
    # pcDataPack.update()
    # usb.usb_send_data(pcDataPack.bin)



def getDataFromMircophone():
    # 打开文件
    txtFile = fileIO.txtFileManager("wave_data.txt")
    
    # 声音记录
    output_file = wave.open(pcm_file, 'wb')
    output_file.setsampwidth(sample_width)
    output_file.setframerate(sample_rate)
    output_file.setnchannels(channels)

    # 噪音记录
    if microphone_type == 'inmp441':
        noise_file = wave.open(noise_pcm_file, 'wb')
        noise_file.setsampwidth(sample_width)
        noise_file.setframerate(sample_rate)
        noise_file.setnchannels(channels)

    # 变量初始化
    txtFile.open()
    wait_time = time()
    ignore_data_cnt = 0
    sampling_data = sampling.samplingData(100)
    sampling_datav2 = sampling.samplingDatav2(30,100)
    # last_status = False

    global err_flag
    global record_flag
    global no_voice_flag

    record_flag = True
    err_flag = False
    no_voice_flag = False

    #开启usb-cdc线程
    t = threading.Thread(target=usb_cdc_thread)
    t.start()

    record_time = time()
    no_voice_time = time()

    while True:
        size = q.qsize()
        # print("size:",size)
        # print("time:",time() - record_time)
        if sampling_datav2.is_voice:
            no_voice_time = time()
        if time() - record_time > 10:
            record_flag = False
            break
        if  time() - no_voice_time > 3:
            record_flag = False
            if abs(record_time - no_voice_time) < 0.01:
                print("no voice for 3 seconds from the beginning")
                no_voice_flag = True
            else:
                print("no voice for 3 seconds")
            break
        if size != 0:
            raw_data = q.get()
            if raw_data and err_flag == False:
                if microphone_type == 'inmp441':
                    if sampling_data.no_voice_for_seconds(3):
                        print("no voice for 3 seconds")
                        break
                    
                    wait_time = 0

                    byte_data = b''
                    for i in raw_data:
                        #将i转为byte
                        byte_data += bytes([i])

                    # inmp441给的数据是24位，stm32为了满足C语言的格式要求，将数据转为32位，所以第一个字节是无效数据，需要去掉
                    byte_data = byte_data[1:]

                    # print("origin byte_data:",byte_data)

                    #24映射到16
                    byte_data = bytetools.byte24to16(byte_data)

                    #字节反转
                    byte_data = byte_data[::-1]
                    
                    # byte_data = byteCheck(byte_data)
                    dec_data_str = bytetools.byte2int(byte_data)

                    sampling_data.update(abs(int(dec_data_str)))
                    
                    #将dec_data_str转为16进制字符串
                    hex_data_str = hex(dec_data_str)
                    

                    print("hex_data_str:",hex_data_str)
                    print("dec_data_str:",dec_data_str)
                    print("byte_data:",byte_data)

                    # print("mean of data:",sampling_data.mean)
                    # print("var of data:",sampling_data.var)
                    # print("status of data:",sampling_data.judge_status())
                    # print("is voice:",sampling_data.is_voice())
                    # print("is noise:",sampling_data.is_noise())


                    # # 以16进制打印
                    # print("hex:" + raw_data.hex())
                    # # 以10进制打印，注意符号位为1的转为负数
                    # print("dec:" + str(int.from_bytes(raw_data, byteorder='big', signed=True)))

                    if ignore_data_cnt > 10:
                        # 将音频数据写入PCM文件
                        output_file.writeframes(byte_data)
                        output_file.writeframes(byte_data)
                        #写入txt文件
                        txtFile.write(str(int(dec_data_str)) + " ")
                    else:
                        ignore_data_cnt += 1

                elif microphone_type == 'atk-mo1053':
                    try:
                        # atk-mo1053
                        byte_data = b''
                        byte_data_list = []
                        dec_data_str_list = []
                        dec_data_list = []
                        hex_data_str_list = []
                        cnt = 0
                        for i in raw_data:
                            #将i转为byte
                            byte_data += bytes([i])
                            cnt += 1
                            if cnt == 2:# 每2个字节为一组
                                byte_data_list.append(byte_data)
                                cnt = 0


                                # 转为10进制
                                dec_data_str = bytetools.byte2int(byte_data)
                                dec_data_list.append(abs(int(dec_data_str)))
                                dec_data_str_list.append(dec_data_str)

                                # 转为16进制
                                hex_data_str = hex(dec_data_str)
                                hex_data_str_list.append(hex_data_str)

                                # if last_status != sampling_datav2.is_voice:
                                #     print(sampling_datav2.is_voice)
                                # last_status = sampling_datav2.is_voice

        
                                # 打印信息
                                # print("hex_data_str:",hex_data_str)
                                # print("dec_data_str:",dec_data_str)
                                # print("byte_data:",byte_data)
                                # print("mean of data:",sampling_data.mean)
                                # print("var of data:",sampling_data.var)
                                # print("status of data:",sampling_data.judge_status())
                                # usb.usb_send_data(b'\x00\x01')
                                # if byte_data != b'\x00\x00':
                                #     print("stm32 has received and resent data")

                                # 将音频数据写入PCM文件
                                output_file.writeframes(byte_data)
                                output_file.writeframes(byte_data)
                                #写入txt文件
                                txtFile.write(str(int(dec_data_str)) + " ")

                                byte_data = b''
                        # 样本更新
                        sampling_datav2.update(dec_data_list)
                    except Exception as e:
                        print(e)
                        err_flag = True
                        break

            else: #wait
                if time() - wait_time > 5:
                    print("wait too long")
                    break
        else:
            pass

    # 关闭PCM文件
    output_file.close()
    txtFile.close()

    # 记录噪音
    if microphone_type == 'inmp441':
        for i in sampling_data.noise_sample:
            noise_file.writeframes(bytetools.int2byte(i))
            noise_file.writeframes(bytetools.int2byte(i))
        noise_file.close()


def sendDataToSpeaker():

    audioFile = open(play_wav_file,'rb')

    txtFile = fileIO.txtFileManager("wave_data.txt")
    txtFile.open()

    pcDataPack.spi_enable = True
    pcDataPack.record_enable = False
    pcDataPack.play_enable = True
    pcDataPack.lost_data_test_enable = False
    pcDataPack.play_commplete = False
    pcDataPack.update()

    cnt = 0
    send_buffer = b''
    send_time = time()
    while audioFile.readable():
        int8_data = audioFile.read(1)
        cnt += 1
        
        if int8_data:
            if cnt % 2 == 1:
                high_byte = int8_data
            elif cnt % 2 == 0:
                low_byte = int8_data
                int16_data = high_byte + low_byte
                send_buffer += int16_data
                # print(int16_data)
                txtFile.write(str(bytetools.byte2int(int16_data)) + " ")

                if cnt == pcDataPack.audioDataLength:
                    pcDataPack.addAudioData(send_buffer)
                    usb.usb_send_data(pcDataPack.bin)
                    # raw_data = usb.usb_read_data(550) # 读取数据
                    # print("bin:")
                    # print(pcDataPack.bin)
                    # print("raw_data:")
                    # print(bytes(raw_data))
                    # print("len of raw_aata:",len(raw_data))
                    # print(len(raw_data))
                    sleep(0.013) #0.015
                    send_buffer = b''
                    cnt = 0
        else:
            break
    #音频发送终止，发送结束标志
    sleep(0.1)
    for i in range(10):
        pcDataPack.record_enable = False
        pcDataPack.play_enable = False
        pcDataPack.update()
        usb.usb_send_data(pcDataPack.bin)

    txtFile.close()

if __name__ == '__main__':

    #--------------------初始化--------------------#
    usb.usb_open()
    chatGPT.pre_knowledge_init()
    # sendDataToSpeaker()

    while True:
        #--------------------录音--------------------#
        for i in range(5):
            pcDataPack.record_enable = True
            pcDataPack.play_enable = False
            pcDataPack.update()
            usb.usb_send_data(pcDataPack.bin)

        sleep(0.1)

        getDataFromMircophone()

        if err_flag == False and no_voice_flag == False:
            #--------------------降噪--------------------#
            pcm2wav.pcm_to_wav(pcm_file, wav_file, sample_width, sample_rate, channels)
            audio, sr = sf.read(wav_file)
            # 设置小波变换参数
            wavelet = 'db10'  # 选择小波基函数
            level = 3  # 分解层数

            # 执行小波变换
            output_audio = filter.wavelet_transform(audio, wavelet, level)

            sf.write(filtered_wav_file, output_audio, sr)

            pcm2wav.wav_to_pcm(filtered_wav_file, filtered_pcm_file)

            #--------------------语音识别--------------------#
            iat_res = iat.iatRun(filtered_pcm_file)
            print("iat_res:",iat_res)

            #--------------------发送问题至chatGPT--------------------#
            chatGPT.send_to_chatGPT(iat_res)

            with open('./config/answer.txt', 'r', encoding='utf-8') as f:
                answer = f.read()

            #--------------------语音合成--------------------#

            tts.ttsRun(answer)
            pcm2wav.pcm_to_wav(play_pcm_file, play_wav_file, sample_width, sample_rate, channels)

            sendDataToSpeaker()

    #--------------------关闭--------------------#
    usb.usb_close()