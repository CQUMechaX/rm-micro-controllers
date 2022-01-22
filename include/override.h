#ifndef __OVERRIDE__H__
#define __OVERRIDE__H__

#include <stm32f4xx_hal.h>

#define GCC_PACKED __packed

#if defined(RM_DEV_A)
    #define TIM_BUZZER_ID 12
    #define TIM_BUZZER_CHANNEL 4
#elif defined(RM_DEV_C)
    #define TIM_BUZZER_ID 4
    #define TIM_BUZZER_CHANNEL 3
    #define HTIM_BUZZER htim4
    #define HTIM_BUZZER_CHANNEL TIM_CHANNEL_3
    #define HSPI_IMU hspi1
    #define HIIC_IMU hi2c3
#endif
// #define HTIM_BUZZER _htim##__TOS(TIM_BUZZER_ID)
// #define HTIM_BUZZER_CHANNEL __LINK_TWO_VAR(TIM_CHANNEL, TIM_BUZZER_CHANNEL)

void delay_us(uint16_t nus);

extern TIM_HandleTypeDef HTIM_BUZZER;
extern SPI_HandleTypeDef HSPI_IMU;
extern I2C_HandleTypeDef HIIC_IMU;
#endif /* __OVERRIDE__H__ */