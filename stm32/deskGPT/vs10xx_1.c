/**
 ****************************************************************************************************
 * @file        vs10xx.c
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

#include "spi.h"
#include "vs10xx_1.h"
#include "delay.h"


/* VS10XXĬ�����ò��� */
static _vs10xx_obj vsset =
{
        220, /* ����     220 */
        6,   /* �������� 60Hz */
        15,  /* �������� 15dB */
        10,  /* �������� 10Khz */
        15,  /* �������� 10.5dB */
        0,   /* �ռ�Ч�� �ر� */
        1,   /* ��������Ĭ�ϴ� */
};

/**
 * @brief       VS10XX SPI��д����
 * @param       data    : Ҫд�������
 * @retval      ���յ�������(1�ֽ�)
 */
uint8_t vs10xx_spi_read_write_byte(uint8_t data)
{
    return spi1_read_write_byte(data);
}

/**
 * @brief       VS10XX SPI����Ϊ����ģʽ(�����ʱ)
 * @param       ��
 * @retval      ��
 */
void vs10xx_spi_speed_low(void)
{
    spi1_set_speed(SPI_SPEED_32); /* ���õ�����ģʽ */
}

/**
 * @brief       VS10XX SPI����Ϊ����ģʽ(���ݴ���ʱ)
 *   @note      ��VS1003B, ���ֵ���ܳ��� 49.152/6 Mhz = 8.192 Mhz
 *   @note      ��VS1053B, ���ֵ���ܳ��� 55.296/6 Mhz = 9.216 Mhz
 * @param       ��
 * @retval      ��
 */
void vs10xx_spi_speed_high(void)
{
    spi1_set_speed(SPI_SPEED_8); /* ���õ�����ģʽ */
}

/**
 * @brief       VS10XX ��ʼ��
 * @param       ��
 * @retval      ��
 */
void vs10xx_init(void)
{
    VS10XX_RST_GPIO_CLK_ENABLE();  /* VS10XX_RST  �� ʱ��ʹ�� */
    VS10XX_XCS_GPIO_CLK_ENABLE();  /* VS10XX_XCS  �� ʱ��ʹ�� */
    VS10XX_XDCS_GPIO_CLK_ENABLE(); /* VS10XX_XDCS �� ʱ��ʹ�� */
    VS10XX_DQ_GPIO_CLK_ENABLE();   /* VS10XX_DQ   �� ʱ��ʹ�� */

    // GPIO_InitTypeDef GPIO_Initure;
    // /* RST  ����ģʽ����,��� */
    // GPIO_Initure.Pin = VS10XX_RST_Pin;
    // GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    // GPIO_Initure.Pull = GPIO_PULLUP;
    // GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    // HAL_GPIO_Init(VS10XX_RST_GPIO_Port, &GPIO_Initure);
    // /* XCS  ����ģʽ����,��� */
    // GPIO_Initure.Pin = VS10XX_XCS_Pin;
    // HAL_GPIO_Init(VS10XX_XCS_GPIO_Port, &GPIO_Initure);
    // /* XDCS ����ģʽ���ã���� */
    // GPIO_Initure.Pin = VS10XX_XDCS_Pin;
    // HAL_GPIO_Init(VS10XX_XDCS_GPIO_Port, &GPIO_Initure);
    // /* DQ   ����ģʽ����,���� */
    // GPIO_Initure.Pin = VS10XX_DQ_Pin;
    // GPIO_Initure.Mode = GPIO_MODE_INPUT;
    // GPIO_Initure.Pull = GPIO_PULLUP;
    // GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    // HAL_GPIO_Init(VS10XX_DQ_GPIO_Port, &GPIO_Initure);

    // spi1_init();
}

/**
 * @brief       VS10XX ��λ
 * @param       ��
 * @retval      ��
 */
