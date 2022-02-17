#include "app/echo_control.h"
#include <cmsis_os2.h>
#include <freertos_os2.h>
#include "app/device_monitor.hpp"
#include "tool/pwm.h"
#include "main.h"
#include "override.h"

void echoControl(void * pvParameters)
{
    while (true)
    {
        for(auto & list_iter : gDeviceMonitor.device_joint_)
        {
            for(DeviceStatus const & device : list_iter)
            {
                if(!device.online)
                {
                    buzzerTrigger(200, 10);
                    for(int i = 1; i <= device.ptr.joint->id; i ++)
                    {
                        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
                        osDelay(100);
                        HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
                        osDelay(100);
                    }
                }
            }
        }
        osDelay(1000);
    }
}