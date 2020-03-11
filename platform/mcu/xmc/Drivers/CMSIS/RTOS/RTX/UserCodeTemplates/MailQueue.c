
#include "cmsis_os.h"                                           // CMSIS RTOS header file

/*----------------------------------------------------------------------------
 *      Mail Queue creation & usage
 *---------------------------------------------------------------------------*/
 
void Thread_MailQueue1 (void const *argument);                  // thread function 1
void Thread_MailQueue2 (void const *argument);                  // thread function 2
osThreadId tid_Thread_MailQueue1;                               // thread id 1
osThreadId tid_Thread_MailQueue2;                               // thread id 2
osThreadDef (Thread_MailQueue1, osPriorityNormal, 1, 0);        // thread object 1
osThreadDef (Thread_MailQueue2, osPriorityNormal, 1, 0);        // thread object 2

#define MAILQUEUE_OBJECTS      16                               // number of Message Queue Objects
typedef struct {                                                // object data type
  uint8_t Buf[32];
  uint8_t Idx;
} MAILQUEUE_OBJ_t;

osMailQId qid_MailQueue;                                        // mail queue id
osMailQDef (MailQueue, MAILQUEUE_OBJECTS, MAILQUEUE_OBJ_t);     // mail queue object


int Init_MailQueue (void) {

  qid_MailQueue = osMailCreate (osMailQ(MailQueue), NULL);      // create mail queue
  if (!qid_MailQueue) {
    ; // Mail Queue object not created, handle failure
  }
  
  tid_Thread_MailQueue1 = osThreadCreate (osThread(Thread_MailQueue1),  NULL);
  if (!tid_Thread_MailQueue1) return(-1);
  tid_Thread_MailQueue2 = osThreadCreate (osThread(Thread_MailQueue2),  NULL);
  if (!tid_Thread_MailQueue2) return(-1);
  
  return(0);
}

void Thread_MailQueue1 (void const *argument) {
  MAILQUEUE_OBJ_t *pMail = 0;

  while (1) {
    ; // Insert thread code here...
    pMail = osMailAlloc (qid_MailQueue, osWaitForever);         // Allocate memory
    if (pMail) {
      pMail->Buf[0] = 0xff;                                     // Set the mail content
      pMail->Idx = 0;
      osMailPut (qid_MailQueue, pMail);                         // Send Mail
    }

    osThreadYield ();                                           // suspend thread
  }
}

void Thread_MailQueue2 (void const *argument) {
  MAILQUEUE_OBJ_t  *pMail = 0;
  osEvent           evt;

  while (1) {
    ; // Insert thread code here...
    evt = osMailGet (qid_MailQueue, osWaitForever);             // wait for mail
    if (evt.status == osEventMail) {
      pMail = evt.value.p;
      if (pMail) {
        ; // process data
        osMailFree (qid_MailQueue, pMail);                      // free memory allocated for mail
      }
    }
  }
}
