/***************************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
***************************************************************/

#include "lib_printf.h"
#include "lib_uart.h"
#include <stdarg.h>

#define DEBUG_UART UART1

int fputc(int ch, FILE *f)
{
    FlagStatus Status = RESET;
    uint32_t Count=0;
    
    UART_SendByte(DEBUG_UART, (unsigned char) ch);
    do
    {
        Status = UART_GetFlagStatus(DEBUG_UART,UART_FLAG_TB);
        Count++;
    }
    while((Status == RESET)&&(Count != 0x1CE2));
    
    if(Count == 0x1CE2)
    {
        return (EOF);
    }
    return (ch);
}

int fgetc(FILE *f)
{
    while (UART_GetFlagStatus(DEBUG_UART, UART_FLAG_ID) == RESET);

    return (int)UART_RecByte(DEBUG_UART);
}

static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;
    
    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }
    
    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
    
    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';
    
        /* Make the value positive. */
        value *= -1;
    }
    
    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;
    
        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }
    
    /* Null terminate the string. */
    *ptr = 0;
    
    return string;

}

ErrorStatus UART_printf(UART_TypeDef* UARTx, uint8_t *Data,...)
{
    const char *s;
    int d;   
    char buf[16];
    ErrorStatus RET = SUCCESS;
    FlagStatus Status = RESET;
    uint32_t Count=0;
    
    va_list ap;
    va_start(ap, Data);
    
    while ( *Data != 0)
    {
        if ( *Data == 0x5c )
        {
            switch ( *++Data )
            {
                case 'r':
                    Count=0;
                    UART_SendByte(UARTx, 0x0d);
                    do
                    {
                        Status = UART_GetFlagStatus(UARTx,UART_FLAG_TB);
                        Count++;
                    }
                    while((Status == RESET)&&(Count != 0x1CE2));
                    
                    if(Count == 0x1CE2)
                    {
                        RET = ERROR;
                    }
                    Data ++;
                break;
                
                case 'n':
                    Count=0;
                    UART_SendByte(UARTx, 0x0a);    
                    do
                    {
                        Status = UART_GetFlagStatus(UARTx,UART_FLAG_TB);
                        Count++;
                    }
                    while((Status == RESET)&&(Count != 0x1CE2));
                    
                    if(Count == 0x1CE2)
                    {
                        RET = ERROR;
                    }
                    Data ++;
                break;
                
                default:
                    Data ++;
                break;
            }             
        }
        else if ( *Data == '%')
        {
            switch ( *++Data )
            {                
                case 's':
                    s = va_arg(ap, const char *);
                    for ( ; *s; s++) 
                    {
                        Count=0;
                        UART_SendByte(UARTx,*s);
                        do
                        {
                            Status = UART_GetFlagStatus(UARTx,UART_FLAG_TB);
                            Count++;
                        }
                        while((Status == RESET)&&(Count != 0x1CE2));
                        
                        if(Count == 0x1CE2)
                        {
                            RET = ERROR;
                        }
                    }
                        Data++;
                    break;
                
                case 'd':
                    d = va_arg(ap, int);
                    itoa(d, buf, 10);
                    for (s = buf; *s; s++)  
                    {
                        Count=0;
                        UART_SendByte(UARTx,*s);
                        do
                        {
                            Status = UART_GetFlagStatus(UARTx,UART_FLAG_TB);
                            Count++;
                        }
                        while((Status == RESET)&&(Count != 0x1CE2));
                        
                        if(Count == 0x1CE2)
                        {
                            RET = ERROR;
                        }
                    }
                    Data++;
                    break;
                default:
                    Data++;
                    break;
            }
        } /* end of else if */
        else 
        {
            Count=0;
            UART_SendByte(UARTx, *Data++);
            do
            {
                Status = UART_GetFlagStatus(UARTx,UART_FLAG_TB);
                Count++;
            }
            while((Status == RESET)&&(Count != 0x1CE2));
            
            if(Count == 0x1CE2)
            {
                RET = ERROR;
            }
        }
    }
    return RET;
}

/*************************END OF FILE**********************/
