#include <stdio.h>
#include <libusb-1.0/libusb.h>

#define MAX_TRANSFER_SIZE 64

void transfer_callback(struct libusb_transfer* transfer)
{
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
        // 异步传输完成
        printf("异步传输完成\n");
        
        // 处理接收到的数据
        for (int i = 0; i < transfer->actual_length; i++) {
            printf("%02x ", transfer->buffer[i]);
        }
        printf("\n");
    } else {
        // 异步传输出错
        printf("异步传输出错");
    }
    
    // 释放传输请求
    libusb_free_transfer(transfer);
}

#define VENDOR_ID 0x0483
#define PRODUCT_ID 0x5740

int main() {
    libusb_context* context = NULL;
    libusb_device_handle* dev_handle = NULL;
    int ret;

    // 初始化libusb
    ret = libusb_init(&context);
    if (ret != LIBUSB_SUCCESS) {
        printf("初始化libusb失败: %s\n", libusb_strerror((libusb_error)ret));
        return 1;
    }

    // 打开设备
    dev_handle = libusb_open_device_with_vid_pid(context, VENDOR_ID, PRODUCT_ID);
    if (dev_handle == NULL) {
        printf("无法打开设备\n");
        libusb_exit(context);
        return 1;
    }

    // 分配传输请求
    struct libusb_transfer* transfer = libusb_alloc_transfer(0);
    if (transfer == NULL) {
        printf("无法分配传输请求\n");
        libusb_close(dev_handle);
        libusb_exit(context);
        return 1;
    }

    // 分配传输缓冲区
    unsigned char buffer[MAX_TRANSFER_SIZE];
    libusb_fill_bulk_transfer(transfer, dev_handle, ENDPOINT_IN, buffer, MAX_TRANSFER_SIZE, transfer_callback, NULL, 0);

    // 提交异步传输请求
    ret = libusb_submit_transfer(transfer);
    if (ret != LIBUSB_SUCCESS) {
        printf("提交异步传输请求失败: %s\n", libusb_strerror((libusb_error)ret));
        libusb_free_transfer(transfer);
        libusb_close(dev_handle);
        libusb_exit(context);
        return 1;
    }

    // 等待异步传输完成
    ret = libusb_handle_events(context);
    if (ret != LIBUSB_SUCCESS) {
        printf("处理事件失败: %s\n", libusb_strerror((libusb_error)ret));
        libusb_cancel_transfer(transfer);
    }

    // 清理资源
    libusb_free_transfer(transfer);
    libusb_close(dev_handle);
    libusb_exit(context);

    return 0;
}
