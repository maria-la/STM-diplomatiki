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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
uint16_t m1,m2;
uint16_t mic1[winLength], mic2[winLength];

uint16_t sensor_value1, sensor_value2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void adc1_callback(void);
static void adc2_callback(void);

static void dma_ch1_callback(void);
static void dma_ch1_callback_h(void);

static void dma_ch2_callback(void);
static void dma_ch2_callback_h(void);
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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

	RCC->AHBENR |= IOPAEN;

	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  	adc1_interrupt_init_ch1();
  	adc2_interrupt_init_ch1();
  	start_conversion_dual();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static void adc1_callback(void){
	sensor_value1 = ADC1->DR;

	/*Move values to mem*/
	dma_init_ch1((uint32_t)&ADC1->DR, (uint32_t)mic1, winLength);
}

static void adc2_callback(void){
	sensor_value2 = ADC2->DR;

	/*Move values to mem*/
	dma_init_ch2((uint32_t)&ADC2->DR, (uint32_t)mic2, winLength);
}

static void dma_ch1_callback(void){
	GPIOA->ODR |= LED_PIN;
	m1 = ADC1->DR;
}

static void dma_ch1_callback_h(void){
	//GPIOA->ODR |= LED_PIN;
	m2 = ADC1->DR;
}

static void dma_ch2_callback(void){
	GPIOA->ODR |= LED_PIN;
	m1 = ADC1->DR;
}

static void dma_ch2_callback_h(void){
	//GPIOA->ODR |= LED_PIN;
	m2 = ADC1->DR;
}

void ADC1_2_IRQHandler(void){
	/*Check if eoc in SR*/
	if((ADC1->ISR & ISR_EOC)!=0){
		/*Clear EOC*/
		ADC1->ISR &=~ ISR_EOC;

		//Do something
		adc1_callback();
	}

	if((ADC2->ISR & ISR_EOC)!=0){
			/*Clear EOC*/
			ADC2->ISR &=~ ISR_EOC;

			//Do something
			adc2_callback();
		}
}



void DMA1_CH1_IRQHandler(void){
	/*Check for transfer complete interrupt*/
	if(DMA1->ISR & ISR_TCIF1){
		/*Clear interrupt flag*/
		DMA1->IFCR |= IFCR_CTCIF1;

		//Do something
		dma_ch1_callback();

	}

	if(DMA1->ISR & ISR_HTIF1){
			/*Clear interrupt flag*/
			DMA1->IFCR |= IFCR_CHTIF1;

			//Do something
			dma_ch1_callback_h();

		}

}

void DMA1_CH2_IRQHandler(void){
	/*Check for transfer complete interrupt*/
	if(DMA1->ISR & ISR_TCIF2){
		/*Clear interrupt flag*/
		DMA1->IFCR |= IFCR_CTCIF2;

		//Do something
		dma_ch2_callback();

	}

	if(DMA1->ISR & ISR_HTIF2){
			/*Clear interrupt flag*/
			DMA1->IFCR |= IFCR_CHTIF2;

			//Do something
			dma_ch2_callback_h();

		}

}
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
