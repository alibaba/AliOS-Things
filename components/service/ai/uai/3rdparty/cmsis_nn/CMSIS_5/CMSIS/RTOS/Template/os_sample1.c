/* ----------------------------------------------------------------------  
 * Copyright (C) 2011 ARM Limited. All rights reserved.  
 *  
 * $Date:        30. November 2011
 * $Revision:    V0.02
 *  
 * Project: 	    CMSIS-RTOS API
 * Title:	    os_sample1.c
 *  
 * Description:	This file shows the usage of the CMSIS-RTOS API.
 *  
 * Version 0.02
 *    Initial Proposal Phase 
 * -------------------------------------------------------------------- */ 


#define osObjectsExternal
#include "my_objects.h"              // Reference CMSIS OS Objects


void thread_sample (void const *argument)  {
  osThreadId my_thread;
  osPriority my_priority;
  int i = 1000;

  my_thread = osThreadGetId();
  my_priority = osThreadGetPriority (my_thread);  // Get priority of own thread
  while (i > 0)  {
    osThreadSetPriority (my_thread, osPriorityAboveNormal);
    i--;
  }
  osThreadSetPriority (my_thread, my_priority);
  osThreadTerminate (my_thread);                  // terminate own thread
}
