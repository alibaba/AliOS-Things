/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#ifdef KEIL_SIMULATOR
#include <stdio.h>
#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

/*---------------------------------------------------------------------------*/
struct __FILE
{
    int handle; /* Add whatever you need here */
};
FILE __stdout;
FILE __stdin;

/*---------------------------------------------------------------------------*/
int  sendchar(int ch)
{
    if (DEMCR & TRCENA)
    {
        while (ITM_Port32(0) == 0);

        ITM_Port8(0) = ch;
    }

    return(ch);
}

/*---------------------------------------------------------------------------*/
int fputc(int ch, FILE *f)
{
    return (sendchar(ch));
}

/*---------------------------------------------------------------------------*/
int fgetc(FILE *f)
{
    return (-1);
}

/*---------------------------------------------------------------------------*/
int ferror(FILE *f)
{
    /* Your implementation of ferror */
    return EOF;
}

/*---------------------------------------------------------------------------*/
void _ttywrch(int ch)
{
    sendchar(ch);
}

/*---------------------------------------------------------------------------*/
void _sys_exit(int return_code)
{
label:
    goto label;  /* endless loop */
}
#endif // KEIL_SIMULATOR

// eof

