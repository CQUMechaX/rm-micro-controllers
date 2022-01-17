/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
#t.name = "defaultTask",
#t.stack_size = 3000 * 4,
#t.priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */
#n
void StartDefaultTask(void *argument);#n
#n
extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
#n

#n
/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
#t/* USER CODE BEGIN Init */
#t
#t/* USER CODE END Init */
#n
#t/* USER CODE BEGIN RTOS_MUTEX */
#t/* add mutexes, ... */
#t/* USER CODE END RTOS_MUTEX */
#n
#t/* USER CODE BEGIN RTOS_SEMAPHORES */
#t/* add semaphores, ... */
#t/* USER CODE END RTOS_SEMAPHORES */
#n
#t/* USER CODE BEGIN RTOS_TIMERS */
#t/* start timers, add new ones, ... */
#t/* USER CODE END RTOS_TIMERS */
#n
#t/* USER CODE BEGIN RTOS_QUEUES */
#t/* add queues, ... */
#t/* USER CODE END RTOS_QUEUES */
#n#t/* Create the thread(s) */
#t/* creation of defaultTask */
#tdefaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
#n
#n
#t/* USER CODE BEGIN RTOS_THREADS */
#t/* add threads, ... */
#t/* USER CODE END RTOS_THREADS */
#n
#n
#t/* USER CODE BEGIN RTOS_EVENTS */
#t/* add events, ... */
#t/* USER CODE END RTOS_EVENTS */
#n
}

#n#n
/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
#t/* init code for USB_DEVICE */
#tMX_USB_DEVICE_Init();#n#t/* USER CODE BEGIN StartDefaultTask */
#t/* Infinite loop */
#tfor(;;)
#t{
#t#tosDelay(1);
#t}
#t/* USER CODE END StartDefaultTask */
}
#n

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
