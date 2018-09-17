#include <k_api.h>
#include "hal_timer.h"
#include "board.h"
#include "spi_flash_ctl_alios.h"
#include <hal/soc/nor.h>
#include <stdio.h>

static struct spi_flash flash_spi;
//×î´ó²Á³ýsectorÎª64k
static char bufsector[64*1024];

static int spi_flash_write_ops(struct spi_flash *flash, uint32_t offset,  size_t len, const void *buf)
{
	uint32_t num = 0;
	uint32_t index = 0;
	uint32_t remain = 0;
	unsigned char *ptr = (unsigned char *)buf;
	
	krhino_mutex_lock(&flash->sfclock, RHINO_WAIT_FOREVER);
	while(len > 0)
	{
		remain = offset % XM_SFC_BUF_SIZE;
		num = ((len >= XM_SFC_BUF_SIZE - remain) ?  XM_SFC_BUF_SIZE - remain : len);

		writel(offset, XM_SFC_ADDR);
		writel(num - 1, XM_SFC_WRNUM);

		index = 0;
		while(index < num)
		{
			writel(*(ptr + index), XM_SFC_BUF + 4 * index);
			index++;
		}
		writel(flash->write_cmd, XM_SFC_CMD);
		writel(FL_OP6, XM_SFC_OP);
		while((readl(XM_SFC_ST) & FL_OP6_OK)  == 0);

		offset  += num; 
		ptr += num;
		len -= num;  
	}
    krhino_mutex_unlock(&flash->sfclock);

	return 0;
}
static int spi_flash_read_ops(struct spi_flash *flash, uint32_t offset, size_t len, void *buf)
{
	int i = 0;
	krhino_mutex_lock(&flash->sfclock, RHINO_WAIT_FOREVER);
	writel(flash->read_cmd, XM_SFC_CACHE_CMD);
	//memcpy(buf, flash->memory_map + offset, len);
	for(i = 0; i < len; i++)
	{
		*((char *)buf + i) = *((char *)(flash->memory_map + offset + i));
	}
    krhino_mutex_unlock(&flash->sfclock);
	return 0;
}
static int spi_flash_erase_ops(struct spi_flash *flash, uint32_t offset, size_t len)
{
	uint32_t erase_size, erase_addr;  

	erase_size = flash->erase_size;
	if (offset % erase_size || len % erase_size) 
	{  
		printf("SF: Erase offset/length not multiple of erase size\n");  
		return -1;  
	}
	
	krhino_mutex_lock(&flash->sfclock, RHINO_WAIT_FOREVER);
	while (len)
	{
		erase_addr = offset;

		writel(erase_addr, XM_SFC_ADDR);
		writel(flash->erase_cmd, XM_SFC_CMD);
		writel(FL_OP2, XM_SFC_OP);

		while((readl(XM_SFC_ST) & FL_OP2_OK)  == 0);

		offset += erase_size; 
		len -= erase_size;
	}
    krhino_mutex_unlock(&flash->sfclock);

	return 0;
}

