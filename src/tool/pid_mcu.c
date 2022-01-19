#include "dji/chassis_behaviour.h"
#include "dji/chassis_task.h"
#include "dji/detect_task.h"
#include "dji/pid.h"
#include "tool/pid_mcu.h"
#include "main.h"
#include <arm_math.h>
#include <freertos_os2.h>
#include <task.h>

#define ISNAN_OR_ZERO(val) (isnan(val) ? 0 : val)

chassis_move_t chassis_move;
void chassisTaskSimple(void * pvParameters)
{
    //wait a time 
    //空闲一段时间
    vTaskDelay(CHASSIS_TASK_INIT_TIME);
    //chassis init
    //底盘初始化
    chassis_init(&chassis_move);
    chassis_move.motor_speed_pid[0].mode = PID_DELTA;
    chassis_move.motor_speed_pid[1].mode = PID_DELTA;
    while (1)
    {
        //chassis data update
        //底盘数据更新
        chassis_feedback_update(&chassis_move);
        chassis_move.motor_chassis[0].speed_set = ISNAN_OR_ZERO(resultArray[0]);
        chassis_move.motor_chassis[1].speed_set = ISNAN_OR_ZERO(resultArray[1]);
        //chassis control pid calculate
        //底盘控制PID计算
        for (int i = 0; i < 2; i++)
        {
            PID_calc(&chassis_move.motor_speed_pid[i], chassis_move.motor_chassis[i].speed, chassis_move.motor_chassis[i].speed_set);
            chassis_move.motor_chassis[i].give_current = chassis_move.motor_speed_pid[i].out;
        }
        CAN_cmd_chassis(chassis_move.motor_chassis[0].give_current, chassis_move.motor_chassis[1].give_current,
                        chassis_move.motor_chassis[2].give_current, chassis_move.motor_chassis[3].give_current);
        //make sure  one motor is online at least, so that the control CAN message can be received
        //确保至少一个电机在线， 这样CAN控制包可以被接收到
        if (!(toe_is_error(CHASSIS_MOTOR1_TOE) && toe_is_error(CHASSIS_MOTOR2_TOE)))// && toe_is_error(CHASSIS_MOTOR3_TOE) && toe_is_error(CHASSIS_MOTOR4_TOE)))
        {
            // HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET); // on
        }
        else
        {
            // HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET); // off

        }
        //os delay
        //系统延时
        vTaskDelay(CHASSIS_CONTROL_TIME_MS);

#if INCLUDE_uxTaskGetStackHighWaterMark
        // chassis_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}