#pragma once
#ifndef __GIMBAL_CONTROL_HPP__
#define __GIMBAL_CONTROL_HPP__

// include
#include "gimbal_control.h"
#include "tool/joint_struct.h"
#include "tool/base_control.hpp"

// definition

// variable

// function and class
class GimbalControl : public BaseControl
{
public:
    enum ControllerMode
    {
        test
    } mode;
    JointData joint_[3];
    bool on_init(void);
    bool update(void);
    double pid_speed(uint32_t tick, JointData * joint, double get, double set);
    double pid_angle(uint32_t tick, JointData * joint, double get, double set);
};

extern GimbalControl gGimbal;

#endif /* __GIMBAL_CONTROL_HPP__ */