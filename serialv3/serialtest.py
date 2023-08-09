from moudule import usb_cdc
from time import sleep
from array import array

vid = 0x0483
pid = 0x5740

usb = usb_cdc.USB_CDC(vid, pid)

usb.usb_open()

while True:
    usb.usb_send_data(b'\x00\x01')

    raw_data = usb.usb_read_data(2)
    if raw_data == array('B', [0, 1]):
        print(raw_data)
        sleep(0.5)
    

usb.usb_close()