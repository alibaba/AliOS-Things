#include "Thread.h"

namespace rtos {

Thread::Thread(void (*task)(void const *argument),
               void *argument,
               osPriority priority,
               uint32_t stacksize) {
    // The actual fields of os_thread_def are implementation specific in every CMSIS-RTOS
#ifdef CMSIS_OS_RTX
    _thread_def.pthread = task;
    _thread_def.tpriority = priority;
    _thread_def.instances = 1;
    _thread_def.stacksize = stacksize;
#endif
    _tid = osThreadCreate(&_thread_def, argument);
}

osStatus Thread::terminate() {
    return osThreadTerminate(_tid);
}

osStatus Thread::set_priority(osPriority priority) {
    return osThreadSetPriority(_tid, priority);
}

osPriority Thread::get_priority() {
    return osThreadGetPriority(_tid);
}

int32_t Thread::signal_set(int32_t signals) {
    return osSignalSet(_tid, signals);
}

osEvent Thread::signal_wait(int32_t signals, uint32_t millisec) {
    return osSignalWait(signals, millisec);
}

osStatus Thread::wait(uint32_t millisec) {
    return osDelay(millisec);
}

osStatus Thread::yield() {
    return osThreadYield();
}

osThreadId Thread::gettid() {
    return osThreadGetId();
}

}
