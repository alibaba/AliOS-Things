#include <stdio.h>

#include "soc_types.h"
#include "cli.h"

#include "uart/drv_uart.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? (__builtin_strrchr(__FILE__, '/') + 1) : (__FILE__))

/*---------------------------------- CMDs -----------------------------------*/
typedef struct _stParam
{
    char    *argv[10];
    int     argc;
} stParam;

extern stParam param;

static void uart_rx_line_status_isr ()
{
    UART_LSR LSR;
    LSR.Reg32 = drv_uart_get_line_status ();

    printf("%s:%d: In function: '%s':\n", __FILENAME__, __LINE__ - 4, __func__);

    printf ("UART Receiver Line Status: 0x%08lX\n", LSR.Reg32);
    printf ("UART Receiver Line Overrun Error: %d\n", LSR.Bits.OE);
    printf ("UART Receiver Line Parity Error: %d\n", LSR.Bits.PE);
    printf ("UART Receiver Line Framing Error: %d\n", LSR.Bits.FE);
    printf ("UART Receiver Line Break Interrupt indicator: %d\n\n", LSR.Bits.BI);
}

static void interrupt (int32_t argc, char *argv[])
{
    int32_t         BaudRate;
    UART_WORD_LEN_E DataBits;
    UART_STOP_BIT_E StopBits;
    UART_PARITY_E   Parity;

    int32_t         retval;

    if (argc == 5) {
        BaudRate    = strtol (argv[1], NULL, 10);
        DataBits    = strtol (argv[2], NULL, 10);
        StopBits    = strtol (argv[3], NULL, 10);
        Parity      = strtol (argv[4], NULL, 10);

    } else {
        printf ("\nUsage: int [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the UART.\n");
        printf ("                       Contains at least the [1, 921600] range\n");
        printf ("DataBits               The number of data bits in each character.\n");
        printf ("                       0: Character's Word Length 5 bits\n");
        printf ("                       1: Character's Word Length 6 bits\n");
        printf ("                       2: Character's Word Length 7 bits\n");
        printf ("                       3: Character's Word Length 8 bits\n");
        printf ("StopBits               The number of stop bits per character.\n");
        printf ("                       0: One stop bit will be transmitted\n");
        printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
        printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
        printf ("                       0: No parity bit is transmitted nor expected\n");
        printf ("                       1: The number of bits including the parity bit must be odd\n");
        printf ("                       3: The number of bits including the parity bit must be even\n");
        printf ("                       5: The parity bit is sent as/checked to be 1\n");
        printf ("                       7: The parity bit is sent as/checked to be 0\n");

        return;
    }

    if (((BaudRate < 1) || (BaudRate > 921600)) ||
        ((DataBits < 0) || (DataBits > 3)) ||
        ((StopBits < 0) || (StopBits > 1)) ||
        ((Parity < 0) || (Parity == 2) || (Parity == 4) || (Parity == 6) || (Parity > 7))) {
        printf ("\nUsage: int [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the UART.\n");
        printf ("                       Contains at least the [1, 921600] range\n");
        printf ("DataBits               The number of data bits in each character.\n");
        printf ("                       0: Character's Word Length 5 bits\n");
        printf ("                       1: Character's Word Length 6 bits\n");
        printf ("                       2: Character's Word Length 7 bits\n");
        printf ("                       3: Character's Word Length 8 bits\n");
        printf ("StopBits               The number of stop bits per character.\n");
        printf ("                       0: One stop bit will be transmitted\n");
        printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
        printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
        printf ("                       0: No parity bit is transmitted nor expected\n");
        printf ("                       1: The number of bits including the parity bit must be odd\n");
        printf ("                       3: The number of bits including the parity bit must be even\n");
        printf ("                       5: The parity bit is sent as/checked to be 1\n");
        printf ("                       7: The parity bit is sent as/checked to be 0\n");

        return;
    }

    printf("uart receiver line status interrupt test program (Build: %s %s)\n", __DATE__, __TIME__);
    while (!drv_uart_is_transmitter_idle ());

    drv_uart_init ();
    drv_uart_sw_rst ();
    drv_uart_set_fifo (UART_INT_RXFIFO_TRGLVL_1, 2);
    retval = drv_uart_set_format (BaudRate, DataBits, StopBits, Parity);
    drv_uart_register_isr (UART_RX_LINESTS_IE, uart_rx_line_status_isr);

    printf ("uart wait for receiver line status interrupt event...\n\n");
    while (1) {}
}

