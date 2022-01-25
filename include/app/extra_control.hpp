#pragma once
#ifndef __EXTRA_CONTROL_HPP__
#define __EXTRA_CONTROL_HPP__

// include
#include "extra_control.h"
#include "tool/base_control.hpp"

// definition

// variable

// function and class
class ExtraControl : public BaseControl
{
public:
    bool on_init(void);
    bool update(void);
    double pid_speed(uint32_t tick, JointData * joint, double get, double set);
};

extern ExtraControl gBulletMotor;

#endif /* __EXTRA_CONTROL_HPP__ */