void vs10xx_soft_reset(void)
{
    uint8_t retry = 0;

    while (VS10XX_DQ == 0); /* �ȴ������λ���� */

    vs10xx_spi_read_write_byte(0Xff);           /* �������� */
    retry = 0;

    while (vs10xx_read_reg(SPI_MODE) != 0x0800) /*  �����λ,��ģʽ */
    {
        vs10xx_write_cmd(SPI_MODE, 0x0804);     /*  �����λ,��ģʽ */
        delay_ms(2);                            /* �ȴ�����1.35ms */

        if (retry++ > 100)
        {
            break;
        }
    }

    while (VS10XX_DQ == 0)
        ; /* �ȴ������λ���� */

    retry = 0;

    while (vs10xx_read_reg(SPI_CLOCKF) != 0X9800)   /* ����VS10XX��ʱ��,3��Ƶ ,1.5xADD */
    {
        vs10xx_write_cmd(SPI_CLOCKF, 0X9800);       /* ����VS10XX��ʱ��,3��Ƶ ,1.5xADD */

        if (retry++ > 100)
        {
            break;
        }
    }

    delay_ms(20);
}

/**
 * @brief       VS10XX Ӳ����λ
 * @param       ��
 * @retval      0, ��λ�ɹ�; 1, ��λʧ��;
 */
