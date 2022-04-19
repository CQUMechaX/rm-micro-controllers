/** @file transimition.c/h
 * @brief unlike override.c/h, any port can be configured by EEPROM's settings.
 * This file controls dbus(usart), motor_message(can), computer_layer_legacy(usart),
 * and the function of 
 */
#pragma once
#ifndef __TRANSIMITION_H__
#define __TRANSIMITION_H__

// include
#include <stdbool.h>
#include <stdint.h>

#include "override.h"

#ifdef __cplusplus
extern "C" {
#endif

// definition
/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W ((uint16_t)1 << 0)
#define KEY_PRESSED_OFFSET_S ((uint16_t)1 << 1)
#define KEY_PRESSED_OFFSET_A ((uint16_t)1 << 2)
#define KEY_PRESSED_OFFSET_D ((uint16_t)1 << 3)
#define KEY_PRESSED_OFFSET_SHIFT ((uint16_t)1 << 4)
#define KEY_PRESSED_OFFSET_CTRL ((uint16_t)1 << 5)
#define KEY_PRESSED_OFFSET_Q ((uint16_t)1 << 6)
#define KEY_PRESSED_OFFSET_E ((uint16_t)1 << 7)
#define KEY_PRESSED_OFFSET_R ((uint16_t)1 << 8)
#define KEY_PRESSED_OFFSET_F ((uint16_t)1 << 9)
#define KEY_PRESSED_OFFSET_G ((uint16_t)1 << 10)
#define KEY_PRESSED_OFFSET_Z ((uint16_t)1 << 11)
#define KEY_PRESSED_OFFSET_X ((uint16_t)1 << 12)
#define KEY_PRESSED_OFFSET_C ((uint16_t)1 << 13)
#define KEY_PRESSED_OFFSET_V ((uint16_t)1 << 14)
#define KEY_PRESSED_OFFSET_B ((uint16_t)1 << 15)
#define RC_CH_VALUE_MIN ((uint16_t)364)
#define RC_CH_VALUE_OFFSET ((uint16_t)1024)
#define RC_CH_VALUE_MAX ((uint16_t)1684)
#define RC_CHANNEL_VALUE_ERROR ((uint16_t)660)

/** DMA cache length */
#define LEGACY_CACHE_BYTE_LEN 64
#define LEGACY_FRAME_BYTE_LEN 32
#define DBUS_CACHE_BYTE_LEN 32
#define DBUS_FRAME_BYTE_LEN 18
#define ACM_CACHE_BYTE_LEN 32

// struct definition for custom
typedef struct GCC_PACKED DbusData
{
  struct GCC_PACKED Rc
  {
    int16_t channel[5];
    uint8_t button[2];
  } rc;
  struct GCC_PACKED Mouse
  {
    int16_t x;
    int16_t y;
    int16_t z;
    uint8_t press_l;
    uint8_t press_r;
  } mouse;
  struct GCC_PACKED Key
  {
    uint16_t v;
  } key;
} DbusData;

extern uint8_t gLegacyCacheArray[2][LEGACY_CACHE_BYTE_LEN], gDbusCacheArray[2][DBUS_CACHE_BYTE_LEN];
extern double * gLegacyResultArray;

extern const uint32_t gCanHeadTarget[17],
  gCanHeadFeedback[17]; /** CAN StdId marco for RM motor, GM6020 id1 :=> id5 */

/** @brief Requested by HAL_UARTEx_RxEventCallback
 * @file transimition.cpp
 * @see HAL_UARTEx_RxEventCallback
 */
HAL_StatusTypeDef transimitionIdleHandler(
  UART_HandleTypeDef * huart, uint16_t rx_byte_len, uint16_t buffer_byte_len,
  uint16_t frame_byte_len, HAL_StatusTypeDef (*callback_function)(bool));
/** @brief Requested by userCodeInit. Enable dual dma channel by setting DMA_SxCR_DBM, etc. Enable HAL_UART_RECEPTION_TOIDLE. IT only idle.
 * @note HAL_UART_RECEPTION_TOIDLE(huart->ReceptionType) is toggled by HAL_UARTEx_ReceiveToIdlexxx(), and in STM32 hal library's 
 *       UART_IRQ_Handler, this reception type will be recognized to catch IDLE_IT and call HAL_UARTEx_RxEventCallback(), instead of
 *       HAL_UART_RxCpltCallback() called by RXNE_IT. Before function call, IDLE_IT might be disabled for next HAL_UARTEx_ReceiveToIdlexxx().
 *       Hence, transimitionDmaInit implements necessary function in UART_Start_Receive_DMA() from ReceiveToIdle_DMA.
 * @file transimition.cpp
 * @see init.c
 */
bool transimitionDmaInit(
  UART_HandleTypeDef * huart, uint8_t * rx1_buf, uint8_t * rx2_buf, uint16_t bufer_byte_len);
HAL_StatusTypeDef transimitionLegacyRx(bool section);
HAL_StatusTypeDef transimitionDbusRx(bool section);
bool transimitionUsbCdcRx(uint8_t * buf, uint32_t len, uint8_t index);
uint32_t transimitionUsbCdcReadBlock(uint8_t * buf, uint32_t len, uint8_t index, uint32_t time_out);
HAL_StatusTypeDef transimitionCanStart(void);

HAL_StatusTypeDef transimitionCanTx(
  CAN_HandleTypeDef * hcan, uint32_t std_id, int16_t motor1, int16_t motor2, int16_t motor3,
  int16_t motor4);

/** @brief Implement of __weak function in stm32f4xx_hal_can.c:
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef * hcan);
/** @brief Implement of __weak function in stm32f4xx_hal_uart.c:2709,
 *   Reception Event Callback (Rx event notification called after use of advanced reception service).
 * @param  huart UART handle
 * @param  Size  Number of data available in application reception buffer (indicates a position in
 *               reception buffer until which, data are available)
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t Size);

#ifdef __cplusplus
}
#endif

#endif /* __TRANSIMITION_H__ */