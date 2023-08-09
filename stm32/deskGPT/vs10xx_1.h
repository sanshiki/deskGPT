/**
 ****************************************************************************************************
 * @file        vs10xx.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-29
 * @brief       VS10XX ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200429
 * ��һ�η���
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
/* VS10XX RST/XCS/XDCS/DQ ���� ���� */

// #define VS10XX_RST_GPIO_Port            GPIOE
// #define VS10XX_RST_Pin             GPIO_PIN_6
#define VS10XX_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */

// #define VS10XX_XCS_GPIO_Port            GPIOF
// #define VS10XX_XCS_Pin             GPIO_PIN_7
#define VS10XX_XCS_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF��ʱ��ʹ�� */

// #define VS10XX_XDCS_GPIO_Port           GPIOF
// #define VS10XX_XDCS_Pin            GPIO_PIN_6
#define VS10XX_XDCS_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF��ʱ��ʹ�� */

// #define VS10XX_DQ_GPIO_Port             GPIOC
// #define VS10XX_DQ_Pin              GPIO_PIN_13
#define VS10XX_DQ_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* PC��ʱ��ʹ�� */

/******************************************************************************************/

/* VS10XX �����ź� */
#define VS10XX_RST(x)       do{ x ? \
                                  HAL_GPIO_WritePin(VS10XX_RST_GPIO_Port, VS10XX_RST_Pin, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(VS10XX_RST_GPIO_Port, VS10XX_RST_Pin, GPIO_PIN_RESET); \
                            }while(0)

#define VS10XX_XCS(x)       do{ x ? \
                                  HAL_GPIO_WritePin(VS10XX_XCS_GPIO_Port, VS10XX_XCS_Pin, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(VS10XX_XCS_GPIO_Port, VS10XX_XCS_Pin, GPIO_PIN_RESET); \
                            }while(0)

#define VS10XX_XDCS(x)      do{ x ? \
                                  HAL_GPIO_WritePin(VS10XX_XDCS_GPIO_Port, VS10XX_XDCS_Pin, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(VS10XX_XDCS_GPIO_Port, VS10XX_XDCS_Pin, GPIO_PIN_RESET); \
                            }while(0)

#define VS10XX_DQ           HAL_GPIO_ReadPin(VS10XX_DQ_GPIO_Port, VS10XX_DQ_Pin)

typedef __PACKED_STRUCT
{
    uint8_t mvol;           /* ������,        ��Χ:0~254 */
    uint8_t bflimit;        /* ����Ƶ���޶�,  ��Χ:2~15 (��λ:10Hz) */
    uint8_t bass;           /* ����,          ��Χ:0~15 0��ʾ�ر�(��λ:1dB) */
    uint8_t tflimit;        /* ����Ƶ���޶�,  ��Χ:1~15(��λ:Khz) */
    uint8_t treble;         /* ����,          ��Χ:0~15(��λ:1.5dB)(ԭ����Χ��:-8~7,ͨ�������޸���) */
    uint8_t effect;         /* �ռ�Ч������.  0,�ر�; 1,��С; 2,�е�; 3,��� */
    uint8_t speakersw;      /* �������ȿ���,  0,�ر�; 1,�� */
    uint8_t saveflag;       /* �����־,      0X0A,�������; ����,����δ���� */
} _vs10xx_obj;


extern _vs10xx_obj vsset;   /* VS10XX���� */

/* VS10XX ��д���� */
#define VS_WRITE_COMMAND    0x02
#define VS_READ_COMMAND     0x03

/* VS10XX �Ĵ������� */
#define SPI_MODE            0x00
#define SPI_STATUS          0x01
#define SPI_BASS            0x02
#define SPI_CLOCKF          0x03
#define SPI_DECODE_TIME     0x04
#define SPI_AUDATA          0x05
#define SPI_WRAM            0x06
#define SPI_WRAMADDR        0x07
#define SPI_HDAT0           0x08
#define SPI_HDAT1           0x09

#define SPI_AIADDR          0x0a
#define SPI_VOL             0x0b
#define SPI_AICTRL0         0x0c
#define SPI_AICTRL1         0x0d
#define SPI_AICTRL2         0x0e
#define SPI_AICTRL3         0x0f
#define SM_DIFF             0x01
#define SM_JUMP             0x02
#define SM_RESET            0x04
#define SM_OUTOFWAV         0x08
#define SM_PDOWN            0x10
#define SM_TESTS            0x20
#define SM_STREAM           0x40
#define SM_PLUSV            0x80
#define SM_DACT             0x100
#define SM_SDIORD           0x200
#define SM_SDISHARE         0x400
#define SM_SDINEW           0x800
#define SM_ADPCM            0x1000
#define SM_ADPCM_HP         0x2000

#define I2S_CONFIG          0XC040
#define GPIO_DDR            0XC017
#define GPIO_IDATA          0XC018
#define GPIO_ODATA          0XC019

/* SPI�����ٶ����� */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7


/* ��̬���� */
static uint16_t vs10xx_get_fill_byte(void);         /* ��ȡ����ֽ� */
static uint16_t vs10xx_read_ram(uint16_t address);  /* ��RAM */
static void vs10xx_write_ram(uint16_t address, uint16_t data);  /* дRAM */


/* �ӿں��� */
void vs10xx_restart_play(void);         /* �и� */
uint16_t vs10xx_get_bitrate(void);      /* ��ȡ������ */
uint16_t vs10xx_get_byterate(void);     /* ��ȡ�ֽ����� */
void vs10xx_reset_decode_time(void);    /* ���ý���ʱ�� */
uint16_t vs10xx_get_decode_time(void);  /* ��ȡ����ʱ�� */
void vs10xx_set_speed(uint8_t speed);   /* ���ò����ٶ� */
uint16_t vs10xx_read_reg(uint8_t address);              /* ���Ĵ��� */
uint8_t vs10xx_send_music_data(uint8_t *buf);           /* ������Ƶ���� */
void vs10xx_load_patch(uint16_t *patch, uint16_t len);  /* ����PATCH */
void vs10xx_write_cmd(uint8_t address, uint16_t data);   /* д���� */

void vs10xx_set_all(void);              /* VS10XX ���ֲ������� */
void vs10xx_set_speaker(uint8_t sw);    /* ���ð������� */
void vs10xx_set_volume(uint8_t volx);   /* ���������� */
void vs10xx_set_effect(uint8_t effect); /* ���ÿռ�Ч�� */
void vs10xx_set_tone(uint8_t bfreq, uint8_t bass, uint8_t tfreq, uint8_t treble);   /* �������� */

void vs10xx_init(void);             /* ��ʼ��VS10XX */
uint8_t vs10xx_reset(void);         /* Ӳ����λVS10XX */
void vs10xx_soft_reset(void);       /* �����λVS10XX */
void vs10xx_sine_test(void);        /* ���Ҳ��� */
uint16_t vs10xx_ram_test(void);     /* RAM���� */

void vs10xx_spi_speed_low(void);    /* SPI���� */
void vs10xx_spi_speed_high(void);   /* SPI���� */
uint8_t vs10xx_spi_read_write_byte(uint8_t data);   /* SPI��дһ���ֽ� */



#endif

















