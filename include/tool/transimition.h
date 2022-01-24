/** @file transimition.c/h
 * @brief unlike override.c/h, any port can be configured by EEPROM's settings.
 * This file controls dbus(usart), motor_message(can), computer_layer_legacy(usart),
 * and the function of 
 */
#pragma once
#ifndef __TRANSIMITION_H__
#define __TRANSIMITION_H__

// include
#include <stdint.h>
#include <stdbool.h>
#include "override.h"

#ifdef __cplusplus
 extern "C" {
#endif

// definition
/* ----------------------- PC Key Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W            ((uint16_t)1 << 0)
#define KEY_PRESSED_OFFSET_S            ((uint16_t)1 << 1)
#define KEY_PRESSED_OFFSET_A            ((uint16_t)1 << 2)
#define KEY_PRESSED_OFFSET_D            ((uint16_t)1 << 3)
#define KEY_PRESSED_OFFSET_SHIFT        ((uint16_t)1 << 4)
#define KEY_PRESSED_OFFSET_CTRL         ((uint16_t)1 << 5)
#define KEY_PRESSED_OFFSET_Q            ((uint16_t)1 << 6)
#define KEY_PRESSED_OFFSET_E            ((uint16_t)1 << 7)
#define KEY_PRESSED_OFFSET_R            ((uint16_t)1 << 8)
#define KEY_PRESSED_OFFSET_F            ((uint16_t)1 << 9)
#define KEY_PRESSED_OFFSET_G            ((uint16_t)1 << 10)
#define KEY_PRESSED_OFFSET_Z            ((uint16_t)1 << 11)
#define KEY_PRESSED_OFFSET_X            ((uint16_t)1 << 12)
#define KEY_PRESSED_OFFSET_C            ((uint16_t)1 << 13)
#define KEY_PRESSED_OFFSET_V            ((uint16_t)1 << 14)
#define KEY_PRESSED_OFFSET_B            ((uint16_t)1 << 15)
#define RC_CH_VALUE_MIN         ((uint16_t)364)
#define RC_CH_VALUE_OFFSET      ((uint16_t)1024)
#define RC_CH_VALUE_MAX         ((uint16_t)1684)
#define RC_CHANNEL_VALUE_ERROR      ((uint16_t)660)

/** DMA cache length */
#define LEGACY_CACHE_BYTE_LEN 32
#define LEGACY_FRAME_BYTE_LEN 16
#define DBUS_CACHE_BYTE_LEN 72
#define DBUS_FRAME_BYTE_LEN 18

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
}DbusData;

extern uint8_t gLegacyCacheArray[2][LEGACY_CACHE_BYTE_LEN], acmCacheArray[LEGACY_CACHE_BYTE_LEN];
extern double *resultArray;
extern uint8_t gDbusCacheArray[2][DBUS_CACHE_BYTE_LEN];
/** CAN id marco */
extern const uint32_t gCanHeadTarget[17], gCanHeadFeedback[17];/** GM6020 id1 == id9 */


void dmaProcessHandler(
    UART_HandleTypeDef uart, DMA_HandleTypeDef rx_dma, uint8_t cache_byte_len,
    uint8_t frame_byte_len, HAL_StatusTypeDef( * callback_function)(bool)
    );
void initDmaCache(UART_HandleTypeDef huart, DMA_HandleTypeDef hdma, uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num);
HAL_StatusTypeDef transimitionLegacyRx(bool section);
HAL_StatusTypeDef transimitionDbusRx(bool section);
HAL_StatusTypeDef transimitionCanStart(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
bool dbusUpdate(uint8_t * rx_data);
HAL_StatusTypeDef transimitionCanTx(
    CAN_HandleTypeDef * hcan, uint32_t std_id,
    uint16_t motor1, uint16_t motor2, uint16_t motor3, uint16_t motor4
    );

#ifdef __cplusplus
 }
#endif

#endif /* __TRANSIMITION_H__ */