#include <cmsis_os2.h>
#include "marco.hpp"
#include "app/gimbal_control.hpp"
#include "app/device_monitor.hpp"
#include "app/microros_param.h"

GimbalControl gGimbal;

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
    this->joint_[0] = JointData{g_joint_default[JointType::RM6020], 5};//, 0x209, 0x2FF};
    this->joint_id_ptr_[5] = &this->joint_[0];
    gDeviceMonitor.register_and_init(*this->hcan_, this->joint_[0]);
    this->joint_[1] = JointData{g_joint_default[JointType::RM6020], 6};//, 0x20A, 0x2FF};
    this->joint_id_ptr_[6] = &this->joint_[1];
    gDeviceMonitor.register_and_init(*this->hcan_, this->joint_[1]);
    this->joint_cnt_ = 2;
    return true;
}

bool GimbalControl::update(void)
{
    for(uint8_t i = 0; i != this->joint_cnt_; ++ i)
    {
        gDeviceMonitor.get_online(this->joint_[i]);
        this->pid_speed(0, &this->joint_[i], this->joint_[i].feedback[0].speed, 30);
    }
    this->update_cmd_current();
    return true;
}

double GimbalControl::pid_speed(uint32_t tick, JointData * joint, double get, double set)
{
    PidCoeff coeff = joint->coeff.pid[0];
    PidInfo * pid = &joint->pid_calc[0];
    pid->feed = get;
    pid->cmd = set;
    pid->error[1] = pid->error[0];
    pid->error[0] = set - get;
    return this->pid_delta(tick, pid, coeff);
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
