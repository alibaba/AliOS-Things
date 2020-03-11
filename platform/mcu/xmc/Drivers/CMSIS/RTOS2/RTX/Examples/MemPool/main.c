/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
 
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif

#include "stdio.h"

int main (void);
void app_main (void *argument);
void app_msg (void *argument);

typedef struct msg_s {
  uint8_t cmd;
  uint8_t len;
  uint8_t data[8];
} msg_t;

static osMessageQueueId_t msgQueue;
static osMemoryPoolId_t memPool;

static const osThreadAttr_t msgAttr = {
  .stack_size = 400U
};

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

void app_main (void *argument) {
  (void)argument;

  msg_t* msg;
  uint32_t cnt = 0UL;

  osStatus_t status;

  while(1) {
    ++cnt;

    msg = osMemoryPoolAlloc(memPool, osWaitForever);
    if (msg == NULL) {
      printf("app_msg: osMemoryPoolAlloc failed.\n");
      continue;
    }

    msg->cmd = 1U;
    msg->len = 4U;
    *((uint32_t*)(msg->data)) = cnt;
    status = osMessageQueuePut(msgQueue, &msg, 0U, osWaitForever);
    if (status != osOK) {
      printf("app_main: osMessageQueuePut failed.\n");
    }

    status = osDelay(osMessageQueueGetCount(msgQueue)*100U);
    if (status != osOK) {
      printf("app_main: osDelay failed.\n");
    }
  }
}

void app_msg (void *argument) {
  (void)argument;

  uint32_t cnt;
  msg_t* msg;
  osStatus_t status;

  while(1) {
    status = osDelay(osMessageQueueGetSpace(msgQueue)*100U);
    if (status != osOK) {
      printf("app_msg: osDelay failed.\n");
    }

    msg = NULL;
    status = osMessageQueueGet(msgQueue, &msg, NULL, osWaitForever);
    if ((status != osOK) || (msg == NULL)) {
      printf("app_msg: osMessageQueueGet failed.\n");
    } else {
      if (msg->len == 4U) {
        cnt = *((uint32_t*)(msg->data));
      }
      printf("app_msg: received [cmd = %d, data = 0x%0X]\n", msg->cmd, cnt);
      
      status = osMemoryPoolFree(memPool, msg);
      if (status != osOK) {
        printf("app_msg: osMemoryPoolFree failed.\n");
      }      
    }
  }
}


int main (void) {

  // System Initialization
  SystemCoreClockUpdate();

#ifdef RTE_Compiler_EventRecorder
  // Initialize and start Event Recorder
  EventRecorderInitialize(EventRecordError, 1U); 
  EventRecorderEnable    (EventRecordAll, 0xFE, 0xFE); 
#endif

  osKernelInitialize();                 // Initialize CMSIS-RTOS

  osThreadNew(app_main, NULL, NULL);     // Create application main thread
  osThreadNew(app_msg, NULL, &msgAttr);  // Create message receiver thread

  // Message queue used to pass pointers to msg_t
  msgQueue = osMessageQueueNew(10U, sizeof(msg_t*), NULL);
  
  // Memory pool for actual message objects
  memPool = osMemoryPoolNew(10U, sizeof(msg_t), NULL);

  osKernelStart();                      // Start thread execution

  for (;;) {}
}
