/**
 ****************************************************************************************
 *
 * @file intc.h
 *
 * @brief Declaration of the Reference Interrupt Controller (INTC) API.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _INTC_H_
#define _INTC_H_

#include "compiler.h"
#include "doubly_list.h"
#include "generic.h"

//#define INTC_DEBUG

#ifdef INTC_DEBUG
#define INTC_PRT       os_printf
#define INTC_WPRT      warning_prf
#else
#define INTC_PRT       os_null_printf
#define INTC_WPRT      os_null_printf
#endif

#define INTC_MAX_COUNT                    32

typedef struct
{
    LIST_HEADER_T list;

    FUNCPTR isr_func;

    UINT8  pri;            // priority
    UINT8  int_num;
    UINT16 resv;
} ISR_T;

typedef struct
{
    LIST_HEADER_T isr;

    UINT8 max;     // maximum interrupt number
    UINT8 min;     // minum interrupt number
} ISR_LIST_T;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize and configure the reference INTCTRL.
 * This function configures the INTC according to the system needs.
 ****************************************************************************************
 */
void intc_init(void);
void intc_irq(void);
void intc_fiq(void);

#endif // _INTC_H_
