/**
 ****************************************************************************************************
 * @file        atk_mo1053.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-10-22
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

#include "spi.h"
#include "atk_mo1053.h"
#include "delay.h"

/* ATK_MO1053 默认设置参数 */
static _vs10xx_obj vsset =
    {
        220, /* 音量     249(100~250) */
        6,   /* 低音上限 60Hz */
        15,  /* 低音提升 15dB */
        10,  /* 高音下限 10Khz */
        15,  /* 高音提升 10.5dB */
        0,   /* 空间效果 关闭 */
        1,   /* 板载喇叭默认打开 */
};

/**
 * @brief       SPI1读写一个字节数据
 * @param       txdata  : 要发送的数据(1字节)
 * @retval      接收到的数据(1字节)
 */
uint8_t spi1_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rxdata, 1, 1000);
    return rxdata; /* 返回收到的数据 */
}

/**
 * @brief       ATK_MO1053 SPI读写函数
 * @param       data    : 要写入的数据
 * @retval      接收到的数据(1字节)
 */
uint8_t atk_mo1053_spi_read_write_byte(uint8_t data)
{
    return spi1_read_write_byte(data);
}

/**
 * @brief       ATK_MO1053 SPI设置为低速模式(命令传输时)
 * @param       无
 * @retval      无
 */
void atk_mo1053_spi_speed_low(void)
{
    spi1_set_speed(SPI_SPEED_32); /* 设置到低速模式 */
}

/**
 * @brief       ATK_MO1053 SPI设置为高速模式(数据传输时)
 *   @note      对VS1003B, 最大值不能超过 49.152/6 Mhz = 8.192 Mhz
 *   @note      对VS1053B, 最大值不能超过 55.296/6 Mhz = 9.216 Mhz
 * @param       无
 * @retval      无
 */
void atk_mo1053_spi_speed_high(void)
{
    spi1_set_speed(SPI_SPEED_8); /* 设置到高速模式 */
}

/**
 * @brief       ATK_MO1053 初始化
 * @param       无
 * @retval      无
 */
uint8_t atk_mo1053_init(void)
{
    VS10XX_RST_GPIO_CLK_ENABLE();  /* ATK_MO1053_RST  脚 时钟使能 */
    VS10XX_XCS_GPIO_CLK_ENABLE();  /* ATK_MO1053_XCS  脚 时钟使能 */
    VS10XX_XDCS_GPIO_CLK_ENABLE(); /* ATK_MO1053_XDCS 脚 时钟使能 */
    VS10XX_DQ_GPIO_CLK_ENABLE();   /* ATK_MO1053_DQ   脚 时钟使能 */

    // GPIO_InitTypeDef GPIO_Initure;
    // /* RST  引脚模式设置,输出 */
    // GPIO_Initure.Pin = VS10XX_RST_Pin;
    // GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    // GPIO_Initure.Pull = GPIO_PULLUP;
    // GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    // HAL_GPIO_Init(VS10XX_RST_GPIO_Port, &GPIO_Initure);
    // /* XCS  引脚模式设置,输出 */
    // GPIO_Initure.Pin = VS10XX_XCS_Pin;
    // HAL_GPIO_Init(VS10XX_XCS_GPIO_Port, &GPIO_Initure);
    // /* XDCS 引脚模式设置，输出 */
    // GPIO_Initure.Pin = VS10XX_XDCS_Pin;
    // HAL_GPIO_Init(VS10XX_XDCS_GPIO_Port, &GPIO_Initure);
    // /* DQ   引脚模式设置,输入 */
    // GPIO_Initure.Pin = VS10XX_DQ_Pin;
    // GPIO_Initure.Mode = GPIO_MODE_INPUT;
    // GPIO_Initure.Pull = GPIO_PULLUP;
    // GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    // HAL_GPIO_Init(VS10XX_DQ_GPIO_Port, &GPIO_Initure);

    // spi1_init();

    return ATK_MO1053_EOK;
}

/**
 * @brief       ATK_MO1053 软复位
 * @param       无
 * @retval      无
 */
