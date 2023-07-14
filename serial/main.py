#串口助手主程序

from moudule import usb_cdc
from moudule import UI
from moudule import fileIO
import tkinter as tk
import threading


dataNumLimit = 1000

rect_width = 30

degree = 0

def getDataFromMircophone():
    pcmFile = fileIO.pcmFileManager("test.pcm")
    txtFile = fileIO.txtFileManager("data0.txt")
    usb = usb_cdc.USB_CDC("COM8", 115200)
    global degree
    if usb.open():
        print("open success")
        while True:
            raw_data = usb.read_data()
            if raw_data:
                # 以16进制打印
                print(raw_data.hex())
                pcmFile.write(raw_data)

                # #将raw_data转为数字
                # data = []
                # for i in range(len(raw_data)):
                #     data.append(raw_data[i])
                #     txtFile.write(str(raw_data[0]) + " ")
                
    else:
        print("open failed")

if __name__ == '__main__':
    # mainUI = UI.SerialUI()
    # mainUI.run()

    getDataFromMircophone()

    