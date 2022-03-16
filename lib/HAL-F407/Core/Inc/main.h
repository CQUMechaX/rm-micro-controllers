/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IMU_SPI_MISO_Pin GPIO_PIN_4
#define IMU_SPI_MISO_GPIO_Port GPIOB
#define IMU_SPI_SCK_Pin GPIO_PIN_3
#define IMU_SPI_SCK_GPIO_Port GPIOB
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define IMU_I2C_SDA_Pin GPIO_PIN_9
#define IMU_I2C_SDA_GPIO_Port GPIOC
#define IMU_I2C_SCL_Pin GPIO_PIN_8
#define IMU_I2C_SCL_GPIO_Port GPIOA
#define CMPS_RESET_Pin GPIO_PIN_6
#define CMPS_RESET_GPIO_Port GPIOG
#define HEAT_Pin GPIO_PIN_6
#define HEAT_GPIO_Port GPIOF
#define LED_RED_Pin GPIO_PIN_12
#define LED_RED_GPIO_Port GPIOH
#define CMPS_INT_Pin GPIO_PIN_3
#define CMPS_INT_GPIO_Port GPIOG
#define CMPS_INT_EXTI_IRQn EXTI3_IRQn
#define ADC_BAT_Pin GPIO_PIN_10
#define ADC_BAT_GPIO_Port GPIOF
#define LED_GREEN_Pin GPIO_PIN_11
#define LED_GREEN_GPIO_Port GPIOH
#define LED_BLUE_Pin GPIO_PIN_10
#define LED_BLUE_GPIO_Port GPIOH
#define BUZZER_Pin GPIO_PIN_14
#define BUZZER_GPIO_Port GPIOD
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOA
#define CS1_ACCEL_Pin GPIO_PIN_4
#define CS1_ACCEL_GPIO_Port GPIOA
#define ACCEL_INT_Pin GPIO_PIN_4
#define ACCEL_INT_GPIO_Port GPIOC
#define ACCEL_INT_EXTI_IRQn EXTI4_IRQn
#define GYRO_INT_Pin GPIO_PIN_5
#define GYRO_INT_GPIO_Port GPIOC
#define GYRO_INT_EXTI_IRQn EXTI9_5_IRQn
#define IMU_SPI_MOSI_Pin GPIO_PIN_7
#define IMU_SPI_MOSI_GPIO_Port GPIOA
#define CS1_GYRO_Pin GPIO_PIN_0
#define CS1_GYRO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
