from moudule import iat
from moudule import tts
from moudule import usb_cdc
from moudule import fileIO
from time import sleep

import wave

APPID = '6267d3f7'
APISecret = 'ZDRlMDQwMmZjNGJiOTBhYTc5ZWIxY2Ex'
APIKey = '29cd024f42b3faa5e6c15303cf304bc1'
AudioFile = r'demo.pcm'



# 配置音频参数
sample_width = 2  # 采样宽度（字节数）
sample_rate = 8000  # 采样率
channels = 1  # 声道数



        
def byte2int(byte_data):
    # flag = 1
    # if byte_data[0] == 255:
    #     byte_data[0] = 0
    #     flag = -1
    return int.from_bytes(byte_data, byteorder='big', signed=True)


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
    

def getDataFromMircophone():
    pcmFile = fileIO.pcmFileManager("test.pcm")
    txtFile = fileIO.txtFileManager("wave_data.txt")
    # pcmFile.open()
    output_file = wave.open('output.pcm', 'wb')
    output_file.setsampwidth(sample_width)
    output_file.setframerate(sample_rate)
    output_file.setnchannels(channels)
    txtFile.open()
    wait_time = 0
    ignore_data_cnt = 0
    while True:
        try:
            raw_data = usb.usb_read_data(8)
        except Exception as e:
            print(e)
            break
        if raw_data:
            
            wait_time = 0

            byte_data = b''
            for i in raw_data:
                #将i转为byte
                byte_data += bytes([i])
            
            # byte_data = byteCheck(byte_data)
            dec_data_str = byte2int(byte_data)
            hex_data_str = byte_data.hex()
            

            print("hex_data_str:",hex_data_str)
            print("dec_data_str:",dec_data_str)
            print("byte_data:",byte_data)


            # # 以16进制打印
            # print("hex:" + raw_data.hex())
            # # 以10进制打印，注意符号位为1的转为负数
            # print("dec:" + str(int.from_bytes(raw_data, byteorder='big', signed=True)))

            if ignore_data_cnt > 10:
                #写入pcm文件
                # pcmFile.write(byte_data)
                # 将音频数据写入PCM文件
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
    pcmFile.close()
    # 关闭PCM文件
    output_file.close()
    txtFile.close()


vid = 0x0483
pid = 0x5740

usb = usb_cdc.USB_CDC(vid, pid)


if __name__ == '__main__':
    usb.usb_open()
    getDataFromMircophone()