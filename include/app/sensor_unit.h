/** @file sensor_unit.h/hpp
 * @dir app
 * @brief task and its class.
 * @author Tiger3018
 * @date 2022-03-16
 */
#pragma once
#ifndef __SENSOR_UNIT_H__
#define __SENSOR_UNIT_H__

// include

#ifdef __cplusplus
extern "C" {
#endif

// definition

// variable

// function and class
void sensorExternInterrupt(void * arguments);
void sensorPeriodical(void * arguments);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_UNIT_HPP__ */