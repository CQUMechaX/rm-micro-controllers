/** @file joint_struct.h
 * @dir tool
 * @brief a backport of ros2_controller standard in mcu.
 * @author Tiger3018
 * @date 2022-01-20
 */
#pragma once
#ifndef __JOINT_STRUCT_H__
#define __JOINT_STRUCT_H__

// include
#include <stdbool.h>
#include <stdint.h>

#include "tool/pid.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  double act2pos, act2vel, act2effort, pos2act, vel2act, effort2act, act2pos_offset, act2vel_offset,
    act2effort_offset;
  PidCoeff pid[2]; /** current control and speed control */
  int16_t angle_limit[2], speed_limit[2], current_limit[2];
} JointCoeff;

typedef struct
{
  // std::string name;
  // std::string type;
  // rclcpp::Time stamp;
  JointCoeff coeff;
  uint8_t can_num, id;
  uint32_t head_target, head_feedback;
  PidInfo pid_calc[2];
  struct CtrlInfo
  {
    int16_t angle, speed, current;
    int8_t temperature;
  } feedback[5], /** Feedback infomation includes 4 kinds of data.  */
    target;      /** You can only control the motor by curent DIRECTLY. */
  bool halted, need_calibration, calibrated, calibration_reading;
  bool locked;
  double cmd_degree, cmd_position;
  // uint16_t q_raw;
  // int16_t qd_raw;
  // int64_t q_circle;
  // uint16_t q_last;
  double offset;
  // For multiple cycle under absolute encoder (RoboMaster motor)
  // LowPassFilter* lp_filter;
} JointData;

typedef enum JointType { kRM3508, kRM2006, kRM6020 } JointType;

extern const JointCoeff gJointDefault[3]; /** @see @file microros_param.c */

#ifdef __cplusplus
}
#endif

#endif /* __JOINT_STRUCT_H__ */