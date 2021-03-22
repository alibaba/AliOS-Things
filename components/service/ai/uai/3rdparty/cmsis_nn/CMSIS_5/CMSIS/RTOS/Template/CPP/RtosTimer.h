/* Copyright (c) 2012 mbed.org */
#ifndef TIMER_H
#define TIMER_H 

#include <stdint.h>
#include "cmsis_os.h"

namespace rtos {

/*! The RtosTimer class allow creating and and controlling of timer functions in the system.
 A timer function is called when a time period expires whereby both on-shot and
 periodic timers are possible. A timer can be started, restarted, or stopped.

 Timers are handled in the thread osTimerThread.
 Callback functions run under control of this thread and may use CMSIS-RTOS API calls. 
*/
class RtosTimer {
public:
    /*! Create and Start timer.
      \param   task      name of the timer call back function.
      \param   type      osTimerOnce for one-shot or osTimerPeriodic for periodic behaviour. (default: osTimerPeriodic)
      \param   argument  argument to the timer call back function. (default: NULL)
    */
    RtosTimer(void (*task)(void const *argument),
          os_timer_type type=osTimerPeriodic,
          void *argument=NULL);
    
    /*! Stop the timer.
      \return  status code that indicates the execution status of the function. 
    */
    osStatus stop(void);
    
    /*! start a timer.
      \param   millisec  time delay value of the timer.
      \return  status code that indicates the execution status of the function. 
    */
    osStatus start(uint32_t millisec);

private:
    osTimerId _timer_id;
    osTimerDef_t _timer;
#ifdef CMSIS_OS_RTX
    uint32_t _timer_data[5];
#endif
};

}

#endif
