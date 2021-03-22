/* Copyright (c) 2012 mbed.org */
#ifndef SEMAPHORE_H
#define SEMAPHORE_H 

#include <stdint.h>
#include "cmsis_os.h"

namespace rtos {

/*! The Semaphore class is used to manage and protect access to a set of shared resources. */
class Semaphore {
public:
    /*! Create and Initialize a Semaphore object used for managing resources. 
      \param number of available resources; maximum index value is (count-1).
    */
    Semaphore(int32_t count);
    
    /*! Wait until a Semaphore resource becomes available. 
      \param   millisec  timeout value or 0 in case of no time-out. (default: osWaitForever).
      \return  number of available tokens, or -1 in case of incorrect parameters
    */
    int32_t wait(uint32_t millisec=osWaitForever);
    
    /*! Release a Semaphore resource that was obtain with Semaphore::wait.
      \return  status code that indicates the execution status of the function. 
    */
    osStatus release(void);

private:
    osSemaphoreId _osSemaphoreId;
    osSemaphoreDef_t _osSemaphoreDef;
#ifdef CMSIS_OS_RTX
    uint32_t _semaphore_data[2];
#endif
};

}
#endif
