#ifndef __OVERRIDE__H__
#define __OVERRIDE__H__

#include <stm32f4xx_hal.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define GCC_PACKED __packed
#define __OVERRIDE_SPACE
#define __OVERRIDE_CONCAT2(a, b) __OVERRIDE_INTERNEL_CONCAT2(a, b)
#define __OVERRIDE_INTERNEL_CONCAT2(a, b) a ## b
#define __OVERRIDE_CONCAT3(a, b, c) __OVERRIDE_INTERNEL_CONCAT3(a, b, c)
#define __OVERRIDE_INTERNEL_CONCAT3(a, b, c) a ## b ## c

#if defined(RM_DEV_A)
    #define TIM_BUZZER_ID 12
    #define TIM_BUZZER_CHANNEL 4
    #define UART1_ID 6
    #define UART2_ID 1
    #define UART_DBUS_ID 3
#elif defined(RM_DEV_C)
    #define TIM_BUZZER_ID 4
    #define TIM_BUZZER_CHANNEL 3
    #define UART1_ID 6
    #define UART2_ID 1
    #define UART_DBUS_ID 3
    #define HTIM_BUZZER htim4
    #define HTIM_BUZZER_CHANNEL TIM_CHANNEL_3
    #define HCAN1 hcan1
    #define HCAN2 hcan2
    #define HSPI_IMU hspi1
    #define HIIC_IMU hi2c3
#endif
#define HUART1 __OVERRIDE_CONCAT2(huart, UART1_ID)
#define HUART2 __OVERRIDE_CONCAT2(huart, UART2_ID)
#define HUART_DBUS __OVERRIDE_CONCAT2(huart, UART_DBUS_ID)
#define HDMA2_RX __OVERRIDE_CONCAT3(hdma_usart, UART2_ID, _rx)
#define HDMA_DBUS_RX __OVERRIDE_CONCAT3(hdma_usart, UART_DBUS_ID, _rx)

void delay_us(uint16_t nus);

extern TIM_HandleTypeDef HTIM_BUZZER;
extern CAN_HandleTypeDef HCAN1, HCAN2;
extern UART_HandleTypeDef HUART1, HUART2, HUART_DBUS;
extern DMA_HandleTypeDef HDMA2_RX, HDMA_DBUS_RX;
extern SPI_HandleTypeDef HSPI_IMU;
extern I2C_HandleTypeDef HIIC_IMU;

#ifdef __cplusplus
 }
#endif

#endif /* __OVERRIDE__H__ */