import serial

usb_cdc = serial.Serial("COM11", 115200, timeout=0.5)

while True:
    if usb_cdc.in_waiting:
        data = usb_cdc.read(usb_cdc.in_waiting)
        print(data)