#define HARDWARE_REVISION   "V1.0"
//#define MODEL               "EFM32G"

#ifdef BOOTLOADER
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 115200
#else
#define STDIO_UART 0
#define STDIO_UART_BUADRATE 115200

/* !
 *  * Begins critical section
 *   */
#define CRITICAL_SECTION_BEGIN( ) uint32_t mask; BoardCriticalSectionBegin( &mask )

/* !
 *  * Ends critical section
 *   */
#define CRITICAL_SECTION_END( ) BoardCriticalSectionEnd( &mask )

#endif
