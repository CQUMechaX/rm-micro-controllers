#pragma once
#ifndef __GIMBAL_CONTROL_HPP__
#define __GIMBAL_CONTROL_HPP__

// include
#include "tool/part_control.hpp"

// definition

// variable

// function and class
class GimbalControl : public PartControl
{
  uint8_t pitch_sub_, yaw_sub_;
  int16_t angle_calibration_[3][3];

public:
  enum ControllerMode { test, zero_speed } mode_;
  bool on_init(void);
  bool update(void);
  bool get_command(void);
};

extern GimbalControl gGimbal;

#endif /* __GIMBAL_CONTROL_HPP__ */