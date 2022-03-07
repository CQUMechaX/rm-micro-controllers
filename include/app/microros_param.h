#pragma once
#include <stm32f4xx_hal.h>
#include "tool/joint_struct.h"

typedef struct PersistParam
{
    struct Part
    {
        CAN_HandleTypeDef * hcan;
        uint8_t can_id;
        struct Joint
        {
            uint8_t id;
            JointType type;
            JointCoeff coeff;
        }joint[4];
    }chassis, gimbal, bullet;
    CAN_HandleTypeDef * hcan_chassis, * hcan_gimbal, * hcan_bullet;
} PersistParam;
extern PersistParam gRosParam;