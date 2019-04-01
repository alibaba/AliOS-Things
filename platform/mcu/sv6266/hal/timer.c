#include <stdlib.h>

#include "hal/soc/soc.h"
#include "drv/tmr/hal_tm.h"

void
hal_timer_init (
    hal_timer_t     *tmr,
    unsigned int    period,
    unsigned char   auto_reload,
    unsigned char   ch,
    hal_timer_cb_t  cb,
    void            *arg
    )
{
    tmr->ch     = ch;
    tmr->priv   = calloc (2 ,sizeof (uint32_t));
    tmr->cb     = cb;
    tmr->arg    = arg;

    if (tmr->priv != NULL) {
        ((uint32_t *)tmr->priv)[0] = auto_reload;
        ((uint32_t *)tmr->priv)[1] = period;
    }
}

int32_t
hal_timer_start (
    hal_timer_t     *tmr
    )
{
    int32_t retval;

    if (tmr->priv == NULL) {
        return -9;
    }

    retval = hal_tm_init ((TM_ID_E) tmr->ch);
    if (retval != 0) {
        return retval;
    }

    if (tmr->cb != NULL) {
        retval = hal_tm_register_irq_handler (
            (TM_ID_E) tmr->ch,
            (isr_func) tmr->cb
            );
    }

    return hal_tm_enable (
        (TM_ID_E) tmr->ch,
        ((uint32_t *)tmr->priv)[0],
        ((uint32_t *)tmr->priv)[1]
        );
}

void
hal_timer_stop (
    hal_timer_t     *tmr
    )
{
    int32_t retval;

    retval = hal_tm_disable ((TM_ID_E) tmr->ch);
}

int32_t
hal_timer_finalize (
    hal_timer_t     *timer
    )
{
    if (timer->priv != NULL) {
        free (timer->priv);
        timer->priv = NULL;
    }

    return hal_tm_deinit ((TM_ID_E) timer->ch);
}
