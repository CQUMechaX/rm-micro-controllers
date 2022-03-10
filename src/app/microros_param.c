#include "app/microros_param.h"
#include "override.h"

const JointCoeff g_joint_default[3] = {
    {/** RM 3508 */
    .act2pos = 0.0007669903, .act2vel = 0.1047197551, .act2effort = 1.90702994e-5,
    .pos2act = 0, .vel2act = 0, .effort2act = 52437.561519,
    .act2pos_offset = 0.0, .act2vel_offset = 0.0, .act2effort_offset = 0.0,
    .pid = {
        {.kp = 3, .ki = 0.01, .kd = 0, .frequency = 1000, .ki_limit = 0, .k_limit = 0},
    },
    .angle_limit = {0, 8191}, .speed_limit = {0}, .current_limit = {-16384, 16384}
    },{/** RM 2006 */
    .act2pos = 2.13078897e-5, .act2vel = 0.0029088820, .act2effort = 0.00018,
    .pos2act = 0, .vel2act = 0, .effort2act = 5555.55555555,
    .act2pos_offset = 0.0, .act2vel_offset = 0.0, .act2effort_offset = 0.0,
    .pid = {
        {.kp = 3, .ki = 0.01, .kd = 0, .frequency = 1000, .ki_limit = 0, .k_limit = 0},
    },
    .angle_limit = {0, 8191}, .speed_limit = {0}, .current_limit = {-10000, 10000}
    },{/** RM 6020 */
    .act2pos = 0.0007670840, .act2vel = 0.1047197551, .act2effort = 5.880969e-5,
    .pos2act = 0, .vel2act = 0, .effort2act = 17004.0,
    .act2pos_offset = 0.0, .act2vel_offset = 0.0, .act2effort_offset = 0.0,
    .pid = {
        {.kp = 350, .ki = 1, .kd = 20, .frequency = 1000, .ki_limit = 0, .k_limit = 0},
        {.kp = 0.08, .ki = 0, .kd = 0, .frequency = 1000, .ki_limit = 0, .k_limit = 0},
    },
    .angle_limit = {0, 8191}, .speed_limit = {0}, .current_limit = {-30000, 30000}
    }
};

PersistParam gRosParam = {
    .hcan_chassis = &hcan1,
    .chassis = {
        .hcan = &HCAN1,
        .can_id = 1,
        .joint = {{
            .id = 1,
            .type = RM3508,
            .coeff = g_joint_default[RM3508], /** see https://stackoverflow.com/a/67000730, fixed 8.1+ */
            .coeff.pid[0] = {
                .kp = 3,
                .ki = 0.01,
                .kd = 0,
                .ki_limit = 200,
            },
        }}
    },
    .hcan_gimbal = &hcan1,
    .gimbal = {
        .hcan = &HCAN1,
        .can_id = 1,
        .joint = {{
            .id = 3, /** pitch id = 3 feedback_id = 7 */
            .type = RM6020,
            .coeff = g_joint_default[RM6020],
            .coeff.pid[0] = { .kp = 90, .ki = 8e-1, .kd = 0 },
            .coeff.pid[1] = { .kp = 0.1, .ki = 2.5e-5, .kd = 0 },
            .coeff.current_limit = {-14000, 14000},
        },{
            .id = 1, /** yaw id = 1 feedback_id = 5 */
            .type = RM6020,
            .coeff = g_joint_default[RM6020],
            .coeff.pid[0] = { .kp = 240, .ki = 5, .kd = 0 },
            .coeff.current_limit = {-14000, 14000},
        }}
    },
    .hcan_bullet = &hcan1,
};