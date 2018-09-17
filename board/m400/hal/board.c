#include "hal/soc/soc.h"
#include <aos/kernel.h>

/* Logic partition on flash devices */
hal_logic_partition_t hal_partitions[HAL_PARTITION_MAX] =
{
    [HAL_PARTITION_PARAMETER_2] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "PARAMETER2",
        .partition_start_addr       = 0x801A000,
        .partition_length           = 0x2000,  // 8k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

void board_init(void)
{
}
