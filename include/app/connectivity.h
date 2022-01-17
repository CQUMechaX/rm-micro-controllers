/** @file connectivity.h
 * @brief unlike overwrite.c/h, com port can be configured by eeprom's settings.
 */
#pragma once
#ifndef __CONNECTIVITY_H__
#define __CONNECTIVITY_H__

#ifdef __cplusplus
 extern "C" {
#endif

// include
#include <stdint.h>
#include "override.h"

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

// struct definition for custom 
typedef struct GCC_PACKED
{
    struct GCC_PACKED
    {
        int16_t ch[5];
        char s[2];
    } rc;
    struct GCC_PACKED
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    } mouse;
    struct GCC_PACKED
    {
        uint16_t v;
    } key;

} RC_ctrl_t;

#ifdef __cplusplus
 }
#endif

#endif /* __CONNECTIVITY_H__ */