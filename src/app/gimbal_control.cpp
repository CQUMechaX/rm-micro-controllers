#include <cmsis_os2.h>
#include "marco.hpp"
#include "app/gimbal_control.hpp"
#include "app/device_monitor.hpp"
#include "app/microros_param.h"

GimbalControl gGimbal;
double gGimbalTest;

void gimbalControl(void * pvParameters)
{
    gGimbal.on_init();
    // osDelay(100);
    while(true)
    {
        if(true);
        gGimbal.update();
        osDelay(gGimbal.tick_ms_);
    }
    return;
}

bool GimbalControl::on_init(void)
{
    this->hcan_ = gRosParam.hcan_gimbal;
    this->joint_add(JointType::RM3508, 4);/** orbit */
    this->joint_[0].coeff.pid->ki_limit = 200;
    this->joint_add(JointType::RM6020, 5);/** yaw */
    this->joint_add(JointType::RM6020, 6);/** pitch */
    return true;
}

bool GimbalControl::update(void)
{
    static double _temp = 1;
    for(uint8_t i = 0; i != this->joint_cnt_; ++ i)
    {
        gDeviceMonitor.get_online(this->joint_[i]);
        // this->pid_speed(0, &this->joint_[i], this->joint_[i].feedback[0].speed, 30);
    }
    if(NORMAL_OR_ZERO(gLegacyResultArray[0]) && NORMAL_OR_ZERO(gLegacyResultArray[1])
        && NORMAL_OR_ZERO(gLegacyResultArray[2])
        && NORMAL_OR_ZERO(gLegacyResultArray[3])
        )
    {
        this->joint_[0].coeff.pid[0].kp = gLegacyResultArray[0];
        this->joint_[0].coeff.pid[0].ki = gLegacyResultArray[2];
        this->joint_[0].coeff.pid[0].kd = gLegacyResultArray[3];
        _temp = gLegacyResultArray[1];
    }
    if(!((bool)gLegacyResultArray[0] | (bool)gLegacyResultArray[1] | (bool)gLegacyResultArray[2] |
        (bool)gLegacyResultArray[3]))
        {this->joint_[0].pid_calc->out = 0;}
    this->pid_speed(0, &this->joint_[0], this->get_mean_speed(this->joint_[0]), _temp);
    gGimbalTest = this->get_mean_speed(this->joint_[0]);
    // this->update_cmd_current();
    return true;
}

double GimbalControl::pid_speed(uint32_t tick, JointData * joint, double get, double set)
{
    PidCoeff coeff = joint->coeff.pid[0];
    PidInfo * pid = &joint->pid_calc[0];
    pid->feed = get;
    pid->cmd = set;
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = set - get;
    return joint->target.current = this->pid_delta(tick, pid, coeff);
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
