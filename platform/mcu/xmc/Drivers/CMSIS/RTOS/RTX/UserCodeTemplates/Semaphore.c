
#include "cmsis_os.h"                                           // CMSIS RTOS header file

/*----------------------------------------------------------------------------
 *      Semaphore creation & usage
 *---------------------------------------------------------------------------*/
 
void Thread_Semaphore (void const *argument);                   // thread function
osThreadId tid_Thread_Semaphore;                                // thread id
osThreadDef (Thread_Semaphore, osPriorityNormal, 1, 0);         // thread object

osSemaphoreId sid_Thread_Semaphore;                             // semaphore id
osSemaphoreDef (SampleSemaphore);                               // semaphore object


int Init_Semaphore (void) {

  sid_Thread_Semaphore = osSemaphoreCreate (osSemaphore(SampleSemaphore), 1);
  if (!sid_Thread_Semaphore) {
    ; // Semaphore object not created, handle failure
  }
  
  tid_Thread_Semaphore = osThreadCreate (osThread(Thread_Semaphore), NULL);
  if (!tid_Thread_Semaphore) return(-1);
  
  return(0);
}

void Thread_Semaphore (void const *argument) {
  int32_t val;

  while (1) {
    ; // Insert thread code here...

    val = osSemaphoreWait (sid_Thread_Semaphore, 10);           // wait 10 mSec
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

    osThreadYield ();                                           // suspend thread
  }
}
