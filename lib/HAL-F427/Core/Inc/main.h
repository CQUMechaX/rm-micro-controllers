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
#define IMU_IST_RESET_Pin GPIO_PIN_2
#define IMU_IST_RESET_GPIO_Port GPIOE
#define IMU_HEAT_PWM_Pin GPIO_PIN_5
#define IMU_HEAT_PWM_GPIO_Port GPIOB
#define LASER_SHOOT_Pin GPIO_PIN_13
#define LASER_SHOOT_GPIO_Port GPIOG
#define BL_RX_Pin GPIO_PIN_6
#define BL_RX_GPIO_Port GPIOD
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define BL_TX_Pin GPIO_PIN_5
#define BL_TX_GPIO_Port GPIOD
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define LED_GREEN_A_Pin GPIO_PIN_8
#define LED_GREEN_A_GPIO_Port GPIOG
#define LED_GREEN_B_Pin GPIO_PIN_7
#define LED_GREEN_B_GPIO_Port GPIOG
#define LED_GREEN_C_Pin GPIO_PIN_6
#define LED_GREEN_C_GPIO_Port GPIOG
#define LED_GREEN_D_Pin GPIO_PIN_5
#define LED_GREEN_D_GPIO_Port GPIOG
#define LED_GREEN_E_Pin GPIO_PIN_4
#define LED_GREEN_E_GPIO_Port GPIOG
#define LED_GREEN_F_Pin GPIO_PIN_3
#define LED_GREEN_F_GPIO_Port GPIOG
#define LED_GREEN_G_Pin GPIO_PIN_2
#define LED_GREEN_G_GPIO_Port GPIOG
#define KEY_Pin GPIO_PIN_2
#define KEY_GPIO_Port GPIOB
#define LED_GREEN_H_Pin GPIO_PIN_1
#define LED_GREEN_H_GPIO_Port GPIOG
#define BUZZER_Pin GPIO_PIN_6
#define BUZZER_GPIO_Port GPIOH
#define LED_RED_Pin GPIO_PIN_11
#define LED_RED_GPIO_Port GPIOE
#define LED_GREEN_Pin GPIO_PIN_14
#define LED_GREEN_GPIO_Port GPIOF
#define SNYCHOR_1_Pin GPIO_PIN_14
#define SNYCHOR_1_GPIO_Port GPIOB
#define SNYCHOR_2_Pin GPIO_PIN_15
#define SNYCHOR_2_GPIO_Port GPIOB
#define CS1_ACCEL_Pin GPIO_PIN_4
#define CS1_ACCEL_GPIO_Port GPIOA
#define INT1_ACCEL_Pin GPIO_PIN_4
#define INT1_ACCEL_GPIO_Port GPIOC
#define INT1_GYRO_Pin GPIO_PIN_5
#define INT1_GYRO_GPIO_Port GPIOC
#define CS1_GYRO_Pin GPIO_PIN_0
#define CS1_GYRO_GPIO_Port GPIOB
#define RST_CMPS_Pin GPIO_PIN_6
#define RST_CMPS_GPIO_Port GPIOG
#define HEAT_Pin GPIO_PIN_6
#define HEAT_GPIO_Port GPIOF
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
