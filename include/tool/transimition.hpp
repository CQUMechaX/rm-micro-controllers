#pragma once
#ifndef __TRANSIMITION_HPP__
#define __TRANSIMITION_HPP__

#include "transimition.h"
#include "app/device_monitor.hpp"
#include <vector>

bool jointUpdate(std::vector<DeviceStatus> & device_list, uint32_t std_id, uint8_t * rx_data);

#endif /* __TRANSIMITION_HPP__ */