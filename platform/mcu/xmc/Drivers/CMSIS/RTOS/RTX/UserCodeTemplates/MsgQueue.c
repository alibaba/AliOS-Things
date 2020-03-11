
#include "cmsis_os.h"                                           // CMSIS RTOS header file

/*----------------------------------------------------------------------------
 *      Message Queue creation & usage
 *---------------------------------------------------------------------------*/
 
void Thread_MsgQueue1 (void const *argument);                   // thread function 1
void Thread_MsgQueue2 (void const *argument);                   // thread function 2
osThreadId tid_Thread_MsgQueue1;                                // thread id 1
osThreadId tid_Thread_MsgQueue2;                                // thread id 2
osThreadDef (Thread_MsgQueue1, osPriorityNormal, 1, 0);         // thread object 1
osThreadDef (Thread_MsgQueue2, osPriorityNormal, 1, 0);         // thread object 2

#define MSGQUEUE_OBJECTS      16                                // number of Message Queue Objects

typedef struct {                                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MEM_BLOCK_t;

typedef struct {                                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MSGQUEUE_OBJ_t;

osPoolId mpid_MemPool2;                                         // memory pool id
osPoolDef (MemPool2, MSGQUEUE_OBJECTS, MEM_BLOCK_t);            // memory pool object
  
osMessageQId mid_MsgQueue;                                      // message queue id
osMessageQDef (MsgQueue, MSGQUEUE_OBJECTS, MSGQUEUE_OBJ_t);     // message queue object


int Init_MsgQueue (void) {

  mpid_MemPool2 = osPoolCreate (osPool (MemPool2));             // create Mem Pool
  if (!mpid_MemPool2) {
    ; // MemPool object not created, handle failure
  }
  
  mid_MsgQueue = osMessageCreate (osMessageQ(MsgQueue), NULL);  // create msg queue
  if (!mid_MsgQueue) {
    ; // Message Queue object not created, handle failure
  }
  
  tid_Thread_MsgQueue1 = osThreadCreate (osThread(Thread_MsgQueue1), NULL);
  if (!tid_Thread_MsgQueue1) return(-1);
  tid_Thread_MsgQueue2 = osThreadCreate (osThread(Thread_MsgQueue2), NULL);
  if (!tid_Thread_MsgQueue2) return(-1);
  
  return(0);
}

void Thread_MsgQueue1 (void const *argument) {
  MEM_BLOCK_t *pMsg = 0;

  while (1) {
    ; // Insert thread code here...
    pMsg = (MEM_BLOCK_t *)osPoolCAlloc (mpid_MemPool2);         // get Mem Block
    if (pMsg) {                                                 // Mem Block was available
      pMsg->Buf[0] = 0x55;                                      // do some work...
      pMsg->Idx    = 0;
      osMessagePut (mid_MsgQueue, (uint32_t)pMsg, osWaitForever); // Send Message
    }

    osThreadYield ();                                           // suspend thread
  }
}

void Thread_MsgQueue2 (void const *argument) {
  osEvent      evt;
  MEM_BLOCK_t *pMsg = 0;

  while (1) {
    ; // Insert thread code here...
    evt = osMessageGet (mid_MsgQueue, osWaitForever);           // wait for message
    if (evt.status == osEventMessage) {
      pMsg = evt.value.p;
      if (pMsg) {
        ; // process data
        osPoolFree (mpid_MemPool2, pMsg);                       // free memory allocated for message
      }
    }
  }
}
