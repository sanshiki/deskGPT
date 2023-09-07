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
#include "spi.h"
#include "usart.h"
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

uint8_t rec_state = PLAY_MODE;

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

// void playSpeakerTest(void)
// {
//     HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
//                 // for(int i=0;i<AUDIO_DATA_SIZE;i+=2)
//                 // {
//                 //     uint16_t audio_data = (uint16_t)audio_buf[i]<<8 | (uint16_t)audio_buf[i+1];
//                 //     HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,audio_data);
//                 //     // HAL_Delay(1);
//                 // }
//     static uint16_t val = 0;
//     HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,val);
//     val += 2;
//     if(val == 4096)
//     {
//         val = 0;
//     }
//     HAL_DAC_Stop(&hdac,DAC_CHANNEL_1);
// }

// void playSpeaker(uint32_t audio_data)
// {
//     HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
//     HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,audio_data);
//     HAL_DAC_Stop(&hdac,DAC_CHANNEL_1);
// }
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
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    atk_mo1053_init();
    atk_mo1053_reset();
    atk_mo1053_soft_reset();
    atk_mo1053_restart_play();
    uint16_t cnt = 0;
    uint16_t idx = 0;
    while (1)
    {
        HAL_UART_Transmit(&huart4, &rec_state, 1, 100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

        /* get audio data from spi and send to usb */
        if (pcDataPack.record_enable && !pcDataPack.play_enable)    //录音
        {
            if(rec_state != RECORD_MODE)
            {
                atk_mo1053_reset();
                atk_mo1053_soft_reset();
                recoder_enter_rec_mode(0);
				atk_mo1053_set_volume(0x00);
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
        else if (!pcDataPack.record_enable && pcDataPack.play_enable)   //播放
        {
            if(rec_state != PLAY_MODE)
            {
                atk_mo1053_reset();
                atk_mo1053_soft_reset();
                atk_mo1053_restart_play();
                atk_mo1053_set_all();
                atk_mo1053_reset_decode_time();
                atk_mo1053_spi_speed_high();
                atk_mo1053_write_cmd(SPI_AICTRL0,8000);	//设置采样�?,设置�?16Khz
                atk_mo1053_write_cmd(SPI_CLOCKF,0X2000);	//设置VS10XX的时�?,MULT:2倍频;ADD:不允�?;CLK:12.288Mhz	
                rec_state = PLAY_MODE;
                delay_ms(10);
            }
            
            getAudioData(audio_buf, recv_buf, &pcDataPack);
            for(int i=0;i<AUDIO_DATA_BUF_SIZE;i+=AUDIO_DATA_SIZE)
            {
                uint16_t timeout_cnt = 0;
                //防止发�?�失败，导致程序卡死
                while(atk_mo1053_send_music_data(audio_buf+i))
                {
                    timeout_cnt++;
                    if(timeout_cnt > 10000)
                    {
                        break;
                    }
                }
            }
        }
        else    //停止
        {
            if(rec_state != STOP_MODE)
            {
                atk_mo1053_reset();
                atk_mo1053_set_volume(0x00);
                rec_state = STOP_MODE;
            }
        }

        /* get pc data from usb */
        if (!pcDataQueueIsEmpty())
        {
            pcDataQueuePop(recv_buf, DATA_SIZE);

            if (checkDataHead(recv_buf, &pcDataPack))
            {
                checkSpiCtrl(recv_buf, &pcDataPack);
            }
        }
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
