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
#include "dma.h"
#include "i2s.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "atk_mo1053.h"
#include "delay.h"
#include <string.h>
#include "recorder.h"
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
uint32_t dma[4];
uint32_t val24;
int val32;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
unsigned cb_cnt=0;
//I2S接收完成回调函数
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if(hi2s==&hi2s2){
		cb_cnt++;//回调次数计数
		//将两�?32整型合并为一�?
		//dat32 example: 0000fffb 00004f00
		val24=(dma[0]<<8)+(dma[1]>>8);
		//�?24位有符号整型扩展�?32�?
		if(val24 & 0x800000){//negative
			val32=0xff000000 | val24;
		}else{//positive
			val32=val24;
		}
		//以采样频率的十分之一，串口发送采样�??
		// if(cb_cnt%1000==0)
			// printf("%d\r\n",val32);
            // CDC_Transmit_FS((uint8_t*)&val32,4);
	}
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S2_Init();
  MX_USB_DEVICE_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//   HAL_I2S_Receive_DMA(&hi2s2,(uint16_t*)dma,4);
  atk_mo1053_init();
//   uint8_t res;
//   res = atk_mo1053_reset();
atk_mo1053_reset();
//    CDC_Transmit_FS(&res,1);
//    atk_mo1053_write_cmd(SPI_AICTRL0,16000U);  // 16KHz
//    atk_mo1053_write_cmd(SPI_AICTRL1,0x0);    //增益
//    atk_mo1053_write_cmd(SPI_AICTRL2,0);  // 4096
//    atk_mo1053_write_cmd(SPI_AICTRL3,0x6);   //二进制：0110，对应线性PCM模式以及左声道
//     atk_mo1053_write_cmd(SPI_CLOCKF,0x2000);
	recoder_enter_rec_mode(0);
  uint16_t cur_mode = atk_mo1053_read_reg(SPI_MODE);
  uint16_t cur_state = atk_mo1053_read_reg(SPI_HDAT1);
  uint16_t cur_data = atk_mo1053_read_reg(SPI_HDAT0);
  uint16_t send_buf[100];
//   atk_mo1053_write_cmd(SPI_MODE, cur_mode | SM_RESET | SM_ADPCM | SM_LINE_IN);
// atk_mo1053_write_cmd(SPI_MODE,0x1804);
delay_ms(10);
uint16_t cnt = 0;
uint16_t idx = 0;
uint8_t flag = 0;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if(cur_state = atk_mo1053_read_reg(SPI_HDAT1))
    {
        uint16_t data = atk_mo1053_read_reg(SPI_HDAT0);
        // cur_data = data;
        cnt++;
        // send_buf[idx++] = cnt;
        send_buf[idx++] = data;
        // send_buf[idx++] = 0;
        // CDC_Transmit_FS((uint8_t*)&data,2);
        // if(cnt % 100 == 0)
        // {
        //     uint16_t send_data = 0;
        //     CDC_Transmit_FS((uint8_t*)&send_data,2);
        // }
        // CDC_Transmit_FS((uint8_t*)&cnt,2);
        if(idx == 100)
        {
            CDC_Transmit_FS((uint8_t*)send_buf,200);
            idx = 0;
        }
    }
    // unsigned char str1[] = "\nstate: ";
    // unsigned char str2[] = "\ndata: ";
    // unsigned char state_char = cur_state + '0';
    // unsigned char data_char = cur_data + '0';
    // // ?????
    // unsigned char main_str[50];
    // strcpy(main_str, str1);
    // strcat(main_str, &state_char);
    // strcat(main_str, str2);
    // strcat(main_str, &data_char);
    // CDC_Transmit_FS(main_str, 50);
    // CDC_Transmit_FS(str1,15);
    // // CDC_Transmit_FS((uint8_t*)&state_char,2);
    // CDC_Transmit_FS(str2,15);
    // // CDC_Transmit_FS((uint8_t*)&data_char,2);
    // delay_ms(10);
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
