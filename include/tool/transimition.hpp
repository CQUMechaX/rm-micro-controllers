#pragma once
#ifndef __TRANSIMITION_HPP__
#define __TRANSIMITION_HPP__

#include "transimition.h"
#include "tool/cache_buffer.hpp"
#include <vector>

extern CacheBuffer<1000> gAcmCacheArray[2];

//static bool jointUpdate(std::vector<DeviceStatus> & device_list, uint32_t std_id, uint8_t * rx_data);
//static bool dbusUpdate(uint8_t * rx_data);

#endif /* __TRANSIMITION_HPP__ */