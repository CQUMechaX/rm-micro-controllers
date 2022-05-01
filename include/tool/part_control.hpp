/** @file part_control.hpp
 * @dir tool
 * implement class PartControl, with monitor read, command publish and pid filter.
 * @author Tiger3018
 * @date 2022-01-23
 */
#pragma once

#include "app/device_monitor.hpp"
#include "marco.hpp"
#include "tool/joint_struct.h"
#include "tool/pid.hpp"
#include "tool/task_control.hpp"

#define __PARTCONTROL_TARGET_NUM 4
#define __PARTCONTROL_JOINT_NUM 6
#define __PARTCONTROL_ID_MAX_NUM 16

class PartControl : public TaskControl, private Pid
{
private:
  /** @brief Do you know what's the static member of a class? They can only accessed by class name. */
  static JointData::CtrlInfo joint_target_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
                                          [__PARTCONTROL_ID_MAX_NUM + 1];
  static uint8_t joint_cmd_group_online_cnt_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
                                            [__PARTCONTROL_ID_MAX_NUM +
                                             1]; /** Update in DeviceMonitor::update()*/
protected:
  enum Calibration { origin_value, offset_max_value };
  uint8_t joint_cnt_; /** count of joint_ */
  uint8_t joint_cmd_group_cnt_[__PARTCONTROL_TARGET_NUM] =
    {}; /** register to different cmd packet. @see gCanHeadTarget */
  JointData joint_[__PARTCONTROL_JOINT_NUM];
  JointData * joint_id_ptr_[__PARTCONTROL_ID_MAX_NUM] = {};

  uint8_t joint_add(JointType joint_type, uint8_t id);
  uint8_t joint_add(JointType joint_type, uint8_t id, const JointCoeff & coeff);
  bool joint_configure(void);
  bool joint_delete(void);

  inline double pid_speed(uint32_t tick, JointData & joint, double get, double set)
  {
    return Pid::pid_speed(tick, joint.pid_calc[0], joint.coeff.pid[0], get, set);
  }
  inline double pid_angle(uint32_t tick, JointData & joint, double get, double set)
  {
    return Pid::pid_angle(
      tick, joint.pid_calc[1], joint.coeff.pid[1], get, set,
      static_cast<double>(joint.coeff.angle_limit[1]));
  }

  bool update_target(void);
  static void set_target(CAN_HandleTypeDef & hcan, JointData & joint);
  static int16_t get_target_current(uint8_t can_num, uint8_t id);

  template <typename T>
  bool get_mode_change(T now_mode)
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
    return (j.feedback[0].speed + j.feedback[1].speed + j.feedback[2].speed + j.feedback[3].speed +
            j.feedback[4].speed) /
           5.0;
  }
  // PartControl() = default;
  // ~PartControl() = default;
};
