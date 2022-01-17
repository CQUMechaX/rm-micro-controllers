#include <stm32f4xx_hal.h>
#include "init.h"
#include "main.h"
#include "tool/buzzer.h"
#include "tool/can_filter.h"

HAL_StatusTypeDef userCodeInit(void)
{
    buzzerStartUp();
    canFilterInit();
    return HAL_OK;
}
