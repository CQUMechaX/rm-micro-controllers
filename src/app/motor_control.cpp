#include "app/motor_control.h"
#include <cmsis_os.h>
#include "app/chassis_control.hpp"
#include "app/gimbal_control.hpp"
#include "app/extra_control.hpp"

void motorControl(void * arguments)
{
    gChassis.on_init();
    gGimbal.on_init();
    gBulletMotor.on_init();
    while(true)
    {
        if(true);
        gChassis.update();
        gGimbal.update();
        gBulletMotor.update();
        PartControl::set_can_current();
        osDelay(gGimbal.tick_ms_);
    }
    return;
}