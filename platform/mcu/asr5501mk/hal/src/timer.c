#include <stdio.h>
#include <stdint.h>
#include "lega_cm4.h"
#include "lega_timer.h"
#include "timer.h"

/**
 * init a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_init(timer_dev_t *tim)
{
    return lega_timer_init((lega_timer_dev_t *)tim);
}

/**
 * start a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_start(timer_dev_t *tim)
{
    return lega_timer_start((lega_timer_dev_t *)tim);
}

/**
 * stop a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  none
 */
void hal_timer_stop(timer_dev_t *tim)
{
    return lega_timer_stop((lega_timer_dev_t *)tim);
}

/**
 * De-initialises an TIMER interface, Turns off an TIMER hardware interface
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_finalize(timer_dev_t *tim)
{
    return lega_timer_finalize((lega_timer_dev_t *)tim);
}
