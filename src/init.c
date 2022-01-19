#include <stm32f4xx_hal.h>
#include "init.h"
#include "main.h"
#include "usart.h"
#include "tool/buzzer.h"
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
    canFilterInit();
#ifdef RM_DEV_C
    extern DMA_HandleTypeDef hdma_usart1_rx;
    initDmaCache(huart1, hdma_usart1_rx, cacheArray[0], cacheArray[1], LEGACY_CACHE_BYTE_LEN);
#endif /* RM_DEV_C */
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