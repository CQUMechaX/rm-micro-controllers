#pragma once
#ifndef __GIMBAL_CONTROL_HPP__
#define __GIMBAL_CONTROL_HPP__

// include
#include "gimbal_control.h"
#include "tool/joint_struct.h"

// definition

// variable

// function and class
void gimbalControl(void * pvParameters);

class GimbalControl
{
    public:
    uint32_t frequency = 1000, tick_ms = 1;
    enum ControllerMode
    {
        test
    } mode;
    JointData joint[3];
    bool init(void);
    bool update(void);
    double pid_speed(uint8_t tick, JointData * joint);
    double pid_angle(uint8_t tick, JointData * joint, double get, double set, double error_delta);
};

extern GimbalControl g_gimbal;

#endif /* __GIMBAL_CONTROL_HPP__ */