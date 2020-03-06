
#include "bootloader.h"
#include "include.h"
#include "arm_arch.h"
#include "sys_config.h"
#include "flash.h"
#include "sys_ctrl.h"
#include "flash_pub.h"
#include "drv_model_pub.h"
#include "uart_pub.h"
#include "sys_ctrl_pub.h"

#define os_printf  printf

#define MODE_STD     0
#define MODE_DUAL    1
#define MODE_QUAD    2

extern const hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;


static const flash_config_t flash_config[] =
{
    {0x1C7016, 1, 2,  0, 2, 0x1F, 0x1F, 0x00, 0x16, 0x01B, 0, 0, 0xA5, 0x01}, //en_25qh32b
    {0x0B4014, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0C, 0x101, 9, 1, 0xA0, 0x01}, //xtx_25f08b
    {0x0B4015, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0D, 0x101, 9, 1, 0xA0, 0x01}, //xtx_25f16b
    {0x0B4016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0E, 0x101, 9, 1, 0xA0, 0x01}, //xtx_25f32b
    {0x0E4016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0E, 0x101, 9, 1, 0xA0, 0x01}, //xtx_FT25H32
    {0xC84015, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0D, 0x101, 9, 1, 0xA0, 0x01}, //gd_25q16c
    {0xC84016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0E, 0x101, 9, 1, 0xA0, 0x01}, //gd_25q32c
    {0xEF4016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x00, 0x101, 9, 1, 0xA0, 0x01}, //w_25q32(bfj)
    {0x204016, 2, 2, 14, 2, 0x1F, 0x1F, 0x00, 0x0E, 0x101, 9, 1, 0xA0, 0x01}, //xmc_25qh32b
    {0xC22315, 1, 2,  0, 2, 0x0F, 0x0F, 0x00, 0x05, 0x005, 6, 1, 0xA5, 0x01}, //mx_25v16b
    {0x000000, 2, 2,  0, 2, 0x1F, 0x00, 0x00, 0x00, 0x000, 0, 0, 0x00, 0x01}, //default
};

static const flash_config_t *flash_current_config = NULL;

static UINT32 flash_id;
static DD_OPERATIONS flash_op =
{
    NULL,
    NULL,
    flash_read,
    flash_write,
    flash_ctrl
};

static void flash_get_current_flash_config(void)
{
    int i;
    for(i = 0; i < (sizeof(flash_config) / sizeof(flash_config_t) - 1); i++)
    {
        if(flash_id == flash_config[i].flash_id)
        {
            flash_current_config = &flash_config[i];
            break;
        }
    }
    if(i == (sizeof(flash_config) / sizeof(flash_config_t) - 1))
    {
        flash_current_config = &flash_config[i];
        os_printf("don't config this flash, choose default config\r\n");
    }
}

static void flash_set_clk(UINT8 clk_conf)
{
    UINT32 value;

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(FLASH_CLK_CONF_MASK << FLASH_CLK_CONF_POSI);
    value |= (clk_conf << FLASH_CLK_CONF_POSI);
    REG_WRITE(REG_FLASH_CONF, value);
}

