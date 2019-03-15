#include "aos/hal/flash.h"
#include "aos/hal/uart.h"
#include <aos/kernel.h>
#include "hfilop/hfilop.h"

const char *aos_get_app_version(void)
{
    return (const char *)SYSINFO_APP_VERSION;
}

int hal_ota_write(void *m, volatile uint32_t *off_set, uint8_t *in_buf , uint32_t in_buf_len)
{
    extern int ota_hal_write(int *off_set, char *in_buf , int in_buf_len);
    return ota_hal_write(off_set, in_buf , in_buf_len);
}

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
#if 0
    [HAL_PARTITION_BOOTLOADER] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Bootloader",
        .partition_start_addr       = 0x18001000,
        .partition_length           = 0x3000,    //12k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
    [HAL_PARTITION_APPLICATION] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "Application",
        .partition_start_addr       = 0x18004000,
        .partition_length           = 0x91000,//580K bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#if 1
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "OTA Storage",
        .partition_start_addr       = 0x18095000,
        .partition_length           = 0x5D000,//372K bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
#endif
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER1",
        .partition_start_addr       = 0x180F7000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x180F8000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_3] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER3",
        .partition_start_addr       = 0x180FA000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER4",
        .partition_start_addr       = 0x180FB000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_SYS_DATA] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "SYS RF Data",
        .partition_start_addr       = 0x180FC000,
        .partition_length           = 0x1000,
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_HFILOP] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "HFILOP",
        .partition_start_addr       = 0x180FD000,
        .partition_length           = 0x2000, //8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};
extern uart_dev_t uart_0;
/*
uart_dev_t uart_0 = {
    0,
    {921600, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},
    NULL,
};
*/
void board_init(void)
{
    hal_uart_init(&uart_0);
}

void board_init_later(void)
{
    hfilop_init_rf_type(MODULE_TYPE);
    hfilop_assis_task_start();
    hfilop_check_ota_state();
    printf("====board_init_later done===\r\n");
}
