//  ==== RTX Extensions ====

/// Suspend the RTX task scheduler.
/// \return number of ticks, for how long the system can sleep or power-down.
uint32_t os_suspend (void);

/// Resume the RTX task scheduler.
/// \param[in]     sleep_time    specifies how long the system was in sleep or power-down mode.
void os_resume (uint32_t sleep_time);

/// OS idle demon (running when no other thread is ready to run).
__NO_RETURN void os_idle_demon (void);

/// OS error callback (called when a runtime error is detected).
/// \param[in]     error_code    actual error code that has been detected.
__NO_RETURN void os_error (uint32_t error_code);
