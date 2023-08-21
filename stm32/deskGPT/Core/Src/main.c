/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dac.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "atk_mo1053.h"
#include "delay.h"
#include <string.h>
#include "recorder.h"
#include "dataPackDecode.h"
#include "dataQueue.h"
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t send_buf[275]; //100
uint8_t recv_buf[DATA_SIZE];
uint8_t audio_buf[AUDIO_DATA_BUF_SIZE];
uint8_t flg = 0;
extern usb_datapack_decode_t pcDataPack;

uint8_t rec_state = STOP_MODE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t CDC_Transmit_One_Byte(uint16_t byte)
{
    static uint16_t cnt = 0;

    send_buf[cnt++] = byte;
    if (cnt == 275)
    {
        CDC_Transmit_FS((uint8_t *)send_buf, 550);
        cnt = 0;
    }
    // for(int i=0;i<100;i++)
    // {
    //     send_buf[i] = byte;
    // }
    // CDC_Transmit_FS((uint8_t *)send_buf, 200);
}

void playSpeakerTest(void)
{
    HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
                // for(int i=0;i<AUDIO_DATA_SIZE;i+=2)
                // {
                //     uint16_t audio_data = (uint16_t)audio_buf[i]<<8 | (uint16_t)audio_buf[i+1];
                //     HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,audio_data);
                //     // HAL_Delay(1);
                // }
    static uint16_t val = 0;
    HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,val);
    val += 2;
    if(val == 4096)
    {
        val = 0;
    }
    HAL_DAC_Stop(&hdac,DAC_CHANNEL_1);
}

