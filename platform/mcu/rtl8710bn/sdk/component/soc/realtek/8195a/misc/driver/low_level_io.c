#include <stdio.h>
#include "hal_api.h"
#include "rtl8195a.h"
#include "platform_opts.h"

#if !defined (__ICCARM__)
extern u8 RAM_IMG1_VALID_PATTEN[];
void *tmp = RAM_IMG1_VALID_PATTEN;
#endif

//for internal test
#ifdef USE_MODE
    extern int use_mode;
    void mode_init(void){use_mode = 1;}
#endif
    
#if defined ( __ICCARM__ )
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize)
{
 	int nChars = 0;
 	/* Check for stdout and stderr 
 	(only necessary if file descriptors are enabled.) */
 	if (Handle != 1 && Handle != 2)
 	{
 		return -1;
 	}
 	for (/*Empty */; Bufsize > 0; --Bufsize)
 	{
 		DiagPutChar(*Buf++);
 		++nChars;
 	}
 	return nChars;
}

size_t __read(int Handle, unsigned char * Buf, size_t Bufsize)
{
	int nChars = 0;
	/* Check for stdin
	(only necessary if FILE descriptors are enabled) */
	if (Handle != 0)
	{
		return -1;
	}
	for (/*Empty*/; Bufsize > 0; --Bufsize)
	{
		int c = DiagGetChar(_FALSE);
		if (c < 0)
			break;
		*(Buf++) = c;
		++nChars;
	}
	return nChars;	
}
#endif



int disablePrintf = FALSE;


__weak VOID HalSerialPutcRtl8195a(IN  u8 c){
  
  	u32 CounterIndex = 0;

	if(disablePrintf == TRUE)	return;

	while(1) {
                CounterIndex++;
                if (CounterIndex >=6540)
				break;

		if (HAL_UART_READ8(UART_LINE_STATUS_REG_OFF) & 0x60)
			break;	
	}
	         HAL_UART_WRITE8(UART_TRAN_HOLD_OFF, c);  
    if (c == 0x0a) {
        HAL_UART_WRITE8(UART_TRAN_HOLD_OFF, 0x0d);  
    } 
}




#include <diag.h>
u32 
DiagPrintf(
    IN  const char *fmt, ...
)
{
	if(disablePrintf == TRUE)	return _TRUE;
	
	(void)VSprintf(0, fmt, ((const int *)&fmt)+1);	
    return _TRUE;
}

extern u32 ConfigDebugErr;
extern u32 ConfigDebugInfo;
extern u32 ConfigDebugWarn;
static u32 backupErr;
static u32 backupInfo;
static u32 backupWarn;
void log_uart_enable_printf(void)
{
	disablePrintf = FALSE;
	ConfigDebugErr = backupErr;
	ConfigDebugInfo = backupInfo;
	ConfigDebugWarn = backupWarn;
}

void log_uart_disable_printf(void)
{
	disablePrintf = TRUE;
	backupErr = ConfigDebugErr;
	backupInfo = ConfigDebugInfo;
	backupWarn = ConfigDebugWarn;
	ConfigDebugErr = 0;
	ConfigDebugInfo = 0;
	ConfigDebugWarn = 0;
}