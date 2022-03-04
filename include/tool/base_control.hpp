/** @file base_control.hpp
 * @dir tool
 * implement class MotorsControl, with pid and filter.
 * implement class TaskControl, with time management.
 * @author Tiger3018
 * @date 2022-01-23
 */
#pragma once

#include "tool/pid_mcu.h"
#include "tool/joint_struct.h"
#include "tool/task_control.hpp"
#include "app/device_monitor.hpp"
#include "marco.hpp"

#define __MOTORSCONTROL_TARGET_NUM 4
#define __MOTORSCONTROL_JOINT_NUM 6
#define __MOTORSCONTROL_ID_MAX_NUM 16

class MotorsControl : public TaskControl
{
protected:
    uint8_t joint_cnt_, /** count of joint_ */
        joint_cmd_group_cnt_[__MOTORSCONTROL_TARGET_NUM] = {}; /** register to different cmd packet. @see gCanHeadTarget */
    JointData joint_[__MOTORSCONTROL_JOINT_NUM];
    JointData * joint_id_ptr_[__MOTORSCONTROL_ID_MAX_NUM] = {};
    static JointData::CtrlInfo joint_target_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
        [__MOTORSCONTROL_ID_MAX_NUM + 1];
    static uint8_t joint_cmd_group_online_cnt_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
        [__MOTORSCONTROL_ID_MAX_NUM + 1]; /** Update in DeviceMonitor::update()*/

    bool joint_add(JointType joint_type, uint8_t id);
    bool joint_clear(void);
    //int16_t get_cmd_current(uint8_t id);
    bool update_cmd_current(void);
    static bool set_target(CAN_HandleTypeDef & hcan, JointData & joint);
    static int16_t get_target_current(uint8_t can_num, uint8_t id);

    template<typename T> bool get_mode_change(T now_mode)
    {
        static T mode;
        return ((mode != now_mode) ? (mode = now_mode, true) : false);
    }

    static constexpr uint8_t can_cast_to_num(CAN_HandleTypeDef & hcan)
    {
        return gDeviceMonitor.can_cast_to_num(hcan);
    }
    static constexpr uint8_t can_cast_to_num(CAN_HandleTypeDef * hcan)
    {
        return gDeviceMonitor.can_cast_to_num(hcan);
    }
    static constexpr CAN_HandleTypeDef & num_cast_to_can(uint8_t num)
    {
        return gDeviceMonitor.num_cast_to_can(num);
    }

public:
    CAN_HandleTypeDef * hcan_;


    double pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff);
    double pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set);

    double pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff);
    /** try
     * @todo error memmove
     */
    double pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set);
    static void update_online(uint8_t can_num, uint8_t id, uint8_t val_change);
    static bool update_can_current(void);

    constexpr double get_mean_speed(JointData j)
    {
        return (j.feedback[0].speed + j.feedback[1].speed + j.feedback[2].speed
            + j.feedback[3].speed + j.feedback[4].speed) / 5.0;
    }
    // MotorsControl() = default;
    // ~MotorsControl() = default;
};
