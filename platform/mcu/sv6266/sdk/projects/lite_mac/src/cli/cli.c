//#include <config.h>
#include <string.h>
#include "soc_types.h"
#include "cli.h"
#include "osal.h"
#include "net_types.h"

#include "uart/drv_uart.h"
#include "uart/hal_uart.h"

OsEvent cli_rx_evt = NULL;

#define fflush(x)

/* Command Line: */
char *sgCmdBuffer = NULL;

static char *sgArgV[CLI_ARG_SIZE];
static u32 sgArgC;
static u32 sgCurPos = 0;

static uint8_t burst_cli_enable = 0;

#define BURST_CLI_SIZE        (4096)

uint32_t clibuf_size = CLI_BUFFER_SIZE;
uint32_t ringbuf_size = CLI_BUFFER_SIZE;
#define RINGBUF_SIZE        (ringbuf_size)
#define RINGBUF_IDX_WRAP    (ringbuf_size   -1)
#define RINGBUF_VIDX_WRAP   (ringbuf_size*2 -1)

typedef struct ringbuf {
    uint32_t vidx_w;
    uint32_t vidx_r;
    uint8_t  *data;
} ringbuf_st;

ringbuf_st *g_uart_rx_ringbuf = NULL;

/* Command Table: */

static void _CliCmdUsage( void )
{
    const CLICmds *CmdPtr;

    printf("\n\nUsage:\n");
    for( CmdPtr=gCliCmdTable; CmdPtr->Cmd; CmdPtr ++ )
    {
        //printf("%-20s\t\t%s\n", CmdPtr->Cmd, CmdPtr->CmdUsage);
        printf("%s%*s\t\t%s\n", CmdPtr->Cmd, (int)(20 - strlen(CmdPtr->Cmd)), " ", CmdPtr->CmdUsage);

    }
    printf("\n%s", CLI_PROMPT);
}
static inline char __get_char (void)
{
    int ret;
    u8 rx_buf[2] = {0};
    
    ret = drv_uart_read_fifo(rx_buf, 1, UART_BLOCKING);
    return rx_buf[0];
}

static inline void __put_char (u8 c)
{
	drv_uart_write_fifo(&c,1,UART_BLOCKING);
}

void Cli_Reset_Buf()
{
    memset( sgCmdBuffer, 0x0, clibuf_size);
    sgCmdBuffer[0] = 0x00;
    sgCurPos = 0;
}

void uart_rx_isr(void)
{
    uint32_t vidx_w_full;
    uint32_t vidx_w;
    uint32_t idx_w;

    OS_EnterCritical();

    vidx_w_full = (g_uart_rx_ringbuf->vidx_r + RINGBUF_SIZE) & RINGBUF_VIDX_WRAP;
    vidx_w      =  g_uart_rx_ringbuf->vidx_w;

    // # when data ready ,move all data in fifo into ringbuf.
    // ## if full, drop all and alert.
    // TODO: rx error
    while(hal_uart_get_line_status () & 1) {
    //while(drv_uart_is_receiver_available()) {
        if(vidx_w != vidx_w_full) {
            // ### write data
            idx_w = vidx_w & RINGBUF_IDX_WRAP;
            g_uart_rx_ringbuf->data[idx_w] =  (uint8_t)hal_uart_direct_read_byte();
            //hal_uart_read_fifo (&g_uart_rx_ringbuf.data[idx_w], 1, UART_NON_BLOCKING);
            if(g_uart_rx_ringbuf->data[idx_w] == 0x00) {
                printf("\n!RX Error %lu!\n", drv_uart_get_line_status());
            }
            // ### update pointer
            vidx_w = (vidx_w+1) & RINGBUF_VIDX_WRAP;
        }
        else {
            // Disable uart interrupt until RX ring buffer is available.
            printf("Queue Full!\n");
            drv_uart_register_isr(UART_DATA_RDY_IE, NULL);
            break;
        }
    }
    // # update idx back to structure
    g_uart_rx_ringbuf->vidx_w = vidx_w;
    OS_ExitCritical();
    OS_EventSet(cli_rx_evt);
}

void Cli_Init( void )
{
    // Sleep to wait other initialization done.
    //hal_sleep(10);

    //printf("\n<<Start Command Line>>\n\n\tPress \'?\'  for help......\n\n\n");
    printf("\n%s", CLI_PROMPT);
    fflush(stdout);
    burst_cli_enable = 0;

    memset( (void *)sgArgV, 0, sizeof(u8 *)*5 );
    sgCmdBuffer = (char *)OS_MemAlloc(clibuf_size);
    if(sgCmdBuffer == NULL) {
        printf("Cli_Init Fail!\n");
    }
    Cli_Reset_Buf();
    sgArgC = 0;

    g_uart_rx_ringbuf = (ringbuf_st *) OS_MemAlloc(sizeof(ringbuf_st));
    if(g_uart_rx_ringbuf == NULL) {
        printf("Cli_Init Fail!\n");
    }
    g_uart_rx_ringbuf->vidx_r = 0;
    g_uart_rx_ringbuf->vidx_w = 0;

    g_uart_rx_ringbuf->data = (uint8_t *)OS_MemAlloc(ringbuf_size);
    if(g_uart_rx_ringbuf->data == NULL) {
        printf("Cli_Init Fail!\n");
    }
    memset(g_uart_rx_ringbuf->data, 0x0, ringbuf_size);

    OS_EventCreate(&cli_rx_evt);
    drv_uart_register_isr(UART_DATA_RDY_IE, uart_rx_isr);
}