uint8_t vs10xx_reset(void)
{
    uint8_t retry = 0;

    VS10XX_RST(0);
    delay_ms(20);
    VS10XX_XDCS(1); /* ȡ�����ݴ��� */
    VS10XX_XCS(1);  /* ȡ�����ݴ��� */
    VS10XX_RST(1);

    while (VS10XX_DQ == 0 && retry < 200) /* �ȴ�DREQΪ�� */
    {
        retry++;
        delay_us(50);
    }

    delay_ms(20);

    if (retry >= 200)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       VS10XX ���Ҳ���
 * @param       ��
 * @retval      ��
 */
void vs10xx_sine_test(void)
{
    vs10xx_reset();
    vs10xx_write_cmd(0x0b, 0X2020);     /* �������� */
    vs10xx_write_cmd(SPI_MODE, 0x0820); /* ����VS10XX�Ĳ���ģʽ */

    while (VS10XX_DQ == 0); /* �ȴ�DREQΪ�� */

    /* printf("mode sin:%x\n", vs10xx_read_reg(SPI_MODE));
     * ��VS10XX�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
     * ����n = 0x24, �趨VS10XX�����������Ҳ���Ƶ��ֵ��������㷽����VS10XX��datasheet
     */
    vs10xx_spi_speed_low(); /* ���� */
    VS10XX_XDCS(0);         /* ѡ�����ݴ��� */
    vs10xx_spi_read_write_byte(0x53);
    vs10xx_spi_read_write_byte(0xef);
    vs10xx_spi_read_write_byte(0x6e);
    vs10xx_spi_read_write_byte(0x24);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    delay_ms(100);
    VS10XX_XDCS(1);
    /* �˳����Ҳ��� */
    VS10XX_XDCS(0); /* ѡ�����ݴ��� */
    vs10xx_spi_read_write_byte(0x45);
    vs10xx_spi_read_write_byte(0x78);
    vs10xx_spi_read_write_byte(0x69);
    vs10xx_spi_read_write_byte(0x74);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    delay_ms(100);
    VS10XX_XDCS(1);

    /* �ٴν������Ҳ��Բ�����nֵΪ0x44���������Ҳ���Ƶ������Ϊ�����ֵ */
    VS10XX_XDCS(0); /* ѡ�����ݴ��� */
    vs10xx_spi_read_write_byte(0x53);
    vs10xx_spi_read_write_byte(0xef);
    vs10xx_spi_read_write_byte(0x6e);
    vs10xx_spi_read_write_byte(0x44);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    delay_ms(100);
    VS10XX_XDCS(1);
    /* �˳����Ҳ��� */
    VS10XX_XDCS(0); /* ѡ�����ݴ��� */
    vs10xx_spi_read_write_byte(0x45);
    vs10xx_spi_read_write_byte(0x78);
    vs10xx_spi_read_write_byte(0x69);
    vs10xx_spi_read_write_byte(0x74);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    delay_ms(100);
    VS10XX_XDCS(1);
}

/**
 * @brief       VS10XX RAM����
 * @param       ��
 * @retval      ���Խ��
 *   @arg       0X807F, VS1003��������
 *   @arg       0X83FF, VS1053��������
 *   @arg       ����,   ����ʧ��
 */
uint16_t vs10xx_ram_test(void)
{
    vs10xx_reset();
    vs10xx_write_cmd(SPI_MODE, 0x0820); /*  ����VS10XX�Ĳ���ģʽ */

    while (VS10XX_DQ == 0); /*  �ȴ�DREQΪ�� */

    vs10xx_spi_speed_low(); /* ���� */
    VS10XX_XDCS(0);         /*  xDCS = 1��ѡ��VS10XX�����ݽӿ� */
    vs10xx_spi_read_write_byte(0x4d);
    vs10xx_spi_read_write_byte(0xea);
    vs10xx_spi_read_write_byte(0x6d);
    vs10xx_spi_read_write_byte(0x54);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    vs10xx_spi_read_write_byte(0x00);
    delay_ms(150);
    VS10XX_XDCS(1);
    return vs10xx_read_reg(SPI_HDAT0); /*  VS1003����õ���ֵΪ0x807F����������; VS1053Ϊ0X83FF */
}

/**
 * @brief       VS10XX д����
 * @param       address : �����ַ
 * @param       data    : ��������
 * @retval      ��
 */
void vs10xx_write_cmd(uint8_t address, uint16_t data)
{
    while (VS10XX_DQ == 0); /* �ȴ����� */

    vs10xx_spi_speed_low(); /* ���� */
    VS10XX_XDCS(1);
    VS10XX_XCS(0);
    vs10xx_spi_read_write_byte(VS_WRITE_COMMAND); /* ����VS10XX��д���� */
    vs10xx_spi_read_write_byte(address);          /* ��ַ */
    vs10xx_spi_read_write_byte(data >> 8);        /* ���͸߰�λ */
    vs10xx_spi_read_write_byte(data);             /* �Ͱ�λ */
    VS10XX_XCS(1);
    vs10xx_spi_speed_high(); /* ���� */
}

/**
 * @brief       VS10XX ���Ĵ���
 * @param       address : �Ĵ�����ַ
 * @retval      ��ȡ��������
 */
uint16_t vs10xx_read_reg(uint8_t address)
{
    uint16_t temp = 0;

    while (VS10XX_DQ == 0); /* �ǵȴ�����״̬ */

    vs10xx_spi_speed_low(); /* ���� */
    VS10XX_XDCS(1);
    VS10XX_XCS(0);
    vs10xx_spi_read_write_byte(VS_READ_COMMAND); /* ����VS10XX�Ķ����� */
    vs10xx_spi_read_write_byte(address);         /* ��ַ */
    temp = vs10xx_spi_read_write_byte(0xff);     /* ��ȡ���ֽ� */
    temp = temp << 8;
    temp += vs10xx_spi_read_write_byte(0xff);    /* ��ȡ���ֽ� */
    VS10XX_XCS(1);
    vs10xx_spi_speed_high(); /* ���� */
    return temp;
}

/**
 * @brief       VS10XX ��RAM
 * @param       address : RAM��ַ
 * @retval      ��ȡ��������
 */
static uint16_t vs10xx_read_ram(uint16_t address)
{
    uint16_t res;
    vs10xx_write_cmd(SPI_WRAMADDR, address);
    res = vs10xx_read_reg(SPI_WRAM);
    return res;
}

/**
 * @brief       VS10XX дRAM
 * @param       address : RAM��ַ
 * @param       data    : Ҫд���ֵ
 * @retval      ��
 */
static void vs10xx_write_ram(uint16_t address, uint16_t data)
{
    vs10xx_write_cmd(SPI_WRAMADDR, address); /* дRAM��ַ */

    while (VS10XX_DQ == 0);                  /* �ȴ����� */

    vs10xx_write_cmd(SPI_WRAM, data);        /* дRAMֵ */
}

/**
 * @brief       ���ò����ٶ�
 *   @note      �ú������� VS1053 ��Ч
 * @param       speed   : ���ű���
 *   @arg       0/1     : �����ٶ�
 *   @arg       ����    : speed��
 * @retval      ��
 */
void vs10xx_set_speed(uint8_t speed)
{
    vs10xx_write_ram(0X1E04, speed); /* д�벥���ٶ� */
}

/* ������Ƶͷ��־ 
 * WAV  HEAD0 : 0X7761      HEAD1 : 0X7665
 * MIDI HEAD0 : other info  HEAD1 : 0X4D54
 * WMA  HEAD0 : data speed  HEAD1 : 0X574D
 * MP3  HEAD0 : data speed  HEAD1 : ID
 * ������Ԥ��ֵ,�ײ�III
 */
static const uint16_t g_bitrate[2][16] =
{
    {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0},
    {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0}
};
/**
 * @brief       ���ص�ǰ��Ƶ������
 * @param       ��
 * @retval      ��ǰ��Ƶ����(Kbps)
 */
uint16_t vs10xx_get_bitrate(void)
{
    uint32_t head0;
    uint16_t head1;

    head0 = vs10xx_read_reg(SPI_HDAT0);
    head1 = vs10xx_read_reg(SPI_HDAT1);

    switch (head1)
    {
        case 0X4D54:            /* MIDI��ʽ */
        case 0X4154:            /* AAC_ADTS */
        case 0X4144:            /* AAC_ADIF */
        case 0X4D34:            /* AAC_MP4/M4A */
        case 0X4F67:            /* OGG */
        case 0X574D:            /* WMA��ʽ */
            return head0 / 125; /* �൱�� * 8 / 1000, �õ�Kbps��λ����Ƶ���� */

        case 0x7665:             /* WAV��ʽ */
            if (head0 == 0XFFFF) /* head0 = 0XFFFF, ˵�� ���ʳ�����uint16�ı�ﷶΧ */
            {
                return 1411; /* ���ڳ������̷�Χ��WAV�ļ�, ֱ��Ĭ��1411���� */
            }

        case 0X664C: /* FLAC��ʽ */
            /* ���� VLSIû����ȷ����FLAC�����ʼ��㹫ʽ, ����ʹ�þ�����㹫ʽ
                 * (head0 * 32) / 1000, �ù�ʽ�Ǹ���head0ʵ�ʵ�ֵ�Լ���Ƶʵ�ʲ�����
                 * �Ƶ�������, ���VLSI����ȷ�ļ��㷽����, ���������������ʽ
                 */
            return (head0 * 32) / 1000; /* �õ�Kbps��λ����Ƶ���� */

        default: /* MP3��ʽ, �����˽ײ�III�ı� */
        {
            head1 >>= 3;
            head1 = head1 & 0x03;

            if (head1 == 3)
            {
                head1 = 1;
            }
            else
            {
                head1 = 0;
            }

            return g_bitrate[head1][head0 >> 12];
        }
    }
}

/**
 * @brief       �õ�ƽ���ֽ���
 * @param       ��
 * @retval      ƽ���ֽ����ٶ�
 */
uint16_t vs10xx_get_byterate(void)
{
    return vs10xx_read_ram(0X1E05); /* ƽ��λ�� */
}

/**
 * @brief       �õ���Ҫ��������
 *   @note      �ڸ����л���ʱ��ʹ��
 * @param       ��
 * @retval      ��Ҫ��������
 */
static uint16_t vs10xx_get_fill_byte(void)
{
    return vs10xx_read_ram(0X1E06); /* ����ֽ� */
}

/**
 * @brief       ����һ����Ƶ����
 *   @note      �̶�Ϊ32�ֽڳ���
 * @param       buf     : ��Ƶ���ݻ������׵�ַ
 * @retval      ���ͽ��
 *   @arg       0, ���ͳɹ�
 *   @arg       1, VS10XX������Ҫ����, ���η���δ���
 *
 */
uint8_t vs10xx_send_music_data(uint8_t *buf)
{
    uint8_t n;

    if (VS10XX_DQ != 0) /* �Ƿ���Ҫ�������ݸ� VS10XX ? */
    {
        VS10XX_XDCS(0);

        for (n = 0; n < 32; n++) /* ����32�ֽ���Ƶ���� */
        {
            vs10xx_spi_read_write_byte(buf[n]);
        }

        VS10XX_XDCS(1);
    }
    else
    {
        return 1;
    }

    return 0; /* �ɹ������� */
}

/**
 * @brief       �и�
 *   @note      ͨ���˺����и�, ��������л���������
 * @param       ��
 * @retval      ��
 */
void vs10xx_restart_play(void)
{
    uint16_t temp;
    uint16_t i;
    uint8_t n;
    uint8_t vsbuf[32];

    for (n = 0; n < 32; n++)
    {
        vsbuf[n] = 0; /* ���� */
    }

    temp = vs10xx_read_reg(SPI_MODE); /* ��ȡSPI_MODE������ */
    temp |= 1 << 3;                   /* ����SM_CANCELλ */
    temp |= 1 << 2;                   /* ����SM_LAYER12λ,������MP1,MP2 */
    vs10xx_write_cmd(SPI_MODE, temp); /* ����ȡ����ǰ����ָ�� */

    for (i = 0; i < 2048;) /* ����2048��0,�ڼ��ȡSM_CANCELλ.���Ϊ0,���ʾ�Ѿ�ȡ���˵�ǰ���� */
    {
        if (vs10xx_send_music_data(vsbuf) == 0) /* ÿ����32���ֽں���һ�� */
        {
            i += 32;                          /* ������32���ֽ� */
            temp = vs10xx_read_reg(SPI_MODE); /* ��ȡSPI_MODE������ */

            if ((temp & (1 << 3)) == 0)
            {
                break; /* �ɹ�ȡ���� */
            }
        }
    }

    if (i < 2048) /* SM_CANCEL���� */
    {
        temp = vs10xx_get_fill_byte() & 0xff; /* ��ȡ����ֽ� */

        for (n = 0; n < 32; n++)
        {
            vsbuf[n] = temp; /* ����ֽڷ������� */
        }

        for (i = 0; i < 2052;)
        {
            if (vs10xx_send_music_data(vsbuf) == 0)
            {
                i += 32; /* ��� */
            }
        }
    }
    else
    {
        vs10xx_soft_reset(); /* SM_CANCEL���ɹ�,�����,��Ҫ��λ */
    }

    temp = vs10xx_read_reg(SPI_HDAT0);
    temp += vs10xx_read_reg(SPI_HDAT1);

    if (temp) /* ��λ,����û�гɹ�ȡ��,��ɱ���,Ӳ��λ */
    {
        vs10xx_reset();      /* Ӳ��λ */
        vs10xx_soft_reset(); /* ��λ */
    }
}

/**
 * @brief       �������ʱ��
 * @param       ��
 * @retval      ��
 */
void vs10xx_reset_decode_time(void)
{
    vs10xx_write_cmd(SPI_DECODE_TIME, 0x0000);
    vs10xx_write_cmd(SPI_DECODE_TIME, 0x0000); /* �������� */
}

/**
 * @brief       �õ���ǰ��Ƶ���ŵ�ʱ��
 * @param       ��
 * @retval      ����ʱ��(��λ: s)
 */
uint16_t vs10xx_get_decode_time(void)
{
    uint16_t dt = 0;
    dt = vs10xx_read_reg(SPI_DECODE_TIME);
    return dt;
}

/**
 * @brief       ����PATCH
 *   @note      �������FLAC���ŵ�PATCH ,����Ƶ�׷�����PATCH��
 * @param       patch   : patch�׵�ַ
 * @param       len     : patch����
 * @retval      ��
 */
void vs10xx_load_patch(uint16_t *patch, uint16_t len)
{
    uint16_t i;
    uint16_t addr, n, val;

    for (i = 0; i < len;)
    {
        addr = patch[i++];
        n = patch[i++];

        if (n & 0x8000U) /* RLE run, replicate n samples */
        {
            n &= 0x7FFF;
            val = patch[i++];

            while (n--)
            {
                vs10xx_write_cmd(addr, val);
            }
        }
        else /* copy run, copy n sample */
        {
            while (n--)
            {
                val = patch[i++];
                vs10xx_write_cmd(addr, val);
            }
        }
    }
}

/**
 * @brief       ����������С
 * @param       volx    : ������С(0~254)
 * @retval      ��
 */
void vs10xx_set_volume(uint8_t volx)
{
    uint16_t volt = 0; /* �ݴ�����ֵ */
    volt = 254 - volx; /* ȡ��һ��,�õ����ֵ,��ʾ���ı�ʾ */
    volt <<= 8;
    volt += 254 - volx;              /* �õ��������ú��С */
    vs10xx_write_cmd(SPI_VOL, volt); /* �������� */
}

/**
 * @brief       ���øߵ���(����)
 * @param       bfreq   : ��Ƶ����Ƶ�� 2~15(��λ:10Hz)
 * @param       bass    : ��Ƶ����     0~15(��λ:1dB)
 * @param       tfreq   : ��Ƶ����Ƶ�� 1~15(��λ:1KHz)
 * @param       treble  : ��Ƶ����     0~15(��λ:1.5dB,С��9��ʱ��Ϊ����)
 * @retval      ��
 */
void vs10xx_set_tone(uint8_t bfreq, uint8_t bass, uint8_t tfreq, uint8_t treble)
{
    uint16_t bass_set = 0; /* �ݴ������Ĵ���ֵ */
    signed char temp = (signed char)treble - 8;

    bass_set = temp & 0X0F; /* �����趨 */
    bass_set <<= 4;
    bass_set += tfreq & 0xf; /* ��������Ƶ�� */
    bass_set <<= 4;
    bass_set += bass & 0xf; /* �����趨 */
    bass_set <<= 4;
    bass_set += bfreq & 0xf;              /* �������� */
    vs10xx_write_cmd(SPI_BASS, bass_set); /* BASS */
}

/**
 * @brief       �趨��Ч
 * @param       effect  : 0, �ر�; 1, ��С; 2, �е�; 3, ���;
 * @retval      ��
 */
void vs10xx_set_effect(uint8_t effect)
{
    uint16_t temp;
    temp = vs10xx_read_reg(SPI_MODE); /* ��ȡSPI_MODE������ */

    temp &= ~(1 << 4); /* ȡ�� LO */
    temp &= ~(1 << 7); /* ȡ�� HI */

    if (effect & 0X01)
    {
        temp |= 1 << 4; /* �趨LO */
    }

    if (effect & 0X02)
    {
        temp |= 1 << 7; /* �趨 HI */
    }

    vs10xx_write_cmd(SPI_MODE, temp); /* �趨ģʽ */
}

/**
 * @brief       �������ȿ�/�����ú���
 *   @note      ս�������� ������HT6872����, ͨ��VS1053��GPIO4(36��), �����乤��/�ر�
 *              �ú���ʵ������VS1053��GPIO���ú���
 *              GPIO4 = 1, HT6872��������
 *              GPIO4 = 0, HT6872�ر�(Ĭ��)
 * @param       sw      : 0, �ر�; 1, ����;
 * @retval      ��
 */
void vs10xx_set_speaker(uint8_t sw)
{
    vs10xx_write_ram(GPIO_DDR, 1 << 4);    /* VS1053��GPIO4���ó���� */
    vs10xx_write_ram(GPIO_ODATA, sw << 4); /* ����VS1053��GPIO4���ֵ(0/1) */
}

/**
 * @brief       VS1053 ���ֲ�������
 *   @note      ��������/�ߵ���/�ռ�Ч��/�������������
 * @param       ��
 * @retval      ��
 */
void vs10xx_set_all(void)
{
    vs10xx_set_volume(vsset.mvol);                                           /* �������� */
    vs10xx_set_tone(vsset.bflimit, vsset.bass, vsset.tflimit, vsset.treble); /* �ߵ������� */
    vs10xx_set_effect(vsset.effect);                                         /* ���ÿռ�Ч�� */
    vs10xx_set_speaker(vsset.speakersw);                                     /* ���ư�������״̬ */
}
