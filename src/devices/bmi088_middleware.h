#pragma once
#ifndef BMI088_MIDDLEWARE_H
#define BMI088_MIDDLEWARE_H

#include <stdint.h>

#define BMI088_USE_SPI
//#define BMI088_USE_IIC

extern void bmi088_GPIO_init(void);
extern void bmi088_COM_init(void);
extern void bmi088_delay_ms(uint16_t ms);
extern void bmi088_delay_us(uint16_t us);

#if defined(BMI088_USE_SPI)
extern void bmi088_ACCEL_NS_L(void);
extern void bmi088_ACCEL_NS_H(void);

extern void bmi088_GYRO_NS_L(void);
extern void bmi088_GYRO_NS_H(void);

extern uint8_t bmi088_read_write_byte(uint8_t reg);

#elif defined(BMI088_USE_IIC)

#endif

#endif
