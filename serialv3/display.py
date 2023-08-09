import matplotlib.pyplot as plt
import numpy as np

#读入txt
def readTxt(fileName):
    data = []
    with open(fileName, "r") as f:
        #txt中的数据是以空格分隔的，只有一行
        #一个数一个数地读入，并转为int类型
        #其中的数据是16进制
        for i in f.read().split(" "):
            if i != "" and abs(int(i)) < 10e7:
                data.append(int(i))
    return data

#画图
def draw(data):
    # #横坐标
    # x = []
    # for i in range(len(data)):
    #     x.append(i)
    # #纵坐标
    # y = data
    # plt.plot(x, y)
    # plt.show()

    #时域图
    #横坐标
    sample_rate = 16000
    t = np.linspace(0, len(data)/sample_rate, len(data))

    #纵坐标
    y = data

    #频域图
    #横坐标
    fft_result = np.fft.fft(data)

    freq_axis = np.fft.fftfreq(len(data), t[1] - t[0]) / 2
    # freq_axis = np.linspace(0, max(freq_axis1), len(fft_result))
    # freq_axis = np.linspace(0, len(fft_result), len(fft_result))

    #分别绘制
    plt.subplot(211)
    plt.plot(t, y)
    plt.xlabel("time(s)")
    plt.ylabel("amplitude")
    plt.title("time domain")

    plt.subplot(212)
    plt.plot(freq_axis, np.abs(fft_result))
    plt.xlabel("frequency(Hz)")
    plt.ylabel("amplitude")
    plt.title("frequency domain")

    plt.tight_layout()
    plt.show()



# draw(readTxt("wave_data.txt"))
draw(readTxt("wave_data.txt"))
# draw(readTxt("audioAvDataDec.txt"))