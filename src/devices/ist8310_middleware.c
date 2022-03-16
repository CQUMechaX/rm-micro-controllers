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

#include "ist8310_middleware.h"

#include <cmsis_os.h>

#include "main.h"
#include "override.h"

void ist8310_GPIO_init(void) {}

void ist8310_com_init(void) {}

uint8_t ist8310_IIC_read_single_reg(uint8_t reg)
{
  uint8_t res;
  HAL_I2C_Mem_Read(&HI2C_IMU, IST8310_IIC_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &res, 1, 100);
  return res;
}
void ist8310_IIC_write_single_reg(uint8_t reg, uint8_t data)
{
  HAL_I2C_Mem_Write(&HI2C_IMU, IST8310_IIC_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}
void ist8310_IIC_read_muli_reg(uint8_t reg, uint8_t * buf, uint8_t len)
{
  HAL_I2C_Mem_Read(&HI2C_IMU, IST8310_IIC_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100);
}
void ist8310_IIC_write_muli_reg(uint8_t reg, uint8_t * data, uint8_t len)
{
  HAL_I2C_Mem_Write(&HI2C_IMU, IST8310_IIC_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
}
void deviceIst8310DelayMs(uint16_t ms) { osDelay(ms); }
void deviceIst8310DelayUs(uint16_t us) { delay_us(us); }

void deviceIst8310ResetRise(void)
{
  HAL_GPIO_WritePin(CMPS_RESET_GPIO_Port, CMPS_RESET_Pin, GPIO_PIN_SET);
}
void deviceIst8310ResetFall(void)
{
  HAL_GPIO_WritePin(CMPS_RESET_GPIO_Port, CMPS_RESET_Pin, GPIO_PIN_RESET);
}
