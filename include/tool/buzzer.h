/**
 * 
 * @author Tiger3018
 */
#pragma once
#ifndef __BUZZER_H__
#define __BUZZER_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
 
#define CM_BUZZER_TF_SIZE 12

extern const uint16_t toneFreq[CM_BUZZER_TF_SIZE];

uint8_t buzzerStartUp(void);

#ifdef __cplusplus
 }
#endif

#endif /* __BUZZER_H__ */