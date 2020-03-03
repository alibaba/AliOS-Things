/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <k_api.h>
#include "aos/hal/timer.h"
#include "board.h"
#include "timer_api.h"

gtimer_t local_timer;

int32_t hal_timer_init(timer_dev_t *tim)
{
    int32_t ret = -1;

    if(tim == NULL)
        return -1;

    memset(&local_timer, 0, sizeof(local_timer));

    switch(tim->port) {
        case PORT_TIMER_0:
        case PORT_TIMER_1:
            gtimer_init(&local_timer, tim->port);
            ret = 0;
            break;
        default:
            printf("ERROR: Please USE Tmer2 and Timer3\n\r");
            break;
    }
    return 0;
}

int32_t hal_timer_start(timer_dev_t *tim)
{
    if(tim == NULL)
        return -1;

    if(tim->config.reload_mode == TIMER_RELOAD_MANU){
        gtimer_start_one_shout(&local_timer, tim->config.period, tim->config.cb, tim->config.arg);
    }else if(tim->config.reload_mode == TIMER_RELOAD_AUTO){
        gtimer_start_periodical(&local_timer, tim->config.period, tim->config.cb, tim->config.arg);
    }else{
        printf("ERROR: Reload mode Set ERROR\n\r");
        return -1;
    }

    return 0;
}

void hal_timer_stop(timer_dev_t *tim)
{
    if (tim != NULL){
        gtimer_stop(&local_timer);
    }
}

int32_t hal_timer_finalize(timer_dev_t *tim)
{
    if(tim == NULL)
        return -1;

    gtimer_deinit(&local_timer);
    return 0;
}
