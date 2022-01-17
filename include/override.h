#ifndef __OVERRIDE__H__
#define __OVERRIDE__H__

#include <stm32f4xx_hal.h>

#define GCC_PACKED __packed

#if defined(STM32F407xx)
    #define HTIM_BUZZER htim4
    #define HTIM_BUZZER_CHANNEL TIM_CHANNEL_3
#elif defined(STM32F427xx)
    #define HTIM_BUZZER htim12
    #define HTIM_BUZZER_CHANNEL TIM_CHANNEL_4
#endif
extern TIM_HandleTypeDef htim_buzzer;

#endif /* __OVERRIDE__H__ */