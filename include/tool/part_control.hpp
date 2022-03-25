/** @file part_control.hpp
 * @dir tool
 * implement class PartControl, with monitor read, command publish and pid filter.
 * @author Tiger3018
 * @date 2022-01-23
 */
#pragma once

#include "tool/joint_struct.h"
#include "tool/task_control.hpp"
#include "tool/pid.hpp"
#include "app/device_monitor.hpp"
#include "marco.hpp"

#define __PARTCONTROL_TARGET_NUM 4
#define __PARTCONTROL_JOINT_NUM 6
#define __PARTCONTROL_ID_MAX_NUM 16

class PartControl : public TaskControl, private Pid
{
protected:
    uint8_t joint_cnt_, /** count of joint_ */
        joint_cmd_group_cnt_[__PARTCONTROL_TARGET_NUM] = {}; /** register to different cmd packet. @see gCanHeadTarget */
    JointData joint_[__PARTCONTROL_JOINT_NUM];
    JointData * joint_id_ptr_[__PARTCONTROL_ID_MAX_NUM] = {};
    static JointData::CtrlInfo joint_target_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
        [__PARTCONTROL_ID_MAX_NUM + 1];
    static uint8_t joint_cmd_group_online_cnt_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
        [__PARTCONTROL_ID_MAX_NUM + 1]; /** Update in DeviceMonitor::update()*/
    
    uint8_t joint_add(JointType joint_type, uint8_t id);
    uint8_t joint_add(JointType joint_type, uint8_t id, const JointCoeff & coeff);
    bool joint_clear(void);

    inline double pid_speed(uint32_t tick, JointData & joint, double get, double set)
    {
        return Pid::pid_speed(tick, joint.pid_calc[0], joint.coeff.pid[0], get, set);
    }
    inline double pid_angle(uint32_t tick, JointData & joint, double get, double set)
    {
        return Pid::pid_angle(tick, joint.pid_calc[0], joint.coeff.pid[0], get, set, static_cast<double>(joint.coeff.angle_limit[1]));
    } 
    
    double get_command_dbus_channel(uint8_t channel, double channel_coeff);
    int16_t command_default(uint8_t channel, double channel_coeff, uint8_t mouse, double mouse_coeff, uint8_t serial, double serial_coeff);
    //template<>
    //int32_t command_noname(uint8_t channel, double coeff);
    
    bool update_target(void);
    static void set_target(CAN_HandleTypeDef & hcan, JointData & joint);
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

    static void set_online_cnt(uint8_t can_num, uint8_t id, uint8_t cnt_change);
    static bool set_can_current(void);

    constexpr double get_mean_speed(JointData j)
    {
        return (j.feedback[0].speed + j.feedback[1].speed + j.feedback[2].speed
            + j.feedback[3].speed + j.feedback[4].speed) / 5.0;
    }
    // PartControl() = default;
    // ~PartControl() = default;
};
