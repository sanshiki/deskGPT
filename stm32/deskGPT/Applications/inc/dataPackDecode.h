#ifndef __DATAPACKDECODE_H__
#define __DATAPACKDECODE_H__

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "delay.h"
#include "dataQueue.h"

typedef struct __pc_usb_datapack_decode_struct
{
    uint8_t data[DATA_SIZE];
    uint8_t head[5];
    uint8_t head_len;

    // spi control
    uint8_t spi_enable;
    uint8_t record_enable;
    uint8_t play_enable;
    uint8_t lost_data_test_enable;
} usb_datapack_decode_t;

void dataPackInit(usb_datapack_decode_t *pdataPack);
uint8_t checkDataHead(uint8_t *buf, usb_datapack_decode_t *pdataPack);
void checkSpiCtrl(uint8_t *buf, usb_datapack_decode_t *pdataPack);

#endif // __DATAPACKDECODE_H__
