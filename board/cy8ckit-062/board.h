#define HARDWARE_REVISION   "V1.0"
#define MODEL               "PSOC6"

#ifdef BOOTLOADER
#define STDIO_UART 5
#define STDIO_UART_BAUDRATE 115200
#else
#define STDIO_UART 5
#define STDIO_UART_BAUDRATE 115200
#endif