uint32_t uart_read(uint32_t char_2read, uint8_t* dst) {

    uint32_t vidx_r;
    uint32_t vidx_r_empty;
    uint32_t idx_r ;
    uint32_t cnt = 0;

    OS_EnterCritical();
    vidx_r       = g_uart_rx_ringbuf->vidx_r;
    vidx_r_empty = g_uart_rx_ringbuf->vidx_w;

    while((vidx_r != vidx_r_empty) && (cnt != char_2read)) {
        idx_r = vidx_r & RINGBUF_IDX_WRAP;
        //
        if(dst) {
            dst[cnt] = g_uart_rx_ringbuf->data[idx_r];
        }
        cnt++;
        //
        vidx_r = (vidx_r+1) & RINGBUF_VIDX_WRAP;
    }
    g_uart_rx_ringbuf->vidx_r = vidx_r;

    drv_uart_register_isr(UART_DATA_RDY_IE, uart_rx_isr);

    OS_ExitCritical();
    return cnt;
}

void Cli_Start( void )
{
    const CLICmds *CmdPtr;
    u8 ch;
    char *pch;

    if(uart_read(1, &ch) < 1) {
        OS_EventWait(cli_rx_evt, portMAX_DELAY);
        return;
    }

    switch (ch)
    {
        case 0x1b: /* Special Key, read again for real data */
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

	    case 0x0a: /* LF: Line Feed */
        case 0x0d: /* CR: Carriage Return */

            //printf("\nsgCurPos: %lu\n", sgCurPos);

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

				if ( (ch==1) && (*pch=='=') && (sgArgC == 0))
                {
                    *pch = 0x00;
                    ch = 0;
                    sgArgC ++;
                }

				if ( (ch==1) && (*pch==',') && (sgArgC != 0))
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
                        CmdPtr->CmdHandler( sgArgC-1, &sgArgV[1] );
                        break;
                    }
                }
                if ( (NULL==CmdPtr->Cmd) && (0!=sgCurPos) )
                {
                    printf("\nCommand not found!!\n");
                    fflush(stdout);
                }
            }
	
            printf("\n%s", CLI_PROMPT);
            fflush(stdout);

            Cli_Reset_Buf();
            break;

        case '?': /* for help */
            if(sgCurPos == 0)
            {
                __put_char(ch);
                fflush(stdout);
                _CliCmdUsage();
            }
            else
            {
                if ( (clibuf_size-1) > sgCurPos )
                {
                    sgCmdBuffer[sgCurPos++] = ch;
                    sgCmdBuffer[sgCurPos] = 0x00;
                    drv_uart_write_fifo(&ch, 1, UART_BLOCKING);
                }                
            }
            break;

        case '~':
            if (burst_cli_enable) {
                burst_cli_enable = 0;
                clibuf_size = CLI_BUFFER_SIZE;
                ringbuf_size = CLI_BUFFER_SIZE;
                printf("!disable burst cli mode\n");
            }
            else {
                burst_cli_enable = 1;
                clibuf_size = BURST_CLI_SIZE;
                ringbuf_size = BURST_CLI_SIZE;
                printf("!enable burst cli mode\n");

            }
            OS_EnterCritical();
            sgCmdBuffer = (char *)OS_MemRealloc(sgCmdBuffer, clibuf_size);// 2k size
            if(sgCmdBuffer == NULL) {
                printf("Error\n");
            }
            Cli_Reset_Buf();

            g_uart_rx_ringbuf->data = (uint8_t *)OS_MemRealloc(g_uart_rx_ringbuf->data, ringbuf_size);// 2k size
            if(g_uart_rx_ringbuf == NULL) {
                printf("Error!\n");
            }
            g_uart_rx_ringbuf->vidx_r = 0;
            g_uart_rx_ringbuf->vidx_w = 0;
            memset(g_uart_rx_ringbuf->data, 0x0, ringbuf_size);
            
            OS_ExitCritical();
            break;

        case '!':
            printf("clibuf_size %lu, ringbuf_size %lu\n", clibuf_size, ringbuf_size);
            break;

        default: /* other characters */
            if ( (clibuf_size-1) > sgCurPos )
            {
                sgCmdBuffer[sgCurPos++] = ch;
                sgCmdBuffer[sgCurPos] = 0x00;
                drv_uart_write_fifo(&ch, 1, UART_BLOCKING);
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

    Cli_Init();

    for( ;; )
    {
        Cli_Start();
    }

}

