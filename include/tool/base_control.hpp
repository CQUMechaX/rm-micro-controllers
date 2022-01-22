#pragma once

#include "tool/pid_mcu.h"
#include "marco.hpp"

class BaseControl
{
public:
    uint32_t frequency = 1000, tick_ms = 1;

    double pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff);
    double pid_delta(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set);

    double pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff);
    /** try
     * @todo error memmove
     */
    double pid_target(uint32_t tick, PidInfo * pid, PidCoeff coeff, double get, double set);
public:
    BaseControl(/* args */) = default;
    ~BaseControl() = default;
};
