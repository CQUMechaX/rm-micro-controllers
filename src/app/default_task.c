#include "app/default_task.h"

#include <cmsis_os.h>
#include <rcl/rcl.h>
#include <rmw_microxrcedds_c/config.h>
// #include <ucdr/microcdr.h>
#include <microros_transports.h>
#include <rmw_microros/rmw_microros.h>
#include <uxr/client/client.h>

#include "app/device_monitor.h"
#include "app/echo_control.h"
#include "app/motor_control.h"
#include "app/pingpong.h"
#include "app/sensor_unit.h"
#include "freertos/allocators.h"
#include "main.h"
#include "override.h"

extern void MX_USB_DEVICE_Init();

static osThreadId_t micro_ros_handle, imu_cmps_handle, imu_handle_2;

void defaultTask(void * argument)
{
  MX_USB_DEVICE_Init();

  bool availableNetwork = false;
  (void)availableNetwork;

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM
  availableNetwork = true;
  rmw_uros_set_custom_transport(
    true, (void *)NULL, transportUxrCdcOpen, transportUxrCdcClose, transportUxrCdcWrite,
    transportUxrCdcRead);
#elif defined(RMW_UXRCE_TRANSPORT_UDP)
  printf("Ethernet Initialization\r\n");

  // Waiting for an IP
  printf("Waiting for IP\r\n");
  int retries = 0;
  while (gnetif.ip_addr.addr == 0 && retries < 10) {
    osDelay(500);
    retries++;
  };

  availableNetwork = (gnetif.ip_addr.addr != 0);
  if (availableNetwork) {
    printf("IP: %s\r\n", ip4addr_ntoa(&gnetif.ip_addr));
  } else {
    printf("Impossible to retrieve an IP\n");
  }
#endif

  // Launch app thread when IP configured
  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
  freeRTOS_allocator.allocate = __freertos_allocate;
  freeRTOS_allocator.deallocate = __freertos_deallocate;
  freeRTOS_allocator.reallocate = __freertos_reallocate;
  freeRTOS_allocator.zero_allocate = __freertos_zero_allocate;

  if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
    printf("Error on default allocators (line %d)\n", __LINE__);
  }

  osThreadAttr_t attributes;
  memset(&attributes, 0x0, sizeof(osThreadAttr_t));
  attributes.stack_size = 3000 * 4;
  attributes.priority = (osPriority_t)osPriorityNormal;
  attributes.name = "micro_ros_app";
  micro_ros_handle = osThreadNew(microRosAppMain, NULL, &attributes);
  attributes.stack_size = 500 * 4;
#ifdef RM_DEV_C
  //attributes.priority = (osPriority_t)osPriorityRealtime;
  attributes.name = "in_board_imu_1";
  imu_cmps_handle = osThreadNew(sensorPeriodical, NULL, &attributes);
  attributes.name = "in_board_imu_2";
  imu_handle_2 = osThreadNew(sensorExternInterrupt, NULL, &attributes);
#endif /* RM_DEV_C */
  attributes.priority = (osPriority_t)osPriorityNormal;
  attributes.name = "motor_control";
  osThreadNew(motorControl, NULL, &attributes);
  attributes.name = "echo_control";
  osThreadNew(echoControl, NULL, &attributes);
  attributes.name = "device_monitor";
  osThreadNew(deviceMonitor, NULL, &attributes);

  char ptrTaskList[500];
  vTaskList(ptrTaskList);
  printf("**********************************\n");
  printf("Task  State   Prio    Stack    Num\n");
  printf("**********************************\n");
  printf(ptrTaskList);
  printf("**********************************\n");

  //TaskHandle_t xHandle;
  //xHandle = xTaskGetHandle("microROS_app");

  //HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET); // on
  while (1) {
    osDelay(200);
  }
  vTaskDelete(NULL);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#ifdef RM_DEV_C
  if (GPIO_Pin == ACCEL_INT_Pin) {
    if (imu_handle_2 != NULL) {
      static BaseType_t xHigherPriorityTaskWoken;
      vTaskNotifyGiveFromISR(imu_handle_2, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  } else if (GPIO_Pin == GYRO_INT_Pin) {
  } else if (GPIO_Pin == CMPS_INT_Pin) {
    if (imu_cmps_handle != NULL) {
      static BaseType_t xHigherPriorityTaskWoken;
      vTaskNotifyGiveFromISR(imu_cmps_handle, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  }
#endif /* RM_DEV_C */
}