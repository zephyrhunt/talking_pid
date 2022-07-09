//
// Created by LIU on 2022/7/5.
//
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "canio.h"
#include "ledio.h"
#include "keyio.h"
#include "keyc.h"

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */

	KeyC key_user(GPIOB, GPIO_PIN_2, 1);
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
	MX_ADC1_Init();
	MX_CAN1_Init();
	MX_CAN2_Init();
	MX_SPI1_Init();
	MX_SPI5_Init();
	MX_DMA_Init();
	MX_USART1_UART_Init();
	MX_USART6_UART_Init();
	MX_TIM4_Init();
	MX_TIM2_Init();
//	MX_USB_DEVICE_Init();
//	MX_UART7_Init();
	MX_UART8_Init();
	/* USER CODE BEGIN 2 */
//	CAN_DriverInit();
	KEY_ConfigInit();
	LED_ConfigInit();
	
	key_user.keyDelay();
	HAL_TIM_Base_Start_IT(&htim2);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	float f;
	uint8_t u;
	while (1)
	{
		/* USER CODE END WHILE */
		
		/* USER CODE BEGIN 3 */
		// usart_printf test
		usart_printf("usart_test:%f, %d\n", f, u);
		f += 1.7; u++;
		HAL_Delay(100);
	}
	/* USER CODE END 3 */
}


