/*!
    \file  main.c
    \brief GPIO running led
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-07-28, V1.0.0, demo for GD32F30x
*/

#include "gd32f4xx.h"
#include "gd32f450z_eval.h"


void soc_init(void);
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{  
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    soc_init();

    while(1){

    }
}
