#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
#include "override.h"
#include "usart.h"


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
  HAL_UART_Transmit(&huart6, (uint8_t *)&ch, 1, 0xff);
  osDelay(1000);
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

   HAL_StatusTypeDef status =
      HAL_UART_Transmit(&huart6, (uint8_t*)data, len, 0xff);

   // return # of bytes written - as best we can tell
   return (status == HAL_OK ? len : 0);
}