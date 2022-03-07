#pragma once
#ifndef __CHASSIS_CONTROL_HPP__
#define __CHASSIS_CONTROL_HPP__

// include
#include "tool/part_control.hpp"

// definition

// variable

// function and class
class ChassisControl : public PartControl
{
    uint8_t single_sub_;
public:
    enum ControllerMode
    {
        test
    } mode_;
    bool on_init(void);
    bool update(void);
    bool get_command(void);};

extern ChassisControl gChassis;

#endif /* __CHASSIS_CONTROL_HPP__ */