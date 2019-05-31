#ifndef _LEGA_TIMER_H_
#define _LEGA_TIMER_H_
#include <stdint.h>

#define LEGA_TIMER1_INDEX 0
#define LEGA_TIMER2_INDEX 1
#define LEGA_TIMER_NUM 2

#define TIMER_RELOAD_AUTO  1  /* timer reload automatic */
#define TIMER_RELOAD_MANU  2  /* timer reload manual */

typedef void (*lega_timer_cb_handler_t)(void *arg);
typedef struct {
    lega_timer_cb_handler_t cb;
    void *arg;
} lega_timer_cb_t;

typedef struct {
    uint32_t       period;   /*us*/
    uint8_t        reload_mode;
    lega_timer_cb_handler_t cb;
    void          *arg;
} lega_timer_config_t;

typedef struct {
    int8_t         port;   /* timer port */
    lega_timer_config_t config; /* timer config */
    void          *priv;   /* priv data */
} lega_timer_dev_t;

/**
 * init a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_timer_init(lega_timer_dev_t *tim);

/**
 * start a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_timer_start(lega_timer_dev_t *tim);

/**
 * stop a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  none
 */
void lega_timer_stop(lega_timer_dev_t *tim);

/**
 * De-initialises an TIMER interface, Turns off an TIMER hardware interface
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_timer_finalize(lega_timer_dev_t *tim);

#endif //_LEGA_TIMER_H_