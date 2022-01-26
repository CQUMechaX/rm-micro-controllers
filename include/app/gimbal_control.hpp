#pragma once
#ifndef __GIMBAL_CONTROL_HPP__
#define __GIMBAL_CONTROL_HPP__

// include
#include "gimbal_control.h"
#include "tool/base_control.hpp"

// definition

// variable

// function and class
class GimbalControl : public BaseControl
{
public:
    enum ControllerMode
    {
        test, zero_speed
    } mode_;
    bool on_init(void);
    bool update(void);
    double pid_speed(uint32_t tick, JointData & joint, double get, double set);
    double pid_angle(uint32_t tick, JointData & joint, double get, double set);
};

extern GimbalControl gGimbal;

#endif /* __GIMBAL_CONTROL_HPP__ */