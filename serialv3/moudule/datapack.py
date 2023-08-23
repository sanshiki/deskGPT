from moudule import bytetools

# 发送至stm32的数据包

"""
数据包格式：
    数据包头部：
    数据包数据部分：
    字节1：
        7.开启/关闭spi
        6.开启/关闭录音
        5.开启/关闭播放
        4.开启/关闭漏数据测试
        3.音频已经发送完毕
        2-0.保留


"""

class dataPack:
    def __init__(self,head=None) -> None:
        self.bin = b''
        self.head = b'\x32\x10\x10\x57\x05'
        if head != None:
            self.head = head

        self.length = 550
        self.audioDataLength = 512

        self.spi_enable = True
        self.record_enable = True
        self.play_enable = False
        self.lost_data_test_enable = False

        #数据包头部
        for i in range(0, len(self.head)):
            self.bin += bytes([self.head[i]])

        #填充数据部分
        for i in range(0, self.length - len(self.head)):
            self.bin += b'\x00'

        #更新数据包
        self.update()


    def update(self):
        #spi控制字
        spi_ctrl = b'\x00'
        if self.spi_enable:
            spi_ctrl = bytetools.setbit(spi_ctrl,7)
        if self.record_enable:
            spi_ctrl = bytetools.setbit(spi_ctrl,6)
        if self.play_enable:
            spi_ctrl = bytetools.setbit(spi_ctrl,5)
        if self.lost_data_test_enable:
            spi_ctrl = bytetools.setbit(spi_ctrl,4)
        self.bin = bytetools.setbyte(self.bin,len(self.head),spi_ctrl)        
    
    def addAudioData(self,audioData):
        #音频数据部分
        # for i in range(0, len(audioData)):
        #     self.bin += bytes([audioData[i]])

        self.bin = bytetools.setbytes(self.bin,len(self.head) + 1,audioData)