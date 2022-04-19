#include "tool/part_control.hpp"

#include "app/device_monitor.hpp"
#include "override.h"
#include "tool/transimition.hpp"

JointData::CtrlInfo PartControl::joint_target_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
                                              [__PARTCONTROL_ID_MAX_NUM + 1] = {0};
uint8_t PartControl::joint_cmd_group_online_cnt_[__MONITOR_CAN_INTERFACE_MAX_NUM + 1]
                                                [__PARTCONTROL_ID_MAX_NUM + 1] = {0};

uint8_t PartControl::joint_add(JointType joint_type, uint8_t id)
{
  return joint_add(joint_type, id, g_joint_default[joint_type]);
}

uint8_t PartControl::joint_add(JointType joint_type, uint8_t id, const JointCoeff & coeff)
{
  if (joint_type == JointType::RM6020) {
    id += 4; /** GM6020 id1 :=> JointData::id = 5 @see gCanHeadTarget */
  }

  if (this->joint_id_ptr_[id]) {
    freertosErrorHandler(__FILE__, __LINE__); /** Same id in one PartControl. Useless */
  } else {
    this->joint_id_ptr_[id] = &this->joint_[this->joint_cnt_];
  }

  this->joint_[this->joint_cnt_] =
    JointData{.coeff = coeff, .can_num = can_cast_to_num(this->hcan_), .id = id};
  gDeviceMonitor.register_and_init(*this->hcan_, this->joint_[this->joint_cnt_]);

  this->joint_cmd_group_cnt_[id >> 2]++;
  set_online_cnt(can_cast_to_num(this->hcan_), id, 1);

  return this->joint_cnt_++;
}

bool PartControl::joint_clear(void) { return true; }

bool PartControl::update_target(void)
{
  for (uint8_t i = 0; i != this->joint_cnt_; ++i) {
    PartControl::set_target(*this->hcan_, this->joint_[i]);
  }
  return true;
}

void PartControl::set_target(CAN_HandleTypeDef & hcan, JointData & joint)
{
  PartControl::joint_target_[can_cast_to_num(hcan)][joint.id] = joint.target;
}

int16_t PartControl::get_target_current(uint8_t can_num, uint8_t id)
{
  return PartControl::joint_target_[can_num][id].current;
}

bool PartControl::set_can_current(void)
{
  for (uint8_t i = 0; i != __MONITOR_CAN_INTERFACE_MAX_NUM; ++i) {
    for (uint8_t j = 0; j != __PARTCONTROL_TARGET_NUM; ++j) {
      if (PartControl::joint_cmd_group_online_cnt_[i][j]) {
        transimitionCanTx(
          &num_cast_to_can(i), gCanHeadTarget[(j << 2) | 1], get_target_current(i, (j << 2) | 1),
          get_target_current(i, (j << 2) | 2), get_target_current(i, (j << 2) | 3),
          get_target_current(i, (j << 2) | 4));
      }
    }
  }
  return true;
}

void PartControl::set_online_cnt(uint8_t can_num, uint8_t id, uint8_t cnt_change)
{
  PartControl::joint_cmd_group_online_cnt_[can_num][id >> 2] += cnt_change;
}

double PartControl::get_command_dbus_channel(uint8_t channel, double channel_coeff)
{
  return channel_coeff * gDeviceMonitor.device_dbus_.data.dbus.rc.channel[channel] /
         RC_CHANNEL_VALUE_ERROR;
}

int16_t PartControl::command_default(
  uint8_t channel, double channel_coeff, uint8_t mouse, double mouse_coeff, uint8_t serial,
  double serial_coeff)
{
  if (mouse) {
    return 0;
  } else if (serial) {
    return 0;
  } else {
    if (gDeviceMonitor.device_dbus_.online) {
      return get_command_dbus_channel(channel, channel_coeff);
    } else {
      return 0;
    }
  }
}