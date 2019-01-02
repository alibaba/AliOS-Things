/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "rec_flash.h"

/* Logic partition on flash devices */
const hal_logic_partition_t rec_hal_partitions[] =
{
    [HAL_PARTITION_PARAMETER_1] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "", //PARAMETER1
        .partition_start_addr       = 0x10000,
        .partition_length           = 0x1000, // 4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_RECOVERY] =
    {
	.partition_owner            = HAL_FLASH_EMBEDDED,
	.partition_description      = "", //Recovery
	.partition_start_addr       = 0x13000,
	.partition_length           = 0x5000,  //28k bytes
	.partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
	[HAL_PARTITION_APPLICATION] =
	{
	    .partition_owner            = HAL_FLASH_EMBEDDED,
	    .partition_description      = "", //Application
	    .partition_start_addr       = 0x18000,
	    .partition_length           = 0xE8000, //928k bytes
	    .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
	},
    [HAL_PARTITION_OTA_TEMP] =
    {
        .partition_owner           = HAL_FLASH_EMBEDDED,
        .partition_description     = "", //OTA Storage
        .partition_start_addr      = 0x100000,
        .partition_length          = 0xE8000, //928k bytes
        .partition_options         = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [HAL_PARTITION_PARAMETER_4] =
    {
        .partition_owner            = HAL_FLASH_EMBEDDED,
        .partition_description      = "", //PARAMETER4
        .partition_start_addr       = 0xD000,
        .partition_length           = 0x1000, //4k bytes
        .partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
};

static void flash_clr_qwfr(void)
{
    UINT32 value;

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
    UINT32 value;

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(MODEL_SEL_MASK << MODEL_SEL_POSI);
    value |= (0x0A << MODEL_SEL_POSI);
    REG_WRITE(REG_FLASH_CONF, value);
}

static void flash_set_line_mode(UINT8 mode)
{
    if(1 == mode)
    {
        flash_clr_qwfr();
    }
    if(2 == mode)
    {
        UINT32 value;

        value = REG_READ(REG_FLASH_CONF);
        value &= ~(MODEL_SEL_MASK << MODEL_SEL_POSI);
        REG_WRITE(REG_FLASH_CONF, value);

        value |= ((MODE_DUAL & MODEL_SEL_MASK) << MODEL_SEL_POSI);
        REG_WRITE(REG_FLASH_CONF, value);
    }
    else if(4 == mode)
    {
        flash_set_qwfr();
    }
}

static void flash_erase_sector(UINT32 address)
{
    UINT32 value;
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

static void flash_read_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    UINT32 i, reg_value;
    UINT32 addr = address & (~(FLASH_WRITE_UNIT-1));
    UINT32 buf[FLASH_WRITE_UNIT/4];
    UINT8 *pb = (UINT8 *)&buf[0];

    if(len == 0)
    {
        return;
    }

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    while(len)
    {
        reg_value = REG_READ(REG_FLASH_OPERATE_SW);
        reg_value = ((addr << ADDR_SW_REG_POSI)
                     | (FLASH_OPCODE_READ << OP_TYPE_SW_POSI)
                     | OP_SW
                     | (reg_value & WP_VALUE));
        REG_WRITE(REG_FLASH_OPERATE_SW, reg_value);
        while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
        addr += FLASH_WRITE_UNIT;

        for(i = 0; i < 8; i++)
        {
            buf[i] = REG_READ(REG_FLASH_DATA_FLASH_SW);
        }

        for(i = address % FLASH_WRITE_UNIT; i < FLASH_WRITE_UNIT; i++)
        {
            *buffer++ = pb[i];
            address++;
            len--;
            if(len == 0)
            {
                break;
            }
        }
    }
}

static void flash_write_data(UINT8 *buffer, UINT32 address, UINT32 len)
{
    UINT32 i, reg_value;
    UINT32 addr = address & (~(FLASH_WRITE_UNIT-1));
    UINT32 buf[FLASH_WRITE_UNIT/4];
    UINT8 *pb = (UINT8 *)&buf[0];

    if(address % FLASH_WRITE_UNIT)
    {
        flash_read_data(pb, addr, FLASH_WRITE_UNIT);
    }
    else
    {
        memset(pb, 0xFF, FLASH_WRITE_UNIT);
    }

    flash_set_line_mode(1);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    while(len)
    {
        for (i = address % FLASH_WRITE_UNIT; i < FLASH_WRITE_UNIT; i++)
        {
            pb[i] = *buffer++;
            address++;
            len--;
            if (len == 0)
                break;
        }

        for (i = 0; i < FLASH_WRITE_UNIT/4; i++)
        {
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
}

/* address convert to offset in flash rom */
unsigned long rec_flash_addr2ofst(unsigned long addr)
{
    if ( addr % FLASH_WRITE_UNIT )
    {
        return NULL;
    }
    return addr*34/32;
}

void rec_flash_init(void)
{
    //flash has init in boot code;
}

/* offset means physical address */
void rec_flash_erase(unsigned long offset)
{
    offset &= ~(FLASH_SECTOR_SIZE-1);
    flash_erase_sector(offset);
}

/* offset means physical address */
void rec_flash_read_data(unsigned char *buffer, unsigned long offset, unsigned long len)
{
    flash_read_data(buffer, offset, len);
}

/* offset means physical address */
void rec_flash_write_data(unsigned char *buffer, unsigned long offset, unsigned long len)
{
    flash_write_data(buffer, offset, len);
}

hal_logic_partition_t *rec_flash_get_info(hal_partition_t pno)
{
    if(pno >= sizeof(rec_hal_partitions)/sizeof(hal_logic_partition_t))
    {
        return NULL;
    }
    return &rec_hal_partitions[pno];
}
