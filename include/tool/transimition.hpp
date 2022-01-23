#pragma once
#ifndef __TRANSIMITION_HPP__
#define __TRANSIMITION_HPP__

#include "transimition.h"
#include <vector>

template<typename T>
bool jointUpdate(T * baseControl, uint32_t std_id, uint8_t * rx_data);

#endif /* __TRANSIMITION_HPP__ */