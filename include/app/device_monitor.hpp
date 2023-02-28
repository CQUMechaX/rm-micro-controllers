#pragma once
#ifndef __DEVICE_MONITOR_HPP__
#define __DEVICE_MONITOR_HPP__

// include
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "device_monitor.h"
#include "marco.hpp"
#include "tool/task_control.hpp"

// definition

// variable

// function and class

enum DeviceErrorList {
  kNormal = 0,
  kOffline,
  kDataMismatch,
  kDataInterrupt,
};

struct DeviceStatus
{
  uint32_t cmd_error_duration;
  union DevicePtr {
    JointData * joint;
  } ptr;
  union DeviceData {
    JointData::CtrlInfo joint;
    DbusData dbus;
  } data;
  uint32_t online_time, offline_time, tick;
  bool enable, online;
  DeviceErrorList error;
};

class DeviceMonitor : public TaskControl
{
  std::unordered_map<void *, DeviceStatus *> device_dict_;
  bool device_dict_add(void * key, DeviceStatus * value);
  DeviceStatus & device_dict_find(void * key);

  bool update_to_controller(void);

public:
  /** @warning Since the MCU doesn't have MMU, we prefer to allocate space in advance. */
  explicit DeviceMonitor()
  {
    for (auto & iter : this->device_joint_) {
      iter.reserve(8);
    }
    device_dict_.reserve(1 << 10);
  }
  ~DeviceMonitor() = default;

  static const uint8_t interface_max_number_ = 2;
  std::vector<DeviceStatus> device_joint_[interface_max_number_];
  DeviceStatus device_dbus_{}, device_imu_{};

  bool update(void);
  /** static process of saving device feedback message's tick, with xTaskGetTickCountFromISR(). @param[in] device as a reference*/
  bool update_single_isr(DeviceStatus & device);
  bool update_single_isr(DeviceStatus & device, DeviceErrorList error);

  bool register_and_init(CAN_HandleTypeDef & hcan, JointData & joint);
  std::vector<DeviceStatus> & get_register_list(CAN_HandleTypeDef & hcan);
  bool get_online(JointData & joint);
  double get_command_dbus_channel(uint8_t channel, double channel_coeff);
  int16_t get_command_default(
    uint8_t channel, double channel_coeff, uint8_t mouse, double mouse_coeff, uint8_t serial,
    double serial_coeff);
  //template<>
  //int32_t get_command_noname(uint8_t channel, double coeff);

  constexpr uint8_t can_cast_to_num(CAN_HandleTypeDef & hcan)
  {
    return this->can_cast_to_num(&hcan);
  }
  constexpr uint8_t can_cast_to_num(CAN_HandleTypeDef * hcan)
  {
    return ((hcan == &HCAN1) ? 1 : ((hcan == &HCAN2) ? 2 : 0));
  }
  constexpr CAN_HandleTypeDef & num_cast_to_can(uint8_t num)
  {
    return ((num == 1) ? HCAN1 : ((num == 2) ? HCAN2 : HCAN1));
  }
};

extern DeviceMonitor gDeviceMonitor;

#endif /* __DEVICE_MONITOR_HPP__ */