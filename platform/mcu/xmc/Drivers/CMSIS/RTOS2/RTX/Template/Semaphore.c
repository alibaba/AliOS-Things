#include "cmsis_os2.h"                                        // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Semaphore creation & usage
 *---------------------------------------------------------------------------*/
 
void Thread_Semaphore (void *argument);                       // thread function
osThreadId_t tid_Thread_Semaphore;                            // thread id
 
osSemaphoreId_t sid_Thread_Semaphore;                         // semaphore id
 
 
int Init_Semaphore (void)
{
 
  sid_Thread_Semaphore = osSemaphoreNew(2, 2, NULL);
  if (!sid_Thread_Semaphore) {
    ; // Semaphore object not created, handle failure
  }
 
  tid_Thread_Semaphore = osThreadNew (Thread_Semaphore, NULL, NULL);
  if (!tid_Thread_Semaphore) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_Semaphore (void *argument)
{
  int32_t val;
 
  while (1) {
    ; // Insert thread code here...
 
    val = osSemaphoreAcquire (sid_Thread_Semaphore, 10);      // wait 10 mSec
    switch (val) {
    case osOK:
      ; // Use protected code here...
      osSemaphoreRelease (sid_Thread_Semaphore);              // Return a token back to a semaphore
      break;
    case osErrorResource:
      break;
    case osErrorParameter:
      break;
    default:
      break;
    }
 
    osThreadYield ();                                         // suspend thread
  }
}
