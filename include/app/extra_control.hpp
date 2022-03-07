#pragma once
#ifndef __EXTRA_CONTROL_HPP__
#define __EXTRA_CONTROL_HPP__

// include
#include "tool/part_control.hpp"

// definition

// variable

// function and class
class ExtraControl : public PartControl
{
public:
    enum ControllerMode
    {
        on, off
    }mode_;
    bool on_init(void);
    bool update(void);
};

extern ExtraControl gBulletMotor;

#endif /* __EXTRA_CONTROL_HPP__ */