# from moudule import iat
# from moudule import tts
from moudule import usb_cdc
from moudule import fileIO
from time import sleep,time

import wave
import math
import threading

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

class samplingData:
    def __init__(self,datanum) -> None:
        self.data = []
        self.datanum = datanum
        self.mean = 0
        self.var = 0
        self.lastdata = 0
        self.noise_amp_threshold = 2500
        self.voice_amp_threshold = 8000
        self.gradient_threshold = 10
        self.var_threshold = 50000000

        self.sample_full = False

        self.noise_sample = []
        self.temp_noise_sample = []
        self.noise_sampling_flag = False

        self.no_voice_time = 0


    def update(self,newdata):
        self.data.append(newdata)
        #更新均值和方差
        if len(self.data) > self.datanum:
            self.lastdata = self.data[0]
            self.data.pop(0)
            if self.sample_full:
                self.mean = (self.mean * len(self.data) + newdata - self.lastdata) / len(self.data)
                self.var = (self.var * len(self.data) + (newdata - self.mean) ** 2 - (self.lastdata - self.mean) ** 2) / len(self.data)
            else:
                self.mean = sum(self.data) / len(self.data)
                self.var = sum([(i - self.mean) ** 2 for i in self.data]) / len(self.data)
                self.sample_full = True
            #更新噪声样本
            self.update_noise_sample(newdata)
    

    def update_noise_sample(self,newdata):
        #判断到噪声，开始采集噪声样本
        if self.is_noise():
            self.temp_noise_sample.append(newdata)

            #开始计时
            if self.noise_sampling_flag == False:
                self.no_voice_time = time()

            self.noise_sampling_flag = True
        elif self.is_voice():#判断到语音，停止采集噪声样本
            if self.noise_sampling_flag:
                self.noise_sampling_flag = False
                #如果噪声样本长度大于已有的噪声样本，则更新噪声样本
                if len(self.temp_noise_sample) > len(self.noise_sample):
                    self.noise_sample = self.temp_noise_sample
                self.temp_noise_sample = []

    def is_noise(self):
        if self.mean < self.noise_amp_threshold:
            return True
        else:
            return False
        
    def is_voice(self):
        if abs(self.mean) > self.voice_amp_threshold:
            return True
        else:
            return False
    
    def judge_status(self):
        test_node1 = self.data[0]
        test_node2 = self.data[int(len(self.data) / 3)]
        test_node3 = self.data[int(len(self.data) * 2 / 3)]
        test_node4 = self.data[-1]
        if self.var > self.var_threshold:
            if test_node4 - test_node3 > self.gradient_threshold and test_node3 - test_node2 > self.gradient_threshold and test_node2 - test_node1 > self.gradient_threshold:
                return 'going_up'
            elif test_node4 - test_node3 < -self.gradient_threshold and test_node3 - test_node2 < -self.gradient_threshold and test_node2 - test_node1 < -self.gradient_threshold:
                return 'going_down'
            else:
                return 'stable'
        else:
            return 'stable'
    
    def no_voice_for_seconds(self,seconds):
        if self.noise_sampling_flag:
            if time() - self.no_voice_time > seconds:
                return True
            else:
                return False
        else:
            return False




        
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
    sampling_data = samplingData(100)


    while True:
        try:
            raw_data = usb.usb_read_data(2)
        except Exception as e:
            print(e)
            break
        if raw_data:
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
                for i in raw_data:
                    #将i转为byte
                    byte_data += bytes([i])

                #字节反转
                # byte_data = byte_data[::-1]

                dec_data_str = byte2int(byte_data)

                #将dec_data_str转为16进制字符串
                hex_data_str = hex(dec_data_str)

                print("hex_data_str:",hex_data_str)
                print("dec_data_str:",dec_data_str)
                print("byte_data:",byte_data)

                if ignore_data_cnt > 10:
                    # 将音频数据写入PCM文件
                    output_file.writeframes(byte_data)
                    output_file.writeframes(byte_data)
                    #写入txt文件
                    txtFile.write(str(int(dec_data_str)) + " ")
                else:
                    ignore_data_cnt += 1

            else: #wait
                wait_time += 1
                if wait_time > 50000:
                    break

    usb.usb_close()
    # 关闭PCM文件
    output_file.close()
    txtFile.close()

    # 记录噪音
    for i in sampling_data.noise_sample:
        noise_file.writeframes(int2byte(i))
        noise_file.writeframes(int2byte(i))
    noise_file.close()


vid = 0x0483
pid = 0x5740

usb = usb_cdc.USB_CDC(vid, pid)


if __name__ == '__main__':
    usb.usb_open()
    getDataFromMircophone()