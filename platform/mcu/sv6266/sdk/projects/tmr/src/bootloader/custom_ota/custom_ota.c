#define M_FLASH_START_ADDR          (0x30000000)
#define M_FLASH_INIT_ADDR           (M_FLASH_START_ADDR+0x00008000)

#include <stdint.h>
#include "hal_gpio.h"
#include "hal_pinmux.h"

#include "ota_flash.h"
#include "tiny_printf.h"

#include "serial.h"

#undef FLASH_TEST_CODE
#undef GPIO_TEST_CODE
#undef UART1_TEST_CODE

#if defined(FLASH_TEST_CODE)
char __flash_testing[]="123123123";
#endif
// System will jump to the return address.
unsigned int custom_boot_addr() {
    // flash test example code.
    int long_wait;
#if defined(FLASH_TEST_CODE)
    long_wait=10000;
    ota_flash_init();
    ota_flash_sector_erase(0xfa000);
    ota_flash_page_program(0xfa000, 10, (unsigned char *)__flash_testing);
    tiny_printf("test ='%s'\n", M_FLASH_START_ADDR+0xfa000);
    while(long_wait--);
#endif

#if defined(GPIO_TEST_CODE)
    long_wait=10000;
    hal_gpio_set_mode(GPIO_13, PIN_MODE_GPIO);
    hal_gpio_set_dir(GPIO_13, GPIO_DIR_IN);
    while(long_wait--);
    gpio_logic_t logic = hal_gpio_get_logic(GPIO_13);

    tiny_printf_display(1);
    if (logic == GPIO_LOGIC_HIGH) {
        tiny_printf("gpio 13 is HIGH\n");
    } else {
        tiny_printf("gpio 13 is LOW\n");
    }
#endif

#if defined(UART1_TEST_CODE)
    serial_uart1_init(921600);
    serial_uart1_tx('u');
    serial_uart1_tx('a');
    serial_uart1_tx('r');
    serial_uart1_tx('t');
    serial_uart1_tx('1');
    serial_uart1_tx('_');
    serial_uart1_tx('t');
    serial_uart1_tx('e');
    serial_uart1_tx('s');
    serial_uart1_tx('t');
    serial_uart1_tx('\r');
    serial_uart1_tx('\n');
    while(!serial_uart1_rx_ready());
    int get_rx_data;
    get_rx_data = serial_uart1_rx();
    serial_uart1_tx('\'');
    serial_uart1_tx(get_rx_data);
    serial_uart1_tx('\'');
#endif
    return M_FLASH_INIT_ADDR;
}

