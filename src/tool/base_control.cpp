#include "tool/base_control.hpp"
#include "tool/transimition.hpp"
#include "app/device_monitor.hpp"
#include "override.h"

bool BaseControl::joint_add(JointType joint_type, uint8_t id)
{
    if(joint_type == JointType::RM6020)
    {
        id += 4; /** GM6020 id1 :=> JointData::id = 5 @see gCanHeadTarget */
    }
    if(this->joint_id_ptr_[id])
    {
        freertosErrorHandler(__FILE__, __LINE__);
    }
    this->joint_[this->joint_cnt_] = JointData{g_joint_default[joint_type], id};
    this->joint_id_ptr_[id] = &this->joint_[this->joint_cnt_];
    gDeviceMonitor.register_and_init(*this->hcan_, this->joint_[this->joint_cnt_]);
    this->joint_target_cnt_[id >> 2] ++;
    this->joint_cnt_ ++;
    return true;
}

bool BaseControl::joint_clear(void)
{
    return true;
}

int16_t BaseControl::get_cmd_current(uint8_t id)
{
    JointData * joint = this->joint_id_ptr_[id];
    return ( joint ? 
        (gDeviceMonitor.set_target(*this->hcan_, *joint), joint->target.current) :
        (gDeviceMonitor.get_target_current(*this->hcan_, id))
        );
}


bool BaseControl::update_cmd_current(void)
{
    for(uint8_t i = 0; i != 4; ++ i)
    {
        if(this->joint_target_cnt_[i])
        {
            transimitionCanTx(this->hcan_, gCanHeadTarget[(i << 2) | 1],
                this->get_cmd_current((i << 2) | 1),
                this->get_cmd_current((i << 2) | 2),
                this->get_cmd_current((i << 2) | 3),
                this->get_cmd_current((i << 2) | 4)
                );
        }
    }
    return true;
}

double BaseControl::pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff)
{
    pid->p_out = coeff.kp * (pid->error[0] - pid->error[1]);
    pid->i_out = CONSTRAIN_ABS(
        coeff.ki * pid->error[0],
        coeff.ki_limit
    );
    pid->d_out = coeff.kd * (pid->error[0] - 2 * pid->error[1] + pid->error[2]);
    pid->out = CONSTRAIN_ABS(
        pid->out + pid->p_out + pid->i_out + pid->d_out,
        coeff.k_limit
    );
    return pid->out;
}

double BaseControl::pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set)
{
    pid->feed = get;
    pid->cmd = set;
    // memmove(pid->error[1] = pid->error[0];
    pid->error[0] = set - get;
    return this->pid_target(tick, pid, coeff);
}

double BaseControl::pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff)
{
    pid->p_out = coeff.kp * pid->error[0];
    pid->i_out = CONSTRAIN_ABS(
        pid->i_out + coeff.ki * pid->error[0],
        coeff.ki_limit
    );
    pid->d_out = coeff.kd * (pid->error[0] - pid->error[1]);
    pid->out = CONSTRAIN_ABS(
        pid->p_out + pid->i_out + pid->d_out,
        coeff.k_limit
    );
    return pid->out;
}

double BaseControl::pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set)
{
    pid->feed = get;
    pid->cmd = set;
    // memmove(pid->error[1] = pid->error[0];
    pid->error[0] = set - get;
    return this->pid_target(tick, pid, coeff);
}