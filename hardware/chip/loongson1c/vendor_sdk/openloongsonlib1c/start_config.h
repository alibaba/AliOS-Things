/**
 * start_config.h - config for eraly initialize
 *
 * Copyright (C) 2018 Jiaxun Yang <jiaxun.yang@flygoat.com>
 *
 * SPDX-License-Identifier:     GPL-2.0
*/


#ifndef __OPENLOONGSON_START_CONFIG_H
#define __OPENLOONGSON_START_CONFIG_H

/*
* SDRAM config
*
* Take IS42S16400 SDRAM chip as a example
* The specfication is
* Size 8MB
* WIDTH: 16 bits
* COL WIDTH: 8 bits, so we should take the 8 power of 2, it's 256
* ROW WIDTH: 12 bits, so we take the 12 power of 2, it's 4K
*
* Please reference macro SD_PARA0 and SDRAM registers intruduction
* in user's manual to edit the SDRAM config.
*/

#if 1
/* 8MByte IS42S16400 appeared in bacai_board V1.x V2.x */
#define MEM_SIZE        (0x02000000)
#define SDRAM_WIDTH     (WIDTH_16)
#define SDRAM_COL       (COL_256)
#define SDRAM_ROW       (ROW_4K)
#else
/* 32MByte appeared in SmartLoong board */
#define MEM_SIZE        (0x02000000)
#define SDRAM_WIDTH     (WIDTH_16)
#define SDRAM_COL       (COL_512)
#define SDRAM_ROW       (ROW_8K)
#endif

/* Early Debug config */
#define EARLY_DEBUG /* UART2 serial print debug in early stage */
#define EARLY_DEBUG_BAUD  (115200)

/* Clock config */
#define PLL_MULT          (0x54)  /* When external clock is 24Mhz PLL=504Mhz */
#define SDRAM_DIV         (0)     /* SDRAM is CPU divided by 2*/
#define CPU_DIV           (2)     /* CPU clock is PLL divided by 2 */



#endif
