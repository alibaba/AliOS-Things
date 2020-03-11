#include "cmsis_os2.h"                                        // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/
 
void Thread_MsgQueue1 (void *argument);                       // thread function 1
void Thread_MsgQueue2 (void *argument);                       // thread function 2
osThreadId_t tid_Thread_MsgQueue1;                            // thread id 1
osThreadId_t tid_Thread_MsgQueue2;                            // thread id 2
 
#define MSGQUEUE_OBJECTS      16                              // number of Message Queue Objects
 
typedef struct {                                              // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MEM_BLOCK_t;
 
typedef struct {                                              // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;
 
osMemoryPoolId_t mpid_MemPool2;                               // memory pool id
 
osMessageQueueId_t mid_MsgQueue;                              // message queue id
 
int Init_MsgQueue (void)
{
 
  mpid_MemPool2 = osMemoryPoolNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
  if (!mpid_MemPool2) {
    ; // MemPool object not created, handle failure
  }
 
  mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
  if (!mid_MsgQueue) {
    ; // Message Queue object not created, handle failure
  }
 
  tid_Thread_MsgQueue1 = osThreadNew (Thread_MsgQueue1, NULL, NULL);
  if (!tid_Thread_MsgQueue1) {
    return(-1);
  }
  tid_Thread_MsgQueue2 = osThreadNew (Thread_MsgQueue2, NULL, NULL);
  if (!tid_Thread_MsgQueue2) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_MsgQueue1 (void *argument)
{
  MEM_BLOCK_t *pMsg = 0;
 
  while (1) {
    ; // Insert thread code here...
    pMsg = (MEM_BLOCK_t *)osMemoryPoolAlloc (mpid_MemPool2, NULL);   // get Mem Block
    if (pMsg) {                                                      // Mem Block was available
      pMsg->Buf[0] = 0x55;                                           // do some work...
      pMsg->Idx    = 0;
      osMessageQueuePut (mid_MsgQueue, &pMsg, NULL, NULL);
    }

    osThreadYield ();                                                // suspend thread
  }
}
 
void Thread_MsgQueue2 (void *argument)
{
  osStatus_t      status;
  MEM_BLOCK_t *pMsg = 0;
 
  while (1) {
    ; // Insert thread code here...
    status = osMessageQueueGet (mid_MsgQueue, &pMsg, NULL, NULL);    // wait for message
    if (status == osOK) {
      if (pMsg) {
        ; // process data
        osMemoryPoolFree (mpid_MemPool2, pMsg);                      // free memory allocated for message
      }
    }
  }
}
