/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "amp_hal_can.h"

int32_t amp_hal_can_init(can_dev_t *can)
{
    return 0;
}

int32_t amp_hal_can_filter_init(can_dev_t *can, const uint8_t filter_grp_cnt, can_filter_item_t *filter_config)
{
    return 0;
}

int32_t amp_hal_can_send(can_dev_t *can, can_frameheader_t *tx_header, const void *data, const uint32_t timeout)
{
    return 0;
}

int32_t amp_hal_can_recv(can_dev_t *can, can_frameheader_t *rx_header, void *data, const uint32_t timeout)
{
    return 0;
}

int32_t amp_hal_can_finalize(can_dev_t *can)
{
    return 0;
}
