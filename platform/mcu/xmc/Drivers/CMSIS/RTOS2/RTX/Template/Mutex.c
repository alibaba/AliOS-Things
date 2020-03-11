#include "cmsis_os2.h"                                        // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Mutex creation & usage
 *---------------------------------------------------------------------------*/
 
void Thread_Mutex (void *argument);                           // thread function
osThreadId_t tid_Thread_Mutex;                                // thread id
 
osMutexId_t mid_Thread_Mutex;                                 // mutex id
 
int Init_Mutex (void)
{
 
  mid_Thread_Mutex = osMutexNew (NULL);
  if (!tid_Thread_Mutex) {
    ; // Mutex object not created, handle failure
  }
 
  tid_Thread_Mutex = osThreadNew (Thread_Mutex, NULL, NULL);
  if (!tid_Thread_Mutex) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_Mutex (void *argument)
{
  osStatus_t status;
 
  while (1) {
    ; // Insert thread code here...
 
    status = osMutexAcquire (mid_Thread_Mutex, NULL);
    switch (status) {
    case osOK:
      ; // Use protected code here...
      osMutexRelease (mid_Thread_Mutex);
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
 
    osThreadYield ();                                         // suspend thread
  }
}
