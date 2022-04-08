#include "app/sensor_unit.hpp"

#include <cmsis_os.h>

#include "devices/bmi088.h"
#include "devices/ist8310.h"
#include "tool/pid.hpp"
#include "tool/pwm.h"

void sensorExternInterrupt(void * arguments)
{
  while (deviceBmi088Init()) {
    osDelay(100);
  }
  PidControl bmi088_temperature(1600, 0.2, 0, 40);
  extern TIM_HandleTypeDef htim10;
  if (HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1) != HAL_OK) {
    
  }

  while (ulTaskNotifyTake(pdFALSE, /*pdMS_TO_TICKS(0))*/ portMAX_DELAY)) {
    float a[3], b[3], c, d;
    deviceBmi088Read(a, b, &c);
    d = bmi088_temperature.feedback(c);
    d = (d > 0) ? d : 0;
    heatTrigger(0, static_cast<uint16_t>(d));
  }

  vTaskDelete(NULL);
}

void sensorPeriodical(void * arguments)
{
  //void &
  while (deviceIst8310Init()) {
    osDelay(100);
  }
  while (ulTaskNotifyTake(pdFALSE, /*pdMS_TO_TICKS(0))*/ portMAX_DELAY)) {
    float a[3];
    deviceIst8310ReadMagnitude(a);
  }

  vTaskDelete(NULL);
}
