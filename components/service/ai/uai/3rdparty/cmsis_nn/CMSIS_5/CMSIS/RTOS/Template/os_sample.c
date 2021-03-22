/* ----------------------------------------------------------------------  
 * Copyright (C) 2011 ARM Limited. All rights reserved.  
 *  
 * $Date:        30. November 2011
 * $Revision:    V0.02
 *  
 * Project: 	 CMSIS-RTOS API
 * Title:	     os_sample.c
 *  
 * Description:	 This file shows the usage of the CMSIS-RTOS API.
 *  
 *  
 * Version 0.02
 *    Initial Proposal Phase 
 * -------------------------------------------------------------------- */ 


#include "my_objects.h"              // Define CMSIS OS Objects

// dummy functions since there is no OS today

/// Add a thread to ActiveThreads and set it to state READY
osThreadId  osThreadCreate (const osThreadDef_t *thread_def, void *argument)  { return osOK; }

/// Terminate execution of a thread and remove it from ActiveThreads
osStatus osThreadTerminate (osThreadId thread_id)   { return osOK; }

/// Change prority of an existing thread  
osStatus osThreadSetPriority (osThreadId thread_id, osPriority priority)   { return osOK; }

/// Get current prority of an existing thread  
osPriority osThreadGetPriority (osThreadId thread_id)   { return osPriorityNormal; }

osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id) { return NULL; }

osThreadId osThreadGetId (void)  {  return 0; }


osStatus status;
osThreadId thread_sample1;
osThreadId thread_sample2;

osMessageQDef(TcpMessageQ0, 10, a_element *);
osMessageQDef(TcpMessageQ1, 10, a_element *);
osMessageQDef(TcpMessageQ2, 10, a_element *);
osMessageQDef(TcpMessageQ3, 10, a_element *);

const osMessageQDef_t *TcpMessageQDef[4]
#if 1
 =  {
  osMessageQ(TcpMessageQ0),
  osMessageQ(TcpMessageQ1),
  osMessageQ(TcpMessageQ2),
  osMessageQ(TcpMessageQ3),
}
#endif
;

osMessageQId TcpMessageQ[4];

void CreateMessageQueues (void)  {
  uint32_t i;

  for (i = 0; i < 4; i++)  {
    TcpMessageQ[i] = osMessageCreate (TcpMessageQDef[i], NULL);
  }
}   


int main (void)  {
  thread_sample1 = osThreadCreate (osThread (thread_sample), NULL);
  thread_sample2 = osThreadCreate (osThread (thread_sample), NULL);
}