static void ascii (int32_t argc, char *argv[])
{
    int32_t         BaudRate;
    UART_WORD_LEN_E DataBits;
    UART_STOP_BIT_E StopBits;
    UART_PARITY_E   Parity;

    int32_t         retval;
    uint8_t         ascii;
    uint8_t         wBuffer[94] = {0};

    if (argc == 5) {
        BaudRate    = strtol (argv[1], NULL, 10);
        DataBits    = strtol (argv[2], NULL, 10);
        StopBits    = strtol (argv[3], NULL, 10);
        Parity      = strtol (argv[4], NULL, 10);

    } else {
        printf ("\nUsage: ascii [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the UART.\n");
        printf ("                       Contains at least the [1, 921600] range\n");
        printf ("DataBits               The number of data bits in each character.\n");
        printf ("                       0: Character's Word Length 5 bits\n");
        printf ("                       1: Character's Word Length 6 bits\n");
        printf ("                       2: Character's Word Length 7 bits\n");
        printf ("                       3: Character's Word Length 8 bits\n");
        printf ("StopBits               The number of stop bits per character.\n");
        printf ("                       0: One stop bit will be transmitted\n");
        printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
        printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
        printf ("                       0: No parity bit is transmitted nor expected\n");
        printf ("                       1: The number of bits including the parity bit must be odd\n");
        printf ("                       3: The number of bits including the parity bit must be even\n");
        printf ("                       5: The parity bit is sent as/checked to be 1\n");
        printf ("                       7: The parity bit is sent as/checked to be 0\n");

        return;
    }

    if (((BaudRate < 1) || (BaudRate > 921600)) ||
        ((DataBits < 0) || (DataBits > 3)) ||
        ((StopBits < 0) || (StopBits > 1)) ||
        ((Parity < 0) || (Parity == 2) || (Parity == 4) || (Parity == 6) || (Parity > 7))) {
        printf ("\nUsage: ascii [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the UART.\n");
        printf ("                       Contains at least the [1, 921600] range\n");
        printf ("DataBits               The number of data bits in each character.\n");
        printf ("                       0: Character's Word Length 5 bits\n");
        printf ("                       1: Character's Word Length 6 bits\n");
        printf ("                       2: Character's Word Length 7 bits\n");
        printf ("                       3: Character's Word Length 8 bits\n");
        printf ("StopBits               The number of stop bits per character.\n");
        printf ("                       0: One stop bit will be transmitted\n");
        printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
        printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
        printf ("                       0: No parity bit is transmitted nor expected\n");
        printf ("                       1: The number of bits including the parity bit must be odd\n");
        printf ("                       3: The number of bits including the parity bit must be even\n");
        printf ("                       5: The parity bit is sent as/checked to be 1\n");
        printf ("                       7: The parity bit is sent as/checked to be 0\n");

        return;
    }

    printf ("uart transmit ASCII 21H~7EH test program (Build: %s %s)\n", __DATE__, __TIME__);
    while (!drv_uart_is_transmitter_idle ());

    drv_uart_init ();
    drv_uart_sw_rst ();
    drv_uart_set_fifo (UART_INT_RXFIFO_TRGLVL_1, 0);
    retval = drv_uart_set_format (BaudRate, DataBits, StopBits, Parity);

    for (ascii = 0x21; ascii <= 0x7E; ascii++) {
        wBuffer[ascii - 0x21] = ascii;
    }

    retval = drv_uart_write_fifo (wBuffer, sizeof (wBuffer), UART_BLOCKING);

    printf ("\ntransmitted length: %ld\n", retval);
}

static void loopback (int32_t argc, char *argv[])
{
    int32_t         BaudRate;
    UART_WORD_LEN_E DataBits;
    UART_STOP_BIT_E StopBits;
    UART_PARITY_E   Parity;

    int32_t         retval;
    uint8_t         Buffer = 0;

    if (argc == 5) {
        BaudRate    = strtol (argv[1], NULL, 10);
        DataBits    = strtol (argv[2], NULL, 10);
        StopBits    = strtol (argv[3], NULL, 10);
        Parity      = strtol (argv[4], NULL, 10);

    } else {
        printf ("\nUsage: loopback [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the UART.\n");
        printf ("                       Contains at least the [1, 921600] range\n");
        printf ("DataBits               The number of data bits in each character.\n");
        printf ("                       0: Character's Word Length 5 bits\n");
        printf ("                       1: Character's Word Length 6 bits\n");
        printf ("                       2: Character's Word Length 7 bits\n");
        printf ("                       3: Character's Word Length 8 bits\n");
        printf ("StopBits               The number of stop bits per character.\n");
        printf ("                       0: One stop bit will be transmitted\n");
        printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
        printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
        printf ("                       0: No parity bit is transmitted nor expected\n");
        printf ("                       1: The number of bits including the parity bit must be odd\n");
        printf ("                       3: The number of bits including the parity bit must be even\n");
        printf ("                       5: The parity bit is sent as/checked to be 1\n");
        printf ("                       7: The parity bit is sent as/checked to be 0\n");

        return;
    }

    if (((BaudRate < 1) || (BaudRate > 921600)) ||
        ((DataBits < 0) || (DataBits > 3)) ||
        ((StopBits < 0) || (StopBits > 1)) ||
        ((Parity < 0) || (Parity == 2) || (Parity == 4) || (Parity == 6) || (Parity > 7))) {
        printf ("\nUsage: loopback [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the UART.\n");
        printf ("                       Contains at least the [1, 921600] range\n");
        printf ("DataBits               The number of data bits in each character.\n");
        printf ("                       0: Character's Word Length 5 bits\n");
        printf ("                       1: Character's Word Length 6 bits\n");
        printf ("                       2: Character's Word Length 7 bits\n");
        printf ("                       3: Character's Word Length 8 bits\n");
        printf ("StopBits               The number of stop bits per character.\n");
        printf ("                       0: One stop bit will be transmitted\n");
        printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
        printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
        printf ("                       0: No parity bit is transmitted nor expected\n");
        printf ("                       1: The number of bits including the parity bit must be odd\n");
        printf ("                       3: The number of bits including the parity bit must be even\n");
        printf ("                       5: The parity bit is sent as/checked to be 1\n");
        printf ("                       7: The parity bit is sent as/checked to be 0\n");

        return;
    }

    printf ("uart loopback test program (Build: %s %s)\n", __DATE__, __TIME__);
    while (!drv_uart_is_transmitter_idle ());

    drv_uart_init ();
    drv_uart_sw_rst ();
    drv_uart_set_fifo (UART_INT_RXFIFO_TRGLVL_8, 0x2);
    retval = drv_uart_set_format (BaudRate, DataBits, StopBits, Parity);

    while (1) {
        retval = drv_uart_read_fifo (&Buffer, 1, UART_BLOCKING);
        retval = drv_uart_write_fifo (&Buffer, 1, UART_BLOCKING);
    }
}

