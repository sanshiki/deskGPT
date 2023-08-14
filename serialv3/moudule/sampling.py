from time import time
import numpy as np

class samplingData:
    def __init__(self,datanum) -> None:
        self.data = []
        self.datanum = datanum
        self.mean = 0
        self.var = 0
        self.lastdata = 0
        self.noise_amp_threshold = 2500     #inmp:2500
        self.voice_amp_threshold = 8000    #inmp:8000
        self.gradient_threshold = 10    #inmp:10
        self.var_threshold = 50000000   #inmp:50000000

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
        
    
class samplingDatav2:
    def __init__(self, packnum, packlen) -> None:
        self.data = []
        self.packnum = packnum
        self.packlen = packlen

        # self.noise_amp_threshold = 3200     #inmp:2500
        self.voice_amp_threshold = 50000    #inmp:8000

        # self.noise_count = 0
        self.voice_count = 0
        self.is_voice = False

        self.sample_cnt = 0



    def update(self,new_data_pack):
        self.data += new_data_pack
        self.sample_cnt += 1

        # #判断新的数据包中有多少个数据点超过了人声阈值
        # self.voice_count = 0
        # for i in new_data_pack:
        #     # print(i)
        #     if abs(i) > self.voice_amp_threshold:
        #         self.voice_count += 1
        
        # #判断是否有人声
        # if self.voice_count > self.packlen * 0.4:
        #     self.is_voice = True
        # else:
        #     self.is_voice = False
        # try:
        #     new_data_fft = np.fft.fft(new_data_pack)

        #     #判断在频率100~1000Hz之间有多少个数据点超过了人声阈值
        #     self.voice_count = 0
        #     for i in range(100,1000):
        #         if abs(new_data_fft[i]) > self.voice_amp_threshold:
        #             self.voice_count += 1
        #     print("voice_count:",self.voice_count)
        #     #判断是否有人声
        #     if self.voice_count > self.packlen * 0.4:
        #         self.is_voice = True
        #     else:
        #         self.is_voice = False
        # except Exception as e:
        #     pass
        
        if len(self.data) / self.packlen > self.packnum:
            self.data = self.data[self.packlen:]
        
        if self.sample_cnt == self.packnum / 10:
            self.sample_cnt = 0
            self.check_voice()

    
    def check_voice(self):
        data_fft = np.fft.fft(self.data)

        #将0~10部分置0
        for i in range(0,10):
            data_fft[i] = 0
        
        #将后半部分置0
        for i in range(int(len(data_fft) / 2),len(data_fft)):
            data_fft[i] = 0

        # 获取定义域
        freq_axis = np.fft.fftfreq(len(self.data), 1 / 16000) / 2

        max_freq = max(freq_axis)

        # 将频率转换为索引
        def freq2index(freq):
            return int(freq / max_freq * len(data_fft))
        
        # 判断在频率100~1000Hz之间有多少个数据点超过了人声阈值
        self.voice_count = 0
        for i in range(freq2index(100), freq2index(1500)):
            if abs(data_fft[i]) > self.voice_amp_threshold:
                self.voice_count += 1
        voice_count_percentage = self.voice_count / self.packlen / self.packnum * 2
        #保留两位小数
        print("voice_count_percentage:",round(voice_count_percentage,2))
        # print("len of data:", len(data_fft))
        # print("100Hz:", freq2index(100))
        # print("1000Hz:", freq2index(1000))
        # print("max:",max(data_fft))
        # print("index of max:",np.argmax(data_fft))
        # 判断是否有人声
        if voice_count_percentage > 0.28:#待测
            self.is_voice = True
        else:
            self.is_voice = False

        # print("is_voice:",self.is_voice)