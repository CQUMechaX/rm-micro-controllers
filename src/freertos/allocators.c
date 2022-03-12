#include "freertos/allocators.h"
#include <cmsis_os.h>
#include <freertos_os2.h>

int absoluteUsedMemory = 0;
int usedMemory = 0;
void *pvPortCalloc(size_t num, size_t xWantedSize);
void *pvPortRealloc(void *pv, size_t xWantedSize);
size_t getBlockSize(void *pv);

void * __freertos_allocate(size_t size, void * state){
  (void) state;
  // printf("-- Alloc %d (prev: %d B)\n",size, xPortGetFreeHeapSize());
  absoluteUsedMemory += size;
  usedMemory += size;
  return pvPortMalloc(size);
}

void __freertos_deallocate(void * pointer, void * state){
  (void) state;
 if( NULL != pointer){
    // printf("-- Free %d (prev: %d B)\n",getBlockSize(pointer), xPortGetFreeHeapSize());
    usedMemory -= getBlockSize(pointer);
    vPortFree(pointer);
 }
}

void * __freertos_reallocate(void * pointer, size_t size, void * state){
  (void) state;
  // printf("-- Realloc %d -> %d (prev: %d B)\n",getBlockSize(pointer),size, xPortGetFreeHeapSize());
  if (NULL != pointer){
    absoluteUsedMemory += size;
    usedMemory += size;
    usedMemory -= getBlockSize(pointer);
    return pvPortRealloc(pointer,size);
  }
  else {
      absoluteUsedMemory += size;
      usedMemory += size;
      return pvPortMalloc(size);
  }
}

void * __freertos_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state){
  (void) state;
  // printf("-- Calloc %d x %d = %d -> (prev: %d B)\n",number_of_elements,size_of_element, number_of_elements*size_of_element, xPortGetFreeHeapSize());
  absoluteUsedMemory += number_of_elements*size_of_element;
  usedMemory += number_of_elements*size_of_element;
  return pvPortCalloc(number_of_elements,size_of_element);
}

/** In cmsis_os2.c
 * @n <by Tiger3018> To avoid RAM (not ccmram) overflow, use section .ccmram
 * to save stack space. @see <_FLASH.ld> startup codes needs to be modified, 
 * refer to AN4296. It (ccm) can be used to parallel code running, but DMA can't
 * access it.
 */
/*
  vApplicationGetIdleTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetIdleTaskMemory (StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Idle task control block and stack */
  static StaticTask_t Idle_TCB;
  static StackType_t  Idle_Stack[configMINIMAL_STACK_SIZE >> 2]  
#ifdef STM32_CCMRAM_EXIST
  __attribute__((section (".ccmram")))
#endif /* STM32_CCMRAM_EXIST */
  ;

  *ppxIdleTaskTCBBuffer   = &Idle_TCB;
  *ppxIdleTaskStackBuffer = &Idle_Stack[0];
  *pulIdleTaskStackSize   = (uint32_t)configMINIMAL_STACK_SIZE >> 2;
}

/*
  vApplicationGetTimerTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  /* Timer task control block and stack */
  static StaticTask_t Timer_TCB;
  static StackType_t  Timer_Stack[configTIMER_TASK_STACK_DEPTH >> 2]
#ifdef STM32_CCMRAM_EXIST
  __attribute__((section (".ccmram"))) 
#endif /* STM32_CCMRAM_EXIST */
  ;

  *ppxTimerTaskTCBBuffer   = &Timer_TCB;
  *ppxTimerTaskStackBuffer = &Timer_Stack[0];
  *pulTimerTaskStackSize   = (uint32_t)configTIMER_TASK_STACK_DEPTH >> 2;
}