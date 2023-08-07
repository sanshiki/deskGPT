// libusb header
#include <libusb-1.0/libusb.h>
#include <iostream>
using namespace std;

uint16_t pid;
uint16_t vid;

#define MAX_TRANSFER_SIZE 64 


class USB_CDC
{
    private:
        //basic params
        uint16_t pid;
        uint16_t vid;
        uint8_t ep_in;
        uint8_t ep_out;
        libusb_device_handle *dev_handle;
        struct libusb_transfer* transfer = NULL;

        //flags
        bool is_open = false;
        bool transfor_success = false;

        //buffers
        unsigned char *trans_buf = NULL;

    public:
        USB_CDC(uint16_t pid, uint16_t vid);
        void open_usb();
        void close_usb();
        int get_endpoint();
        uint8_t* read_usb(int len);
        int send_usb(uint8_t *data, int len);
};

void transferCallback(struct libusb_transfer* transfer)
{
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {
        // 异步传输完成
        cout << "异步传输完成" << endl;
        
        // 处理接收到的数据
        for (int i = 0; i < transfer->actual_length; i++) {
            cout << hex << (int)transfer->buffer[i] << " ";
        }
        cout << endl;
    } else {
        // 异步传输出错
        cout << "异步传输出错" << endl;
    }
    
    // 释放传输请求
    libusb_free_transfer(transfer);
}

USB_CDC::USB_CDC(uint16_t pid, uint16_t vid) : pid(pid), vid(vid)
{
}

// open usb
void USB_CDC::open_usb()
{
    libusb_init(NULL);
    libusb_set_debug(NULL, 3);
    dev_handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (dev_handle == NULL)
    {
        cout << "Cannot open device" << endl;
        return;
    }
    else
    {
        cout << "Device Opened" << endl;
    }

    //allocate transfer
    transfer = libusb_alloc_transfer(0);
    if(transfer == NULL) {
        cout << "libusb_alloc_transfer failed" << endl;
        transfor_success = false;
        return;
    }
    else
    {
        transfor_success = true;

        //get endpoint
        get_endpoint();

        libusb_fill_bulk_transfer(transfer, dev_handle, ep_in, trans_buf, MAX_TRANSFER_SIZE, transferCallback, NULL, 0);
    }
}

// close usb
void USB_CDC::close_usb()
{
    libusb_close(dev_handle);
    libusb_exit(NULL);
    return;
}

// get endpoint
int USB_CDC::get_endpoint() {
    struct libusb_config_descriptor *config;
    const struct libusb_interface *interface;
    const struct libusb_interface_descriptor *interface_desc;
    const struct libusb_endpoint_descriptor *endpoint_desc;
    int endpoint_count, i, j;

    int ret = libusb_get_active_config_descriptor(libusb_get_device(dev_handle), &config);
    if (ret != LIBUSB_SUCCESS) {
        printf("获取设备配置描述符失败: %s\n", libusb_strerror((libusb_error)ret));
        return libusb_error(ret);
    }

    // 遍历接口
    for (i = 0; i < config->bNumInterfaces; i++) {
        interface = &config->interface[i];
        // 遍历接口描述符
        for (j = 0; j < interface->num_altsetting; j++) {
            interface_desc = &interface->altsetting[j];
            // 遍历端点
            for (endpoint_count = 0; endpoint_count < interface_desc->bNumEndpoints; endpoint_count++) {
                endpoint_desc = &interface_desc->endpoint[endpoint_count];
                // 检查端点方向
                if ((endpoint_desc->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN) {
                    ep_in = endpoint_desc->bEndpointAddress;
                } else {
                    ep_out = endpoint_desc->bEndpointAddress;
                }
            }
        }
    }
    libusb_free_config_descriptor(config);

    return LIBUSB_SUCCESS;
}


// read usb
uint8_t* USB_CDC::read_usb(int len)
{
    int actual_length;
    uint8_t *data = new uint8_t[len];
    for(int i=0;i<len;i++) data[i] = 0;
    int r = libusb_bulk_transfer(dev_handle, ep_in, data, len, &actual_length, 0);
    if (r == 0 && actual_length == len)
    {
        // cout << "Read success" << endl;
        data[actual_length] = '\0';

    }
    else
    {
        cout << "Read error" << endl;
        // 将endpoint输出为16进制
        cout << "endpoint: 0x" << hex << ep_in << endl;
    }
    return data;
}

// send data to usb
int USB_CDC::send_usb(uint8_t *data, int len)
{
    int actual_length;
    int r = libusb_bulk_transfer(dev_handle, ep_out, data, len, &actual_length, 0);
    if (r == 0 && actual_length == len)
    {
        // cout << "Send success" << endl;
        return 0;
    }
    else
    {
        cout << "Send error" << endl;
        // 将endpoint输出为16进制
        cout << "endpoint: 0x" << hex << ep_out << endl;
        return -1;
    }
}

int main(void)
{
    libusb_device_handle *dev_handle;
    int r;

    int wait_cnt = 0;

    vid = 0x0483;
    pid = 0x5740;

    USB_CDC usb_cdc(pid, vid);
    usb_cdc.open_usb();
    usb_cdc.get_endpoint();

    while(true)
    {
        usb_cdc.send_usb((uint8_t *)"Hello World", 11);
        uint8_t *data = usb_cdc.read_usb(11);
        cout << *data << endl;
        break;
    }

    usb_cdc.close_usb();


    return 0;
}