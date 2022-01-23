#pragma once
#include <stm32f4xx_hal.h>

typedef struct
{
    CAN_HandleTypeDef * hcan_chassis, * hcan_gimbal;
} PersistParam;
extern PersistParam gRosParam;