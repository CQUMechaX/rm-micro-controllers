#include "tool/base_control.hpp"
#include "tool/transimition.hpp"
#include "app/device_monitor.hpp"
#include "override.h"

JointData::CtrlInfo MotorsControl::joint_target_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
    [__MOTORSCONTROL_ID_MAX_NUM + 1] = {0};
uint8_t MotorsControl::joint_cmd_group_online_cnt_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
    [__MOTORSCONTROL_ID_MAX_NUM + 1] = {0};

bool MotorsControl::joint_add(JointType joint_type, uint8_t id)
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
    this->joint_cmd_group_cnt_[id >> 2] ++;
    update_online(can_cast_to_num(this->hcan_), id, 1);
    this->joint_cnt_ ++;
    return true;
}

bool MotorsControl::joint_clear(void)
{
    return true;
}

bool MotorsControl::update_cmd_current(void)
{
    for(uint8_t i = 0; i != this->joint_cnt_; ++ i)
    {
        MotorsControl::set_target(*this->hcan_, this->joint_[i]);
    }
    return true;
}

bool MotorsControl::set_target(CAN_HandleTypeDef & hcan, JointData & joint)
{
    MotorsControl::joint_target_[can_cast_to_num(hcan)][joint.id] = joint.target;
    return true;
}

int16_t MotorsControl::get_target_current(uint8_t can_num, uint8_t id)
{
    return MotorsControl::joint_target_[can_num][id].current;
}

bool MotorsControl::update_can_current(void)
{
    for(uint8_t i = 0; i != __MONITOR_CAN_INTERFACE_MAX_NUM; ++ i)
    {
        for(uint8_t j = 0; j != __MOTORSCONTROL_TARGET_NUM; ++ j )
        {
            if(MotorsControl::joint_cmd_group_online_cnt_[i][j])
            {
                transimitionCanTx(&num_cast_to_can(i), gCanHeadTarget[(j << 2) | 1],
                    get_target_current(i, (j << 2) | 1),
                    get_target_current(i, (j << 2) | 2),
                    get_target_current(i, (j << 2) | 3),
                    get_target_current(i, (j << 2) | 4)
                    );
            }
        }
    }
    return true;
}

void MotorsControl::update_online(uint8_t can_num, uint8_t id, uint8_t val_change)
{
    MotorsControl::joint_cmd_group_online_cnt_[can_num][id >> 2] += val_change;
}

double MotorsControl::pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff)
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

double MotorsControl::pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set)
{
    pid->feed = get;
    pid->cmd = set;
    // memmove(pid->error[1] = pid->error[0];
    pid->error[0] = set - get;
    return this->pid_target(tick, pid, coeff);
}

double MotorsControl::pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff)
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

double MotorsControl::pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set)
{
    pid->feed = get;
    pid->cmd = set;
    // memmove(pid->error[1] = pid->error[0];
    pid->error[0] = set - get;
    return this->pid_target(tick, pid, coeff);
}