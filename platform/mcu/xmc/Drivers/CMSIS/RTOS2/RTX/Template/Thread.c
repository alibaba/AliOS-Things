#include "cmsis_os2.h"                                        // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
void Thread (void *argument);                                 // thread function
osThreadId_t tid_Thread;                                      // thread id
 
int Init_Thread (void) {
 
  tid_Thread = osThreadNew (Thread, NULL, NULL);
  if (!tid_Thread) return(-1);
  
  return(0);
}
 
void Thread (void *argument) {
 
  while (1) {
    ; // Insert thread code here...
    osThreadYield ();                                         // suspend thread
  }
}
