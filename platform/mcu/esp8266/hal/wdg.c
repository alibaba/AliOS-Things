/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "aos/hal/wdg.h"

int32_t hal_wdg_init(wdg_dev_t *wdg){

    return 0;
}

void hal_wdg_reload(wdg_dev_t *wdg){
    (void)wdg;
}

int32_t hal_wdg_finalize(wdg_dev_t *wdg){
    (void)wdg;
    return 0;
}
