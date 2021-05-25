#include <stdio.h>
#include <stdlib.h>
#include <data_uart.h>
#include <os_msg.h>
#include <trace_app.h>
#include <app_msg.h>
#include "ameba_soc.h"

#ifdef CONFIG_BT_USER_COMMAND
#define USE_DEDICATED_BT_DATA_UART    CONFIG_BT_USER_COMMAND
#else
#define USE_DEDICATED_BT_DATA_UART    0
#endif

static void *h_event_q;
static void *h_io_q;
UART_TypeDef *data_uart_def;

int data_uart_send_char(int ch);

void data_uart_msg_init(void *event_queue_handle, void *io_queue_handle)
{
    h_event_q = event_queue_handle;
    h_io_q = io_queue_handle;
}

void data_uart_msg_send(uint8_t rx_char)
{
    T_IO_MSG io_driver_msg_send;
    uint8_t event  = EVENT_IO_TO_APP;
    io_driver_msg_send.type = IO_MSG_TYPE_UART;
    io_driver_msg_send.subtype = rx_char;

    if (os_msg_send(h_io_q, &io_driver_msg_send, 0) == false)
    {
        APP_PRINT_ERROR0("data_uart_msg_send: send data failed");
    }
    else if (os_msg_send(h_event_q, &event, 0) == false)
    {
        APP_PRINT_ERROR0("data_uart_msg_send: send event failed");
    }
}

int data_uart_vsprintf(char *buf, const char *fmt, const int *dp)
{
    char *p, *s;

    s = buf;
    for (; *fmt != '\0'; ++fmt)
    {
        if (*fmt != '%')
        {
            buf ? *s++ = *fmt : data_uart_send_char(*fmt);
            continue;
        }
        if (*++fmt == 's')
        {
            for (p = (char *)*dp++; *p != '\0'; p++)
            {
                buf ? *s++ = *p : data_uart_send_char(*p);
            }
        }
        else    /* Length of item is bounded */
        {
            char tmp[20], *q = tmp;
            int shift = 28;

            if ((*fmt  >= '0') && (*fmt  <= '9'))
            {
                int width;
                unsigned char fch = *fmt;
                for (width = 0; (fch >= '0') && (fch <= '9'); fch = *++fmt)
                {
                    width = width * 10 + fch - '0';
                }
                shift = (width - 1) * 4;
            }
            /*
             * Before each format q points to tmp buffer
             * After each format q points past end of item
             */

            if ((*fmt == 'x') || (*fmt == 'X') || (*fmt == 'p') || (*fmt == 'P'))
            {
                /* With x86 gcc, sizeof(long) == sizeof(int) */
                const long *lp = (const long *)dp;
                long h = *lp++;
                int ncase = (*fmt & 0x20);
                int alt = 0;

                dp = (const int *)lp;
                if ((*fmt == 'p') || (*fmt == 'P'))
                {
                    alt = 1;
                }
                if (alt)
                {
                    *q++ = '0';
                    *q++ = 'X' | ncase;
                }
                for (; shift >= 0; shift -= 4)
                {
                    * q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
                }
            }
            else if (*fmt == 'd')
            {
                int i = *dp++;
                char *r;
                if (i < 0)
                {
                    *q++ = '-';
                    i = -i;
                }
                p = q;      /* save beginning of digits */
                do
                {
                    *q++ = '0' + (i % 10);
                    i /= 10;
                }
                while (i);
                /* reverse digits, stop in middle */
                r = q;      /* don't alter q */
                while (--r > p)
                {
                    i = *r;
                    *r = *p;
                    *p++ = i;
                }
            }
            else if (*fmt == 'c')
            {
                *q++ = *dp++;
            }
            else
            {
                *q++ = *fmt;
            }
            /* now output the saved string */
            for (p = tmp; p < q; ++p)
            {
                buf ? *s++ = *p : data_uart_send_char(*p);
            }
        }
    }
    if (buf)
    {
        *s = '\0';
    }
    return (s - buf);
}

