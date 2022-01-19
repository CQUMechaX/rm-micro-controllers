#include "app/microros_param.h"
#include "can.h"
PersistParam g_ros_param = {
    // .hcan_chassis = &hcan1,
    .hcan_gimbal = &hcan1,
};