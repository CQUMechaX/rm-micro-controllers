#pragma once
#ifndef __DEVICE_MONITOR_HPP__
#define __DEVICE_MONITOR_HPP__

// include
#include "device_monitor.h"
#include "tool/base_control.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>

// definition

// variable

// function and class

enum ErrorList
{
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
    bool enable, online, error;
};

class DeviceMonitor : public TaskControl
{
    std::vector<DeviceStatus> device_joint_[3];

    std::unordered_map<void*, DeviceStatus*> device_dict_;
    bool device_dict_add(void * key, DeviceStatus * value);
    DeviceStatus & device_dict_find(void* key);

    uint8_t can_cast_to_joint(CAN_HandleTypeDef & hcan);
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
    DeviceStatus device_dbus_{}, device_imu_{};
    bool update(void);
    bool update_single_isr(DeviceStatus & device);
    bool register_and_init(CAN_HandleTypeDef & hcan, JointData & joint);
    std::vector<DeviceStatus> & get_register_list(CAN_HandleTypeDef & hcan);
    bool get_online(JointData & joint);
    
};

extern DeviceMonitor gDeviceMonitor;


#endif /* __DEVICE_MONITOR_HPP__ */