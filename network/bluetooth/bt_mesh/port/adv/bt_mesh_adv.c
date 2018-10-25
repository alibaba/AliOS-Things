/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "bt_mesh_adv.h"

int bt_mesh_adv_start(const struct bt_le_adv_param *param,
                      const struct bt_data *ad, size_t ad_len,
                      const struct bt_data *sd, size_t sd_len)
{
    return bt_le_adv_start(param, ad, ad_len, sd, sd_len);
}

int bt_mesh_adv_stop(void)
{
    return bt_le_adv_stop();
}

int bt_mesh_scan_start(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb)
{
    return bt_le_scan_start(param, cb);
}

int bt_mesh_scan_stop(void)
{
    return bt_le_scan_stop();
}