void atk_mo1053_soft_reset(void)
{
    uint8_t retry = 0;

    while (VS10XX_DQ == 0)
        ; /* 等待软件复位结束 */

    atk_mo1053_spi_read_write_byte(0Xff); /* 启动传输 */
    retry = 0;

    while (atk_mo1053_read_reg(SPI_MODE) != 0x0800) /* 软件复位,新模式 */
    {
        atk_mo1053_write_cmd(SPI_MODE, 0x0804); /* 软件复位,新模式 */
        delay_ms(2);                            /* 等待至少1.35ms */

        if (retry++ > 100)
        {
            break;
        }
    }

    while (VS10XX_DQ == 0)
        ; /* 等待软件复位结束 */

    retry = 0;

    while (atk_mo1053_read_reg(SPI_CLOCKF) != 0X9800) /* 设置ATK_MO1053的时钟,3倍频 ,1.5xADD */
    {
        atk_mo1053_write_cmd(SPI_CLOCKF, 0X9800); /* 设置ATK_MO1053的时钟,3倍频 ,1.5xADD */

        if (retry++ > 100)
        {
            break;
        }
    }

    delay_ms(20);
}

/**
 * @brief       ATK_MO1053 硬件复位
 * @param       无
 * @retval      0, 复位成功; 1, 复位失败;
 */
