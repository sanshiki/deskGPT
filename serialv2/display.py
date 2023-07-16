import matplotlib.pyplot as plt

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
    print(data)
    return data

#画图
def draw(data):
    #横坐标
    x = []
    for i in range(len(data)):
        x.append(i)
    #纵坐标
    y = data
    plt.plot(x, y)
    plt.show()


draw(readTxt("wave_data.txt"))
# draw(readTxt("audioAvDataDec.txt"))