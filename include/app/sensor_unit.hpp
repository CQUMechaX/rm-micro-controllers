#pragma once
#ifndef __SENSOR_UNIT_HPP__
#define __SENSOR_UNIT_HPP__

// include
#include "sensor_unit.h"

// definition

// variable

// function and class
class Sensor
{
    virtual void init(void);
    virtual void update(void);
};

#endif /* __SENSOR_UNIT_HPP__ */