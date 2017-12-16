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
//  maintainer: Halley

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


void _sbrk_r()
{
		return;
}
void _write_r()
{
		return;
}
void _close_r()
{
		return;
}

void _read_r()
{
		return;
}

void _fstat_r()
{
		return;
}

void _isatty_r()
{
		return;
}

void _lseek_r()
{
		return;
}

#if 0
int ferror(FILE *f)
{
	/* Your implementation of ferror */
	return EOF;
}
#endif


void _ttywrch(int ch) 
{
	//sendchar (ch);
}

void _exit(int return_code)
{
	;//while(1);    /* endless loop */
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
void SwUartSend(unsigned char* Buf, unsigned int BufLen)
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

#include <stdarg.h>
#if 0
void mvprintf(const char* format, ...)
{
	char *str;
	int i;
	double flt;
	va_list va;

	/*
	** 初始化
	*/
	va_start(va,format);

	while(*format)
	{
		/*
		** 输出一般的字符
		*/
		if('%' != *format)
		{
			putchar(*format++);
			continue;
		}
		/*
		** 判定是什么样的输出格式
		*/
		switch(*++format)
		{
			case 'd':
			{
				i = va_arg(va,int);
				printf("%d",i);
			}
				break;
			case 'f':
			{
				flt = va_arg(va,double);
				printf("%f",flt);
			}
				break;
			case 's':
			{
				str = va_arg(va,char*);
				printf("%s",str);
			}
				break;
			case 'x':
			{
				i = va_arg(va,int);
				printf("%x",i);
			}
				break;
			case 'o':
			{
				i = va_arg(va,int);
				printf("%o",i);
			}
				break;
			default:
			{
				putchar(*format);
			}
				break;
		}/* end for switch */

		/*
		** 访问格式输出的下一个字符
		*/
		format++;

	}/* end while */

	/*
	** 处理完成
	*/
	va_end(va);
}
#endif
