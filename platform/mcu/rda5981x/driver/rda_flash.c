#include "rda_def.h"
#include "rda_flash.h"

r_u32 flash_size = FLASH_SIZE;

int rda5981_read_flash(r_u32 addr, char *buf, r_u32 len)
{
    int ret = 0;
    char *temp_buf = NULL, *temp_buf_aligned;

    addr &= (flash_size -1);
    if (addr < RDA5991H_PARTITION_TABLE_END_ADDR-0x18000000) {
        printf(" couldn't read system data\n");
        return -1;
    }

    if ((r_u32)buf % 4) {
        temp_buf = r_malloc(len + 3);
        if (temp_buf == NULL) {
            ret = -1;
            goto out;
        }
        if ((r_u32)temp_buf % 4)
            temp_buf_aligned = temp_buf + (4-(r_u32)temp_buf%4);
        else
            temp_buf_aligned = temp_buf;

    } else {
        temp_buf_aligned = buf;
    }

    core_util_critical_section_enter();
    //RDA5991H_READ_FLASH(addr, temp_buf_aligned, len);
    spi_flash_flush_cache();
    SPI_FLASH_READ_DATA_FOR_MBED((void *)addr, temp_buf_aligned, len);
    core_util_critical_section_exit();

    if (temp_buf_aligned != buf)
        r_memcpy(buf, temp_buf_aligned, len);

out:
    if (temp_buf)
        r_free(temp_buf);

    return ret;
}

void rda5981_spi_flash_erase_64KB_block(r_u32 addr)
{
    if (rda_ccfg_hwver() < 4) {
        spi_wip_reset();
        spi_write_reset();
        WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_64KB_BLOCK_ERASE | (addr<<8));
        wait_busy_down();
        spi_wip_reset();
    } else {
        spi_wip_reset_4();
        spi_write_reset_4();
        WRITE_REG32(FLASH_CTL_TX_CMD_ADDR_REG, CMD_64KB_BLOCK_ERASE | (addr<<8));
        wait_busy_down_4();
        spi_wip_reset_4();
    }
}
void rda5981_spi_erase_partition(void *src, r_u32 counts)
{
    r_u32 a4k, a64k, a64kend, a4kend, atmp;

    if (counts > 0x00) {
        a4k  = ((r_u32)src                          ) & (~((0x01UL << 12) - 0x01UL));
        a64k = ((r_u32)src + (0x01UL << 16) - 0x01UL) & (~((0x01UL << 16) - 0x01UL));
        a64kend = ((r_u32)src + counts                          ) & (~((0x01UL << 16) - 0x01UL));
        a4kend  = ((r_u32)src + counts + (0x01UL << 12) - 0x01UL) & (~((0x01UL << 12) - 0x01UL));

        for (atmp = a4k; atmp < a4kend; atmp += (0x01UL << 12)) {
            if (a64kend > a64k) {
                if (atmp == a64k) {
                    for (; atmp < a64kend; atmp += (0x01UL << 16)) {
                        core_util_critical_section_enter();
                        rda5981_spi_flash_erase_64KB_block(atmp);
                        core_util_critical_section_exit();
                    }
                    if (atmp == a4kend)
                        break;
                }
            }
            core_util_critical_section_enter();
            rda5981_spi_flash_erase_4KB_sector(atmp);
            core_util_critical_section_exit();
        }
    }
}
//@len must be 4k aligment
int rda5981_erase_flash(r_u32 addr, r_u32 len)
{
    //printf( "erase flash :%x:%x\n", addr, len);

    addr &= (flash_size - 1);
    if (addr < RDA5991H_PARTITION_TABLE_END_ADDR-0x18000000) {
        printf( "couldn't erase system data\n");
        return -1;
    }

    //addr &= (flash_size -1);

    if (len & (SECTOR_SIZE-1)) {
        len = ((len+SECTOR_SIZE) & (~(SECTOR_SIZE-1)));
    }

    //core_util_critical_section_enter();
    rda5981_spi_erase_partition((void*)addr, len);
    //core_util_critical_section_exit();
    return 0;
}

//@len must be 256 aligment
int rda5981_write_flash(r_u32 addr, char *buf, r_u32 len)
{
    int ret = 0;
    r_u8 *temp_buf = NULL, *temp_buf_aligned;
    r_u8 *check_buf = NULL;
    addr &= (flash_size -1);
    if (addr < RDA5991H_PARTITION_TABLE_END_ADDR-0x18000000) {
        printf( "couldn't write system data\n");
        return -1;
    }

    if ((r_u32)buf % 4) {
        temp_buf = r_malloc(len + 3);
        if (temp_buf == NULL) {
            goto out;
        }
        if ((r_u32)temp_buf % 4)
            temp_buf_aligned = temp_buf + (4-(r_u32)temp_buf%4);
        else
            temp_buf_aligned = temp_buf;
        r_memcpy(temp_buf_aligned, buf, len);
    } else {
        temp_buf_aligned = (r_u8*)buf;
    }
    core_util_critical_section_enter();
    //RDA5991H_ERASE_FLASH(addr, len);
    RDA5991H_WRITE_FLASH(addr, temp_buf_aligned, len);
    core_util_critical_section_exit();

#ifdef FLASH_READ_CHECK
    if (ret == 0) {
        check_buf = r_malloc(len);
        rda5981_read_flash(addr, check_buf, len);
        if (r_memcmp(buf, check_buf, len) == 0)
            ret = 0;
        else
            ret = -1;
    }
#endif /*FLASH_READ_CHECK*/

out:
    if (temp_buf)
        r_free(temp_buf);
    if (check_buf)
        r_free(check_buf);
    return ret;
}

int rda5981_set_flash_size(const r_u32 size)
{

    if (size == 0x100000 ||
        size == 0x200000 ||
        size == 0x400000) {
        flash_size = size;
        return 0;
    }

    return -1;
}


