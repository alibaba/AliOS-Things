/* Copyright (c) 2012 mbed.org */
#ifndef THREAD_H
#define THREAD_H 

#include <stdint.h>
#include "cmsis_os.h"

#define DEFAULT_STACK_SIZE 0x1000

namespace rtos {

/*! The Thread class allow defining, creating, and controlling thread functions in the system. */
class Thread {
public:
    /*! Create a new thread, and start it executing the specified function.
      \param   task      function to be executed by this thread.
      \param   argument  pointer that is passed to the thread function as start argument. (default: NULL).
      \param   priority  initial priority of the thread function. (default: osPriorityNormal).
      \param   stacksz   stack size (in bytes) requirements for the thread function. (default: DEFAULT_STACK_SIZE).
    */
    Thread(void (*task)(void const *argument),
           void *argument=NULL,
           osPriority priority=osPriorityNormal,
           uint32_t stacksize=DEFAULT_STACK_SIZE);
    
    /*! Terminate execution of a thread and remove it from Active Threads
      \return  status code that indicates the execution status of the function.
    */
    osStatus terminate();
    
    /*! Set priority of an active thread
      \param   priority  new priority value for the thread function.
      \return  status code that indicates the execution status of the function.
    */
    osStatus set_priority(osPriority priority);
    
    /*! Get priority of an active thread
      \ return  current priority value of the thread function.
    */
    osPriority get_priority();
    
    /*! Set the specified Signal Flags of an active thread.
      \param   signals  specifies the signal flags of the thread that should be set.
      \return  previous signal flags of the specified thread or 0x80000000 in case of incorrect parameters.
    */
    int32_t signal_set(int32_t signals);
    
    /*! Wait for one or more Signal Flags to become signaled for the current RUNNING thread. 
      \param   signals   wait until all specified signal flags set or 0 for any single signal flag.
      \param   millisec  timeout value or 0 in case of no time-out. (default: osWaitForever).
      \return  event flag information or error code.
    */
    static osEvent signal_wait(int32_t signals, uint32_t millisec=osWaitForever);
    
    
    /*! Wait for a specified time period in millisec:
      \param   millisec  time delay value
      \return  status code that indicates the execution status of the function. 
    */
    static osStatus wait(uint32_t millisec);
    
    /*! Pass control to next thread that is in state READY.
      \return  status code that indicates the execution status of the function.
    */
    static osStatus yield();
    
    /*! Get the thread id of the current running thread.
      \return  thread ID for reference by other functions or NULL in case of error.
    */
    static osThreadId gettid();

private:
    osThreadId _tid;
    osThreadDef_t _thread_def;
};

}
#endif
