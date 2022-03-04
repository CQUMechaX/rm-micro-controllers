#pragma once
#ifndef __DEVICE_MONITOR_HPP__
#define __DEVICE_MONITOR_HPP__

// include
#include "device_monitor.h"
#include "tool/task_control.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include "marco.hpp"

// definition
#define __MONITOR_CAN_INTERFACE_MAX_NUM 2

// variable

// function and class

enum DeviceErrorList
{
    kNormal = 0,
    kOffline,
    kDataMismatch,
    kDataInterrupt,
};

struct DeviceStatus
{
    uint32_t cmd_error_duration;
    union DevicePtr
    {
        JointData * joint;
    } ptr;
    union DeviceData
    {
        JointData::CtrlInfo joint;
        DbusData dbus;
    } data;
    uint32_t online_time, offline_time, tick;
    bool enable, online;
    DeviceErrorList error;
};

class DeviceMonitor : public TaskControl
{

    std::unordered_map<void*, DeviceStatus*> device_dict_;
    bool device_dict_add(void * key, DeviceStatus * value);
    DeviceStatus & device_dict_find(void* key);

    bool update_to_controller(void);

    public:
    explicit DeviceMonitor() {
        for(auto & iter : this->device_joint_)
        {
            iter.reserve(8);
        }
        device_dict_.reserve(1<<10);
    }
    ~DeviceMonitor() = default;

    std::vector<DeviceStatus> device_joint_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1];
    DeviceStatus device_dbus_{}, device_imu_{};

    bool update(void);
    /** static process of saving device feedback message's tick, with xTaskGetTickCountFromISR(). @param[in] device as a reference*/
    bool update_single_isr(DeviceStatus & device);
    bool update_single_isr(DeviceStatus & device, DeviceErrorList error);

    bool register_and_init(CAN_HandleTypeDef & hcan, JointData & joint);
    std::vector<DeviceStatus> & get_register_list(CAN_HandleTypeDef & hcan);
    bool get_online(JointData & joint);

    constexpr uint8_t can_cast_to_num(CAN_HandleTypeDef & hcan)
    {
        return this->can_cast_to_num(&hcan);
    }
    constexpr uint8_t can_cast_to_num(CAN_HandleTypeDef * hcan)
    {
        return ( (hcan == &HCAN1) ? 1 : ( (hcan == &HCAN2) ? 2 : 0 ) );
    }
    constexpr CAN_HandleTypeDef & num_cast_to_can(uint8_t num)
    {
        return ( (num == 1) ? HCAN1 : ( (num == 2) ? HCAN2 : HCAN1 ) );
    }
    
};

extern DeviceMonitor gDeviceMonitor;


#endif /* __DEVICE_MONITOR_HPP__ */