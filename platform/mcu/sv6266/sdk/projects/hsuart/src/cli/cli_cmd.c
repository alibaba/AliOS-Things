#include <stdio.h>

#include "soc_types.h"
#include "cli.h"

#include "hsuart/drv_hsuart.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? (__builtin_strrchr(__FILE__, '/') + 1) : (__FILE__))

/*---------------------------------- CMDs -----------------------------------*/
typedef struct _stParam
{
    char    *argv[10];
    int     argc;
} stParam;

extern stParam param;

static void hsuart_isr ()
{
    uint8_t Buffer[32] = {0};
    int32_t retval;

    printf("%s:%d: In function: '%s':\n", __FILENAME__, __LINE__ - 4, __func__);
    retval = drv_hsuart_read_fifo (Buffer, 32, HSUART_NON_BLOCKING);
    retval = drv_hsuart_write_fifo (Buffer, retval, HSUART_BLOCKING);
    printf ("received length: %ld\n", retval);
    printf ("\n");
}

static void interrupt (int32_t argc, char *argv[])
{
    int32_t         BaudRate;
    HSUART_WLS_E    DataBits;
    HSUART_STB_E    StopBits;
    HSUART_PARITY_E Parity;

    int32_t         retval;

    if (argc == 5) {
        BaudRate    = strtol (argv[1], NULL, 10);
        DataBits    = strtol (argv[2], NULL, 10);
        StopBits    = strtol (argv[3], NULL, 10);
        Parity      = strtol (argv[4], NULL, 10);

    } else {
        printf ("\nUsage: int [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the High Speed UART.\n");
        printf ("                       Contains at least the [1, 5000000] range\n");
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

    if (((BaudRate < 1) || (BaudRate > 5000000)) ||
        ((DataBits < 0) || (DataBits > 3)) ||
        ((StopBits < 0) || (StopBits > 1)) ||
        ((Parity < 0) || (Parity == 2) || (Parity == 4) || (Parity == 6) || (Parity > 7))) {
        printf ("\nUsage: int [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the High Speed UART.\n");
        printf ("                       Contains at least the [1, 5000000] range\n");
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

    printf("hsuart receiver data ready interrupt test program (Build: %s %s)\n", __DATE__, __TIME__);

    drv_hsuart_init ();
    drv_hsuart_sw_rst ();
    drv_hsuart_set_fifo (HSUART_INT_RX_FIFO_TRIG_LV_16);
    drv_hsuart_set_hardware_flow_control (16, 24);
    retval = drv_hsuart_set_format (BaudRate, DataBits, StopBits, Parity);
    drv_hsuart_register_isr (HSUART_RX_DATA_READY_IE, hsuart_isr);

    printf ("hsuart wait for receiver data ready interrupt event...\n");
}

static void loopback (int32_t argc, char *argv[])
{
    int32_t         BaudRate;
    HSUART_WLS_E    DataBits;
    HSUART_STB_E    StopBits;
    HSUART_PARITY_E Parity;

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
        printf ("BaudRate               The baud rate of the High Speed UART.\n");
        printf ("                       Contains at least the [1, 5000000] range\n");
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

    if (((BaudRate < 1) || (BaudRate > 5000000)) ||
        ((DataBits < 0) || (DataBits > 3)) ||
        ((StopBits < 0) || (StopBits > 1)) ||
        ((Parity < 0) || (Parity == 2) || (Parity == 4) || (Parity == 6) || (Parity > 7))) {
        printf ("\nUsage: loopback [BaudRate] [DataBits] [StopBits] [Parity]\n");
        printf ("Options:\n");
        printf ("BaudRate               The baud rate of the High Speed UART.\n");
        printf ("                       Contains at least the [1, 5000000] range\n");
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

    printf ("hsuart loopback test program (Build: %s %s)\n", __DATE__, __TIME__);

    drv_hsuart_init ();
    drv_hsuart_sw_rst ();
    drv_hsuart_set_fifo (HSUART_INT_RX_FIFO_TRIG_LV_16);
    drv_hsuart_set_hardware_flow_control (16, 24);
    retval = drv_hsuart_set_format (BaudRate, DataBits, StopBits, Parity);

    while (1) {
        retval = drv_hsuart_read_fifo (&Buffer, 1, HSUART_BLOCKING);
        retval = drv_hsuart_write_fifo (&Buffer, 1, HSUART_BLOCKING);
    }
}

static void help (int32_t argc, char *argv[])
{
    printf ("Usage: loopback [BaudRate] [DataBits] [StopBits] [Parity]\n");
    printf ("Options:\n");
    printf ("BaudRate               The baud rate of the High Speed UART.\n");
    printf ("                       Contains at least the [1, 5000000] range\n");
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
    printf ("BaudRate               The baud rate of the High Speed UART.\n");
    printf ("                       Contains at least the [1, 5000000] range\n");
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
    {"help",            help,               "hsuart help"},
    {"loopback",        loopback,           "hsuart loopback test"},
    {"int",             interrupt,          "hsuart interrupt test"},
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};
