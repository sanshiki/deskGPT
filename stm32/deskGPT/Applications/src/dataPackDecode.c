#include "dataPackDecode.h"
#include <string.h>

usb_datapack_decode_t pcDataPack;

uint8_t datapack_init_flag = 0;

void dataPackInit(usb_datapack_decode_t *pdataPack)
{
    pdataPack->head[0] = 0x32;
    pdataPack->head[1] = 0x10;
    pdataPack->head[2] = 0x10;
    pdataPack->head[3] = 0x57;
    pdataPack->head[4] = 0x05;
    pdataPack->head_len = 5;
    pdataPack->ctrl_len = 1;
    pdataPack->spi_enable = 1;
    pdataPack->record_enable = 0;
    pdataPack->play_enable = 0;
    pdataPack->lost_data_test_enable = 0;
    pdataPack->play_complete = 0;
}

uint8_t checkDataHead(uint8_t *buf, usb_datapack_decode_t *pdataPack)
{
    if (datapack_init_flag == 0)
    {
        dataPackInit(pdataPack);
        datapack_init_flag = 1;
    }
    for (int i = 0; i < pdataPack->head_len; i++)
    {
        if (buf[i] != pdataPack->head[i])
        {
            return 0;
        }
    }
    return 1;
}

void checkSpiCtrl(uint8_t *buf, usb_datapack_decode_t *pdataPack)
{
    uint8_t ctrl = buf[pdataPack->head_len];

    // 第七位：spi_enable
    pdataPack->spi_enable = (ctrl >> 7) & 0x01;
    // 第六位：record_enable
    pdataPack->record_enable = (ctrl >> 6) & 0x01;
    // 第五位：play_enable
    pdataPack->play_enable = (ctrl >> 5) & 0x01;
    // 第四位：lost_data_test_enable
    pdataPack->lost_data_test_enable = (ctrl >> 4) & 0x01;
    // 第三位：play_complete
    pdataPack->play_complete = (ctrl >> 3) & 0x01;
}
void getAudioData(uint8_t *audioDataBuf, uint8_t *buf, usb_datapack_decode_t *pdataPack)
{
    //音频数据在校验头和控制字之后
    memcpy(audioDataBuf, buf + pdataPack->head_len + pdataPack->ctrl_len, AUDIO_DATA_BUF_SIZE);
    // for(int i = 0; i < AUDIO_DATA_BUF_SIZE; i++)
    // {
    //     audioDataBuf[i] = buf[pdataPack->head_len + pdataPack->ctrl_len + i];
    // }
}