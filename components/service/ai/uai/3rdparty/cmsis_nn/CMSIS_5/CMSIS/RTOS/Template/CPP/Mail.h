/* Copyright (c) 2012 mbed.org */
#ifndef MAIL_H
#define MAIL_H 

#include <stdint.h>
#include <string.h>

#include "cmsis_os.h"

namespace rtos {

/*! The Mail class allow to control, send, receive, or wait for mail.
 A mail is a memory block that is send to a thread or interrupt service routine.
  \tparam  T         data type of a single message element.
  \tparam  queue_sz  maximum number of messages in queue.
*/
template<typename T, uint32_t queue_sz>
class Mail {
public:
    /*! Create and Initialise Mail queue. */
    Mail() {
    #ifdef CMSIS_OS_RTX
        memset(_mail_q, 0, sizeof(_mail_q));
        _mail_p[0] = _mail_q;
        
        memset(_mail_m, 0, sizeof(_mail_m));
        _mail_p[1] = _mail_m;
        
        _mail_def.pool = _mail_p;
        _mail_def.queue_sz = queue_sz;
        _mail_def.item_sz = sizeof(T);
    #endif
        _mail_id = osMailCreate(&_mail_def, NULL);
    }
    
    /*! Allocate a memory block of type T
      \param   millisec  timeout value or 0 in case of no time-out. (default: 0).
      \return  pointer to memory block that can be filled with mail or NULL in case error.
    */
    T* alloc(uint32_t millisec=0) {
        return (T*)osMailAlloc(_mail_id, millisec);
    }
    
    /*! Allocate a memory block of type T and set memory block to zero. 
      \param   millisec  timeout value or 0 in case of no time-out.  (default: 0).
      \return  pointer to memory block that can be filled with mail or NULL in case error.
    */
    T* calloc(uint32_t millisec=0) {
        return (T*)osMailCAlloc(_mail_id, millisec);
    }
    
    /*! Put a mail in the queue.
      \param   mptr  memory block previously allocated with Mail::alloc or Mail::calloc.
      \return  status code that indicates the execution status of the function. 
    */
    osStatus put(T *mptr) {
        return osMailPut(_mail_id, (void*)mptr);
    }
    
    /*! Get a mail from a queue.
      \param   millisec  timeout value or 0 in case of no time-out. (default: osWaitForever).
      \return  event that contains mail information or error code.
    */
    osEvent get(uint32_t millisec=osWaitForever) {
        return osMailGet(_mail_id, millisec);
    }
    
    /*! Free a memory block from a mail.
      \param   mptr  pointer to the memory block that was obtained with Mail::get. 
      \return  status code that indicates the execution status of the function.
    */
    osStatus free(T *mptr) {
        return osMailFree(_mail_id, (void*)mptr);
    }

private:
    osMailQId    _mail_id;
    osMailQDef_t _mail_def;
#ifdef CMSIS_OS_RTX
    uint32_t     _mail_q[4+(queue_sz)];
    uint32_t     _mail_m[3+((sizeof(T)+3)/4)*(queue_sz)];
    void        *_mail_p[2];
#endif
};

}

#endif

