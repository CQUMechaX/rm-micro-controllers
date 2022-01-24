#include "tool/base_control.hpp"
#include "tool/transimition.hpp"

int16_t BaseControl::get_cmd_current(JointData * joint)
{
    return ( joint ? joint->target.current : 0 );
}


bool BaseControl::update_cmd_current(void)
{
    for(uint8_t i = 0; i != 4; ++ i)
    {
        if(this->joint_target_cnt_[i])
            transimitionCanTx(this->hcan_, gCanHeadTarget[(i << 2) | 1],
                this->get_cmd_current(joint_id_ptr_[(i << 2) | 1]),
                this->get_cmd_current(joint_id_ptr_[(i << 2) | 2]),
                this->get_cmd_current(joint_id_ptr_[(i << 2) | 3]),
                this->get_cmd_current(joint_id_ptr_[(i << 2) | 4])
                );
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
    pid->d_out = coeff.kd * (pid->error[0] - pid->error[1]);
    pid->out = CONSTRAIN_ABS(
        pid->p_out + pid->i_out + pid->d_out,
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