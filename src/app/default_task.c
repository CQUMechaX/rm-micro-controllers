#include "app/default_task.h"

#include <cmsis_os.h>
#include <rcl/rcl.h>
#include <rmw_microxrcedds_c/config.h>
// #include <ucdr/microcdr.h>
// #include <uxr/client/client.h>
#include <rmw_microros/rmw_microros.h> 
#include <microros_transports.h> 

#include "override.h"
#include "freertos/allocators.h"
#include "app/pingpong.h"
#include "app/motor_control.h"
#include "app/echo_control.h"
#include "app/device_monitor.h"

extern void MX_USB_DEVICE_Init();

void defaultTask(void * argument)
{
  MX_USB_DEVICE_Init();
  
  bool availableNetwork = false;

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM 
  availableNetwork = true; 
  rmw_uros_set_custom_transport( 
    true, 
    (void *) NULL, 
    CDCUxrOpen,
    CDCUxrClose, 
    CDCUxrWrite, 
    CDCUxrRead); 
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

  if (!rcutils_set_default_allocator(&freeRTOS_allocator))
  {
    printf("Error on default allocators (line %d)\n", __LINE__);
  }

  osDelay(100);
  osThreadAttr_t attributes;
  memset(&attributes, 0x0, sizeof(osThreadAttr_t));
  attributes.stack_size = 5000;
  attributes.priority = (osPriority_t)osPriorityNormal;
  attributes.name = "microROS_app";
  osThreadNew(appMain, NULL, &attributes);
  //osDelay(500);
  attributes.name = "motor";
  osThreadNew(motorControl, NULL, &attributes);
  attributes.name = "echo";
  osThreadNew(echoControl, NULL, &attributes);
  attributes.name = "monitor";
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
  while (1) 
  {
    osDelay(200);
  }
  /* USER CODE END StartDefaultTask */
}