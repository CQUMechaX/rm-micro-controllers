#include <cmsis_os2.h>
#include "marco.hpp"
#include "app/gimbal_control.hpp"

GimbalControl g_gimbal;

void gimbalControl(void * pvParameters)
{
    g_gimbal.on_init();
    osDelay(100);
    while(true)
    {
        if(true);
        g_gimbal.update();
        osDelay(g_gimbal.tick_ms);
    }
    return;
}

bool GimbalControl::on_init(void)
{
    this -> joint[0] = JointData{g_joint_default[JointType::RM6020], 0x209, 0x2FF};
    this -> joint[1] = JointData{g_joint_default[JointType::RM6020], 0x20A, 0x2FF};
    return true;
}

bool GimbalControl::update(void)
{
    return true;
}

double GimbalControl::pid_speed(uint32_t tick, JointData * joint, double get, double set)
{
    PidCoeff coeff = joint->coeff.pid[0];
    PidInfo * pid = &joint->pid_calc[0];
    return pid->out;
} 

double GimbalControl::pid_angle(uint32_t tick, JointData * joint, double get, double set)
{
    PidCoeff coeff = joint->coeff.pid[1];
    PidInfo * pid = &joint->pid_calc[1];
    pid->feed = get;
    pid->cmd = set;
    pid->error[1] = pid->error[0];
    pid->error[0] = RAD_FORMAT(set - get);
    return this->pid_target(tick, pid, coeff);
} 
