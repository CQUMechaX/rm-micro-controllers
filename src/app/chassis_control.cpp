#include "app/chassis_control.hpp"

#include <cmsis_os.h>

#include "app/device_monitor.hpp"
#include "app/microros_param.h"
#include "marco.hpp"

ChassisControl gChassis;

bool ChassisControl::on_init(void)
{
  hcan_ = gRosParam.chassis.hcan;
  single_sub_ = this->joint_add(
    gRosParam.chassis.joint[0].type, gRosParam.chassis.joint[0].id,
    const_cast<const JointCoeff &>(gRosParam.chassis.joint[0].coeff));

  return true;
}

bool ChassisControl::update(void)
{
  JointData & joint_single = joint_[single_sub_];
  get_command();

  bool mode_changed = get_mode_change(this->mode_);
  if (mode_changed)
    ;

  if (1 /*mode_ == ControllerMode::*/) {
    joint_single.target.current =
      pid_speed(0, joint_single, get_mean_speed(this->joint_[0]), joint_single.target.speed);
  }

  update_target();
  return true;
}

bool ChassisControl::get_command(void)
{
  // Get mode change.
  (void)0;

  // Get control command.
  joint_[single_sub_].target.speed = command_default(0, 2700, 0, 0, 0, 0);

  return true;
}
