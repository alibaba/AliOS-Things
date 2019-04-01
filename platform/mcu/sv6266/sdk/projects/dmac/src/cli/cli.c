//#include <config.h>
#include "soc_types.h"

#include "cli.h"
#include "osal.h"
#include "net_types.h"

#include "uart/drv_uart.h"

static OsSemaphore cli_rx_sem = NULL;

#define CLI_HISTORY_ENABLE      (0)
#define fflush(x)

#define USE_SERIAL_DRV

/* Command Line: */
static char sgCmdBuffer[CLI_BUFFER_SIZE+1];
static char *sgArgV[CLI_ARG_SIZE];
static u32 sgArgC;
static u32 sgCurPos = 0;


/* Command Table: */

static void _CliCmdUsage( void )
{
    const CLICmds *CmdPtr;

    printf("\n\nUsage:\n");
    for( CmdPtr=gCliCmdTable; CmdPtr->Cmd; CmdPtr ++ )
    {
#if defined(TINY_LIBC)
        printf("%-20s\t\t%s\n", CmdPtr->Cmd, CmdPtr->CmdUsage);
#else
        printf("%s%*s\t\t%s\n", CmdPtr->Cmd, 20 - strlen(CmdPtr->Cmd), " ", CmdPtr->CmdUsage);
#endif
    }
    printf("\n%s", CLI_PROMPT);
}

#if 0
static char __get_char (void)
{
#ifndef USE_SERIAL_DRV
	return getch();
#else
	signed portBASE_TYPE ret;
	char c;
	do {
		ret = xSerialGetChar(CONSOLE_SERIAL_PORT, (signed char *)&c, portMAX_DELAY);
	} while (ret != pdTRUE);

	return c;
#endif // 0
}


static void __put_char (char c)
{
#ifndef USE_SERIAL_DRV
	putchar(c);
#else
	xSerialPutChar(CONSOLE_SERIAL_PORT, (signed portCHAR)c, portMAX_DELAY);
#endif
}
#else
s32 drv_uart_rx_0(void);

static inline char __get_char (void)
{
#ifndef USE_SERIAL_DRV
    int ret;
    char c;

    c = dbgcon_rx();//hal_console_getc((signed char *)&c);
    return c;
#else
    int ret;
    u8 rx_buf[2] = {0};
    
    ret = drv_uart_read_fifo(rx_buf, 1, UART_BLOCKING);
    return rx_buf[0];
#endif    //USE_SERIAL_DRV
}
#endif

void drv_uart_tx_0(s8 ch);

static inline void __put_char (char c)
{
	drv_uart_write_fifo(&c,1,UART_BLOCKING);
}

#if (CLI_HISTORY_ENABLE==1)


extern char gCmdHistoryBuffer[CLI_HISTORY_NUM][CLI_BUFFER_SIZE+1];
extern char gCmdHistoryIdx;
extern char gCmdHistoryCnt;

static void Cli_EraseCmdInScreen()
{
	u32 i;
	for(i= 0;i < (strlen(sgCmdBuffer)+strlen(CLI_PROMPT));i++)
	{
		__put_char(0x08);
		__put_char(0x20);
		__put_char(0x08);
	}
	fflush(stdout);
}

static void Cli_PrintCmdInScreen()
{
	u32 i;
	printf("%s", CLI_PROMPT);
	for(i= 0;i<strlen(sgCmdBuffer);i++)
	{
		 __put_char(sgCmdBuffer[i]);
	}
	fflush(stdout);
}



static u8 Cli_GetPrevHistoryIdx()
{
	char CmdIdx = gCmdHistoryIdx;
	if(gCmdHistoryIdx == 0)
            CmdIdx =0;
        else
	    CmdIdx--;


	return 	CmdIdx;
}

static u8 Cli_GetNextHistoryIdx()
{
	char CmdIdx = gCmdHistoryIdx;

	CmdIdx++;

	if(CmdIdx >= CLI_HISTORY_NUM || CmdIdx > gCmdHistoryCnt)
		CmdIdx--;


	return CmdIdx;
}