static void flash_write_enable(void)
{
    UINT32 value;

    value = (FLASH_OPCODE_WREN << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
}

static void flash_write_disable(void)
{
    UINT32 value;

    value = (FLASH_OPCODE_WRDI << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
}


static UINT16 flash_read_sr(UINT8 sr_width)
{
	UINT16 sr;
    UINT32 value;

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    value = (FLASH_OPCODE_RDSR << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    value = REG_READ(REG_FLASH_SR_DATA_CRC_CNT);
    sr = value & 0x00FF;

	if(sr_width == 2)
	{
	    value = (FLASH_OPCODE_RDSR2 << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
	    REG_WRITE(REG_FLASH_OPERATE_SW, value);
	    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

	    value = REG_READ(REG_FLASH_SR_DATA_CRC_CNT);
	    sr |= (value & 0x00FF) << 8;
	}
	//os_printf("--read sr:%x--\r\n",sr);
    return sr;
}

static void flash_write_sr(UINT8 sr_width,  UINT16 val)
{
    UINT32 value;
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(WRSR_DATA_MASK << WRSR_DATA_POSI);

    value |= (val << WRSR_DATA_POSI);

    REG_WRITE(REG_FLASH_CONF, value);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    if(sr_width == 1)
    {
        value = (FLASH_OPCODE_WRSR << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
        REG_WRITE(REG_FLASH_OPERATE_SW, value);
    }
    else if(sr_width == 2)
    {
        value = (FLASH_OPCODE_WRSR2 << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
        REG_WRITE(REG_FLASH_OPERATE_SW, value);
    }

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
}

static UINT8 flash_read_qe(void)
{
    UINT8 temp;
    UINT32 value;

    if(1 == flash_current_config->sr_size)
    {
        value = (FLASH_OPCODE_RDSR << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
        REG_WRITE(REG_FLASH_OPERATE_SW, value);
    }
    else
    {
        value = (FLASH_OPCODE_RDSR2 << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
        REG_WRITE(REG_FLASH_OPERATE_SW, value);
    }

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    value = REG_READ(REG_FLASH_SR_DATA_CRC_CNT);
    temp = (value & 0xFF);
    return temp;
}

static void flash_set_qe(void)
{
    UINT32 value, param;

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    param = flash_read_sr(flash_current_config->sr_size);
    if((param & (flash_current_config->qe_bit << flash_current_config->qe_bit_post)))
    {
        return ;
    }
    value = REG_READ(REG_FLASH_CONF);
    value &= ~(WRSR_DATA_MASK << WRSR_DATA_POSI);
    value |= (((flash_current_config->qe_bit << flash_current_config->qe_bit_post)
        | param) << WRSR_DATA_POSI);
    REG_WRITE(REG_FLASH_CONF, value);

    value = REG_READ(REG_FLASH_OPERATE_SW);

    if(1 == flash_current_config->sr_size)
    {
        value = (value & (ADDR_SW_REG_MASK << ADDR_SW_REG_POSI))
                | (FLASH_OPCODE_WRSR << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
    }
    else
    {
        value = (value & (ADDR_SW_REG_MASK << ADDR_SW_REG_POSI))
                | (FLASH_OPCODE_WRSR2 << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
    }

    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
}

static void flash_set_qwfr(void)
{
    UINT32 value;

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(MODEL_SEL_MASK << MODEL_SEL_POSI);
    value |= (flash_current_config->mode_sel << MODEL_SEL_POSI);
    REG_WRITE(REG_FLASH_CONF, value);
}

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

static void flash_set_wsr(UINT16 data)
{
    UINT32 value;

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    value = REG_READ(REG_FLASH_CONF);
    value &= ~(WRSR_DATA_MASK << WRSR_DATA_POSI);
    value |= (data << WRSR_DATA_POSI);
    REG_WRITE(REG_FLASH_CONF, value);

    value = REG_READ(REG_FLASH_OPERATE_SW);
    value = (value & (ADDR_SW_REG_MASK << ADDR_SW_REG_POSI))
            | (FLASH_OPCODE_WRSR2 << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
}

UINT8 flash_get_line_mode(void)
{
    return flash_current_config->line_mode;
}

void flash_set_line_mode(UINT8 mode)
{
    UINT32 value;

    if(1 == mode)
    {
        flash_clr_qwfr();
    }
    if(2 == mode)
    {
        flash_clr_qwfr();
        value = REG_READ(REG_FLASH_CONF);
        value &= ~(MODEL_SEL_MASK << MODEL_SEL_POSI);
        value |= ((MODE_DUAL & MODEL_SEL_MASK) << MODEL_SEL_POSI);
        REG_WRITE(REG_FLASH_CONF, value);
    }
    else if(4 == mode)
    {
        flash_clr_qwfr();
        value = REG_READ(REG_FLASH_SR_DATA_CRC_CNT);
        value &= ~(M_VALUE_MASK << M_VALUE_POST);
        value |= (flash_current_config->m_value<< M_VALUE_POST);

        REG_WRITE(REG_FLASH_SR_DATA_CRC_CNT, value);

        value = REG_READ(REG_FLASH_SR_DATA_CRC_CNT);

        if(1 == flash_current_config->qe_bit)
        {
            flash_set_qe();
        }

        flash_set_qwfr();
    }
}

static UINT32 flash_get_id(void)
{
    UINT32 value;

    value = (FLASH_OPCODE_RDID << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
    REG_WRITE(REG_FLASH_OPERATE_SW, value);

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

	flash_id = REG_READ(REG_FLASH_RDID_DATA_FLASH);
    return flash_id;
}

static UINT32 flash_read_mid(void)
{
    UINT32 value;
    UINT32 flash_id;

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    value = REG_READ(REG_FLASH_OPERATE_SW);
    value = ((value & (ADDR_SW_REG_MASK << ADDR_SW_REG_POSI))
             | (FLASH_OPCODE_RDID << OP_TYPE_SW_POSI)
             | OP_SW
             | (value & WP_VALUE));
    REG_WRITE(REG_FLASH_OPERATE_SW, value);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);

    flash_id = REG_READ(REG_FLASH_RDID_DATA_FLASH);

    return flash_id;
}

static void set_flash_protect(PROTECT_TYPE type)
{
    UINT32 param, value, cmp;

	switch (type)
	{
		case FLASH_PROTECT_NONE:
            param = flash_current_config->protect_none & 0xff;
            cmp = (flash_current_config->protect_none >> 8) & 0xff;
            break;

		case FLASH_PROTECT_ALL:
			param = flash_current_config->protect_all & 0xff;
			cmp = (flash_current_config->protect_all >> 8) & 0xff;
			break;

        case FLASH_PROTECT_HALF:
			param = flash_current_config->protect_half & 0xff;
			cmp = (flash_current_config->protect_half >> 8) & 0xff;
			break;

        case FLASH_UNPROTECT_LAST_BLOCK:
			param = flash_current_config->unprotect_last_block& 0xff;
			cmp = (flash_current_config->unprotect_last_block >> 8) & 0xff;
			break;

		default:
			param = flash_current_config->protect_all & 0xff;
            cmp = (flash_current_config->protect_all >> 8) & 0xff;
			break;
	}

    value = flash_read_sr(flash_current_config->sr_size);

	if(((param << flash_current_config->protect_post) !=
        (value & (flash_current_config->protect_mask << flash_current_config->protect_post)))
        || ((cmp << flash_current_config->cmp_post) !=
        (value & (0x01 << flash_current_config->cmp_post))))
	{
        value = (value & (~(flash_current_config->protect_mask
            << flash_current_config->protect_post))) | (param << flash_current_config->protect_post);
		value &= ~(1 << flash_current_config->cmp_post);
		value |= ((cmp & 0x01) << flash_current_config->cmp_post);
		os_printf("--write status reg:%x,%x--\r\n", value, flash_current_config->sr_size);
		flash_write_sr(flash_current_config->sr_size, value);
	}
}
static void flash_erase_sector(UINT32 address)
{
    UINT32 value;

    //while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    //flash_set_line_mode(0);

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    value = REG_READ(REG_FLASH_OPERATE_SW);
    value = ((address << ADDR_SW_REG_POSI)
             | (FLASH_OPCODE_SE << OP_TYPE_SW_POSI)
             | OP_SW
             | (value & WP_VALUE));
    REG_WRITE(REG_FLASH_OPERATE_SW, value);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);


    //flash_set_line_mode(2);
}

static void flash_set_hpm(void)
{
    UINT32 value;

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    value = REG_READ(REG_FLASH_OPERATE_SW);
    value = ((0x0 << ADDR_SW_REG_POSI)
             | (FLASH_OPCODE_HPM << OP_TYPE_SW_POSI)
             | (OP_SW)
             | (value & WP_VALUE));
    REG_WRITE(REG_FLASH_OPERATE_SW, value);
    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
}

static void flash_read_data_inner(UINT8 *buffer, UINT32 address, UINT32 len)
{
    UINT32 i, reg_value;
    UINT32 addr = address & (~0x1F);
    UINT32 buf[8];
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
        addr += 32;

        for(i = 0; i < 8; i++)
        {
            buf[i] = REG_READ(REG_FLASH_DATA_FLASH_SW);
        }

        for(i = address % 32; i < 32; i++)
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

static void flash_write_data_inner(UINT8 *buffer, UINT32 address, UINT32 len)
{
    UINT32 i, reg_value;
    UINT32 addr = address & (~0x1F);
    UINT32 buf[8];
    UINT8 *pb = (UINT8 *)&buf[0];

    if(address % 32)
    {
        flash_read_data(pb, addr, 32);
    }
    else
    {
        memset(pb, 0xFF, 32);
    }

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    while(len)
    {
        if(len < 32)
        {
            flash_read_data(pb, addr, 32);
            while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
        }
        for (i = address % 32; i < 32; i++)
        {
            pb[i] = *buffer++;
            address++;
            len--;
            if (len == 0)
                break;
        }

        for (i = 0; i < 8; i++)
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
        addr += 32;
        memset(pb, 0xFF, 32);
    }
}


void flash_protection_op(UINT8 mode, PROTECT_TYPE type)
{
    //if((mode==0) && (flash_id == FLASH_XTX_16M))
    //	return;
    set_flash_protect(type);
}

void flash_init(void)
{
    UINT32 id, param;

    while(REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW);
    id = flash_get_id();
    FLASH_PRT("[Flash]id:0x%x\r\n", id);
    flash_get_current_flash_config();

    set_flash_protect(FLASH_PROTECT_NONE);

    flash_set_line_mode(flash_current_config->line_mode);

    flash_set_clk(5);  // 60M

    //ddev_register_dev(FLASH_DEV_NAME, &flash_op);

    os_printf("flash_init end\r\n");
}

void flash_exit(void)
{
    ddev_unregister_dev(FLASH_DEV_NAME);
}

UINT32 flash_read(char *user_buf, UINT32 count, UINT32 address)
{
    //peri_busy_count_add();

    flash_read_data_inner((UINT8 *)user_buf, address, count);

    //peri_busy_count_dec();

    return FLASH_SUCCESS;
}

UINT32 flash_write(char *user_buf, UINT32 count, UINT32 address)
{
    //peri_busy_count_add();

    if(4 == flash_current_config->line_mode)
    {
        flash_set_line_mode(LINE_MODE_TWO);
        //os_printf("change line mode 2\r\n");
    }

    flash_write_data_inner((UINT8 *)user_buf, address, count);

    if(4 == flash_current_config->line_mode)
    {
        flash_set_line_mode(LINE_MODE_FOUR);
        //os_printf("change line mode 4\r\n");
    }
    //peri_busy_count_dec();

    return FLASH_SUCCESS;
}

hal_logic_partition_t *flash_get_info(hal_partition_t pno)
{
    if(pno >= hal_partitions_amount)
    {
        return NULL;
    }
    return &hal_partitions[pno];
}

int32_t hal_flash_init(void)
{
    return 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;
    unsigned int           addr;
    unsigned int           start_addr;
    unsigned int           end_addr;

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    start_addr = ROUND_DOWN(partition->partition_start_addr + off_set, SECTOR_SIZE);
    end_addr = ROUND_DOWN((partition->partition_start_addr + off_set + size - 1), SECTOR_SIZE);

    for (addr = start_addr; addr <= end_addr; addr += SECTOR_SIZE) {
        flash_erase_sector(addr);
    }

    return 0;
}

int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set, void *out_buf, uint32_t in_buf_len)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;

    if (off_set == NULL) {
        return -1;
    }

    if (out_buf == NULL) {
        return -1;
    }

    if (in_buf_len == 0) {
        return -1;
    }

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    ret = flash_read((char*)out_buf, (UINT32)in_buf_len,  (UINT32)(partition->partition_start_addr + *off_set));
    if (ret != 0) {
        return -1;
    }

    *off_set += in_buf_len;

    return 0;
}

int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    int                    ret = 0;
    hal_logic_partition_t *partition;

    if (off_set == NULL) {
        return -1;
    }

    if (in_buf == NULL) {
        return -1;
    }

    if (in_buf_len == 0) {
        return -1;
    }

    partition = flash_get_info(in_partition);
    if (partition == NULL) {
        return -1;
    }

    ret = flash_write((char *)in_buf, (UINT32)in_buf_len, (UINT32)(partition->partition_start_addr + *off_set));
    if (0 != ret) {
        return -1;
    }

    *off_set += in_buf_len;

    return 0;
}


