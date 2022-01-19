#pragma once
#ifndef __DEVICE_DETECT_H__
#define __DEVICE_DETECT_H__

// include
#include "device_detect.h"

// definition

// variable

// function and class
void deviceDetect(void * pvParameters);
void (* deviceDetectExternC)(void * pvParameters) = deviceDetect;


#endif /* __DEVICE_DETECT_HPP__ */