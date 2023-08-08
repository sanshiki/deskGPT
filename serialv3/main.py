# from moudule import iat
# from moudule import tts
from moudule import usb_cdc
from moudule import fileIO
from moudule import sampling
from time import sleep,time

import wave
import math
import threading
from queue import Queue

APPID = '6267d3f7'
APISecret = 'ZDRlMDQwMmZjNGJiOTBhYTc5ZWIxY2Ex'
APIKey = '29cd024f42b3faa5e6c15303cf304bc1'
AudioFile = r'demo.pcm'

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


        
def byte2int(byte_data):
    # flag = 1
    # if byte_data[0] == 255:
    #     byte_data[0] = 0
    #     flag = -1
    # return int.from_bytes(byte_data, byteorder='big', signed=True)  # 大端模式，有符号数
    return int.from_bytes(byte_data, byteorder='little', signed=True) # 小端模式，有符号数

def int2byte(int_data):
    return int_data.to_bytes(2, byteorder='little', signed=True)

#建立有符号24位和16位之间的映射关系，例如-32768对应-8388608
def byte24to16(byte_data):
    float24_data = float(byte2int(byte_data))
    float16_data = float24_data / 256
    return int2byte(int(float16_data))



#对字节值进行处理，如果是补码形式，转为原码，返回原码
def byteCheck(byte_data):
    origin_data = b''
    if byte_data[0] == 255:
        for i in byte_data:
            origin_data += bytes([255 - i])
        #把origin_data的第一个字节改成ff，以表示负数

        return origin_data
    else:
        return byte_data
    
pcm_file = 'audioFile/pcmFile/output.pcm'
noise_pcm_file = 'audioFile/pcmFile/noise.pcm'

# usb-cdc线程函数
err_flag = False
record_flag = True
def usb_cdc_thread():
    while record_flag == True:
        try:
            raw_data = usb.usb_read_data(200)
            # print("raw_date in usb_cdc_thread: ", raw_data)
            if raw_data:
                q.put(raw_data)
        except Exception as e:
            print(e)
            err_flag = True



def getDataFromMircophone():
    # 打开文件
    txtFile = fileIO.txtFileManager("wave_data.txt")
    
    # 声音记录
    output_file = wave.open(pcm_file, 'wb')
    output_file.setsampwidth(sample_width)
    output_file.setframerate(sample_rate)
    output_file.setnchannels(channels)

    # 噪音记录
    noise_file = wave.open(noise_pcm_file, 'wb')
    noise_file.setsampwidth(sample_width)
    noise_file.setframerate(sample_rate)
    noise_file.setnchannels(channels)

    txtFile.open()
    wait_time = 0
    ignore_data_cnt = 0
    sampling_data = sampling.samplingData(100)

    #开启usb-cdc线程
    t = threading.Thread(target=usb_cdc_thread)
    t.start()

    record_time = 0

    while True:
        size = q.qsize()
        print("size:",size)
        print("record_time:",record_time)
        record_time += 1
        if record_time >= 10000:
            record_flag = False
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
                    byte_data = byte24to16(byte_data)

                    #字节反转
                    byte_data = byte_data[::-1]
                    
                    # byte_data = byteCheck(byte_data)
                    dec_data_str = byte2int(byte_data)

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
                    # atk-mo1053
                    byte_data = b''
                    byte_data_list = []
                    cnt = 0
                    for i in raw_data:
                        #将i转为byte
                        byte_data += bytes([i])
                        cnt += 1
                        if cnt == 2:#每4个字节为一组
                            byte_data_list.append(byte_data)
                            byte_data = b''
                            cnt = 0

                    #字节反转
                    # byte_data = byte_data[::-1]
                    dec_data_str_list = []
                    # dec_data_str = byte2int(byte_data)
                    for byte_data in byte_data_list:
                        # byte_data = byteCheck(byte_data)
                        dec_data_str = byte2int(byte_data)
                        dec_data_str_list.append(dec_data_str)

                    hex_data_str_list = []

                    #将dec_data_str转为16进制字符串
                    # hex_data_str = hex(dec_data_str)
                    for dec_data_str in dec_data_str_list:
                        hex_data_str_list.append(hex(dec_data_str))

                    for i in range(len(hex_data_str_list)):
                    #     print("hex_data_str:",hex_data_str_list[i])
                    #     print("dec_data_str:",dec_data_str_list[i])
                    #     print("byte_data:",byte_data_list[i])

                        # 将音频数据写入PCM文件
                        output_file.writeframes(byte_data_list[i])
                        output_file.writeframes(byte_data_list[i])
                        #写入txt文件
                        txtFile.write(str(int(dec_data_str_list[i])) + " ")

            else: #wait
                wait_time += 1
                if wait_time > 50000:
                    print("wait too long")
                    break
        else:
            # print("queue is empty")
            pass

    usb.usb_close()
    # 关闭PCM文件
    output_file.close()
    txtFile.close()

    # 记录噪音
    for i in sampling_data.noise_sample:
        noise_file.writeframes(int2byte(i))
        noise_file.writeframes(int2byte(i))
    noise_file.close()



if __name__ == '__main__':
    usb.usb_open()
    getDataFromMircophone()