uint8_t atk_mo1053_reset(void)
{
    uint8_t retry = 0;

    VS10XX_RST(0);
    delay_ms(20);
    VS10XX_XDCS(1); /* 取消数据传输 */
    VS10XX_XCS(1);  /* 取消数据传输 */
    VS10XX_RST(1);

    while (VS10XX_DQ == 0 && retry < 200) /* 等待DREQ为高 */
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
 * @brief       ATK_MO1053 正弦测试
 * @param       无
 * @retval      无
 */
void atk_mo1053_sine_test(void)
{
    atk_mo1053_reset();
    atk_mo1053_write_cmd(0x0b, 0X2020);     /* 设置音量 */
    atk_mo1053_write_cmd(SPI_MODE, 0x0820); /* 进入ATK_MO1053 的测试模式 */

    while (VS10XX_DQ == 0)
        ; /* 等待DREQ为高 */

    /* printf("mode sin:%x\n", vs10xx_read_reg(SPI_MODE));
     * 向VS10XX发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
     * 其中n = 0x24, 设定VS10XX所产生的正弦波的频率值，具体计算方法见VS10XX的datasheet
     */
    atk_mo1053_spi_speed_low(); /* 低速 */
    VS10XX_XDCS(0);             /* 选中数据传输 */
    atk_mo1053_spi_read_write_byte(0x53);
    atk_mo1053_spi_read_write_byte(0xef);
    atk_mo1053_spi_read_write_byte(0x6e);
    atk_mo1053_spi_read_write_byte(0x24);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    delay_ms(100);
    VS10XX_XDCS(1);
    /* 退出正弦测试 */
    VS10XX_XDCS(0); /* 选中数据传输 */
    atk_mo1053_spi_read_write_byte(0x45);
    atk_mo1053_spi_read_write_byte(0x78);
    atk_mo1053_spi_read_write_byte(0x69);
    atk_mo1053_spi_read_write_byte(0x74);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    delay_ms(100);
    VS10XX_XDCS(1);

    /* 再次进入正弦测试并设置n值为0x44，即将正弦波的频率设置为另外的值 */
    VS10XX_XDCS(0); /* 选中数据传输 */
    atk_mo1053_spi_read_write_byte(0x53);
    atk_mo1053_spi_read_write_byte(0xef);
    atk_mo1053_spi_read_write_byte(0x6e);
    atk_mo1053_spi_read_write_byte(0x44);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    delay_ms(100);
    VS10XX_XDCS(1);
    /* 退出正弦测试 */
    VS10XX_XDCS(0); /* 选中数据传输 */
    atk_mo1053_spi_read_write_byte(0x45);
    atk_mo1053_spi_read_write_byte(0x78);
    atk_mo1053_spi_read_write_byte(0x69);
    atk_mo1053_spi_read_write_byte(0x74);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    delay_ms(100);
    VS10XX_XDCS(1);
}

/**
 * @brief       ATK_MO1053 RAM测试
 * @param       无
 * @retval      测试结果
 *   @arg       0X807F, ATK_MO1053测试正常
 *   @arg       0X83FF, ATK_MO1053测试正常
 *   @arg       其他,   测试失败
 */
uint16_t atk_mo1053_ram_test(void)
{
    atk_mo1053_reset();
    atk_mo1053_write_cmd(SPI_MODE, 0x0820); /*  进入VS10XX的测试模式 */

    while (VS10XX_DQ == 0)
        ; /*  等待DREQ为高 */

    atk_mo1053_spi_speed_low(); /* 低速 */
    VS10XX_XDCS(0);             /*  xDCS = 1，选择VS10XX的数据接口 */
    atk_mo1053_spi_read_write_byte(0x4d);
    atk_mo1053_spi_read_write_byte(0xea);
    atk_mo1053_spi_read_write_byte(0x6d);
    atk_mo1053_spi_read_write_byte(0x54);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    atk_mo1053_spi_read_write_byte(0x00);
    delay_ms(150);
    VS10XX_XDCS(1);
    return atk_mo1053_read_reg(SPI_HDAT0); /* ATK_MO1053 如果得到的值为0x807F，则表明完好;ATK_MO1053为0X83FF */
}

/**
 * @brief       ATK_MO1053 写命令
 * @param       address : 命令地址
 * @param       data    : 命令数据
 * @retval      无
 */
void atk_mo1053_write_cmd(uint8_t address, uint16_t data)
{
    while (VS10XX_DQ == 0)
        ; /* 等待空闲 */

    atk_mo1053_spi_speed_low(); /* 低速 */
    VS10XX_XDCS(1);
    VS10XX_XCS(0);
    atk_mo1053_spi_read_write_byte(VS_WRITE_COMMAND); /* 发送ATK_MO1053 的写命令 */
    atk_mo1053_spi_read_write_byte(address);          /* 地址 */
    atk_mo1053_spi_read_write_byte(data >> 8);        /* 发送高八位 */
    atk_mo1053_spi_read_write_byte(data);             /* 低八位 */
    VS10XX_XCS(1);
    atk_mo1053_spi_speed_high(); /* 高速 */
}

/**
 * @brief       ATK_MO1053 读寄存器
 * @param       address : 寄存器地址
 * @retval      读取到的数据
 */
uint16_t atk_mo1053_read_reg(uint8_t address)
{
    uint16_t temp = 0;

    while (VS10XX_DQ == 0)
        ; /* 非等待空闲状态 */

    atk_mo1053_spi_speed_low(); /* 低速 */
    VS10XX_XDCS(1);
    VS10XX_XCS(0);
    atk_mo1053_spi_read_write_byte(VS_READ_COMMAND); /* 发送ATK_MO1053 的读命令 */
    atk_mo1053_spi_read_write_byte(address);         /* 地址 */
    temp = atk_mo1053_spi_read_write_byte(0xff);     /* 读取高字节 */
    temp = temp << 8;
    temp += atk_mo1053_spi_read_write_byte(0xff); /* 读取低字节 */
    VS10XX_XCS(1);
    atk_mo1053_spi_speed_high(); /* 高速 */
    return temp;
}

/**
 * @brief       ATK_MO1053 读RAM
 * @param       address : RAM地址
 * @retval      读取到的数据
 */
static uint16_t atk_mo1053_read_ram(uint16_t address)
{
    uint16_t res;
    atk_mo1053_write_cmd(SPI_WRAMADDR, address);
    res = atk_mo1053_read_reg(SPI_WRAM);
    return res;
}

/**
 * @brief       ATK_MO1053 写RAM
 * @param       address : RAM地址
 * @param       data    : 要写入的值
 * @retval      无
 */
static void atk_mo1053_write_ram(uint16_t address, uint16_t data)
{
    atk_mo1053_write_cmd(SPI_WRAMADDR, address); /* 写RAM地址 */

    while (VS10XX_DQ == 0)
        ; /* 等待空闲 */

    atk_mo1053_write_cmd(SPI_WRAM, data); /* 写RAM值 */
}

/**
 * @brief       设置播放速度
 *   @note      该函数仅对 VS1053 有效
 * @param       speed   : 播放倍速
 *   @arg       0/1     : 正常速度
 *   @arg       其他    : speed倍
 * @retval      无
 */
void atk_mo1053_set_speed(uint8_t speed)
{
    atk_mo1053_write_ram(0X1E04, speed); /* 写入播放速度 */
}

/* 各种音频头标志
 * WAV  HEAD0 : 0X7761      HEAD1 : 0X7665
 * MIDI HEAD0 : other info  HEAD1 : 0X4D54
 * WMA  HEAD0 : data speed  HEAD1 : 0X574D
 * MP3  HEAD0 : data speed  HEAD1 : ID
 * 比特率预定值,阶层III
 */
static const uint16_t g_bitrate[2][16] =
    {
        {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0},
        {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0}};
/**
 * @brief       返回当前音频的码率
 * @param       无
 * @retval      当前音频码率(Kbps)
 */
uint16_t atk_mo1053_get_bitrate(void)
{
    uint32_t head0;
    uint16_t head1;

    head0 = atk_mo1053_read_reg(SPI_HDAT0);
    head1 = atk_mo1053_read_reg(SPI_HDAT1);

    switch (head1)
    {
    case 0X4D54: /* MIDI格式 */
    case 0X4154: /* AAC_ADTS */
    case 0X4144: /* AAC_ADIF */
    case 0X4D34: /* AAC_MP4/M4A */
    case 0X4F67: /* OGG */
    case 0X574D: /* WMA格式 */
    {
        return head0 / 125; /* 相当于 * 8 / 1000, 得到Kbps单位的音频码率 */
    }

    case 0x7665:             /* WAV格式 */
        if (head0 == 0XFFFF) /* head0 = 0XFFFF, 说明 码率超出了uint16的表达范围 */
        {
            return 1411; /* 对于超出量程范围的WAV文件, 直接默认1411码率 */
        }

    case 0X664C:                    /* FLAC格式 */
                                    /* 由于 VLSI没有明确给出FLAC的码率计算公式, 这里使用经验计算公式
                                     * (head0 * 32) / 1000, 该公式是根据head0实际的值以及音频实际采样率
                                     * 推导出来的, 如果VLSI有明确的计算方法了, 后续再修正这个公式
                                     */
        return (head0 * 32) / 1000; /* 得到Kbps单位的音频码率 */

    default: /* MP3格式, 仅做了阶层III的表 */
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
 * @brief       得到平均字节数
 * @param       无
 * @retval      平均字节数速度
 */
uint16_t atk_mo1053_get_byterate(void)
{
    return atk_mo1053_read_ram(0X1E05); /* 平均位速 */
}

/**
 * @brief       得到需要填充的数字
 *   @note      在歌曲切换的时候使用
 * @param       无
 * @retval      需要填充的数字
 */
static uint16_t vs10xx_get_fill_byte(void)
{
    return atk_mo1053_read_ram(0X1E06); /* 填充字节 */
}

/**
 * @brief       发送一次音频数据
 *   @note      固定为32字节长度
 * @param       buf     : 音频数据缓冲区首地址
 * @retval      发送结果
 *   @arg       0, 发送成功
 *   @arg       1, VS10XX还不需要数据, 本次发送未完成
 *
 */
uint8_t atk_mo1053_send_music_data(uint8_t *buf)
{
    uint8_t n;

    if (VS10XX_DQ != 0) /* 是否需要发送数据给 ATK_MO1053 ? */
    {
        VS10XX_XDCS(0);

        for (n = 0; n < 32; n++) /* 发送32字节音频数据 */
        {
            atk_mo1053_spi_read_write_byte(buf[n]);
        }

        VS10XX_XDCS(1);
    }
    else
    {
        return 1;
    }

    return 0; /* 成功发送了 */
}

/**
 * @brief       切歌
 *   @note      通过此函数切歌, 不会出现切换“噪声”
 * @param       无
 * @retval      无
 */
void atk_mo1053_restart_play(void)
{
    uint16_t temp;
    uint16_t i;
    uint8_t n;
    uint8_t vsbuf[32];

    for (n = 0; n < 32; n++)
    {
        vsbuf[n] = 0; /* 清零 */
    }

    temp = atk_mo1053_read_reg(SPI_MODE); /* 读取SPI_MODE的内容 */
    temp |= 1 << 3;                       /* 设置SM_CANCEL位 */
    temp |= 1 << 2;                       /* 设置SM_LAYER12位,允许播放MP1,MP2 */
    atk_mo1053_write_cmd(SPI_MODE, temp); /* 设置取消当前解码指令 */

    for (i = 0; i < 2048;) /* 发送2048个0,期间读取SM_CANCEL位.如果为0,则表示已经取消了当前解码 */
    {
        if (atk_mo1053_send_music_data(vsbuf) == 0) /* 每发送32个字节后检测一次 */
        {
            i += 32;                              /* 发送了32个字节 */
            temp = atk_mo1053_read_reg(SPI_MODE); /* 读取SPI_MODE的内容 */

            if ((temp & (1 << 3)) == 0)
            {
                break; /* 成功取消了 */
            }
        }
    }

    if (i < 2048) /* SM_CANCEL正常 */
    {
        temp = vs10xx_get_fill_byte() & 0xff; /* 读取填充字节 */

        for (n = 0; n < 32; n++)
        {
            vsbuf[n] = temp; /* 填充字节放入数组 */
        }

        for (i = 0; i < 2052;)
        {
            if (atk_mo1053_send_music_data(vsbuf) == 0)
            {
                i += 32; /* 填充 */
            }
        }
    }
    else
    {
        atk_mo1053_soft_reset(); /* SM_CANCEL不成功,坏情况,需要软复位 */
    }

    temp = atk_mo1053_read_reg(SPI_HDAT0);
    temp += atk_mo1053_read_reg(SPI_HDAT1);

    if (temp) /* 软复位,还是没有成功取消,放杀手锏,硬复位 */
    {
        atk_mo1053_reset();      /* 硬复位 */
        atk_mo1053_soft_reset(); /* 软复位 */
    }
}

/**
 * @brief       重设解码时间
 * @param       无
 * @retval      无
 */
void atk_mo1053_reset_decode_time(void)
{
    atk_mo1053_write_cmd(SPI_DECODE_TIME, 0x0000);
    atk_mo1053_write_cmd(SPI_DECODE_TIME, 0x0000); /* 操作两次 */
}

/**
 * @brief       得到当前音频播放的时间
 * @param       无
 * @retval      解码时长(单位: s)
 */
uint16_t atk_mo1053_get_decode_time(void)
{
    uint16_t dt = 0;
    dt = atk_mo1053_read_reg(SPI_DECODE_TIME);
    return dt;
}

/**
 * @brief       加载PATCH
 *   @note      比如加载FLAC播放的PATCH ,加载频谱分析的PATCH等
 * @param       patch   : patch首地址
 * @param       len     : patch长度
 * @retval      无
 */
void atk_mo1053_load_patch(uint16_t *patch, uint16_t len)
{
    uint16_t i;
    uint16_t addr, n, val;

    for (i = 0; i < len;)
    {
        addr = patch[i++];
        n = patch[i++];

        if (n & 0x8000U) /* RLE运行，复制n个样本 */
        {
            n &= 0x7FFF;
            val = patch[i++];

            while (n--)
            {
                atk_mo1053_write_cmd(addr, val);
            }
        }
        else /* 复制运行，复制n个样本 */
        {
            while (n--)
            {
                val = patch[i++];
                atk_mo1053_write_cmd(addr, val);
            }
        }
    }
}

/**
 * @brief       设置音量大小
 * @param       volx    : 音量大小(0~254)
 * @retval      无
 */
void atk_mo1053_set_volume(uint8_t volx)
{
    uint16_t volt = 0; /* 暂存音量值 */
    volt = 254 - volx; /* 取反一下,得到最大值,表示最大的表示 */
    volt <<= 8;
    volt += 254 - volx;                  /* 得到音量设置后大小 */
    atk_mo1053_write_cmd(SPI_VOL, volt); /* 设置音量 */
}

/**
 * @brief       设置高低音(音调)
 * @param       bfreq   : 低频上限频率 2~15(单位:10Hz)
 * @param       bass    : 低频增益     0~15(单位:1dB)
 * @param       tfreq   : 高频下限频率 1~15(单位:1KHz)
 * @param       treble  : 高频增益     0~15(单位:1.5dB,小于9的时候为负数)
 * @retval      无
 */
void atk_mo1053_set_tone(uint8_t bfreq, uint8_t bass, uint8_t tfreq, uint8_t treble)
{
    uint16_t bass_set = 0; /* 暂存音调寄存器值 */
    signed char temp = (signed char)treble - 8;

    bass_set = temp & 0X0F; /* 高音设定 */
    bass_set <<= 4;
    bass_set += tfreq & 0xf; /* 高音下限频率 */
    bass_set <<= 4;
    bass_set += bass & 0xf; /* 低音设定 */
    bass_set <<= 4;
    bass_set += bfreq & 0xf;                  /* 低音上限 */
    atk_mo1053_write_cmd(SPI_BASS, bass_set); /* BASS */
}

/**
 * @brief       设定音效
 * @param       effect  : 0, 关闭; 1, 最小; 2, 中等; 3, 最大;
 * @retval      无
 */
void atk_mo1053_set_effect(uint8_t effect)
{
    uint16_t temp;
    temp = atk_mo1053_read_reg(SPI_MODE); /* 读取SPI_MODE的内容 */

    temp &= ~(1 << 4); /* 取消 LO */
    temp &= ~(1 << 7); /* 取消 HI */

    if (effect & 0X01)
    {
        temp |= 1 << 4; /* 设定LO */
    }

    if (effect & 0X02)
    {
        temp |= 1 << 7; /* 设定 HI */
    }

    atk_mo1053_write_cmd(SPI_MODE, temp); /* 设定模式 */
}

/**
 * @brief       板载喇叭开/关设置函数
 *   @note      战舰开发板 板载了HT6872功放, 通过ATK_MO1053 的GPIO4(36脚), 控制其工作/关闭
 *              该函数实际上是VS1053的GPIO设置函数
 *              GPIO4 = 1, HT6872正常工作
 *              GPIO4 = 0, HT6872关闭(默认)
 * @param       sw      : 0, 关闭; 1, 开启;
 * @retval      无
 */
void atk_mo1053_set_speaker(uint8_t sw)
{
    atk_mo1053_write_ram(GPIO_DDR, 1 << 4);    /* ATK_MO1053 的GPIO4设置成输出 */
    atk_mo1053_write_ram(GPIO_ODATA, sw << 4); /* 控制ATK_MO1053的GPIO4输出值(0/1) */
}

/**
 * @brief       ATK_MO1053 各种参数设置
 *   @note      包括音量/高低音/空间效果/板载喇叭输出等
 * @param       无
 * @retval      无
 */
void atk_mo1053_set_all(void)
{
    atk_mo1053_set_volume(vsset.mvol);                                           /* 设置音量 */
    atk_mo1053_set_tone(vsset.bflimit, vsset.bass, vsset.tflimit, vsset.treble); /* 高低音设置 */
    atk_mo1053_set_effect(vsset.effect);                                         /* 设置空间效果 */
    atk_mo1053_set_speaker(vsset.speakersw);                                     /* 控制板载喇叭状态 */
}
