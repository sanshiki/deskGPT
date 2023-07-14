# USB串口类

import serial as usb_cdc


class USB_CDC:
    def __init__(self, port, baudrate):
        self.port = port
        self.baudrate = baudrate
    
    def open(self):
        try:
            self.ser = usb_cdc.Serial(port=self.port, baudrate=self.baudrate, timeout=0.5)   
            self.ser.flushInput()
            self.ser.flushOutput()
            self.ser.close()
            self.ser.open()
        except Exception as e:
            return False
        return True

    def read_data(self):
        try:
            if self.ser.in_waiting:
                return self.ser.read(self.ser.in_waiting)
            else:
                return None
        except:
            return None

    def write_data(self, data):
        try:
            self.ser.write(data)
        except:
            pass

    def close(self):
        self.ser.close()