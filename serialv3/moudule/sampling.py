from time import time

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