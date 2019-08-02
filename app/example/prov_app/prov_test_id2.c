/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "ls_osa.h"
#include "ls_hal.h"
#include "prov_pro.h"
#include "prov_res.h"

int prov_test_id2(uint8_t *buf, uint32_t buf_len)
{
    int ret = 0;
    uint32_t res = 0;
    uint32_t prov_res = 0;

    res = prov_item(ITEM_NAME_ID2, buf, buf_len);
    if (res) {
        ls_osa_print("prov failed 0x%x\n", res);
        return -1;
    }

    ret = prov_get_prov_res(&prov_res);
    if (ret || prov_res) {         
        ls_osa_print("prov_get prov res failed ret %d res is 0x%x\n", ret, (unsigned int)prov_res);
        return ret;
    }
    ls_osa_print("get prov res success prov_res is 0x%x\n", (unsigned int)prov_res);

    ls_osa_print("prov test id2 success\n");

    return ret;
}

