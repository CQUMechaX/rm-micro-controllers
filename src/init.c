#include <stm32f4xx_hal.h>
#include "init.h"
#include "main.h"
#include "usart.h"
#include "tool/pwm.h"
#include "tool/can_filter.h"
#include "tool/transimition.h"

HAL_StatusTypeDef startRedLED(void)
{
    // HAL_GPIO_Init(LED_RED_GPIO_Port, &(GPIO_InitTypeDef){LED_RED_Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP});
    // HAL_GPIO_Init(LED_BLUE_GPIO_Port, &(GPIO_InitTypeDef){LED_BLUE_Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP});
    // HAL_GPIO_Init(LED_GREEN_GPIO_Port, &(GPIO_InitTypeDef){LED_GREEN_Pin, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP});
#ifdef RM_DEV_C
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
#endif /* RM_DEV_C */
    //LED Green on
    return HAL_OK;
}

HAL_StatusTypeDef userCodeInit(void)
{
    // buzzerStartUp();
    startRedLED();
    transimitionCanStart();
    // transimitionImuStart();
    initDmaCache(HUART2, HDMA2_RX, gLegacyCacheArray[0], gLegacyCacheArray[1], LEGACY_CACHE_BYTE_LEN);
    initDmaCache(HUART_DBUS, HDMA_DBUS_RX, gDbusCacheArray[0], gDbusCacheArray[1], DBUS_CACHE_BYTE_LEN);
#if defined(RM_DEV_A)
#elif defined(RM_DEV_C)
#endif /* RM_DEV_C */
// while(1)
    // HAL_UART_Transmit(&huart1, "help", 4, 1000);
    return HAL_OK;
}

#define TickType_t 
#define C(x)    x
#define D(x)    C x
#include "FreeRTOSConfig.h"
#if D(D(configTICK_RATE_HZ)) != 1000
# error "configTICK_RATE_HZ != 1000 cause incorrect call of vTaskDelay(). Aborting."
#endif
#undef TickType_t
/* https://stackoverflow.com/questions/19406246/remove-cast-from-constant-in-preprocessor */