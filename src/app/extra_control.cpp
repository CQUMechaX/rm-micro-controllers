#include <cmsis_os2.h>
#include "marco.hpp"
#include "app/extra_control.hpp"
#include "app/device_monitor.hpp"
#include "app/microros_param.h"

ExtraControl gBulletMotor;

void extraControl(void * pvParameters)
{
    gBulletMotor.on_init();
    while(true)
    {
        if(true);
        gBulletMotor.update();
        osDelay(gBulletMotor.tick_ms_);
    }
    return;
}

bool ExtraControl::on_init(void)
{
    this->hcan_ = gRosParam.hcan_bullet;
    this->joint_add(JointType::RM2006, 2);/** circle */
    this->joint_add(JointType::RM3508, 3);/** fabric1 */
    this->joint_add(JointType::RM3508, 4);/** fabric2 */
    this->joint_[0].coeff.pid[0].kp = 3;
    this->joint_[1].coeff.pid[0].kp = this->joint_[2].coeff.pid[0].kp = 3;
    return true;
}

bool ExtraControl::update(void)
{
    static double temp[4] = {-2500, -4000, 4000};
    for(uint8_t i = 0; i != this->joint_cnt_; ++ i)
    {
        gDeviceMonitor.get_online(this->joint_[i]);
        double hope = (
            (gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 1) ? temp[i] :
            ((gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 2) ? temp[i] : 0)
            );
        // if((gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 3)||
        //    (gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 0))
            // {this->joint_[i].pid_calc->out = 0;}
        this->pid_speed(0, &this->joint_[i], this->get_mean_speed(this->joint_[i]), hope);
    }
    // this->joint_[0].target.current *= 2;
    // this->joint_[1].target.current = -this->joint_[1].target.current / 10;
    // this->joint_[2].target.current = this->joint_[2].target.current / 10;
    this->update_cmd_current();
    // transimitionCanTx(&HCAN1, 0x200, 0, -10, -50, 50);
    return true;
}

double ExtraControl::pid_speed(uint32_t tick, JointData * joint, double get, double set)
{
    PidCoeff coeff = joint->coeff.pid[0];
    PidInfo * pid = &joint->pid_calc[0];
    pid->feed = get;
    pid->cmd = set;
    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->error[0] = set - get;
    return joint->target.current = static_cast<int16_t>(this->pid_delta(tick, pid, coeff));
} 