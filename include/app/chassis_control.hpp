#pragma once
#ifndef __CHASSIS_CONTROL_HPP__
#define __CHASSIS_CONTROL_HPP__

// include
#include "chassis_control.h"
#include "tool/base_control.hpp"

// definition

// variable

// function and class
class ChassisControl : public BaseControl
{
public:
    enum ControllerMode
    {
        test
    } mode;
    bool on_init(void);
    bool update(void);
    double pid_speed(uint32_t tick, JointData * joint, double get, double set);
    double pid_angle(uint32_t tick, JointData * joint, double get, double set);
};

extern ChassisControl gChassis;

#endif /* __CHASSIS_CONTROL_HPP__ */