/*!
    \file  main.c
    \brief GPIO running led
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-28, V1.0.0, demo for GD32F3x0
*/

#include "gd32f3x0.h"
#include "gd32f350r_eval.h"
#include <k_api.h>
#include <stdio.h>

void soc_init(void);
    
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{  
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    soc_init();

    while(1){
    }
}
