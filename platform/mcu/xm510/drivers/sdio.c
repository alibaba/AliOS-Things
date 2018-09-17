#include <hal/soc/soc.h>
#include <assert.h>
#include "board.h"
#include "arasan.h"
#include "sdio/mmcsd_card.h"
#include "sdio/sdio.h"


static struct rt_mmcsd_card card;
static hal_sd_info_t sd_info;
static unsigned int wifi_power_io = 0;

static void sdio0_powerup(void)
{
	uint32_t value;
	value = readl(GPIO5_MULT_USE_EN);
	value |= 0xff;
	value &= ~(0xf<<4); // no detect, no powersave, // no write protect, no ledo
	writel(value, GPIO5_MULT_USE_EN);
}

struct arasan_platform_data sdio_sd_data = {
	.id = 0,
	.irq = IRQ_XM510_SDIO0,
	.need_detect = 0,
	.auto_cmd12 = 1,
	.card_irq = 0,
	.base = XM510_SDIO0_BASE,
	.sdio_4bit_data = 1,
	.p_powerup = sdio0_powerup,
};
static void sdio1_powerup(void)
{
	uint32_t value;
	value = readl(GPIO6_MULT_USE_EN);
	value |= 0xff;
	value &= ~(0xf<<4); // no detect, no powersave, // no write protect, no ledo
	writel(value, GPIO6_MULT_USE_EN);
	if (wifi_power_io == 0x65){
		//0x65 muti as GPIO above
		value = readl(GPIO6_OUT_EN);
		value |= (1<<5);
		writel(value, GPIO6_OUT_EN);

		value = readl(GPIO6_OUT_REG);
		value &= ~(1<<5);
		writel(value, GPIO6_IO);
		aos_msleep(2000);

		value = readl(GPIO6_OUT_REG);
		value |= (1<<5);
		writel(value, GPIO6_IO);
		aos_msleep(2000);
	}else if (wifi_power_io == 0x54){
		value = readl(GPIO5_MULT_USE_EN);
		value &= ~(1<<4); // 0x54
		writel(value, GPIO5_MULT_USE_EN);

		value = readl(GPIO5_OUT_EN);
		value |= (1<<4);
		writel(value, GPIO5_OUT_EN);

		value = readl(GPIO5_OUT_REG);
		value &= ~(1<<4);
		writel(value, GPIO5_IO);
		aos_msleep(2000);

		value = readl(GPIO5_OUT_REG);
		value |= (1<<4);
		writel(value, GPIO5_IO);
		aos_msleep(2000);
	}else{
		printf("sdio1_powerup powerport not support!\n");
	}
}

struct arasan_platform_data sdio_wifi_data = {
	.id = 1,
	.irq = IRQ_XM510_SDIO1,
	.need_detect = 0,
	.auto_cmd12 = 0,
	.card_irq = 0,
	.base = XM510_SDIO1_BASE,
	.sdio_4bit_data = 0,
	.p_powerup = sdio1_powerup,
};

int sdio_sd_init(void)
{
	arasan_probe((void *)&sdio_sd_data);
	printf("sdio_sd init is ok!\n");
	return 0;
}
int sdio_wifi_init(unsigned int power_io)
{
	wifi_power_io = power_io;
	arasan_probe((void *)&sdio_wifi_data);
	printf("sdio_wifi init is ok!\n");
	return 0;
}

int sdio_wifi_driver_register(int32_t (*func_probe)(struct rt_mmcsd_card *))
{
	struct rt_sdio_driver *drv;

	drv = aos_malloc(sizeof(struct rt_sdio_driver));
	if(drv == NULL)
		return 0;

	drv->id = aos_malloc(sizeof(struct rt_sdio_device_id));
	if(drv->id == NULL)
		return 0;

	drv->id->func_code = 0; 
	drv->id->manufacturer = SDIO_ANY_MAN_ID;
	drv->id->product = 0;
	drv->probe = func_probe;
	//drv->remove = sdio_wifi_exit;
	sdio_register_driver(drv);

	return 0;
}

int sdio_init(void)
{
	rt_mmcsd_core_init();
	return 0;
}
int32_t hal_sd_init(sd_dev_t *sd)
{
	static int sd_initialize = 0;
	int32_t ret;
	int32_t timeout = 0;
	//printf("%s[%d] %s\n", __FUNCTION__, __LINE__, __FILE__);
	while(!sd_initialize) {
		memset(&card, 0, sizeof(struct rt_mmcsd_card));
		memset(&sd_info, 0, sizeof(hal_sd_info_t));

		sd->port = 0;
		//ret = rt_mmcsd_blk_rescan(&card, sd->port);
		ret = rt_mmcsd_blk_rescan(&card);
		if (ret != 0) {
			if(timeout++ >= 50)
			{
				printf("[hal_sd_init]: init sd failed\n");
				return -1;
			}
			aos_msleep(100);
			continue;
		}
	    sd_initialize = 1;
	}
	sd->config.bus_wide = card.host->io_cfg.bus_width;
	sd->config.freq = 50000000;
	sd->priv = (void*)&card;
	sd_info.blk_size = card.card_blksize;
	sd_info.blk_nums = card.card_capacity / card.card_blksize * 1024;
	return 0;
}
int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                         uint32_t blks, uint32_t timeout)
{
	//printf("%s[%d] %s: blk_addr=%lu, blks=%lu\n", __FUNCTION__, __LINE__, __FILE__, blk_addr, blks);
	int ret = 0;
	if(sd == NULL)
	{
		assert(0);
	}
	struct rt_mmcsd_card * card = (struct rt_mmcsd_card *)sd->priv;
    ret = rt_mmcsd_req_blk(card, blk_addr, data, blks, 0);
	return ret;
}
int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                          uint32_t blks, uint32_t timeout)
{
	//printf("%s[%d] %s: blk_addr=%lu, blks=%lu\n", __FUNCTION__, __LINE__, __FILE__, blk_addr, blks);
	int ret = 0;
	if(sd == NULL)
	{
		assert(0);
	}
	struct rt_mmcsd_card * card = (struct rt_mmcsd_card *)sd->priv;
    ret = rt_mmcsd_req_blk(card, blk_addr, data, blks, 1);

	return ret;
}
int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
	//printf("%s[%d] %s [%ld %ld]\n", __FUNCTION__, __LINE__, __FILE__, sd_info.blk_size, sd_info.blk_nums);
	info->blk_size = sd_info.blk_size;
	info->blk_nums = sd_info.blk_nums;
	return 0;
}
int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
	//printf("%s[%d] %s\n", __FUNCTION__, __LINE__, __FILE__);
	*stat = SD_STAT_TRANSFER;
	return 0;
}
