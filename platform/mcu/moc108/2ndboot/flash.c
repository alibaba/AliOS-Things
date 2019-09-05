/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "flash.h"
#include "aos/hal/flash.h"
#include "2ndboot.h"

extern const hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;

hal_logic_partition_t *flash_get_info(hal_partition_t pno)
{
    if(pno >= hal_partitions_amount)
    {
        return NULL;
    }
    return &hal_partitions[pno];
}

static void flash_clr_qwfr(void)
{
    unsigned int value;

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(MODEL_SEL_MASK << MODEL_SEL_POSI);
    REG_WRITE(REG_FLASH_CONF, value);

    value = REG_READ(REG_FLASH_OPERATE_SW);
    value = ((0 << ADDR_SW_REG_POSI)
             | (FLASH_OPCODE_CRMR << OP_TYPE_SW_POSI)
             | OP_SW
             | (value & WP_VALUE));
    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
}

static void flash_set_qwfr(void)
{
    unsigned int value;

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(MODEL_SEL_MASK << MODEL_SEL_POSI);
    value |= (0x0A << MODEL_SEL_POSI);
    REG_WRITE(REG_FLASH_CONF, value);
}

static void flash_set_line_mode(unsigned char mode)
{
    if(1 == mode) {
        flash_clr_qwfr();
    }
    if(2 == mode) {
        unsigned int value;

        value = REG_READ(REG_FLASH_CONF);
        value &= ~(MODEL_SEL_MASK << MODEL_SEL_POSI);
        REG_WRITE(REG_FLASH_CONF, value);

        value |= ((MODE_DUAL & MODEL_SEL_MASK) << MODEL_SEL_POSI);
        REG_WRITE(REG_FLASH_CONF, value);
    } else if(4 == mode) {
        flash_set_qwfr();
    }
}

static void flash_erase_sector(unsigned int address)
{
    unsigned int value;
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    flash_set_line_mode(0);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    value = REG_READ(REG_FLASH_OPERATE_SW);
    value = ((address << ADDR_SW_REG_POSI)
             | (FLASH_OPCODE_SE << OP_TYPE_SW_POSI)
             | OP_SW
             | (value & WP_VALUE));
    REG_WRITE(REG_FLASH_OPERATE_SW, value);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    flash_set_line_mode(2);
}

int flash_read_data(unsigned int address, unsigned char *buffer, unsigned int len)
{
    int ret = 0;
    unsigned int i, reg_value;
    unsigned int addr = address & (~(FLASH_WRITE_UNIT-1));
    unsigned int buf[FLASH_WRITE_UNIT/4];
    unsigned char *pb = (unsigned char *)&buf[0];
    if(len == 0) {
        return -1;
    }
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    while(len) {
        reg_value = REG_READ(REG_FLASH_OPERATE_SW);
        reg_value = ((addr << ADDR_SW_REG_POSI)
                     | (FLASH_OPCODE_READ << OP_TYPE_SW_POSI)
                     | OP_SW
                     | (reg_value & WP_VALUE));
        REG_WRITE(REG_FLASH_OPERATE_SW, reg_value);
        while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
        addr += FLASH_WRITE_UNIT;

        for(i = 0; i < 8; i++) {
            buf[i] = REG_READ(REG_FLASH_DATA_FLASH_SW);
        }

        for(i = address % FLASH_WRITE_UNIT; i < FLASH_WRITE_UNIT; i++) {
            *buffer++ = pb[i];
            address++;
            len--;
            if(len == 0) {
                break;
            }
        }
    }
    return ret;
}

int flash_write_data(unsigned int address, unsigned char *buffer, unsigned int len)
{
    int ret = 0;
    unsigned int i, reg_value;
    unsigned int addr = address & (~(FLASH_WRITE_UNIT-1));
    unsigned int buf[FLASH_WRITE_UNIT/4];
    unsigned char *pb = (unsigned char *)&buf[0];

    if(address % FLASH_WRITE_UNIT) {
        flash_read_data(pb, addr, FLASH_WRITE_UNIT);
    }
    else {
        memset(pb, 0xFF, FLASH_WRITE_UNIT);
    }

    flash_set_line_mode(1);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    while(len) {
        for (i = address % FLASH_WRITE_UNIT; i < FLASH_WRITE_UNIT; i++) {
            pb[i] = *buffer++;
            address++;
            len--;
            if (len == 0)
                break;
        }

        for (i = 0; i < FLASH_WRITE_UNIT/4; i++) {
            REG_WRITE(REG_FLASH_DATA_SW_FLASH, buf[i]);
        }

        reg_value = REG_READ(REG_FLASH_OPERATE_SW);
        reg_value = ((addr << ADDR_SW_REG_POSI)
                     | (FLASH_OPCODE_PP << OP_TYPE_SW_POSI)
                     | OP_SW
                     | (reg_value & WP_VALUE));
        REG_WRITE(REG_FLASH_OPERATE_SW, reg_value);
        while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
        addr += FLASH_WRITE_UNIT;
        memset(pb, 0xFF, FLASH_WRITE_UNIT);
    }

    flash_set_line_mode(2);
    return ret;
}

int flash_erase(unsigned int offset,  unsigned int size)
{
    int ret = 0;
    unsigned int addr;
    unsigned int start_addr, end_addr;

    start_addr = ROUND_DOWN(offset, SECTOR_SIZE);
    end_addr = ROUND_DOWN((offset + size - 1), SECTOR_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE) {
        flash_erase_sector(addr);
    }
    return ret;
}
