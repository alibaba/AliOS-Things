/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ACTIVATION_H
#define ACTIVATION_H

#ifndef NULL
#define NULL    ((void *)0)
#endif

#define VERSION_NUM_SIZE    4
#define RANDOM_NUM_SIZE     4
#define MAC_ADDRESS_SIZE    8
#define CHIP_CODE_SIZE      4
#define INPUT_SIZE          (VERSION_NUM_SIZE+RANDOM_NUM_SIZE+MAC_ADDRESS_SIZE+CHIP_CODE_SIZE)
#define OUTPUT_SPACE_SIZE   (INPUT_SIZE*2*2+1)

/* tigger activation report */
int activation_report(void );

#endif

