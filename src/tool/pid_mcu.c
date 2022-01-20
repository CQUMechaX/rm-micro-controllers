#include "dji/chassis_behaviour.h"
#include "dji/chassis_task.h"
#include "dji/detect_task.h"
#include "dji/pid.h"
#include "tool/pid_mcu.h"

chassis_move_t chassis_move;
void chassis_task_simple(void const *pvParameters)
{
    //wait a time 
    //空闲一段时间
    vTaskDelay(CHASSIS_TASK_INIT_TIME);
    //chassis init
    //底盘初始化
    chassis_init(&chassis_move);
    //make sure all chassis motor is online,
    //判断底盘电机是否都在线
    while (toe_is_error(CHASSIS_MOTOR1_TOE) || toe_is_error(CHASSIS_MOTOR2_TOE) || toe_is_error(CHASSIS_MOTOR3_TOE) || toe_is_error(CHASSIS_MOTOR4_TOE))// || toe_is_error(DBUS_TOE))
    {
        vTaskDelay(CHASSIS_CONTROL_TIME_MS);
    }

    // chassis_move.motor_chassis[0].speed_set = chassis_move.motor_chassis[1].speed_set = 3;
    // chassis_move.motor_chassis[2].speed_set = chassis_move.motor_chassis[3].speed_set = -3;
    while (1)
    {
        //set chassis control mode
        //设置底盘控制模式
        // chassis_set_mode(&chassis_move);
        //when mode changes, some data save
        //模式切换数据保存
        // chassis_mode_change_control_transit(&chassis_move);
        //chassis data update
        //底盘数据更新
        // chassis_feedback_update(&chassis_move);
        chassis_move.motor_chassis[0].speed = resultArray[0];
        chassis_move.motor_chassis[1].speed = resultArray[1];
        //set chassis control set-point 
        //底盘控制量设置
        // chassis_set_contorl(&chassis_move);
        //chassis control pid calculate
        //底盘控制PID计算
        // chassis_control_loop(&chassis_move);

        for (int i = 0; i < 2; i++)
        {
            PID_calc(&chassis_move.motor_speed_pid[i], chassis_move.motor_chassis[i].speed, chassis_move.motor_chassis[i].speed_set);
            chassis_move.motor_chassis[i].give_current = chassis_move.motor_speed_pid[i].out;
        }

        //make sure  one motor is online at least, so that the control CAN message can be received
        //确保至少一个电机在线， 这样CAN控制包可以被接收到
        if (!(toe_is_error(CHASSIS_MOTOR1_TOE) && toe_is_error(CHASSIS_MOTOR2_TOE)))// && toe_is_error(CHASSIS_MOTOR3_TOE) && toe_is_error(CHASSIS_MOTOR4_TOE)))
        {
            //when remote control is offline, chassis motor should receive zero current. 
            //当遥控器掉线的时候，发送给底盘电机零电流.
            // if (toe_is_error(DBUS_TOE))
            {
                // CAN_cmd_chassis(0, 0, 0, 0);
            }
            // else
            {
                //send control current
                //发送控制电流
                // CAN_cmd_chassis(chassis_move.motor_chassis[0].give_current, chassis_move.motor_chassis[1].give_current,
                //                 chassis_move.motor_chassis[2].give_current, chassis_move.motor_chassis[3].give_current);
                CAN_cmd_chassis(0x4000, 0x4000,
                                -0x4000, -0x4000);
            }
            #include "main.h"
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
        }
        else{
            
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
        }
        //os delay
        //系统延时
        vTaskDelay(CHASSIS_CONTROL_TIME_MS);

#if INCLUDE_uxTaskGetStackHighWaterMark
        chassis_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}