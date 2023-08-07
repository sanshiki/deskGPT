/**
 ****************************************************************************************************
 * @file        atk_mo1053.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-06-21
 * @brief       ATK_MO1053 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200429
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __VS10XX_H
#define __VS10XX_H

// #include "./SYSTEM/sys/sys.h"
#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/******************************************************************************************/
/* VS10XX RST/XCS/XDCS/DQ 引脚 定义 */

// #define VS10XX_RST_GPIO_Port GPIOC
// #define VS10XX_RST_Pin GPIO_PIN_0
#define VS10XX_RST_GPIO_CLK_ENABLE()  \
    do                                \
    {                                 \
        __HAL_RCC_GPIOC_CLK_ENABLE(); \
    } while (0) /* PE口时钟使能 */

// #define VS10XX_XCS_GPIO_Port GPIOB
// #define VS10XX_XCS_Pin GPIO_PIN_10
#define VS10XX_XCS_GPIO_CLK_ENABLE()  \
    do                                \
    {                                 \
        __HAL_RCC_GPIOB_CLK_ENABLE(); \
    } while (0) /* PF口时钟使能 */

// #define VS10XX_XDCS_GPIO_Port GPIOB
// #define VS10XX_XDCS_Pin GPIO_PIN_11
#define VS10XX_XDCS_GPIO_CLK_ENABLE() \
    do                                \
    {                                 \
        __HAL_RCC_GPIOB_CLK_ENABLE(); \
    } while (0) /* PF口时钟使能 */

// #define VS10XX_DQ_GPIO_Port GPIOF
// #define VS10XX_DQ_Pin GPIO_PIN_6
#define VS10XX_DQ_GPIO_CLK_ENABLE()   \
    do                                \
    {                                 \
        __HAL_RCC_GPIOF_CLK_ENABLE(); \
    } while (0) /* PC口时钟使能 */

/******************************************************************************************/

