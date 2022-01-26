#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "override.h"
#include "usart.h"
#include "tool/pwm.h"
#include <cmsis_os.h>


#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief This function should be implemented when keil used.
 */
PUTCHAR_PROTOTYPE
{
//   HAL_UART_Transmit(&huart6, (uint8_t *)&ch, 1, 0xff);
//   osDelay(1000);
  return ch;
}

/**
 * @ref https://electronics.stackexchange.com/questions/206113
 */
int _write(int file, char *data, int len)
{
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
    {
        errno = EBADF;
        return -1;
    }

    HAL_StatusTypeDef status = 0;
    // status = HAL_UART_Transmit(&huart6, (uint8_t*)data, len, 0xff);
    // status = HAL_UART_Transmit_DMA(&huart6, data, len);

    // return # of bytes written - as best we can tell
    return (status == HAL_OK ? len : 0);
}

/** with SysTick and SystemCoreClock
 */
void delay_us(uint16_t nus)
{
    static uint32_t ticks, told, tnow;
    const uint32_t us_clock = SystemCoreClock / 100000000;
    uint32_t tcnt = 0;
    ticks = nus * us_clock;
    told = SysTick->VAL;
    while (true)
    {
        tnow = SysTick->VAL;
        if (tnow <= told)
        {
            tcnt += told - tnow;
        }
        else
        {
            tcnt += SysTick->LOAD - tnow + told;
        }
        told = tnow;
        if (tcnt >= ticks)
        {
            break;
        }
    }
}


void freertosErrorHandler(const char *file, uint32_t line)
{
#ifndef __RELEASE_BUILD
    taskENTER_CRITICAL();
    while(true)
    {
        printf("Error on %s:%ld.", file, line);
        buzzerTrigger(100, 10);
    }
    taskEXIT_CRITICAL();
#endif /* __RELEASE_BUILD */
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