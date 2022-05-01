/** @file protocol_dbus.h
 * @brief This file implements DJI Dbus specification, and corresponding enum type.
 * @author Tiger3018
 * @date 2022-04-28
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// include
#include <stdbool.h>
#include <stdint.h>

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

typedef enum DbusDataEnum {
  channel_left_x = 2,
  channel_left_y = 3,
  channel_right_x = 0,
  channel_right_y = 1,
  channel_toggle = 4,
  switch_left = 1,
  switch_right = 0,
  left_off = 1,
  left_cl = 0,
  left_hl = 2,
  right_gps = 1,
  right_still = 0, /** center ATTI */
  right_down = 2, /** buttom ATTI */
} DbusDataEnum;

#ifdef __cplusplus
}
#endif