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
    this->joint_add(JointType::RM3508, 1);/** orbit */
    this->joint_[0].coeff.pid[0].ki_limit = 200;
    this->joint_add(JointType::RM6020, 1);/** pitch H3600 - 4800 - 6000L */
    // this->joint_[1].coeff.pid[1].kp = 3.5;
    this->joint_[1].coeff.pid[1].ki = 3e-3;
    this->joint_[1].coeff.current_limit[0] = -10000;
    this->joint_[1].coeff.current_limit[1] = 10000;
    this->joint_add(JointType::RM6020, 2);/** yaw L6000 - 7500 - 809(9000)R */
    this->joint_[2].coeff.current_limit[0] = -10000;
    this->joint_[2].coeff.current_limit[1] = 10000;
    // this->joint_[2].coeff.pid[1].kd = 0.05;
    // this->joint_[2].coeff.pid[1].ki = 1;
    // this->joint_[2].coeff.pid[1].ki = 0;
    return true;
}

bool GimbalControl::update(void)
{
    static int16_t angle_list[3][3] = {{0, 0}, {2000, 1200}, {7500, 1500}};
    this->joint_[0].target.current = 
        this->pid_speed(0, this->joint_[0], this->get_mean_speed(this->joint_[0]), 
            2700 * [](uint8_t channel)->double{
                return 1.0 * gDeviceMonitor.device_dbus_.data.dbus.rc.channel[channel] / RC_CHANNEL_VALUE_ERROR;
            }(0));

    /** NO DATA or ATTI(1)*/
    if((gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 3)||
        (gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 0))
    {
        this->mode_ = test;
    }
    else
    {
        this->mode_ = zero_speed;
    }

    /** OFF */
    if(gDeviceMonitor.device_dbus_.data.dbus.rc.button[1] == 1)
    {
        angle_list[1][0] = this->joint_[1].feedback[0].angle;
        angle_list[2][0] = this->joint_[2].feedback[0].angle;
    }

    bool mode_changed = this->get_mode_change(this->mode_);
    if(mode_changed && this->mode_ == ControllerMode::zero_speed)
    {
        angle_list[1][2] = this->joint_[1].feedback[0].angle;
        angle_list[2][2] = this->joint_[2].feedback[0].angle;
    }
    for(uint8_t i = 1; i != this->joint_cnt_ ; ++ i)
    {
        if(!gDeviceMonitor.get_online(this->joint_[i]) || mode_changed)
        {
            this->joint_[i].pid_calc[0].i_out = this->joint_[i].pid_calc[0].out = 0;
            this->joint_[i].pid_calc[1].i_out = this->joint_[i].pid_calc[1].out = 0;
        }
        if(this->mode_ == ControllerMode::test)
        {
            this->joint_[i].target.angle = 
                /*FORMAT_ANGLE*/(angle_list[i][0] + angle_list[i][1] * [](uint8_t channel)->double{
                    return 1.0 * gDeviceMonitor.device_dbus_.data.dbus.rc.channel[channel] / RC_CHANNEL_VALUE_ERROR;
                }((i==1)?(3):(2)))/*, this->joint_[i].coeff.angle_limit[0])*/;
        }
        else
        {
            this->joint_[i].target.angle = angle_list[i][2];
        }
        if(gDeviceMonitor.get_online(this->joint_[i]) && ! (gDeviceMonitor.device_dbus_.data.dbus.rc.button[1] == 1))
        {
            this->joint_[i].target.speed = 
                this->pid_angle(0, this->joint_[i], this->joint_[i].feedback[0].angle, this->joint_[i].target.angle);
            this->joint_[i].target.current = 
                this->pid_speed(0, this->joint_[i], this->get_mean_speed(this->joint_[i]), this->joint_[i].target.speed);
            this->joint_[i].target.current = CONSTRAIN_ARR(this->joint_[i].target.current, this->joint_[i].coeff.current_limit);
        }
        else
        {
            this->joint_[i].target.current = 0;
        }
    }
    this->hcan_ = &hcan1; //???
    this->update_cmd_current();
    return true;
}

double GimbalControl::pid_speed(uint32_t tick, JointData & joint, double get, double set)
{
    PidCoeff coeff = joint.coeff.pid[0];
    PidInfo * pid = &joint.pid_calc[0];
    pid->feed = get;
    pid->cmd = set;
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = set - get;
   return this->pid_delta(tick, pid, coeff);
} 

double GimbalControl::pid_angle(uint32_t tick, JointData & joint, double get, double set)
{
    PidCoeff coeff = joint.coeff.pid[1];
    PidInfo * pid = &joint.pid_calc[1];
    pid->feed = get;
    pid->cmd = set;
    // pid->error[2] = pid->error[1];
    // pid->error[1] = pid->error[0];
    // pid->error[0] = set - get;
    // return this->pid_delta(tick, pid, coeff);
    pid->error[1] = pid->error[0];
    // pid->error[0] = RAD_FORMAT(set - get);
    pid->error[0] = FORMAT_ANGLE_DELTA(set - get, joint.coeff.angle_limit[1]);
    return this->pid_target(tick, pid, coeff);
} 
