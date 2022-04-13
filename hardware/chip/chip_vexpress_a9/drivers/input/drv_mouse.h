#ifndef __DRV_MOUSE_H__
#define __DRV_MOUSE_H__
#include <stdio.h>
#include "realview.h"
#include "board.h"

int drv_mouse_hw_init(void);

enum {
    INPUT_OK = 0,
    INPUT_TRUE = 0,
    INPUT_FALSE,
    INPUT_ERROR
};

#endif