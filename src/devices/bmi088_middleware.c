#include "bmi088_middleware.h"
#include "main.h"
#include "override.h"
#include <cmsis_os.h>
// #include "bsp_delay.h"

void bmi088_GPIO_init(void)
{

}

void bmi088_COM_init(void)
{


}

void bmi088_delay_ms(uint16_t ms)
{

    osDelay(ms);
}

void bmi088_delay_us(uint16_t us)
{
    delay_us(us);
    
}




void bmi088_ACCEL_NS_L(void)
{
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_RESET);
}
void bmi088_ACCEL_NS_H(void)
{
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_SET);
}

void bmi088_GYRO_NS_L(void)
{
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_RESET);
}
void bmi088_GYRO_NS_H(void)
{
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_SET);
}

uint8_t bmi088_read_write_byte(uint8_t txdata)
{
    uint8_t rx_data = 0;
    HAL_SPI_TransmitReceive(&HSPI_IMU, &txdata, &rx_data, 1, 1000);
    return rx_data;
}

