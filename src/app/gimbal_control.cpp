#include "app/gimbal_control.hpp"

#include <cmsis_os.h>

#include "app/device_monitor.hpp"
#include "app/microros_param.h"
#include "marco.hpp"

GimbalControl gGimbal;
double gGimbalTest;

bool GimbalControl::on_init(void)
{
  this->hcan_ = gRosParam.gimbal.hcan;
  pitch_sub_ = this->joint_add(
    gRosParam.gimbal.joint[0].type, gRosParam.gimbal.joint[0].id,
    const_cast<const JointCoeff &>(gRosParam.gimbal.joint[0].coeff));
  angle_calibration_[pitch_sub_][0] = 6000;
  angle_calibration_[pitch_sub_][1] = 1200;
  yaw_sub_ = this->joint_add(
    gRosParam.gimbal.joint[1].type, gRosParam.gimbal.joint[1].id,
    const_cast<const JointCoeff &>(gRosParam.gimbal.joint[1].coeff));
  angle_calibration_[yaw_sub_][0] = 4800;
  angle_calibration_[yaw_sub_][1] = 1500;
  /** pitch H3600 - 3500 - 6000L DELTA 1200*/
  /** yaw L3300 - 4800 - 6300R DELTA 1500*/
  return true;
}

bool GimbalControl::update(void)
{
  JointData &joint_pitch = joint_[pitch_sub_], &joint_yaw = joint_[yaw_sub_];
  get_command();

  bool mode_changed = get_mode_change(this->mode_);
  if (mode_changed) {
    if (ControllerMode::zero_speed) {
      angle_calibration_[1][2] = this->joint_[1].feedback[0].angle;
      angle_calibration_[2][2] = this->joint_[2].feedback[0].angle;
    }
  }

  /** OFF */
  if (gDeviceMonitor.device_dbus_.data.dbus.rc.button[1] == 1) {
    angle_calibration_[1][0] = this->joint_[1].feedback[0].angle;
    angle_calibration_[2][0] = this->joint_[2].feedback[0].angle;
  }

  for (uint8_t i = 0; i != this->joint_cnt_; ++i) {
    if (!gDeviceMonitor.get_online(this->joint_[i]) || mode_changed) {
      if (i == 1) {
        // this->joint_[1].target.speed = 0;
        // this->joint_[i].pid_calc[0].p_out = 0;
      }
      this->joint_[i].pid_calc[0].i_out = this->joint_[i].pid_calc[0].out = 0;
      this->joint_[i].pid_calc[1].i_out = this->joint_[i].pid_calc[1].out = 0;
    }
    if (this->mode_ == ControllerMode::test) {
    } else {
      // this->joint_[i].target.angle = angle_calibration_[i][2];
    }
    if (gDeviceMonitor.get_online(this->joint_[i])) {
      this->joint_[i].target.speed = this->pid_angle(
        0, this->joint_[i], this->joint_[i].feedback[0].angle, this->joint_[i].target.angle);
      this->joint_[i].target.current = this->pid_speed(
        0, this->joint_[i], this->get_mean_speed(this->joint_[i]), this->joint_[i].target.speed);
      this->joint_[i].target.current =
        CONSTRAIN_ARR(this->joint_[i].target.current, this->joint_[i].coeff.current_limit);
    } else {
      this->joint_[i].target.current = 0;
    }
  }
  // this->hcan_ = &hcan1;  //???
  this->update_target();
  return true;
}

bool GimbalControl::get_command(void)
{
  // Get mode change.
  /** NO DATA or ATTI(1)*/
  if (
    (gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 3) ||
    (gDeviceMonitor.device_dbus_.data.dbus.rc.button[0] == 0)) {
    this->mode_ = test;
  } else {
    this->mode_ = zero_speed;
  }

  // Get control command.
  joint_[pitch_sub_].target.angle =
    angle_calibration_[pitch_sub_][0] +
    command_default(3, angle_calibration_[pitch_sub_][1], 0, 0, 0, 0);
  joint_[yaw_sub_].target.angle = angle_calibration_[yaw_sub_][0] +
                                  command_default(2, angle_calibration_[yaw_sub_][1], 0, 0, 0, 0);

  return true;
}