

class pcmFileManager:
    def __init__(self, path):
        self.path = path
        self.is_open = False
        #如果文件存在，清空文件
        with open(self.path, "w") as f:
            f.write("")
    
    def open(self):
        if self.is_open:
            return True
        else:
            try:
                self.file = open(self.path, "ab")
                self.is_open = True
                return True
            except:
                return False
            
    def close(self):
        if self.is_open:
            self.file.close()
            self.is_open = False
            return True
        else:
            return False

    def write(self, data):#二进制写入，在文件后追加
        if self.is_open:
            self.file.write(data)
        else:
            self.open()
            self.file.write(data)
        
        
    def read(self):
        with open(self.path, "rb") as f:
            return f.read()
        
    
class txtFileManager:
    def __init__(self, path):
        self.path = path
        self.is_open = False
        #如果文件存在，清空文件
        with open(self.path, "w") as f:
            f.write("")
    
    def open(self):
        if self.is_open:
            return True
        else:
            try:
                self.file = open(self.path, "a")
                self.is_open = True
                return True
            except:
                return False
            
    def close(self):
        if self.is_open:
            self.file.close()
            self.is_open = False
            return True
        else:
            return False

    def write(self, data):
        if self.is_open:
            self.file.write(data)
        else:
            self.open()
            self.file.write(data)
    
    def setPath(self, path):
        self.path = path
        #如果文件存在，清空文件
        with open(self.path, "w") as f:
            f.write("")
        
    def read(self):
        with open(self.path, "r") as f:
            return f.read()
        
    def clear(self):
        with open(self.path, "w") as f:
            f.write("")

class wavFileManager:
    def __init__(self, path):
        self.path = path

    def write(self, data):
        with open(self.path, "wb") as f:
            f.write(data)
        
        
    def read(self):
        with open(self.path, "rb") as f:
            return f.read()
        
    def clear(self):
        with open(self.path, "w") as f:
            f.write("")
    
    def getWavHeader(self, sampleRate, bitsPerSample, channels):
        # 生成wav文件头
        import struct
        # 生成wav文件头
        # 二进制数据
        # 采样率
        sampleRate = int(sampleRate)
        # 通道数
        channels = int(channels)
        # 采样精度
        bitsPerSample = int(bitsPerSample)
        # 数据长度
        dataLength = len(self.read())
        # 文件长度
        fileLength = dataLength + 36
        # 生成wav文件头
        riff = b'RIFF'
        wavefmt = b'WAVEfmt '
        data = b'data'
        # 未压缩的pcm格式编码是1
        fmtChunkSize = 16
        fmt = 1
        byteRate = sampleRate * channels * bitsPerSample // 8
        blockAlign = channels * bitsPerSample // 8
        wavHeader = struct.pack(
            riff, fileLength, wavefmt, data, fmtChunkSize, fmt,
            channels, sampleRate, byteRate, blockAlign, bitsPerSample, data
        )
        # 生成wav文件头
        return wavHeader