static inline void Cli_StoreCmdBufToHistory(u8 history)
{
	u32 len = strlen((const char*)sgCmdBuffer);
	memcpy(&gCmdHistoryBuffer[history], sgCmdBuffer, len);
	gCmdHistoryBuffer[history][len]=0x00;
}


static inline void Cli_RestoreHistoryToCmdBuf(u8 history)
{
	u32 len = strlen((const char*)&gCmdHistoryBuffer[history]);
	memcpy(sgCmdBuffer, &gCmdHistoryBuffer[history], len);
	sgCmdBuffer[len]= 0x00;
	sgCurPos = len;
}




static void Cli_MovetoPrevHistoryCmdBuf()
{
	u8 CmdIdx = gCmdHistoryIdx;
	u8 NewCmdIdx = Cli_GetPrevHistoryIdx();

	if(CmdIdx == NewCmdIdx)
		return;

	Cli_EraseCmdInScreen();
	Cli_StoreCmdBufToHistory(CmdIdx);
	Cli_RestoreHistoryToCmdBuf(NewCmdIdx);
	Cli_PrintCmdInScreen();
	gCmdHistoryIdx = NewCmdIdx;
}






static void Cli_MovetoNextHistoryCmdBuf()
{
	u8 CmdIdx = gCmdHistoryIdx;
		u8 NewCmdIdx = Cli_GetNextHistoryIdx();

		if(CmdIdx == NewCmdIdx)
		{

			return;
                }

		Cli_EraseCmdInScreen();
		Cli_StoreCmdBufToHistory(CmdIdx);
		Cli_RestoreHistoryToCmdBuf(NewCmdIdx);
		Cli_PrintCmdInScreen();
		gCmdHistoryIdx = NewCmdIdx;
}



static void Cli_RecordInHistoryCmdBuf()
{
	u32 i = CLI_HISTORY_NUM-2;
	u32 len;

	if(sgCurPos)
	{
		//shift history log
		for(; i>0 ; i--)
		{
			len = strlen((const char*)&gCmdHistoryBuffer[i]);
			memcpy(&gCmdHistoryBuffer[i+1], &gCmdHistoryBuffer[i], len);
			gCmdHistoryBuffer[i+1][len] = 0x00;
		}


		//copy bud to index 1
		len = strlen((const char*)&sgCmdBuffer);
		memcpy(&gCmdHistoryBuffer[1], &sgCmdBuffer, len);
		gCmdHistoryBuffer[1][len] = 0x00;


		//Record how much history we record
		gCmdHistoryCnt++;
		if(gCmdHistoryCnt>=CLI_HISTORY_NUM)
			gCmdHistoryCnt = CLI_HISTORY_NUM-1;
	}

	//Reset buf
	gCmdHistoryBuffer[0][0]=0x00;
	gCmdHistoryIdx = 0;


}

#endif


void Cli_Reset_Buf()
{
    memset( sgCmdBuffer, 0x0, strlen(sgCmdBuffer));
    sgCurPos = 0;
}

void uart_rx_isr(void)
{
    drv_uart_register_isr(UART_DATA_RDY_IE, NULL);

    OS_SemSignal(cli_rx_sem);
}

void Cli_Init( void )
{
    // Sleep to wait other initialization done.
    //hal_sleep(10);

    //printf("\n<<Start Command Line>>\n\n\tPress \'?\'  for help......\n\n\n");
    printf("\n%s", CLI_PROMPT);
    fflush(stdout);

    memset( (void *)sgArgV, 0, sizeof(u8 *)*5 );
    sgCmdBuffer[0] = 0x00;
    sgCurPos = 0;
    sgArgC = 0;

    if (OS_SemInit(&cli_rx_sem, 1, 0) == OS_FAILED)
    {
        printf("Cli_Init Fail!\n");
    }
    drv_uart_register_isr(UART_DATA_RDY_IE, uart_rx_isr);
}



