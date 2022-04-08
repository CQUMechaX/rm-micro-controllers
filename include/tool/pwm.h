/**
 * 
 * @author Tiger3018
 */
#pragma once
#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
 
#define CM_BUZZER_TF_SIZE 12

extern const uint16_t toneFreq[CM_BUZZER_TF_SIZE];

HAL_StatusTypeDef heatTrigger(uint16_t psc, uint16_t pwm);
HAL_StatusTypeDef buzzerTrigger(uint16_t psc, uint16_t pwm);
HAL_StatusTypeDef buzzerTone(void);

#ifdef __cplusplus
 }
#endif

#endif /* __PWM_H__ */