
#include "cmsis_os.h"                                           // CMSIS RTOS header file

/*----------------------------------------------------------------------------
 *      Memory Pool creation & usage
 *---------------------------------------------------------------------------*/
 
#define MEMPOOL_OBJECTS      16                                 // number of Memory Pool Objects

typedef struct {                                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MEM_BLOCK_t;

void Thread_MemPool (void const *argument);                     // thread function
osThreadId tid_Thread_MemPool;                                  // thread id
osThreadDef (Thread_MemPool, osPriorityNormal, 1, 0);           // thread object

osPoolId mpid_MemPool;                                          // memory pool id
osPoolDef (MemPool, MEMPOOL_OBJECTS, MEM_BLOCK_t);              // memory pool object


int Init_MemPool (void) {

  mpid_MemPool = osPoolCreate (osPool (MemPool));               // create Mem Pool
  if (!mpid_MemPool) {
    ; // MemPool object not created, handle failure
  }
  
  tid_Thread_MemPool = osThreadCreate (osThread(Thread_MemPool), NULL);
  if (!tid_Thread_MemPool) return(-1);
  
  return(0);
}

void Thread_MemPool (void const *argument) {
  osStatus     status;
  MEM_BLOCK_t *pMem = 0;

  while (1) {
    ; // Insert thread code here...

    pMem = (MEM_BLOCK_t *)osPoolCAlloc (mpid_MemPool);          // get Mem Block
    if (pMem) {                                                 // Mem Block was available
      pMem->Buf[0] = 0x55;                                      // do some work...
      pMem->Idx    = 0;      
      
      status = osPoolFree (mpid_MemPool, pMem);                 // free mem block
      switch (status)  {
        case osOK:
          break;
        case osErrorParameter:
          break;
        case osErrorValue:
          break;
        default:
          break;
      }
    }

    osThreadYield ();                                           // suspend thread
  }
}