static int spi_flash_read_cmd(uint8_t cmd, void *data, size_t data_len)
{
	if(cmd == FL_CMD_READ_ID)
	{
		writel(FL_CMD_READ_ID, XM_SFC_CMD);
		writel(2, XM_SFC_RW_SR_BSEL);
		writel(FL_OP3, XM_SFC_OP);
		udelay(1);

		while((readl(XM_SFC_ST) & FL_OP3_OK)  == 0);

		*((uint8_t*)data + 0) = (uint8_t)((readl(XM_SFC_SRR) & 0x00FF0000) >> 16);
		*((uint8_t*)data + 1) = (uint8_t)((readl(XM_SFC_SRR) & 0x0000FF00) >> 8);
		*((uint8_t*)data + 2) = (uint8_t)((readl(XM_SFC_SRR) & 0x000000FF));
		*((uint8_t*)data + 3) = 0;
		*((uint8_t*)data + 4) = 0;
	}
	return 0;
}

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int spi_mode)
{
	const struct spi_flash_params *params;  
	struct spi_flash * flash = &flash_spi;
	int ret = 0;
	uint8_t idcode[5]; 
	uint16_t jedec = 0;
	uint16_t ext_jedec = 0;

	ret = spi_flash_read_cmd(FL_CMD_READ_ID, idcode, sizeof(idcode));   
	if (ret) 
	{ 
		printf("SF: Failed to get idcodes\n");    
		return NULL;
	}
	jedec = idcode[1] << 8 | idcode[2];
	ext_jedec = idcode[3] << 8 | idcode[4];

	params = spi_flash_params_table;
	for (; params->name != NULL; params++)
	{
		if ((params->jedec >> 16) == idcode[0])
		{
			if ((params->jedec & 0xFFFF) == jedec) 
			{
				if (params->ext_jedec == 0)
					break;
				else if (params->ext_jedec == ext_jedec)
					break;
			}   
		}   
	} 
	if (!params->name) 
	{
		printf("SF: Unsupported flash IDs: ");
		printf("manuf %02x, jedec %04x, ext_jedec %04x using default\n",
				idcode[0], jedec, ext_jedec);

		params = &spi_flash_params_default;
	}
	
	memset(flash, 0, sizeof(*flash));
	/* Assign spi data */
	flash->name = params->name;
	flash->memory_map = (void *)0x08000000;
	flash->dual_flash = 0;

	/* Compute the flash size */
	flash->shift = (flash->dual_flash & SF_DUAL_PARALLEL_FLASH) ? 1 : 0;
	flash->page_size = 256;
	flash->page_size <<= flash->shift;
	flash->sector_size = params->sector_size << flash->shift;
	flash->size = flash->sector_size * params->nr_sectors << flash->shift;

	/* Compute erase sector and command */
	if (params->flags & SECT_4K) {
		flash->erase_cmd = FL_CMD_ERASE_4K;
		flash->erase_size = 4096 << flash->shift;
	} else if (params->flags & SECT_32K) {
		flash->erase_cmd = FL_CMD_ERASE_32K;
		flash->erase_size = 32768 << flash->shift;
	} else {
		flash->erase_cmd = FL_CMD_ERASE_64K;
		flash->erase_size = flash->sector_size;
	}
	flash->read_cmd = FL_CMD_READ_DATA;
	flash->write_cmd = FL_CMD_WRITE_DATA;
	flash->dummy_byte = 1;
	 /* Poll cmd selection */ 
	flash->poll_cmd = 0;  
	/* initialize mutex */
	if (krhino_mutex_create(&flash->sfclock, "sfc") != 0)
	{
		printf("init sfc lock mutex failed\n");
	}

	printf("flash: %s,size: 0x%08x\n",flash->name, (unsigned int)flash->size);
	return flash;
}

int32_t hal_nor_init(nor_dev_t *nor)
{
	static int nor_initialize = 0;
    if(nor_initialize == 0) {
		spi_flash_probe(0,0,0,0);
		nor_initialize = 1;
	}
    nor->config.block_size = flash_spi.erase_size;
    return 0;
}

int32_t hal_nor_read(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{
    int ret;
    uint32_t pos = nor->base_addr + *addr;
    ret = spi_flash_read_ops(&flash_spi, pos, len, data);
    return ret;
}

int32_t hal_nor_write(nor_dev_t *nor, uint32_t *addr, uint8_t *data, uint32_t len)
{
    int ret;
    uint32_t pos = nor->base_addr + *addr;
    ret = spi_flash_write_ops(&flash_spi, pos, len, data);
    return ret;
}

int32_t hal_nor_erase_block(nor_dev_t *nor, uint32_t *addr, uint32_t block_count)
{
    int ret;
    uint32_t pos = nor->base_addr + *addr;
    ret = spi_flash_erase_ops(&flash_spi, pos, flash_spi.erase_size * block_count);
    return ret;
}

