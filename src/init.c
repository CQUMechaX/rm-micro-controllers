#include "init.h"

#include <stm32f4xx_hal.h>

#include "main.h"
#include "tool/can_filter.h"
#include "tool/pwm.h"
#include "tool/transimition.h"
#include "usart.h"

HAL_StatusTypeDef startRedLED(void)
{
  // HAL_GPIO_Init(LED_RED_GPIO_Port, &(GPIO_InitTypeDef){LED_RED_Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP});
  // HAL_GPIO_Init(LED_BLUE_GPIO_Port, &(GPIO_InitTypeDef){LED_BLUE_Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP});
  HAL_GPIO_Init(
    LED_GREEN_GPIO_Port, &(GPIO_InitTypeDef){LED_GREEN_Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP});
  // #ifdef RM_DEV_C
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
  // #endif /* RM_DEV_C */
  //LED Green on
  return HAL_OK;
}

/** @return HAL_StatusTypeDef::HAL_OK
 * This function should be called before any task, to initilize CAN communication.
 */
HAL_StatusTypeDef userCodeInit(void)
{
  // buzzerStartUp();
  startRedLED();
  transimitionCanStart();
  // transimitionImuStart();
  // while(1)
  {
    // HAL_UART_Receive(&HUART_DBUS, gDbusCacheArray[0], DBUS_CACHE_BYTE_LEN, 10000);
    // transimitionDbusRx(0);
  }
  //HAL_UART_Receive_DMA(&HUART2, gDbusCacheArray[0], 16);
  transimitionDmaInit(&HUART2, gLegacyCacheArray[0], gLegacyCacheArray[1], LEGACY_CACHE_BYTE_LEN);
  transimitionDmaInit(&HUART_DBUS, gDbusCacheArray[0], gDbusCacheArray[1], DBUS_CACHE_BYTE_LEN);
#if defined(RM_DEV_A)
#elif defined(RM_DEV_C)
#endif /* RM_DEV_C */
  return HAL_OK;
}
