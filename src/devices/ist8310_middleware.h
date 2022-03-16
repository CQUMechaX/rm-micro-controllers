/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       IST8310middleware.c/h
  * @brief      IST8310�������м�㣬���IST8310��IICͨ��,��Ϊ����MPU6500��SPIͨ��
  *             �������õ���ͨ��mpu6500��IIC_SLV0��ɶ�ȡ��IIC_SLV4���д�롣
  * @note       IST8310ֻ֧��IIC��ȡ
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. ���
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

#define IST8310_IIC_ADDRESS (0x0E << 1)  //IST8310��IIC��ַ
#define IST8310_IIC_READ_MSB (0x80) //IST8310��SPI��ȡ���͵�һ��bitΪ1

void ist8310_GPIO_init(void); //ist8310��io��ʼ��
void ist8310_com_init(void);  //ist8310��ͨѶ��ʼ��
uint8_t ist8310_IIC_read_single_reg(uint8_t reg);
void ist8310_IIC_write_single_reg(uint8_t reg, uint8_t data);
void ist8310_IIC_read_muli_reg(uint8_t reg, uint8_t *buf, uint8_t len);
void ist8310_IIC_write_muli_reg(uint8_t reg, uint8_t *data, uint8_t len);
void deviceIst8310DelayMs(uint16_t ms);
void deviceIst8310DelayUs(uint16_t us);
void deviceIst8310ResetRise(void); //��λIO �ø� posedge
void deviceIst8310ResetFall(void); //��λIO �õ� nededge �õػ�����ist8310����

#endif
