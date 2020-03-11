
#include "cmsis_os.h"                                           // CMSIS RTOS header file

/*----------------------------------------------------------------------------
 *      Mutex creation & usage
 *---------------------------------------------------------------------------*/
 
void Thread_Mutex (void const *argument);                       // thread function
osThreadId tid_Thread_Mutex;                                    // thread id
osThreadDef (Thread_Mutex, osPriorityNormal, 1, 0);             // thread object

osMutexId mid_Thread_Mutex;                                     // mutex id
osMutexDef (SampleMutex);                                       // mutex name definition


int Init_Mutex (void) {

  mid_Thread_Mutex = osMutexCreate (osMutex (SampleMutex));
  if (!tid_Thread_Mutex) {
    ; // Mutex object not created, handle failure
  }
  
  tid_Thread_Mutex = osThreadCreate (osThread(Thread_Mutex), NULL);
  if (!tid_Thread_Mutex) return(-1);
  
  return(0);
}

void Thread_Mutex (void const *argument) {
  osStatus status;

  while (1) {
    ; // Insert thread code here...

    status = osMutexWait (mid_Thread_Mutex, NULL);
    switch (status) {
      case osOK:
        ; // Use protected code here...
        osMutexRelease (mid_Thread_Mutex);
        break;
      case osErrorTimeoutResource:
        break;
      case osErrorResource:
        break;
      case osErrorParameter:
        break;
      case osErrorISR:
        break;
      default:
        break;
    }

    osThreadYield ();                                           // suspend thread
  }
}