void Cli_Start( void )
{
    const CLICmds *CmdPtr;
    u8 ch;
    char *pch;

	#ifndef USE_SERIAL_DRV
    if ( !dbgcon_rx_ready() )
    {
        return;
    }
	#endif // USE_SERIAL_DRV

    if (!drv_uart_is_receiver_available())
    {
        drv_uart_register_isr(UART_DATA_RDY_IE, uart_rx_isr);
        OS_SemWait(cli_rx_sem, portMAX_DELAY);
        return;        
    }

    drv_uart_read_fifo(&ch, 1, UART_BLOCKING);

    switch (ch)
    {
        case 0x1b: /* Special Key, read again for real data */
            ch = __get_char();
 #if (CLI_HISTORY_ENABLE==1)
            if(ch == 0x5b)
            {

                ch = __get_char();
				if(0x41 == ch)//up arrow key
				{
					Cli_MovetoNextHistoryCmdBuf();
				}
				else if(0x42 == ch)//down arrow key
				{

					Cli_MovetoPrevHistoryCmdBuf();
				}
				else
				{

				}
             }

#endif
            break;
        case 0x08: /* Backspace key */
		case 0x7f: /* del key */	
            if ( 0 < sgCurPos )
            {
				
                //fflush(stdout);
                sgCurPos --;
                sgCmdBuffer[sgCurPos] = 0x00;
                __put_char(0x08);
                __put_char(0x20);
                __put_char(0x08);
            }
            break;
#ifdef __linux__
        case 0x0a: /* Enter */
#else
        case 0x0d: /* Enter */
#endif
#if (CLI_HISTORY_ENABLE==1)
			Cli_RecordInHistoryCmdBuf();
#endif

//			if(strlen(sgCmdBuffer) > 0)
//			{
//				//printf("\n^sgCmdBuffer=%s^,len=%d\n",sgCmdBuffer,strlen(sgCmdBuffer));
//				At_Parser(sgCmdBuffer,strlen(sgCmdBuffer));
//			}

#if 1
            for( sgArgC=0,ch=0, pch=sgCmdBuffer; (*pch!=0x00)&&(sgArgC<CLI_ARG_SIZE); pch++ )
            {
                if ( (ch==0) && (*pch!=' ') )
                {
                    ch = 1;
                    sgArgV[sgArgC] = pch;
                }

                if ( (ch==1) && (*pch==' ') )
                {
                    *pch = 0x00;
                    ch = 0;
                    sgArgC ++;
                }
            }
            if ( ch == 1)
            {
                sgArgC ++;
            }
            else if ( sgArgC > 0 )
            {
                *(pch-1) = ' ';
            }

            if ( sgArgC > 0 )
            {
                /* Dispatch command */
                for( CmdPtr=gCliCmdTable; CmdPtr->Cmd; CmdPtr ++ )
                {
                    if ( !strcmp(sgArgV[0], CmdPtr->Cmd) )
                    {
                        printf("\n");
                        fflush(stdout);
                        CmdPtr->CmdHandler( sgArgC, sgArgV );
                        break;
                    }
                }
                if ( (NULL==CmdPtr->Cmd) && (0!=sgCurPos) )
                {
                    printf("\nCommand not found!!\n");
                    fflush(stdout);
                }
            }
#endif			
            printf("\n%s", CLI_PROMPT);
            fflush(stdout);

            sgCmdBuffer[0] = 0x00;
            sgCurPos = 0;
            break;

        case '?': /* for help */
            __put_char(ch);
            fflush(stdout);
            _CliCmdUsage();
            break;

        default: /* other characters */
            if ( (CLI_BUFFER_SIZE-1) > sgCurPos )
            {
                sgCmdBuffer[sgCurPos++] = ch;
                sgCmdBuffer[sgCurPos] = 0x00;
                __put_char(ch);
                fflush(stdout);
            }
            break;
    }

}





/**
 *  CLI (Command Line Interface) Task:
 *
 *  Create CLI Task for console mode debugging.
 */
void Cli_Task( void *args )
{
//    printf("%s (%s) starts running.\n", OS_TaskGetName(), __FUNCTION__);
    /* Init command line before using it */
    Cli_Init();

    for( ;; )
    {
        Cli_Start();
        //hal_delay(100);
    }

}