void data_uart_print(char *fmt, ...)
{
    (void)data_uart_vsprintf(0, fmt, ((const int *)&fmt) + 1);
}

int data_uart_send_char(int ch)
{
#if (USE_DEDICATED_BT_DATA_UART == 0)
		LOGUART_PutChar((uint8_t)ch);
#else
    UART_CharPut(data_uart_def, (u8)ch);
    UART_WaitBusy(data_uart_def, 10);
#endif
    return ch;
}

/****************************************************************************/
/* UART interrupt                                                           */
/****************************************************************************/
u32 data_uart_irq(void *data)
{
    (void) data;
    volatile u8 reg_iir;
    u8 int_id;
    u32 reg_val;
    uint8_t rx_char;

    reg_iir = UART_IntStatus(data_uart_def);
    if ((reg_iir & RUART_IIR_INT_PEND) != 0)
    {
        /* No pending IRQ */
        return 0;
    }

    int_id = (reg_iir & RUART_IIR_INT_ID) >> 1;

    switch (int_id)
    {
    case RUART_RECEIVER_DATA_AVAILABLE:
    case RUART_TIME_OUT_INDICATION:
        {
            if (UART_Readable(data_uart_def))
            {
                UART_CharGet(data_uart_def, &rx_char);
                data_uart_msg_send(rx_char);
            }
        }
        break;

    case RUART_RECEIVE_LINE_STATUS:
        reg_val = (UART_LineStatusGet(data_uart_def));
        APP_PRINT_INFO1("data_uart_irq: LSR %08x interrupt", reg_val);
        break;

    default:
        APP_PRINT_ERROR1("data_uart_irq: Unknown interrupt type %u", int_id);
        break;
    }

    return 0;
}

#define DATAUART_IRQ_PRIO      11
/** 
  * UART Parameter
  * BaudRate: 115200
  * Word Length: 8 bit
  * Stop Bit: 1 bit
  * Parity: None
  * RX FIFO: 1 byte
  */
void data_uart_init(void *event_queue_handle, void *io_queue_handle)
{
    data_uart_msg_init(event_queue_handle, io_queue_handle);
#if (USE_DEDICATED_BT_DATA_UART == 1)
//PIN TX:A_18 RX:PA 19
    IRQn_Type irqn = UART0_IRQ;
    UART_InitTypeDef    UART_InitStruct;
    Pinmux_Config(_PA_18, PINMUX_FUNCTION_UART);
    Pinmux_Config(_PA_19, PINMUX_FUNCTION_UART);

    PAD_PullCtrl(_PA_18, GPIO_PuPd_UP);
    PAD_PullCtrl(_PA_19, GPIO_PuPd_UP);
    data_uart_def = UART0_DEV;

    //UART_PinMuxInit(0, S0);
    UART_StructInit(&UART_InitStruct);
    UART_InitStruct.WordLen = RUART_WLS_8BITS;
    UART_InitStruct.StopBit = RUART_STOP_BIT_1;
    UART_InitStruct.Parity = RUART_PARITY_DISABLE;
    UART_InitStruct.ParityType = RUART_EVEN_PARITY;
    UART_InitStruct.StickParity = RUART_STICK_PARITY_DISABLE;
    UART_InitStruct.RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_1BYTES;
    UART_InitStruct.FlowControl = FALSE;
    UART_Init(data_uart_def, &UART_InitStruct);
    UART_SetBaud(data_uart_def, 115200);

    InterruptDis(irqn);
    InterruptUnRegister(irqn);
    InterruptRegister((IRQ_FUN)data_uart_irq, irqn, NULL, DATAUART_IRQ_PRIO);
    InterruptEn(irqn, DATAUART_IRQ_PRIO);
    UART_INTConfig(data_uart_def, RUART_IER_ERBI | RUART_IER_ETOI | RUART_IER_ELSI, ENABLE);

    UART_RxCmd(data_uart_def, ENABLE);
#endif
}