void playSpeaker(uint32_t audio_data)
{
    HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,audio_data);
    HAL_DAC_Stop(&hdac,DAC_CHANNEL_1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
    dataPackInit(&pcDataPack);
    pcDataQueueInit();
    audioDataQueueInit();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_SPI1_Init();
  MX_DAC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    atk_mo1053_init();
    //   uint8_t res;
    // delay_ms(10);
    uint16_t cnt = 0;
    uint16_t idx = 0;
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

        /* get audio data from spi and send to usb */
        if (pcDataPack.record_enable && !pcDataPack.play_enable)
        {
            if(rec_state != RECORD_MODE)
            {
                atk_mo1053_reset();
                atk_mo1053_soft_reset();
                recoder_enter_rec_mode(0);
                rec_state = RECORD_MODE;
                delay_ms(10);
            }
            if (atk_mo1053_read_reg(SPI_HDAT1))
            {
                uint16_t data = atk_mo1053_read_reg(SPI_HDAT0);
                cnt++;
                // send_buf[idx++] = cnt;
                send_buf[idx++] = data;
                // send_buf[idx++] = 0;
                if (idx == 100)
                {
                    CDC_Transmit_FS((uint8_t *)send_buf, 200);
                    idx = 0;
                }
            }
        }
        else if (!pcDataPack.record_enable && pcDataPack.play_enable)
        {
            if(rec_state != PLAY_MODE)
            {
                atk_mo1053_reset();
                atk_mo1053_soft_reset();
                atk_mo1053_restart_play();
                atk_mo1053_set_all();
                // atk_mo1053_write_cmd(SPI_AICTRL0,16000);	//设置采样率,设置为16Khz
                atk_mo1053_reset_decode_time();
                atk_mo1053_spi_speed_high();
                // atk_mo1053_sine_test();

                // atk_mo1053_write_cmd(SPI_BASS,0x0000);    
                atk_mo1053_write_cmd(SPI_AICTRL0,8000);	//设置采样率,设置为16Khz
                // // atk_mo1053_write_cmd(SPI_AICTRL1,0);		//设置增益,0,自动增益.1024相当于1倍,512相当于0.5倍,最大值65535=64倍	
                // // atk_mo1053_write_cmd(SPI_AICTRL2,0);		//设置增益最大值,0,代表最大值65536=64X
                // // atk_mo1053_write_cmd(SPI_AICTRL3,6);		//左通道(MIC单声道输入)，线性PCM
                atk_mo1053_write_cmd(SPI_CLOCKF,0X2000);	//设置VS10XX的时钟,MULT:2倍频;ADD:不允许;CLK:12.288Mhz
                // atk_mo1053_write_cmd(SPI_MODE,0x0804);		
                rec_state = PLAY_MODE;
                delay_ms(10);
            }
            if(pcDataPack.play_complete == 0)
            {
                getAudioData(audio_buf, recv_buf, &pcDataPack);
                // CDC_Transmit_FS(audio_buf, AUDIO_DATA_BUF_SIZE);
                // for(int i=0;i<AUDIO_DATA_BUF_SIZE;i+=2)
                // {
                //     uint16_t audio_data = (uint16_t)audio_buf[i]<<8 | (uint16_t)audio_buf[i+1];
                //     CDC_Transmit_One_Byte(audio_data);
                //     if(i % AUDIO_DATA_SIZE == 0)
                //     {
                //         atk_mo1053_send_music_data(audio_buf+i);
                //     }
                // }
                for(int i=0;i<AUDIO_DATA_BUF_SIZE;i+=AUDIO_DATA_SIZE)
                {
                    // audioDataQueuePush8(audio_buf+i);
                    uint16_t timeout_cnt = 0;
                    //防止发送失败，导致程序卡死
                    while(atk_mo1053_send_music_data(audio_buf+i))
                    {
                        timeout_cnt++;
                        if(timeout_cnt > 10000)
                        {
                            break;
                        }
                    }
                    
                    // //判斷數據是否全0
                    // uint8_t is_all_zero = 1;
                    // for(int j=0;j<AUDIO_DATA_SIZE;j++)
                    // {
                    //     uint16_t timeout_cnt = 0;
                    //     //如果不是全0数据，就发送
                    //     if(audio_buf[i+j] != 0)
                    //     {
                    //         is_all_zero = 0;
                    //         //防止发送失败，导致程序卡死
                    //         while(atk_mo1053_send_music_data(audio_buf+i))
                    //         {
                    //             timeout_cnt++;
                    //             if(timeout_cnt > 10000)
                    //             {
                    //                 break;
                    //             }
                    //         }
                    //         timeout_cnt = 0;
                    //         break;
                    //     }
                    // }
                    // // 数据全0，终止播放
                    // if(is_all_zero)
                    // {
                    //     rec_state = STOP_MODE;
                    //     atk_mo1053_reset();
                    //     atk_mo1053_soft_reset();
                    //     break;
                    // }
                    // atk_mo1053_send_music_data(audio_buf+i);
                }
            // for(int i=0;i<DATA_SIZE;i+=2)
            // {
            //     uint16_t data = (uint16_t)recv_buf[i]<<8 | (uint16_t)recv_buf[i+1];
            //     CDC_Transmit_One_Byte(data);
            // }
            // uint8_t idx = 0;
            // do
            // {
            //     if (atk_mo1053_send_music_data(audio_buf + idx) == 0)   /* 给VS10XX发送音频数据 */
            //     {
            //         idx += 32;
            //     }
            // }while(idx < AUDIO_DATA_BUF_SIZE);

            // // while();
            // atk_mo1053_reset(); //不确定要不要�?
            // atk_mo1053_write_cmd(0x0b, 0X2020);  //不确定要不要�?
            // res = atk_mo1053_send_music_data(audio_buf);
            // // CDC_Transmit_One_Byte((uint16_t)rec_state);
            // /*
            // 存在问题�?
            // atk_mo1053_send_music_data可能会发送失�?
            // 如果套一层while循环，会导致程序卡死
            // 正弦测试通过，但是播放音乐会卡死
            // */

            // playSpeakerTest();
            // atk_mo1053_sine_test();
            // if(!audioDataQueueIsEmpty())
            // {
            //     uint8_t audio_data[AUDIO_DATA_SIZE];
            //     uint8_t res;
            //     extern audioDataQueue_t audioDataQueue;
            //     audioDataQueuePop(audio_data);
            //     res = atk_mo1053_send_music_data(audio_data);
            // }
            // CDC_Transmit_One_Byte((uint16_t)audioDataQueue.size);
            }
            else
            {
                if(rec_state != STOP_MODE)
                {
                    atk_mo1053_reset();
                    rec_state = STOP_MODE;
                }
            }
        }
        else
        {
            if(rec_state != STOP_MODE)
            {
                atk_mo1053_reset();
                rec_state = STOP_MODE;
            }
        }
        // CDC_Transmit_One_Byte((uint16_t)rec_state); //测试用，之后删除
        CDC_Transmit_FS(recv_buf, 550);
        // CDC_Transmit_FS(&rec_state, 1);
        /* get pc data from usb */
        if (!pcDataQueueIsEmpty())
        {
            pcDataQueuePop(recv_buf, DATA_SIZE);

            if (checkDataHead(recv_buf, &pcDataPack))
            {
                checkSpiCtrl(recv_buf, &pcDataPack);
            }
            // playSpeakerTest();
            // getAudioData(audio_buf, recv_buf, &pcDataPack);
            /* get audio data from buffer and push into audio data queue */
            // audioDataQueuePush8(recv_buf, AUDIO_DATA_SIZE);
        }

        // if(!audioDataQueueIsEmpty())
        // {
        //     // playSpeaker(audioDataQueuePop());
        //     playSpeakerTest();
        // }
        
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