static void help (int32_t argc, char *argv[])
{
    printf ("Usage: loopback [BaudRate] [DataBits] [StopBits] [Parity]\n");
    printf ("Options:\n");
    printf ("BaudRate               The baud rate of the UART.\n");
    printf ("                       Contains at least the [1, 921600] range\n");
    printf ("DataBits               The number of data bits in each character.\n");
    printf ("                       0: Character's Word Length 5 bits\n");
    printf ("                       1: Character's Word Length 6 bits\n");
    printf ("                       2: Character's Word Length 7 bits\n");
    printf ("                       3: Character's Word Length 8 bits\n");
    printf ("StopBits               The number of stop bits per character.\n");
    printf ("                       0: One stop bit will be transmitted\n");
    printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
    printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
    printf ("                       0: No parity bit is transmitted nor expected\n");
    printf ("                       1: The number of bits including the parity bit must be odd\n");
    printf ("                       3: The number of bits including the parity bit must be even\n");
    printf ("                       5: The parity bit is sent as/checked to be 1\n");
    printf ("                       7: The parity bit is sent as/checked to be 0\n");
    printf ("\n");

    printf ("Usage: ascii [BaudRate] [DataBits] [StopBits] [Parity]\n");
    printf ("Options:\n");
    printf ("BaudRate               The baud rate of the UART.\n");
    printf ("                       Contains at least the [1, 921600] range\n");
    printf ("DataBits               The number of data bits in each character.\n");
    printf ("                       0: Character's Word Length 5 bits\n");
    printf ("                       1: Character's Word Length 6 bits\n");
    printf ("                       2: Character's Word Length 7 bits\n");
    printf ("                       3: Character's Word Length 8 bits\n");
    printf ("StopBits               The number of stop bits per character.\n");
    printf ("                       0: One stop bit will be transmitted\n");
    printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
    printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
    printf ("                       0: No parity bit is transmitted nor expected\n");
    printf ("                       1: The number of bits including the parity bit must be odd\n");
    printf ("                       3: The number of bits including the parity bit must be even\n");
    printf ("                       5: The parity bit is sent as/checked to be 1\n");
    printf ("                       7: The parity bit is sent as/checked to be 0\n");
    printf ("\n");

    printf ("Usage: int [BaudRate] [DataBits] [StopBits] [Parity]\n");
    printf ("Options:\n");
    printf ("BaudRate               The baud rate of the UART.\n");
    printf ("                       Contains at least the [1, 921600] range\n");
    printf ("DataBits               The number of data bits in each character.\n");
    printf ("                       0: Character's Word Length 5 bits\n");
    printf ("                       1: Character's Word Length 6 bits\n");
    printf ("                       2: Character's Word Length 7 bits\n");
    printf ("                       3: Character's Word Length 8 bits\n");
    printf ("StopBits               The number of stop bits per character.\n");
    printf ("                       0: One stop bit will be transmitted\n");
    printf ("                       1: Two stop bits (1.5 with 5-bit data) will be transmitted\n");
    printf ("Parity                 That is computed or checked as each character is transmitted or received.\n");
    printf ("                       0: No parity bit is transmitted nor expected\n");
    printf ("                       1: The number of bits including the parity bit must be odd\n");
    printf ("                       3: The number of bits including the parity bit must be even\n");
    printf ("                       5: The parity bit is sent as/checked to be 1\n");
    printf ("                       7: The parity bit is sent as/checked to be 0\n");
    printf ("\n");
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
extern void _notify_host_reset();
const CLICmds gCliCmdTable[] =
{
    {"help",            help,               "uart help"},
    {"loopback",        loopback,           "uart loopback test"},
    {"ascii",           ascii,              "uart ascii test"},
    {"int",             interrupt,          "uart interrupt test"},
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};
