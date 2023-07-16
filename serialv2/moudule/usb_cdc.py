# # USB串口类

# import serial as usb_cdc


# class USB_CDC:
#     def __init__(self, port, baudrate):
#         self.port = port
#         self.baudrate = baudrate
    
#     def open(self):
#         try:
#             self.ser = usb_cdc.Serial(port=self.port, baudrate=self.baudrate, timeout=0.5)   
#             self.ser.flushInput()
#             self.ser.flushOutput()
#             self.ser.close()
#             self.ser.open()
#         except Exception as e:
#             return False
#         return True

#     def read_data(self):
#         try:
#             if self.ser.in_waiting:
#                 return self.ser.read(self.ser.in_waiting)
#             else:
#                 return None
#         except:
#             return None

#     def write_data(self, data):
#         try:
#             self.ser.write(data)
#         except:
#             pass

#     def close(self):
#         self.ser.close()


import usb.core
import usb.util
import sys

class USB_CDC:
    def __init__(self, vid, pid):
        self.device = usb.core.find(idVendor=vid, idProduct=pid)
        if self.device is None:
            raise ValueError("设备未找到")

        self.interface = 0  # CDC设备的接口号
        self.endpoint_out = 0x01  # 输出端点地址
        self.endpoint_in = 0x81  # 输入端点地址
        self.usb_get_endpoint()

        for cfg in self.device:
            for intf in cfg:
                if self.device.is_kernel_driver_active(intf.bInterfaceNumber):
                    try:
                        self.device.detach_kernel_driver(intf.bInterfaceNumber)
                    except usb.core.USBError as e:
                        sys.exit("Could not detatch kernel driver from interface({0}): {1}".format(intf.bInterfaceNumber, str(e)))

        # 设置设备配置
        self.device.set_configuration()

    def usb_open(self):
        # 打开设备的接口
        usb.util.claim_interface(self.device, self.interface)

    def usb_close(self):
        # 释放设备的接口
        usb.util.release_interface(self.device, self.interface)
        usb.util.dispose_resources(self.device)

    def usb_get_endpoint(self):
        # 获取配置描述符
        config = self.device.get_active_configuration()

        # 遍历接口
        for interface in config:
            # 遍历端点
            for endpoint in interface:
                # 检查端点方向
                if usb.util.endpoint_direction(endpoint.bEndpointAddress) == usb.util.ENDPOINT_IN:
                    self.endpoint_in = endpoint.bEndpointAddress
                else:
                    self.endpoint_out = endpoint.bEndpointAddress

        #打印端点地址
        print("端点地址: 0x{:02x}, 0x{:02x}".format(self.endpoint_out, self.endpoint_in))



    def usb_send_data(self, data):
        # 发送数据到设备
        self.device.write(self.endpoint_out, data)

    def usb_read_data(self, length):
        # 从设备读取数据
        return self.device.read(self.endpoint_in, length)