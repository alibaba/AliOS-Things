/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "aos_hal_dac.h"

int32_t aos_hal_dac_init(dac_dev_t *dac)
{
    return hal_dac_init(dac);
}

int32_t aos_hal_dac_start(dac_dev_t *dac, uint32_t channel)
{
    return hal_dac_start(dac, channel);
}

int32_t aos_hal_dac_stop(dac_dev_t *dac, uint32_t channel)
{
    return hal_dac_stop(dac, channel);
}

int32_t aos_hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data)
{
    return hal_dac_set_value(dac, channel, data);
}

int32_t aos_hal_dac_get_value(dac_dev_t *dac, uint32_t channel)
{
    return hal_dac_get_value(dac, channel);
}

int32_t aos_hal_dac_finalize(dac_dev_t *dac)
{
    return hal_dac_finalize(dac);
}