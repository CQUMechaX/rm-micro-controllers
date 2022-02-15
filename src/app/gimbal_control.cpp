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
    this->joint_[0].coeff.pid->ki_limit = 200;
    this->joint_add(JointType::RM6020, 1);/** pitch H3600 - 4800 - 6000L */
    this->joint_add(JointType::RM6020, 2);/** yaw L6000 - 7500 - 809(9000)R */
    return true;
}

bool GimbalControl::update(void)
{
    static int16_t angle_list[3][2] = {{0, 0}, {4800, 1200}, {7500, 1500}};
    this->joint_[0].target.current = 
        this->pid_speed(0, this->joint_[0], this->get_mean_speed(this->joint_[0]), 
            2700 * [](uint8_t channel)->double{
                return 1.0 * gDeviceMonitor.device_dbus_.data.dbus.rc.channel[channel] / RC_CHANNEL_VALUE_ERROR;
            }(0));

    /** NO DATA or ATTI(1)*/
    if((gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 3)||
        (gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 0))
    {
        this->mode_ = test; //off
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
    for(uint8_t i = 1; i != 2/*this->joint_cnt_*/ ; ++ i)
    {
        if(!gDeviceMonitor.get_online(this->joint_[i]) || mode_changed)
        {
            this->joint_[i].pid_calc[0].i_out = this->joint_[i].pid_calc[0].out = 0;
            this->joint_[i].pid_calc[1].i_out = this->joint_[i].pid_calc[1].out = 0;
        }
        if(this->mode_ == ControllerMode::test)
        {
            this->joint_[i].target.angle = 
                FORMAT_ANGLE(angle_list[i][0] + angle_list[i][1] * [](uint8_t channel)->double{
                    return 1.0 * gDeviceMonitor.device_dbus_.data.dbus.rc.channel[channel] / RC_CHANNEL_VALUE_ERROR;
                }((i==1)?(3):(2)), this->joint_[i].coeff.angle_limit[0]);
            this->joint_[i].target.speed = 
                this->pid_angle(0, this->joint_[i], this->joint_[i].feedback[0].angle, this->joint_[i].target.angle);
            this->joint_[i].target.current = 
                this->pid_speed(0, this->joint_[i], this->get_mean_speed(this->joint_[i]), this->joint_[i].target.speed);
        }
        else
        {
            this->joint_[i].target.current = 
                this->pid_speed(0, this->joint_[i], this->get_mean_speed(this->joint_[i]), 0);
        }
    }
    // if(NORMAL_OR_ZERO(gLegacyResultArray[0]) && NORMAL_OR_ZERO(gLegacyResultArray[1])
    //     && NORMAL_OR_ZERO(gLegacyResultArray[2])
    //     && NORMAL_OR_ZERO(gLegacyResultArray[3])
    //     )
    // {
    //     this->joint_[0].coeff.pid[0].kp = gLegacyResultArray[0];
    //     this->joint_[0].coeff.pid[0].ki = gLegacyResultArray[2];
    //     this->joint_[0].coeff.pid[0].kd = gLegacyResultArray[3];
    //     _temp = gLegacyResultArray[1];
    // }
    // if(!((bool)gLegacyResultArray[0] | (bool)gLegacyResultArray[1] | (bool)gLegacyResultArray[2] |
    //     (bool)gLegacyResultArray[3]))
    //     {this->joint_[0].pid_calc->out = 0;}
    // this->pid_speed(0, &this->joint_[0], this->get_mean_speed(this->joint_[0]), _temp);
    // gGimbalTest = this->get_mean_speed(this->joint_[0]);
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
    pid->error[0] = RAD_FORMAT(set - get);
    return this->pid_target(tick, pid, coeff);
} 