/* VS10XX 控制信号 */
#define VS10XX_RST(x)                                                                                                                                        \
    do                                                                                                                                                       \
    {                                                                                                                                                        \
        x ? HAL_GPIO_WritePin(VS10XX_RST_GPIO_Port, VS10XX_RST_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(VS10XX_RST_GPIO_Port, VS10XX_RST_Pin, GPIO_PIN_RESET); \
    } while (0)

#define VS10XX_XCS(x)                                                                                                                                        \
    do                                                                                                                                                       \
    {                                                                                                                                                        \
        x ? HAL_GPIO_WritePin(VS10XX_XCS_GPIO_Port, VS10XX_XCS_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(VS10XX_XCS_GPIO_Port, VS10XX_XCS_Pin, GPIO_PIN_RESET); \
    } while (0)

#define VS10XX_XDCS(x)                                                                                                                                           \
    do                                                                                                                                                           \
    {                                                                                                                                                            \
        x ? HAL_GPIO_WritePin(VS10XX_XDCS_GPIO_Port, VS10XX_XDCS_Pin, GPIO_PIN_SET) : HAL_GPIO_WritePin(VS10XX_XDCS_GPIO_Port, VS10XX_XDCS_Pin, GPIO_PIN_RESET); \
    } while (0)

#define VS10XX_DQ HAL_GPIO_ReadPin(VS10XX_DQ_GPIO_Port, VS10XX_DQ_Pin)

typedef __PACKED_STRUCT
{
    uint8_t mvol;      /* 主音量,        范围:0~254 */
    uint8_t bflimit;   /* 低音频率限定,  范围:2~15 (单位:10Hz) */
    uint8_t bass;      /* 低音,          范围:0~15 0表示关闭(单位:1dB) */
    uint8_t tflimit;   /* 高音频率限定,  范围:1~15(单位:Khz) */
    uint8_t treble;    /* 高音,          范围:0~15(单位:1.5dB)(原本范围是:-8~7,通过函数修改了) */
    uint8_t effect;    /* 空间效果设置.  0,关闭; 1,最小; 2,中等; 3,最大 */
    uint8_t speakersw; /* 板载喇叭开关,  0,关闭; 1,打开 */
    uint8_t saveflag;  /* 保存标志,      0X0A,保存过了; 其他,还从未保存 */
}
_vs10xx_obj;

extern _vs10xx_obj vsset; /* VS10XX设置 */

/* VS10XX 读写命令 */
#define VS_WRITE_COMMAND 0x02
#define VS_READ_COMMAND 0x03

/* VS10XX 寄存器定义 */
#define SPI_MODE 0x00
#define SPI_STATUS 0x01
#define SPI_BASS 0x02
#define SPI_CLOCKF 0x03
#define SPI_DECODE_TIME 0x04
#define SPI_AUDATA 0x05
#define SPI_WRAM 0x06
#define SPI_WRAMADDR 0x07
#define SPI_HDAT0 0x08
#define SPI_HDAT1 0x09

#define SPI_AIADDR 0x0a
#define SPI_VOL 0x0b
#define SPI_AICTRL0 0x0c
#define SPI_AICTRL1 0x0d
#define SPI_AICTRL2 0x0e
#define SPI_AICTRL3 0x0f
#define SM_DIFF 0x01
#define SM_JUMP 0x02
#define SM_RESET 0x04
#define SM_OUTOFWAV 0x08
#define SM_PDOWN 0x10
#define SM_TESTS 0x20
#define SM_STREAM 0x40
#define SM_PLUSV 0x80
#define SM_DACT 0x100
#define SM_SDIORD 0x200
#define SM_SDISHARE 0x400
#define SM_SDINEW 0x800
#define SM_ADPCM 0x1000
#define SM_ADPCM_HP 0x2000

#define I2S_CONFIG 0XC040
#define GPIO_DDR 0XC017
#define GPIO_IDATA 0XC018
#define GPIO_ODATA 0XC019

// /* MODE寄存器位定义 */
// #define SM_DIFF 0x00
// #define SM_LAYER12 0x01
// #define SM_RESET 0x02
// #define SM_CANCEL 0x04
// #define SM_EARSPEAKER_LO 0x08
// #define SM_TESTS 0x10
// #define SM_STREAM 0x20
// #define SM_EARSPEAKER_HI 0x40
// #define SM_DACT 0x100
// #define SM_SDIORD 0x200
// #define SM_SDISHARE 0x400
// #define SM_SDINEW 0x800
// #define SM_ADPCM 0x1000
// #define SM_ADPCM_HP 0x2000
#define SM_LINE_IN 0x4000
#define SM_CLK_RANGE 0x8000


/* SPI总线速度设置 */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7

/* 函数错误代码 */
#define ATK_MO1053_EOK 0  /* 没有错误 */
#define ATK_MO1053_EACK 1 /* SPI通讯错误 */

/* 静态函数 */
static uint16_t atk_mo1053_get_fill_byte(void);                    /* 获取填充字节 */
static uint16_t atk_mo1053_read_ram(uint16_t address);             /* 读RAM */
static void atk_mo1053_write_ram(uint16_t address, uint16_t data); /* 写RAM */

/* 接口函数 */
void atk_mo1053_restart_play(void);                        /* 切歌 */
uint16_t atk_mo1053_get_bitrate(void);                     /* 获取比特率 */
uint16_t atk_mo1053_get_byterate(void);                    /* 获取字节速率 */
void atk_mo1053_reset_decode_time(void);                   /* 重置解码时间 */
uint16_t atk_mo1053_get_decode_time(void);                 /* 获取解码时间 */
void atk_mo1053_set_speed(uint8_t speed);                  /* 设置播放速度 */
uint16_t atk_mo1053_read_reg(uint8_t address);             /* 读寄存器 */
uint8_t atk_mo1053_send_music_data(uint8_t *buf);          /* 发送音频数据 */
void atk_mo1053_load_patch(uint16_t *patch, uint16_t len); /* 加载PATCH */
void atk_mo1053_write_cmd(uint8_t address, uint16_t data); /* 写命令 */

void atk_mo1053_set_all(void);                                                        /* VS10XX 各种参数设置 */
void atk_mo1053_set_speaker(uint8_t sw);                                              /* 设置板载喇叭 */
void atk_mo1053_set_volume(uint8_t volx);                                             /* 设置主音量 */
void atk_mo1053_set_effect(uint8_t effect);                                           /* 设置空间效果 */
void atk_mo1053_set_tone(uint8_t bfreq, uint8_t bass, uint8_t tfreq, uint8_t treble); /* 设置音调 */

uint8_t atk_mo1053_init(void);      /* 初始化VS10XX */
uint8_t atk_mo1053_reset(void);     /* 硬件复位VS10XX */
void atk_mo1053_soft_reset(void);   /* 软件复位VS10XX */
void atk_mo1053_sine_test(void);    /* 正弦测试 */
uint16_t atk_mo1053_ram_test(void); /* RAM测试 */

void atk_mo1053_spi_speed_low(void);                  /* SPI低速 */
void atk_mo1053_spi_speed_high(void);                 /* SPI高速 */
uint8_t atk_mo1053_spi_read_write_byte(uint8_t data); /* SPI读写一个字节 */

#endif
