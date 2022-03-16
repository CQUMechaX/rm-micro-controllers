/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       IST8310middleware.c/h
  * @brief      IST8310磁力计中间层，完成IST8310的IIC通信,因为设置MPU6500的SPI通信
  *             所以设置的是通过mpu6500的IIC_SLV0完成读取，IIC_SLV4完成写入。
  * @note       IST8310只支持IIC读取
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */
#pragma once
#ifndef IST8310DRIVER_MIDDLEWARE_H
#define IST8310DRIVER_MIDDLEWARE_H

// #include "struct_typedef.h"
#include <stdint.h>

#define IST8310_IIC_ADDRESS (0x0E << 1)  //IST8310的IIC地址
#define IST8310_IIC_READ_MSB (0x80) //IST8310的SPI读取发送第一个bit为1

void ist8310_GPIO_init(void); //ist8310的io初始化
void ist8310_com_init(void);  //ist8310的通讯初始化
uint8_t ist8310_IIC_read_single_reg(uint8_t reg);
void ist8310_IIC_write_single_reg(uint8_t reg, uint8_t data);
void ist8310_IIC_read_muli_reg(uint8_t reg, uint8_t *buf, uint8_t len);
void ist8310_IIC_write_muli_reg(uint8_t reg, uint8_t *data, uint8_t len);
void deviceIst8310DelayMs(uint16_t ms);
void deviceIst8310DelayUs(uint16_t us);
void deviceIst8310ResetRise(void); //复位IO 置高 posedge
void deviceIst8310ResetFall(void); //复位IO 置地 nededge 置地会引起ist8310重启

#endif
