#include <stm32f4xx_hal.h>
#include "init.h"
#include "main.h"
#include "tool/buzzer.h"
#include "tool/can_filter.h"
#include "app/connectivity.h"

HAL_StatusTypeDef startRedLED(void)
{
    HAL_GPIO_Init(LED_RED_GPIO_Port, &(GPIO_InitTypeDef){LED_RED_Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP});
    HAL_GPIO_Init(LED_GREEN_GPIO_Port, &(GPIO_InitTypeDef){LED_GREEN_Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP});
    // HAL_Delay(2000);
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
    return HAL_OK;
}

HAL_StatusTypeDef userCodeInit(void)
{
    // buzzerStartUp();
    startRedLED();
    canFilterInit();
    initDmaCache(cacheArray[0], cacheArray[1], LEGACY_CACHE_CHAR_LEN);
    return HAL_OK;
}
