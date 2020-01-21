/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "ls_osa.h"
#include "ls_hal.h"
#include "prov_pro.h"
#include "prov_res.h"

int prov_test_id2(uint8_t *buf, uint32_t buf_len)
{
    uint32_t res = 0;

    res = prov_get_prov_res();
    if (res) {
        ls_osa_print("prov res is 0x%x start prov\n", res);
    } else {
        ls_osa_print("has proved, prov again\n");
    }

    res = prov_item(ITEM_NAME_ID2, buf, buf_len);
    if (res) {
        ls_osa_print("prov failed 0x%x\n", res);
    }

    res = prov_get_prov_res();
    if (res) {
        ls_osa_print("prov_get prov res failed res is 0x%x\n", (unsigned int)res);
        return -1;
    }
    ls_osa_print("get prov res success prov_res is 0x%x\n", res);

    ls_osa_print("prov test id2 success\n");

    return 0;
}

