#include <cmsis_os2.h>
#include "marco.hpp"
#include "app/gimbal_control.hpp"

void (* gimbalControlExternC)(void * pvParameters) = gimbalControl;
GimbalControl g_gimbal;

void gimbalControl(void * pvParameters)
{
    g_gimbal.init();
    osDelay(100);
    while(true)
    {
        if(true);
        g_gimbal.update();
        osDelay(g_gimbal.tick_ms);
    }
    return;
}

bool GimbalControl::init(void)
{
    this -> joint[0] = JointData{g_joint_default[JointType::RM6020], 0x209, 0x2FF};
    this -> joint[1] = JointData{g_joint_default[JointType::RM6020], 0x20A, 0x2FF};
    return true;
}

bool GimbalControl::update(void)
{
    return true;
}

double GimbalControl::pid_speed(uint8_t tick, JointData * joint)
{
    JointCoeff coeff = joint->coeff;
    JointData::PidInfo * pid = &joint->pid_calc[1];
    return pid->out;
} 

double GimbalControl::pid_angle(uint8_t tick, JointData * joint, double get, double set, double error_delta)
{
    JointCoeff coeff = joint->coeff;
    JointData::PidInfo * pid = &joint->pid_calc[1];
    pid->feed = get;
    pid->cmd = set;
    // pid->error = rad_format(set - get);
    pid->p_out = coeff.kp[1] * pid->error;
    pid->i_out = CONSTRAIN_ABS(
        pid->i_out + coeff.ki[1] * pid->error,
        coeff.ki_limit[1]
    );
    pid->d_out = coeff.kd[1] * error_delta;
    pid->out = CONSTRAIN_ABS(
        pid->p_out + pid->i_out + pid->d_out,
        coeff.k_limit[1]
    );
    return pid->out;
} 
