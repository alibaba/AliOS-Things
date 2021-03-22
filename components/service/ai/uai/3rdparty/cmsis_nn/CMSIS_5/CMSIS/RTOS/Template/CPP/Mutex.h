/* Copyright (c) 2012 mbed.org */
#ifndef MUTEX_H
#define MUTEX_H 

#include <stdint.h>
#include "cmsis_os.h"

namespace rtos {

/*! The Mutex class is used to synchronise the execution of threads.
 This is for example used to protect access to a shared resource.
*/
class Mutex {
public:
    /*! Create and Initialize a Mutex object */
    Mutex();
    
    /*! Wait until a Mutex becomes available.
      \param   millisec  timeout value or 0 in case of no time-out. (default: osWaitForever)
      \return  status code that indicates the execution status of the function.
     */ 
    osStatus lock(uint32_t millisec=osWaitForever);
    
    /*! Try to lock the mutex, and return immediately
      \return  true if the mutex was acquired, false otherwise.
     */
    bool trylock();
    
    /*! Unlock the mutex that has previously been locked by the same thread
      \return  status code that indicates the execution status of the function. 
     */
    osStatus unlock();

private:
    osMutexId _osMutexId;
    osMutexDef_t _osMutexDef;
#ifdef CMSIS_OS_RTX
    int32_t _mutex_data[3];
#endif
};

}
#endif
