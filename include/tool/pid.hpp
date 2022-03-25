/** @file pid.hpp
 * @brief
 * @author Tiger3018
 * @date 2022-01-17
 * @li MIT License
 */
#pragma once
#ifndef __PID_HPP__
#define __PID_HPP__

// include
#include <cstdint>

#include "pid.h"

// definition
#ifndef __FPU_PRESENT
#define __FPU_PRESENT 1U  // defined in hal.h | In case
#endif                    /* __FPU_PRESENT */
#ifndef ARM_MATH_CM4
#define ARM_MATH_CM4  // defined in extra_script.py | For intlligence.
#endif                /* ARM_MATH_CM4 */
#include <arm_math.h>

// struct

// function

class MathTool
{
public:
  template <typename T>
  constexpr T constrain_abs_no_zero(T value, T limit)
  {
    return limit ? ((value < limit) ? ((value < -limit) ? -limit : value) : limit) : value;
  }

  template <typename T>
  constexpr T format_angle_delta(T delta, T limit)
  {
    return (fabs(delta) > (limit / 2)) ? ((delta > 0) ? (delta - limit) : (delta + limit)) : delta;
  }
};

class Pid : private MathTool
{
protected:
  inline double pid_delta(uint32_t tick, PidInfo & pid, const PidCoeff & coeff)
  {
    (void)tick;
    pid.p_out = coeff.kp * (pid.error[0] - pid.error[1]);
    pid.i_out = constrain_abs_no_zero(coeff.ki * pid.error[0], coeff.ki_limit);
    pid.d_out = coeff.kd * (pid.error[0] - 2 * pid.error[1] + pid.error[2]);
    pid.out = constrain_abs_no_zero(pid.out + pid.p_out + pid.i_out + pid.d_out, coeff.k_limit);
    return pid.out;
  }
  inline double pid_delta(
    uint32_t tick, PidInfo & pid, const PidCoeff & coeff, double get, double set)
  {
    pid.feed = get;
    pid.cmd = set;
    pid.error[0] = set - get;
    return pid_delta(tick, pid, coeff);
  }
  inline double pid_target(uint32_t tick, PidInfo & pid, const PidCoeff & coeff)
  {
    (void)tick;
    pid.p_out = coeff.kp * pid.error[0];
    pid.i_out = constrain_abs_no_zero(pid.i_out + coeff.ki * pid.error[0], coeff.ki_limit);
    pid.d_out = coeff.kd * (pid.error[0] - pid.error[1]);
    pid.out = constrain_abs_no_zero(pid.p_out + pid.i_out + pid.d_out, coeff.k_limit);
    return pid.out;
  }
  inline double pid_target(
    uint32_t tick, PidInfo & pid, const PidCoeff & coeff, double get, double set)
  {
    pid.feed = get;
    pid.cmd = set;
    pid.error[0] = set - get;
    return pid_target(tick, pid, coeff);
  }
  inline double pid_speed(
    uint32_t tick, PidInfo & pid, const PidCoeff & coeff, double get, double set)
  {
    pid.feed = get;
    pid.cmd = set;
    pid.error[2] = pid.error[1];
    pid.error[1] = pid.error[0];
    pid.error[0] = set - get;
    return pid_delta(tick, pid, coeff);
  }
  inline double pid_angle(
    uint32_t tick, PidInfo & pid, const PidCoeff & coeff, double get, double set,
    double angle_limit)
  {
    pid.feed = get;
    pid.cmd = set;
    pid.error[1] = pid.error[0];
    pid.error[0] = format_angle_delta(set - get, angle_limit);
    return pid_target(tick, pid, coeff);
  }
};

#endif /* __PID_HPP__ */