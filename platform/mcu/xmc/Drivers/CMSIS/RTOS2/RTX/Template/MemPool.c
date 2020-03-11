#include "cmsis_os2.h"                                        // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Memory Pool creation & usage
 *---------------------------------------------------------------------------*/
 
#define MEMPOOL_OBJECTS      16                               // number of Memory Pool Objects
 
typedef struct {                                              // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MEM_BLOCK_t;
 
void Thread_MemPool (void *argument);                         // thread function
osThreadId_t tid_Thread_MemPool;                              // thread id
 
osMemoryPoolId_t mpid_MemPool;                                // memory pool id
 
int Init_MemPool (void)
{
 
  mpid_MemPool = osMemoryPoolNew(MEMPOOL_OBJECTS,sizeof(MEM_BLOCK_t), NULL);
  if (mpid_MemPool == NULL) {
    ; // MemPool object not created, handle failure
  }
 
  tid_Thread_MemPool = osThreadNew (Thread_MemPool,NULL , NULL);
  if (tid_Thread_MemPool == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_MemPool (void *argument)
{
  osStatus_t     status;
  MEM_BLOCK_t *pMem = 0;
 
  while (1) {
    ; // Insert thread code here...
 
    pMem = (MEM_BLOCK_t *)osMemoryPoolAlloc (mpid_MemPool, NULL);    // get Mem Block
    if (pMem) {                                                      // Mem Block was available
      pMem->Buf[0] = 0x55;                                           // do some work...
      pMem->Idx    = 0;
 
      status = osMemoryPoolFree (mpid_MemPool, pMem);                // free mem block
      switch (status)  {
      case osOK:
        break;
      case osErrorParameter:
        break;
      case osErrorNoMemory:
        break;
      default:
        break;
      }
    }
 
    osThreadYield ();                                                // suspend thread
  }
}
