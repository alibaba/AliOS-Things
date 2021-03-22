/* Copyright (c) 2012 mbed.org */
#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H 

#include <stdint.h>
#include <string.h>

#include "cmsis_os.h"

namespace rtos {

/*! Define and manage fixed-size memory pools of objects of a given type.
  \tparam  T         data type of a single object (element).
  \tparam  queue_sz  maximum number of objects (elements) in the memory pool.
*/
template<typename T, uint32_t pool_sz>
class MemoryPool {
public:
    /*! Create and Initialize a memory pool. */
    MemoryPool() {
    #ifdef CMSIS_OS_RTX
        memset(_pool_m, 0, sizeof(_pool_m));
        _pool_def.pool = _pool_m;
        
        _pool_def.pool_sz = pool_sz;
        _pool_def.item_sz =  sizeof(T);
    #endif
        _pool_id = osPoolCreate(&_pool_def);
    }
    
    /*! Allocate a memory block of type T from a memory pool.
      \return  address of the allocated memory block or NULL in case of no memory available.
    */
    T* alloc(void) {
        return (T*)osPoolAlloc(_pool_id);
    }
    
    /*! Allocate a memory block of type T from a memory pool and set memory block to zero.
      \return  address of the allocated memory block or NULL in case of no memory available. 
    */
    T* calloc(void) {
        return (T*)osPoolCAlloc(_pool_id);
    }
    
    /*! Return an allocated memory block back to a specific memory pool.
      \param   address of the allocated memory block that is returned to the memory pool.
      \return  status code that indicates the execution status of the function. 
    */
    osStatus free(T *block) {
        return osPoolFree(_pool_id, (void*)block);
    }

private:
    osPoolId    _pool_id;
    osPoolDef_t _pool_def;
#ifdef CMSIS_OS_RTX
    uint32_t    _pool_m[3+((sizeof(T)+3)/4)*(pool_sz)];
#endif
};

}
#endif
