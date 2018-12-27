/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:Retarget.c
//
//		Description	:
//					process timeout checking,short time delay,timer set and so on
//
//		Changelog	:					
//                  2014-05-29  Use ARM library I/O redirection and Addition by Robert
///////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include "type.h"
#include "uart.h"


#pragma import(__use_no_semihosting_swi)

extern int  sendchar(int ch);  /* in Serial.c */
extern int  getkey(void);      /* in Serial.c */
extern long timeval;           /* in Time.c   */

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int ferror(FILE *f) 
{
	/* Your implementation of ferror */
	return EOF;
}


void _ttywrch(int ch) 
{
	//sendchar (ch);
}


void _sys_exit(int return_code) 
{
	while(1);    /* endless loop */
}



// begin added by lean_xiong @2013-12-19
static unsigned char sOsDebugFlag = 1;
void OsSetDebugFlag(unsigned char Flag)
{
	sOsDebugFlag = Flag;
}
// end added by lean_xiong @2013-12-19

static unsigned char IsSwUartActedAsFuartFlag = 0;
void EnableSwUartAsFuart(unsigned char EnableFlag)
{
	IsSwUartActedAsFuartFlag = EnableFlag;
}

//This is used as dummy function in case that appilcation dont define this function.
__weak void SwUartSend(unsigned char* Buf, unsigned int BufLen)
{

}


int fputc(int c, FILE *f) 
{
    
// begin added by lean_xiong @2013-12-19
    if(!sOsDebugFlag)
    {
        return c;
    }
// end added by lean_xiong @2013-12-19
    /*
     * putchar is link to uart driver etc.
     */
    if(IsSwUartActedAsFuartFlag)
    {
        if((unsigned char)c == '\n')
        {
            const char lfca[2] = "\r\n";
            SwUartSend((unsigned char*)lfca, 2);

        }
        else
        {
            SwUartSend((unsigned char*)&c, 1);
        }
    }
    else
    {
        if((unsigned char)c == '\n')
        {
            const char lfca[2] = "\r\n";
            FuartSend((unsigned char*)lfca, 2);
        }
        else
        {
            FuartSend((unsigned char*)&c, 1);
        }
    }
    
	return c;
}


int fgetc(FILE *f) 
{    
    char c;
#ifdef  DEBUG
	FuartRecv(&c, 1, 0);
#endif
	return c;
    //return (sendchar(getkey()));
}
