#include "app/extra_control.hpp"

#include <cmsis_os.h>

#include "app/device_monitor.hpp"
#include "app/microros_param.h"
#include "marco.hpp"

ExtraControl gBulletMotor;

bool ExtraControl::on_init(void)
{
  this->hcan_ = gRosParam.hcan_bullet;
  this->joint_add(JointType::kRM2006, 2); /** bullet circle */
  this->joint_add(JointType::kRM3508, 3); /** fabric1 */
  this->joint_add(JointType::kRM3508, 4); /** fabric2 */
  this->joint_[0].coeff.pid[0].kp = 3;
  this->joint_[1].coeff.pid[0].kp = this->joint_[2].coeff.pid[0].kp = 3;
  return true;
}

bool ExtraControl::update(void)
{
  static const double temp[4] = {-2500, -4000, 4000};
  if (
    (gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 3) ||
    (gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 0)) {
    this->mode_ = off;
  } else {
    this->mode_ = on;
  }
  bool mode_changed = this->get_mode_change(this->mode_);
  for (uint8_t i = 0; i != this->joint_cnt_; ++i) {
    gDeviceMonitor.get_online(this->joint_[i]);
    if (mode_changed) {
      this->joint_[i].pid_calc[0].i_out = this->joint_[i].pid_calc[0].out = 0;
      this->joint_[i].pid_calc[1].i_out = this->joint_[i].pid_calc[1].out = 0;
    }
    joint_[i].target.current = this->pid_speed(
      0, this->joint_[i], this->get_mean_speed(this->joint_[i]),
      (this->mode_ == ControllerMode::on) ? temp[i] : 0);
  }
  // this->joint_[0].target.current *= 2;
  // this->joint_[1].target.current = -this->joint_[1].target.current / 10;
  // this->joint_[2].target.current = this->joint_[2].target.current / 10;
  this->update_target();
  // transimitionCanTx(&HCAN1, 0x200, 0, -10, -50, 50);
  return true;
}