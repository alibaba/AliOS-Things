/*
 *  Copyright (C) 2015 YunOS Project. All rights reserved.
 */

#include <stdlib.h>
#include "utils_timer.h"

int pal_get_random()
{
    static int flag = 0;

    /* srand must be called only once! */
    if (flag == 0) {
        srand((unsigned int)utils_time_get_ms());
        flag = 1;
    }

    return rand();
}

