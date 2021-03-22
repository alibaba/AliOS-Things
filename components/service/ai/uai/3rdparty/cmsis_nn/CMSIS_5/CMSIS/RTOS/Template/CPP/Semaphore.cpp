#include "Semaphore.h"

#include <string.h>
//#include "error.h"

namespace rtos {

Semaphore::Semaphore(int32_t count) {
#ifdef CMSIS_OS_RTX
    memset(_semaphore_data, 0, sizeof(_semaphore_data));
    _osSemaphoreDef.semaphore = _semaphore_data;
#endif
    _osSemaphoreId = osSemaphoreCreate(&_osSemaphoreDef, count);
}

int32_t Semaphore::wait(uint32_t millisec) {
    return osSemaphoreWait(_osSemaphoreId, millisec);
}

osStatus Semaphore::release(void) {
    return osSemaphoreRelease(_osSemaphoreId);
}

}
