#include "app/microros_param.h"
#include "can.h"
PersistParam gRosParam = {
    // .hcan_chassis = &hcan1,
    .hcan_gimbal = &hcan1,
};