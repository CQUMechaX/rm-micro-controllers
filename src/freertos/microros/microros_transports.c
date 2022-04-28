#include "microros_transports.h"

#include <cmsis_os.h>
#include <uxr/client/transport.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dma.h"
#include "tool/transimition.h"
#include "usbd_cdc_if.h"

// #include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// --- micro-ROS Transports ---
/*
#define UART_DMA_BUFFER_SIZE 2048

static uint8_t dma_buffer[UART_DMA_BUFFER_SIZE];
static size_t dma_head = 0, dma_tail = 0;

bool transportUxrSerialOpen(struct uxrCustomTransport * transport)
{
  UART_HandleTypeDef * uart = (UART_HandleTypeDef *)transport->args;
  HAL_UART_Receive_DMA(uart, dma_buffer, UART_DMA_BUFFER_SIZE);
  return true;
}

bool transportUxrSerialClose(struct uxrCustomTransport * transport)
{
  UART_HandleTypeDef * uart = (UART_HandleTypeDef *)transport->args;
  HAL_UART_DMAStop(uart);
  return true;
}

size_t transportUxrSerialWrite(
  struct uxrCustomTransport * transport, const uint8_t * buf, size_t len, uint8_t * err)
{
  UART_HandleTypeDef * uart = (UART_HandleTypeDef *)transport->args;

  HAL_StatusTypeDef ret;
  if (uart->gState == HAL_UART_STATE_READY) {
    ret = HAL_UART_Transmit_DMA(uart, (uint8_t *)buf, len);
    while (ret == HAL_OK && uart->gState != HAL_UART_STATE_READY) {
      osDelay(1);
    }

    return (ret == HAL_OK) ? len : 0;
  } else {
    return 0;
  }
}

size_t transportUxrSerialRead(
  struct uxrCustomTransport * transport, uint8_t * buf, size_t len, int timeout, uint8_t * err)
{
  UART_HandleTypeDef * uart = (UART_HandleTypeDef *)transport->args;

  __disable_irq();
  dma_tail = UART_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(uart->hdmarx);
  __enable_irq();

  size_t wrote = 0;
  while ((dma_head != dma_tail) && (wrote < len)) {
    buf[wrote] = dma_buffer[dma_head];
    dma_head = (dma_head + 1) % UART_DMA_BUFFER_SIZE;
    wrote++;
  }

  return wrote;
}
*/

bool transportUxrCdcOpen(struct uxrCustomTransport * transport) { return true; }
bool transportUxrCdcClose(struct uxrCustomTransport * transport) { return true; }
size_t transportUxrCdcWrite(
  struct uxrCustomTransport * transport, const uint8_t * buf, size_t len, uint8_t * err)
{
  uint8_t ret_val;
  while (true) {
    ret_val = CDC_Transmit_FS((uint8_t *)buf, len, 0);
    if (ret_val == USBD_OK) {
      return len;
    } else if (ret_val != USBD_BUSY) {
      return 0; /** This will simply stop any further transmition in this message frame. @see stream_framing_protocol.c*/
    }
  }
}
size_t transportUxrCdcRead(
  struct uxrCustomTransport * transport, uint8_t * buf, size_t len, int timeout, uint8_t * err)
{
  return transimitionUsbCdcReadBlock(buf, len, 0, timeout);
}