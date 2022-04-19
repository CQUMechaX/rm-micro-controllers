#include "app/echo_control.h"

#include <cmsis_os.h>

#include "app/device_monitor.hpp"
#include "main.h"
#include "override.h"
#include "tool/pwm.h"

void echoControl(void * arguments)
{
  buzzerTrigger(50, 900);
  osDelay(200);
  buzzerTrigger(50, 50);
  osDelay(200);
  HAL_TIM_PWM_Stop(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL);
  while (true) {
    for (auto & list_iter : gDeviceMonitor.device_joint_) {
      for (DeviceStatus const & device : list_iter) {
        if (!device.online) {
          buzzerTrigger(10, 50);
          osDelay(200);
          HAL_TIM_PWM_Stop(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL);
          for (int i = 1; i <= device.ptr.joint->id; i++) {
            buzzerTrigger(20, 70);
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
            osDelay(100);
            HAL_TIM_PWM_Stop(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL);
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
            osDelay(200);
          }
        }
      }
    }
    if (
      !gDeviceMonitor.device_dbus_.online ||
      gDeviceMonitor.device_dbus_.error != DeviceErrorList::kNormal) {
      buzzerTrigger(20, 70);
      osDelay(400);
      if (!gDeviceMonitor.device_dbus_.online) {
        HAL_TIM_PWM_Stop(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL);
        osDelay(100);
        buzzerTrigger(20, 70);
        osDelay(400);
      }
      HAL_TIM_PWM_Stop(&HTIM_BUZZER, HTIM_BUZZER_CHANNEL);
    }
    osDelay(5000);
  }
}