#ifndef __OVERRIDE__H__
#define __OVERRIDE__H__

#include <stm32f4xx_hal.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define GCC_PACKED __packed

// #define __TRANSIMITION_DBUS_PART

#define __OVERRIDE_SPACE
#define __OVERRIDE_CONCAT2(a, b) __OVERRIDE_INTERNEL_CONCAT2(a, b)
#define __OVERRIDE_INTERNEL_CONCAT2(a, b) a ## b
#define __OVERRIDE_CONCAT3(a, b, c) __OVERRIDE_INTERNEL_CONCAT3(a, b, c)
#define __OVERRIDE_INTERNEL_CONCAT3(a, b, c) a ## b ## c

#ifndef __GIT_COMMIT_ID__
# define __GIT_COMMIT_ID__ "[ERROR]-Please-enable-extra_script.py."
#endif /* __GIT_COMMIT_ID__ */

#if defined(HAL_TIM_MODULE_ENABLED)
#else
# warning "TIM Module didn't enabled for STM32 HAL LIB."
  typedef uint8_t TIM_HandleTypeDef;
#endif /* defined(HAL_TIM_MODULE_ENABLED) */
#if defined(HAL_CAN_MODULE_ENABLED)
#else
# warning "CAN Module didn't enabled for STM32 HAL LIB."
  typedef uint8_t CAN_HandleTypeDef;
#endif /* defined(HAL_CAN_MODULE_ENABLED) */
#if defined(HAL_UART_MODULE_ENABLED)
#else
# warning "UART Module didn't enabled for STM32 HAL LIB."
  typedef uint8_t UART_HandleTypeDef;
#endif /* defined(HAL_UART_MODULE_ENABLED) */
#if defined(HAL_DMA_MODULE_ENABLED)
#else
# warning "DMA Module didn't enabled for STM32 HAL LIB."
  typedef uint8_t DMA_HandleTypeDef;
#endif /* defined(HAL_DMA_MODULE_ENABLED) */
#if defined(HAL_SPI_MODULE_ENABLED)
#else
# warning "SPI Module didn't enabled for STM32 HAL LIB."
  typedef uint8_t SPI_HandleTypeDef;
#endif /* defined(HAL_SPI_MODULE_ENABLED) */
#if defined(HAL_I2C_MODULE_ENABLED)
#else
# warning "I2C Module didn't enabled for STM32 HAL LIB."
  typedef uint8_t I2C_HandleTypeDef;
#endif /* defined(HAL_I2C_MODULE_ENABLED) */


#if defined(RM_DEV_A)
  #define TIM_BUZZER_ID 12
  #define TIM_BUZZER_CHANNEL 4
  #define UART1_ID 7
  #define UART2_ID 6
  #define UART3_ID 8
  #define UART_DBUS_ID 1
  #define HTIM_BUZZER htim12
  #define HTIM_BUZZER_CHANNEL TIM_CHANNEL_1
  #define HCAN1 hcan1
  #define HCAN2 hcan2
#elif defined(RM_DEV_C)
  #define STM32_CCMRAM_EXIST
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
#define HUART_LEGACY __OVERRIDE_CONCAT2(huart, UART2_ID)
#define HDMA2_RX __OVERRIDE_CONCAT3(hdma_usart, UART2_ID, _rx)
#define HDMA_DBUS_RX __OVERRIDE_CONCAT3(hdma_usart, UART_DBUS_ID, _rx)

void delay_us(uint16_t nus);
void freertosErrorHandler(const char *file, uint32_t line);

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