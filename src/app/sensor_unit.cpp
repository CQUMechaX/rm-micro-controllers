#include "app/sensor_unit.hpp"

#include <cmsis_os.h>

#include "devices/ist8310.h"

void sensorExternInterrupt(void * arguments) { vTaskDelete(NULL); }

void sensorPeriodical(void * arguments)
{
  while (deviceIst8310Init()) {
    osDelay(100);
  }
  while (ulTaskNotifyTake(pdFALSE, /*pdMS_TO_TICKS(0))*/ portMAX_DELAY)) {
    float a[3];
    deviceIst8310ReadMagnitude(a);
  }

  vTaskDelete(NULL);
}
