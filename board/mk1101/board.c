
#include <aos/kernel.h>

/* Logic partition on flash devices */
const hal_logic_partition_t hal_partitions[] =
{
  [HAL_PARTITION_BOOTLOADER] =
  {
    .partition_owner           = HAL_FLASH_SPI,
    .partition_description     = "Bootloader",
    .partition_start_addr      = 0x0,
    .partition_length          = 0xA000,    //40k bytes + 4k bytes empty space
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [HAL_PARTITION_APPLICATION] =
  {
    .partition_owner           = HAL_FLASH_SPI,
    .partition_description     = "Application",
    .partition_start_addr      = 0xB000,
    .partition_length          = 0xC0000,   //768k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [HAL_PARTITION_ATE] =
  {
    .partition_owner           = HAL_FLASH_SPI,
    .partition_description     = "ATE",
    .partition_start_addr      = 0xCB000,
    .partition_length          = 0x50000,  //320k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
  },
  [HAL_PARTITION_RF_FIRMWARE] =
  {
    .partition_owner           = HAL_FLASH_NONE,
    .partition_description     = "RF Firmware",
    .partition_start_addr      = 0x0,
    .partition_length          = 0x0, 
    .partition_options         = PAR_OPT_READ_DIS | PAR_OPT_WRITE_DIS,
  },
  [HAL_PARTITION_OTA_TEMP] =
  {
    .partition_owner           = HAL_FLASH_SPI,
    .partition_description     = "OTAStorage",
    .partition_start_addr      = 0x11B000,
    .partition_length          = 0xC0000, //768k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
  [HAL_PARTITION_PARAMETER_1] =
  {
    .partition_owner           = HAL_FLASH_SPI,
    .partition_description     = "PARAMETER1",
    .partition_start_addr      = 0x1DB000,
    .partition_length          = 0x2000, // 4k bytes
    .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
  },
};

void board_init(void)
{
}
