#ifndef _WLAND_FLASH_H_
#define _WLAND_FLASH_H_
//#include "wland_types.h"
// #include "rda5981_flash.h"
#include "rda_ccfg_api.h"
#include "rda_def.h"

//#define FLASH_READ_CHECK

#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif /*min*/


#define SECTOR_SIZE     4096
#define FLASH_SIZE 0x100000

#define RDA5991H_PARTITION_TABLE_END_ADDR 0x18001000 //partition table end addr

#define FLASH_CTL_REG_BASE 0x17fff000
#define FLASH_CTL_TX_CMD_ADDR_REG (FLASH_CTL_REG_BASE + 0x00)
#define CMD_64KB_BLOCK_ERASE (0x000000d8UL)
#define WRITE_REG32(REG, VAL)    ((*(volatile unsigned int*)(REG)) = (unsigned int)(VAL))

#define FLASH_ERASE_FUN_ADDR 0x21f1//smartlink_erase_for_mbed
#define FLASH_WRITE_FUN_ADDR 0x2241//smartlink_write_for_mbed
#define FLASH_READ_FUN_ADDR 0x2243//smartlink_read_for_mbed
#define FLASH_ERASE_PARTITION_FUN_ADDR 0x2139//spi_flash_erase_partition
#define SPI_FLASH_READ_DATA_FOR_MBED_ADDR 0x2007//spi_flash_read_data_for_mbed
#define spi_flash_disable_cache_addr 0x1eb7//spi_flash_disable_cache
#define spi_flash_flush_cache_addr 0x1ecd//spi_flash_flush_cache
#define spi_flash_cfg_cache_addr 0x1e9f//spi_flash_cfg_cache
#define spi_flash_erase_4KB_sector_addr 0x23a3
#define spi_wip_reset_addr 0x1d8b
#define spi_write_reset_addr 0x1d9f
#define wait_busy_down_addr 0x1d81

#define FLASH_ERASE_FUN_ADDR_4 0x2221//smartlink_erase_for_mbed
#define FLASH_WRITE_FUN_ADDR_4 0x2271//smartlink_write_for_mbed
#define FLASH_READ_FUN_ADDR_4 0x2273//smartlink_read_for_mbed
#define FLASH_ERASE_PARTITION_FUN_ADDR_4 0x2169//spi_flash_erase_partition
#define SPI_FLASH_READ_DATA_FOR_MBED_ADDR_4 0x2037//spi_flash_read_data_for_mbed
#define spi_flash_disable_cache_addr_4 0x1ee7//spi_flash_disable_cache
#define spi_flash_flush_cache_addr_4 0x1efd//spi_flash_flush_cache
#define spi_flash_cfg_cache_addr_4 0x1ecf//spi_flash_cfg_cache
#define spi_flash_erase_4KB_sector_addr_4 0x23d3
#define spi_wip_reset_addr_4 0x1dbb
#define spi_write_reset_addr_4 0x1dcf
#define wait_busy_down_addr_4 0x1db1

/*
 * return 0 if find
 */
void rda5981_spi_erase_partition(void *src, r_u32 counts);

static inline void wait_busy_down(void)
{
    ((void(*)(void))wait_busy_down_addr)();
}

static inline void spi_write_reset(void)
{
    ((void(*)(void))spi_write_reset_addr)();
}

static inline void spi_wip_reset(void)
{
    ((void(*)(void))spi_wip_reset_addr)();
}

static inline void wait_busy_down_4(void)
{
    ((void(*)(void))wait_busy_down_addr_4)();
}

static inline void spi_write_reset_4(void)
{
    ((void(*)(void))spi_write_reset_addr_4)();
}

static inline void spi_wip_reset_4(void)
{
    ((void(*)(void))spi_wip_reset_addr_4)();
}

static inline void spi_flash_enable_cache(void)
{
    unsigned int func = spi_flash_cfg_cache_addr;
    if (rda_ccfg_hwver() >= 4) {
        func = spi_flash_cfg_cache_addr_4;
    }
    ((void(*)(void))func)();
}

static inline void spi_flash_disable_cache(void)
{
    unsigned int func = spi_flash_disable_cache_addr;
    if (rda_ccfg_hwver() >= 4) {
        func = spi_flash_disable_cache_addr_4;
    }
    ((void(*)(void))func)();
}

static inline void spi_flash_flush_cache(void)
{
    unsigned int func = spi_flash_flush_cache_addr;
    if (rda_ccfg_hwver() >= 4) {
        func = spi_flash_flush_cache_addr_4;
    }
    ((void(*)(void))func)();
}

static inline void rda5981_spi_flash_erase_4KB_sector(r_u32 addr)
{
    unsigned int func = spi_flash_erase_4KB_sector_addr;
    if (rda_ccfg_hwver() >= 4) {
        func = spi_flash_erase_4KB_sector_addr_4;
    }
    ((void(*)(r_u32))func)(addr);
}

static inline void RDA5991H_ERASE_FLASH(void *addr, r_u32 len)
{
    unsigned int func = FLASH_ERASE_FUN_ADDR;
    if (rda_ccfg_hwver() >= 4) {
        func = FLASH_ERASE_FUN_ADDR_4;
    }
    ((void(*)(void *, r_u32))func)(addr, len);
}

static inline void RDA5991H_WRITE_FLASH(r_u32 addr, r_u8 *data, r_u32 len)
{
    unsigned int func = FLASH_WRITE_FUN_ADDR;
    if (rda_ccfg_hwver() >= 4) {
        //printf("rda_ccfg_hw > 4 \r\n");
        func = FLASH_WRITE_FUN_ADDR_4;
    }
    ((void(*)(r_u32, r_u8 *, r_u32))func)(addr, data, len);
}

static inline void RDA5991H_READ_FLASH(r_u32 addr, r_u8 *buf, r_u32 len)
{
    unsigned int func = FLASH_READ_FUN_ADDR;
    if (rda_ccfg_hwver() >= 4) {
        func = FLASH_READ_FUN_ADDR_4;
    }
    ((void(*)(r_u32, r_u8 *, r_u32))func)(addr, buf, len);
}

static inline void SPI_FLASH_READ_DATA_FOR_MBED(void *addr, void *buf, r_u32 len)
{
    unsigned int func = SPI_FLASH_READ_DATA_FOR_MBED_ADDR;
    if (rda_ccfg_hwver() >= 4) {
        func = SPI_FLASH_READ_DATA_FOR_MBED_ADDR_4;
    }
    ((void(*)(void *, void *, r_u32))func)(buf, addr, len);
}

#endif /*_WLAND_FLASH_H_*/

