/** @file default_task.h/hpp
 * @dir app
 * @brief task and its class. It will run g
 * @author Tiger3018
 * @date 2022-03-14
 */
#pragma once
#ifndef __DEFAULT_TASK_H__
#define __DEFAULT_TASK_H__

// include
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// definition

// variable

// function and class
void defaultTask(void * arguments);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __DEFAULT_TASK_HPP__ */