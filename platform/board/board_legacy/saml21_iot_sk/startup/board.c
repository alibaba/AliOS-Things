/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, demo for GD32F3x0
*/
#include "saml21.h"

void hal_reboot(void)
{
    NVIC_SystemReset();
}

void board_init(void)
{
    aos_heap_set();
    /* Initializes MCU, drivers and middleware */
    _init_chip();
}
