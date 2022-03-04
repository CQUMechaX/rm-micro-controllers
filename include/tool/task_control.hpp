#pragma once

#include "marco.hpp"

class TaskControl
{
public:
    uint32_t frequency_ = 1000, tick_ms_ = 1;
    bool on_init(uint32_t frequency);
    virtual bool update(void) = 0;
    // explicit TaskControl(void) {}
    // ~TaskControl() = default;
};
