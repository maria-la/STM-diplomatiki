/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>
#include "stm32f3xx.h"
#include "uart.h"
#include "adc.h"
#include "dma.h"
/*
#include "arm_math.h"
#include "arm_const_structs.h"
#include "arm_common_tables.h"
*/
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define winLength 		256

#define IOPAEN			(1U<<17)
#define PIN5 			(1U<<5)

#define LED_PIN			PIN5

#define IFCR_CTCIF1 	(1U<<1)
#define IFCR_CHTIF1 	(1U<<2)

#define ISR_TCIF1 		(1U<<1)
#define ISR_HTIF1 		(1U<<2)

#define IFCR_CTCIF2 	(1U<<5)
#define IFCR_CHTIF2 	(1U<<6)

#define ISR_TCIF2 		(1U<<5)
#define ISR_HTIF2 		(1U<<6)
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
