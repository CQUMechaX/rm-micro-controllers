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
public:
    enum ControllerMode
    {
        test, zero_speed
    } mode_;
    bool on_init(void);
    bool update(void);
};

extern GimbalControl gGimbal;

#endif /* __GIMBAL_CONTROL_HPP__ */