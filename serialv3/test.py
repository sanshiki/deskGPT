import usb.core
import usb.util
import asyncio

async def transfer_callback(transfer):
    try:
        if transfer.status == usb.TRANSFER_COMPLETED:
            # 异步传输完成
            print("异步传输完成")
            
            # 处理接收到的数据
            for data in transfer:
                print(data)
        else:
            # 异步传输出错
            print("异步传输出错:", usb.libusb_error_name(transfer.status))
    finally:
        # 释放传输请求
        transfer.close()

async def main():
    dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
    if dev is None:
        raise ValueError("设备未找到")

    interface = 0  # CDC设备的接口号
    endpoint_in = 0x81  # 输入端点地址

    # 设置设备配置
    usb.util.claim_interface(dev, interface)

    # 分配传输请求
    transfer = dev.get_transfer()
    transfer.set_endpoint(endpoint_in)
    transfer.set_callback(transfer_callback)

    # 提交异步传输请求
    transfer.submit()

    # 等待异步传输完成
    while not transfer.is_completed():
        await asyncio.sleep(0.1)

    # 清理资源
    usb.util.release_interface(dev, interface)
    usb.util.dispose_resources(dev)

if __name__ == "__main__":
    # 替换成你的设备的Vendor ID和Product ID
    VENDOR_ID = 0x0483
    PRODUCT_ID = 0x5740

    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())
