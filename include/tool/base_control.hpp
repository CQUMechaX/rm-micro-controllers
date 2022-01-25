/** @file base_control.hpp
 * @dir tool
 * implement class BaseControl, with pid and filter.
 * implement class TaskControl, with time management.
 * @author Tiger3018
 * @date 2022-01-23
 */
#pragma once

#include "tool/pid_mcu.h"
#include "tool/joint_struct.h"
#include "marco.hpp"

#define __BASECONTROL_JOINT_NUM 6

class TaskControl
{
public:
    uint32_t frequency_ = 1000, tick_ms_ = 1;
    bool on_init(uint32_t frequency);
    virtual bool update(void) = 0;
    // explicit TaskControl(void) {}
    // ~TaskControl() = default;
};

class BaseControl : public TaskControl
{
protected:
    uint8_t joint_cnt_, /** a tot of joint_ */
        joint_target_cnt_[4] = {}; /** register to different cmd packet. @see gCanHeadTarget */
    JointData joint_[__BASECONTROL_JOINT_NUM];
    JointData * joint_id_ptr_[17] = {};
    bool joint_add(JointType joint_type, uint8_t id);
    bool joint_clear(void);
    int16_t get_cmd_current(JointData * joint);
public:
    CAN_HandleTypeDef * hcan_;

    bool update_cmd_current(void);

    double pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff);
    double pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set);

    double pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff);
    /** try
     * @todo error memmove
     */
    double pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set);

    constexpr double get_mean_speed(JointData j)
    {
        return (j.feedback[0].speed + j.feedback[1].speed + j.feedback[2].speed
            + j.feedback[3].speed + j.feedback[4].speed) / 5.0;
    }
    // BaseControl() = default;
    // ~BaseControl() = default;
};
