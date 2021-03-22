#include "RtosTimer.h"

#include <string.h>

#include "cmsis_os.h"
//#include "error.h"

namespace rtos {

RtosTimer::RtosTimer(void (*periodic_task)(void const *argument), os_timer_type type, void *argument) {
#ifdef CMSIS_OS_RTX
    _timer.ptimer = periodic_task;
    
    memset(_timer_data, 0, sizeof(_timer_data));
    _timer.timer = _timer_data;
#endif
    _timer_id = osTimerCreate(&_timer, type, argument);
}

osStatus RtosTimer::start(uint32_t millisec) {
    return osTimerStart(_timer_id, millisec);
}

osStatus RtosTimer::stop(void) {
    return osTimerStop(_timer_id);
}

}
