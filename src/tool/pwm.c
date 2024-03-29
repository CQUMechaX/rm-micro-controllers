#include "tool/pwm.h"

#include <stdarg.h>

#include "marco.h"
#include "override.h"
#include "tim.h"

const uint16_t toneFreq[CM_BUZZER_TF_SIZE] = {0,    350,  650,  950,  1250, 100,
                                              1650, 2250, 2450, 2650, 2750, 1350};

bool g_state_pwm[10][10] = {0};

uint8_t buzzerList(
  __IO uint32_t * CCR, const uint16_t freqList[], uint32_t freqLen, uint32_t freqSelect, ...)
{
  va_list listNum;
  uint32_t i = 1;
  // return HAL_OK;
  for (va_start(listNum, freqSelect); i != freqLen; ++i) {
    *CCR = freqList[va_arg(listNum, int32_t)];
    HAL_Delay(1000);
    *CCR = 0;
    HAL_Delay(500);
  }
  return HAL_OK;
}

HAL_StatusTypeDef heatTrigger(uint16_t psc, uint16_t pwm)
{
  __HAL_TIM_SetCompare(&htim10, TIM_CHANNEL_1, pwm);
  return HAL_OK;
}

HAL_StatusTypeDef buzzerTrigger(uint16_t psc, uint16_t pwm)
{
  if (HAL_TIM_PWM_Start(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL) != HAL_OK) {
    return HAL_ERROR;
  }
  __HAL_TIM_PRESCALER(&HTIM_BUZZER, psc);
  __HAL_TIM_SetCompare(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL, pwm);
  return HAL_OK;
}

HAL_StatusTypeDef buzzerTone(void)
{
  if (HAL_TIM_PWM_Start(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL) != HAL_OK) {
    return HAL_ERROR;
  }
  (HTIM_BUZZER.Instance->CCR1) = 1000;
  (HTIM_BUZZER.Instance->PSC) = 100;
  HAL_Delay(700);
  (HTIM_BUZZER.Instance->PSC) = 200;
  HAL_Delay(700);
  (HTIM_BUZZER.Instance->PSC) = 50;
  HAL_Delay(700);
  // (HTIM_BUZZER.Instance -> PSC) = 40;
  // HAL_Delay(700);
  // (HTIM_BUZZER.Instance -> PSC) = 30;
  // HAL_Delay(700);
  // (HTIM_BUZZER.Instance -> PSC) = 20;
  // HAL_Delay(700);
  (HTIM_BUZZER.Instance->CCR1) = 100;
  (HTIM_BUZZER.Instance->PSC) = 10;
  HAL_Delay(700);
  (HTIM_BUZZER.Instance->CCR1) = 1000;
  (HTIM_BUZZER.Instance->PSC) = 10;
  HAL_Delay(700);
  buzzerList(&HTIM_BUZZER.Instance->CCR1, toneFreq, 11, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 5);
  HAL_TIM_PWM_Stop(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL);
  return HAL_OK;
}