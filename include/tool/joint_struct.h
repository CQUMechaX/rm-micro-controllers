/** @file joint_struct.h
 * @dir tool
 * @brief a backport of ros2_controller standard in mcu.
 * @author Tiger3018
 * @date 2022-01-20
 */
#pragma once
#ifndef __JOINT_STRUCT_H__
#define __JOINT_STRUCT_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "pid_mcu.h"

struct JointCoeff
{
    double act2pos, act2vel, act2effort, pos2act, vel2act, effort2act,
        act2pos_offset, act2vel_offset, act2effort_offset;
    PidCoeff pid[2];
    double angle_limit[2], speed_limit[2], current_limit[2];
};

struct JointData
{
    // std::string name;
    // std::string type;
    // rclcpp::Time stamp;
    JointCoeff coeff;
    uint32_t head_target, head_feedback;
    PidInfo pid_calc[2];
    struct CtrlInfo
    {
        int16_t angle, speed, current;
        int8_t temperature;
    } feedback[2], target;
    bool halted = false, need_calibration = false, calibrated = false, calibration_reading = false;
    bool locked = false;
    double cmd_degree, cmd_position;
    // uint16_t q_raw;
    // int16_t qd_raw;
    // int64_t q_circle;
    // uint16_t q_last;
    double offset;
    // For multiple cycle under absolute encoder (RoboMaster motor)
    // LowPassFilter* lp_filter;
};

enum JointType
{
    RM3508, RM2006, RM6020
};

const JointCoeff g_joint_default[3] = {
    {/** RM 3508 */
    .act2pos = 0.0007669903, .act2vel = 0.1047197551, .act2effort = 1.90702994e-5,
    .pos2act = 0, .vel2act = 0, .effort2act = 52437.561519,
    .act2pos_offset = 0.0, .act2vel_offset = 0.0, .act2effort_offset = 0.0,
    .pid = {
        {.kp = {}, .ki = {}, .kd = {}, .frequency = 1000, .ki_limit = {}, .k_limit = {}},
    },
    .angle_limit = {0, 8191}, .speed_limit = {}, .current_limit = {-16384, 16384}
    },{/** RM 2006 */
    .act2pos = 2.13078897e-5, .act2vel = 0.0029088820, .act2effort = 0.00018,
    .pos2act = 0, .vel2act = 0, .effort2act = 5555.55555555,
    .act2pos_offset = 0.0, .act2vel_offset = 0.0, .act2effort_offset = 0.0,
    .pid = {
        {.kp = {}, .ki = {}, .kd = {}, .frequency = 1000, .ki_limit = {}, .k_limit = {}},
    },
    .angle_limit = {0, 8191}, .speed_limit = {}, .current_limit = {-10000, 10000}
    },{/** RM 6020 */
    .act2pos = 0.0007670840, .act2vel = 0.1047197551, .act2effort = 5.880969e-5,
    .pos2act = 0, .vel2act = 0, .effort2act = 17004.0,
    .act2pos_offset = 0.0, .act2vel_offset = 0.0, .act2effort_offset = 0.0,
    .pid = {
        {.kp = 10000, .ki = 0, .kd = 0.2, .frequency = 1000, .ki_limit = {}, .k_limit = {}},
    },
    .angle_limit = {0, 8191}, .speed_limit = {}, .current_limit = {-30000, 30000}
    }
};

#ifdef __cplusplus
 }
#endif

#endif /* __JOINT_STRUCT_H__ */