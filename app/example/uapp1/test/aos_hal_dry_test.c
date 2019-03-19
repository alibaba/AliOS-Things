#include <hal/soc/soc.h>
#include <stdlib.h>
#include <string.h>


#define AOS_HAL_TEST_DELAY(ticks)       krhino_task_sleep((ticks))
#define AOS_HAL_TEST_SLEEP(ticks)       krhino_task_sleep((ticks))

#define ADC_TEST_CHANNEL         (5)
#define ADC_TEST_SAMPLE_CYCLE    (48000)
#define ADC_TEST_TIMEOUT         (1000)

static void adc_test_cmd(void)
{
	adc_dev_t adc_channel;
	uint32_t ret, value;

	adc_channel.port = (uint8_t)ADC_TEST_CHANNEL;
	adc_channel.config.sampling_cycle = ADC_TEST_SAMPLE_CYCLE;
	ret = hal_adc_init(&adc_channel);
	if (ret) {
		printf("hal adc init failed,ret = %d\n",ret);
		return;
	}

	ret = hal_adc_value_get(&adc_channel, &value, ADC_TEST_TIMEOUT);
	if (ret) {
		printf("hal adc get value failed,ret = %d\n",ret);
	}

	printf("hal adc get value = %u \n", value);
	ret = hal_adc_finalize(&adc_channel);
	if (ret) {
		printf("hal adc finanlize failed,ret = %d\n",ret);
		return;
	}

	printf("AOS_HAL_ADC\n");
}



#define FLASH_TEST_PTN       (HAL_PARTITION_PARAMETER_1)
#define FLASH_TEST_BUFSZ     (40)

static void flash_get_info_cmd(void)
{
	hal_logic_partition_t *lp = NULL;

	lp = hal_flash_get_info((hal_partition_t)FLASH_TEST_PTN);
	if (!lp) {
		printf("hal flash get info failed,ret = %d\n",lp);
		return;
	}

	printf("partition owner = %d\n", lp->partition_owner);
	printf("partition decription = %s\n", lp->partition_description);
	printf("start addr = 0x%08x\n", lp->partition_start_addr);
	printf("partition length = %u\n", lp->partition_length);
	printf("partition option = 0x%08x\n", lp->partition_options);

	printf("AOS_HAL_FLASH_GETINFO\n");
}

static void flash_read_write_cmd(void)
{
	int ret;
	uint8_t *read_ptr, *write_ptr;
	uint32_t offset = 0;

	read_ptr = (uint8_t *)malloc(FLASH_TEST_BUFSZ);
	if (!read_ptr) {
		printf("malloc failed,ret = %d\n",ret);
		return;
	}

	memset(read_ptr, 0, FLASH_TEST_BUFSZ);

	write_ptr = (uint8_t *)malloc(FLASH_TEST_BUFSZ);
	if (!write_ptr) {
		printf("malloc failed,ret = %d\n",ret);
		free(read_ptr);
		return;
	}

	memset(write_ptr, 0xA5, FLASH_TEST_BUFSZ);
	ret = hal_flash_write((hal_partition_t)FLASH_TEST_PTN, &offset, write_ptr, FLASH_TEST_BUFSZ);
	if (ret < 0) {
		printf("hal flash write failed,ret = %d\n",ret);
		goto err;
	}

	ret = hal_flash_read((hal_partition_t)FLASH_TEST_PTN, &offset, read_ptr, FLASH_TEST_BUFSZ);
	if (ret < 0) {
		printf("flash read failed,ret = %d\n",ret);
		goto err;
	}

	if (memcmp(write_ptr, read_ptr, FLASH_TEST_BUFSZ)) {
		printf("flash read not equal to what write\n");
		goto err;
	}

	ret = hal_flash_erase((hal_partition_t)FLASH_TEST_PTN, offset, 1024);
	if (ret < 0) {
		printf("flash erase write\n");
		goto err;
	}

	memset(read_ptr, 0x01, FLASH_TEST_BUFSZ);
	memset(write_ptr, 0xFF, FLASH_TEST_BUFSZ);
	ret = hal_flash_read((hal_partition_t)FLASH_TEST_PTN, &offset, read_ptr, FLASH_TEST_BUFSZ);
	if (ret < 0) {
		printf("flash erase write\n");
		goto err;
	}

	if (memcmp(write_ptr, read_ptr, FLASH_TEST_BUFSZ)) {
		printf("flash read not equal to what write\n");
		goto err;
	}

	printf("AOS_HAL_FLASH\n");
err:
	free(write_ptr);
	free(read_ptr);
}

static void flash_secure_cmd(void)
{
	uint32_t ret;

	ret = hal_flash_enable_secure((hal_partition_t)FLASH_TEST_PTN, 0, FLASH_TEST_BUFSZ);
	if (ret) {
		printf("flash not support secure\n");
		return;
	}

	ret = hal_flash_dis_secure((hal_partition_t)FLASH_TEST_PTN, 0, FLASH_TEST_BUFSZ);
	if (ret) {
		printf("flash not support secure\n");
		return;
	}

	printf("AOS_HAL_FLASH\n");
}


#define MTP_TEST_GPIO_INDEX     (38)

static void gpio_output_cmd()
{
	gpio_dev_t gpio;
	uint32_t value;
	int32_t ret;

	gpio.port = MTP_TEST_GPIO_INDEX;
	gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
	ret = hal_gpio_init(&gpio);
	if (ret) {
		printf("hal gpio initial failed, ret = %d\n", ret);
		return;
	}

	ret = hal_gpio_output_high(&gpio);
	if (ret) {
		printf("hal gpio output high failed, ret = %d\n", ret);
		goto err;
	}

	AOS_HAL_TEST_DELAY(10);

	ret = hal_gpio_input_get(&gpio, &value);
	if (ret) {
		printf("hal gpio get failed, ret = %d\n", ret);
		goto err;
	}

	if (value == 1)
		printf("AOS_HAL_GPIO_OUTPUT HIGH\n");

	ret = hal_gpio_output_low(&gpio);
	if (ret) {
		printf("hal gpio output low failed, ret = %d\n", ret);
		goto err;
	}

	AOS_HAL_TEST_DELAY(10);

	ret = hal_gpio_input_get(&gpio, &value);
	if (ret) {
		printf("hal gpio get failed, ret = %d\n", ret);
		goto err;
	}

	if (value == 0)
		printf("AOS_HAL_GPIO_OUTPUT LOW\n");

	ret = hal_gpio_finalize(&gpio);
	if (ret) {
		printf("hal gpio finalize failed, ret = %d\n", ret);
		goto err;
	}

	printf("AOS_HAL_GPIO_OUTPUT FINISH\n");

	return;
err:
	hal_gpio_finalize(&gpio);
}

static void gpio_toggle_cmd()
{
	gpio_dev_t gpio;
	int32_t ret;

	gpio.port = MTP_TEST_GPIO_INDEX;
	gpio.config = OUTPUT_OPEN_DRAIN_PULL_UP;
	ret = hal_gpio_init(&gpio);
	if (ret) {
		printf("hal gpio initial failed, ret = %d\n", ret);
		return;
	}

	ret = hal_gpio_output_toggle(&gpio);
	if (ret) {
		printf("hal gpio output toggle failed, ret = %d\n", ret);
		goto err;
	}

	ret = hal_gpio_finalize(&gpio);
	if (ret) {
		printf("hal gpio finalize failed, ret = %d\n", ret);
		return;
	}

	printf("AOS_HAL_GPIO_TOGGLE\n");

	return;
err:
	hal_gpio_finalize(&gpio);
}

static void gpio_input_cmd()
{
	gpio_dev_t gpio;
	uint32_t value;
	int32_t ret;

	gpio.port = MTP_TEST_GPIO_INDEX;
	gpio.config = INPUT_PULL_UP;
	ret = hal_gpio_init(&gpio);
	if (ret) {
		printf("hal gpio initial failed, ret = %d\n", ret);
		return;
	}

	ret = hal_gpio_input_get(&gpio, &value);
	if (ret) {
		printf("hal gpio get failed, ret = %d\n", ret);
		goto err;
	}

	printf("gpio read value = %d\n", value);

	ret = hal_gpio_finalize(&gpio);
	if (ret) {
		printf("hal gpio finalize failed, ret = %d\n", ret);
		return;
	}

	printf("AOS_HAL_GPIO_INPUT");

	return;
err:
	hal_gpio_finalize(&gpio);
}

static gpio_dev_t irq_gpio;
static int irq_gpio_init = 0;

static void aos_gpio_irq_handle(void *arg)
{
	printf("AOS_HAL_GPIO_IRQ_HANDLE");
	return;
}

static void gpio_irq_enable_cmd()
{
	gpio_dev_t gpio;
	uint32_t ret;

	if (irq_gpio_init) {
		printf("already initial\n");
		return;
	}

	irq_gpio.port = MTP_TEST_GPIO_INDEX;
	irq_gpio.config = IRQ_MODE;
	ret = hal_gpio_init(&irq_gpio);
	if (ret) {
		printf("hal gpio initial failed, ret = %d\n", ret);
		return;
	}

	ret = hal_gpio_enable_irq(&irq_gpio, IRQ_TRIGGER_FALLING_EDGE, aos_gpio_irq_handle, &irq_gpio);
	if (ret) {
		printf("hal gpio enable irq failed, ret = %d\n", ret);
		hal_gpio_finalize(&irq_gpio);
		return;
	}

	irq_gpio_init = 1;
	printf("AOS_HAL_GPIO_IRQ_ENABLE");
}

static void gpio_irq_disable_cmd()
{
	uint32_t ret = 0;

	if (!irq_gpio_init) {
		printf("gpio irq didn't enable\n");
		return;
	}

	ret = hal_gpio_disable_irq(&irq_gpio);
	if (ret) {
		hal_gpio_finalize(&irq_gpio);
		printf("gpio irq disable failed, ret = %d\n", ret);
		return;
	}
	printf("AOS_HAL_GPIO_IRQ_DSIABLE");
}

static void gpio_irq_clear_cmd()
{
	uint32_t ret = 0;

	if (!irq_gpio_init) {
		printf("gpio irq didn't initial\n");
		return;
	}

	ret = hal_gpio_clear_irq(&irq_gpio);
	if (ret) {
		printf("gpio irq clear failed, ret = %d\n", ret);
		return;
	}

	printf("AOS_HAL_GPIO_IRQ_CLEAR");
}


//below just fake setting, change them as needed
#define I2C_TEST_MASTER_ID          (4)
#define I2C_TEST_SLAVE_ADDR         (0x52)
#define I2C_TEST_SLAVE_ADDR_LEN     (I2C_MEM_ADDR_SIZE_16BIT)
#define I2C_TEST_FREQUENCY          (400 * 1000) //400K
#define I2C_TEST_TIMEOUT            (1000)
#define I2C_TEST_REGISTER_ADDR      (0x00)

#define I2C_TEST_MEM_ADDR           (0x1212)
#define I2C_TEST_MEM_SIZE           (2)

static void i2c_master_cmd(void)
{
	i2c_dev_t i2c_master;
	uint8_t write_data[4] = {0x11, 0x22, 0x33, 0x44};
	uint8_t	read_data[4];
	int32_t ret;

	i2c_master.port = I2C_TEST_MASTER_ID;
	i2c_master.config.address_width = I2C_TEST_SLAVE_ADDR_LEN;
	i2c_master.config.freq = I2C_TEST_FREQUENCY;
	i2c_master.config.mode = I2C_MODE_MASTER;
	i2c_master.config.dev_addr = I2C_TEST_SLAVE_ADDR;
	ret = hal_i2c_init(&i2c_master);
	if (ret) {
		printf("hal i2c init failed,ret = %d\n",ret);
		return;
	}

	ret = hal_i2c_master_send(&i2c_master, I2C_TEST_MEM_ADDR, write_data, 4, I2C_TEST_TIMEOUT);
	if (ret) {
		printf("hal i2c master send failed,ret = %d\n",ret);
		goto finally;
	}

	AOS_HAL_TEST_DELAY(5);

	ret = hal_i2c_master_recv(&i2c_master, I2C_TEST_MEM_ADDR, read_data, 4, I2C_TEST_TIMEOUT);
	if (ret) {
		printf("hal i2c master receive failed,ret = %d\n",ret);
		goto finally;
	}

	ret = hal_i2c_finalize(&i2c_master);
	if (ret) {
		printf("hal i2c master finanlize failed,ret = %d\n",ret);
		return;
	}

	if (!memcmp(write_data, read_data, 4)) {
		printf("AOS_HAL_I2C_MASTER\n");
	} else {
		printf("AOS_HAL_I2C_MASTER\n");
	}

	return;
finally:
	hal_i2c_finalize(&i2c_master);
}

static void i2c_memory_cmd()
{
	i2c_dev_t i2c_master;
	uint8_t write_data[4] = {0x11, 0x22, 0x33, 0x44};
	uint8_t read_data[4];
	int32_t ret;

	i2c_master.port = I2C_TEST_MASTER_ID;
	i2c_master.config.address_width = I2C_TEST_SLAVE_ADDR_LEN;
	i2c_master.config.freq = I2C_TEST_FREQUENCY;
	i2c_master.config.mode = I2C_MODE_MASTER;
	i2c_master.config.dev_addr = I2C_TEST_SLAVE_ADDR;
	ret = hal_i2c_init(&i2c_master);
	if (ret) {
		printf("hal i2c init failed,ret = %d\n",ret);
		return;
	}

	ret = hal_i2c_mem_write(&i2c_master, I2C_TEST_SLAVE_ADDR,
		I2C_TEST_MEM_ADDR, I2C_TEST_MEM_SIZE,
		write_data, 4, I2C_TEST_TIMEOUT);
	if (ret) {
		printf("hal i2c memory write failed,ret = %d\n",ret);
		goto finally;
	}

	ret = hal_i2c_mem_read(&i2c_master, I2C_TEST_SLAVE_ADDR,
		I2C_TEST_MEM_ADDR, I2C_TEST_MEM_SIZE,
		read_data, 4, I2C_TEST_TIMEOUT);
	if (ret) {
		printf("hal i2c memory read failed,ret = %d\n",ret);
		goto finally;
	}

	ret = hal_i2c_finalize(&i2c_master);
	if (ret) {
		printf("hal i2c finanlize failed,ret = %d\n",ret);
		return;
	}

	if (!memcmp(read_data, write_data, 4)) {
		printf("AOS_HAL_I2C_MEMORY\n");
	} else {
		printf("AOS_HAL_I2C_MEMORY\n");
	}

	return;
finally:
	hal_i2c_finalize(&i2c_master);
}

#define RTC_TEST_PORT               (0)
#define RTC_TEST_FORMAT_MODE        (HAL_RTC_FORMAT_DEC)
static void rtc_cmd()
{
	rtc_time_t rtc_time, rtc_time_check;
	rtc_dev_t rtc;
	int32_t ret;

	rtc.port = RTC_TEST_PORT;
	rtc.config.format = RTC_TEST_FORMAT_MODE;

	ret = hal_rtc_init(&rtc);
	if (ret) {
		printf("hal rtc init failed,ret = %d\n",ret);
		return;
	}

	memset(&rtc_time, 0, sizeof(rtc_time_t));
	ret = hal_rtc_get_time(&rtc, &rtc_time);
	if (ret) {
		printf("hal rtc read failed,ret = %d\n",ret);
		goto finally;
	}

	printf("Before Setting: %d-%d-%d:%d-%d-%d\n",
		rtc_time.year, rtc_time.month, rtc_time.date,
		rtc_time.hr, rtc_time.min, rtc_time.sec);

#if 0
	AOS_HAL_TEST_SLEEP(500);
	ret = hal_rtc_set_time(&rtc, &rtc_time);
	if (ret) {
		printf("hal rtc set time failed,ret = %d\n",ret);
		goto finally;
	}

	ret = hal_rtc_get_time(&rtc, &rtc_time_check);
	if (ret) {
		printf("hal rtc read time failed,ret = %d\n",ret);
		goto finally;
	}

	printf("After setting: %d-%d-%d:%d-%d-%d\n",
		rtc_time.year, rtc_time.month, rtc_time.date,
		rtc_time.hr, rtc_time.min, rtc_time.sec);
#endif
	ret = hal_rtc_finalize(&rtc);
	if (ret) {
		printf("hal rtc finanlize failed,ret = %d\n",ret);
		return;
	}

	printf("AOS_HAL_RTC\n");
	return;
finally:
	hal_rtc_finalize(&rtc);
}


//below just fake setting, change them as needed
#define SPI_TEST_MASTER_ID          (2)
#define SPI_TEST_FREQUENCY          (10 * 1000 * 1000)
#define SPI_TEST_TIMEOUT            (1000)

// change below read and write data buffer for mtp device
// we just test spi loopback
static void spi_test_cmd()
{
	spi_dev_t spi_dev;
	uint8_t write_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
	uint8_t read_data[16];
	int32_t ret;

	spi_dev.port = SPI_TEST_MASTER_ID;
	spi_dev.config.freq = SPI_TEST_FREQUENCY;
	spi_dev.config.mode = HAL_SPI_MODE_MASTER;
	ret = hal_spi_init(&spi_dev);
	if (ret) {
		printf("hal spi init failed,ret = %d\n",ret);
		return;
	}

	ret = hal_spi_send_recv(&spi_dev, write_data, read_data, 8, SPI_TEST_TIMEOUT);
	if (ret) {
		printf("hal spi send failed,ret = %d\n",ret);
		goto finally;
	}

	ret = hal_spi_finalize(&spi_dev);
	if (ret) {
		printf("hal spi master finanlize failed,ret = %d\n",ret);
		return;
	}

	if (!memcmp(write_data, read_data, 8)) {
		printf("AOS_HAL_SPI\n");
	} else {
		printf("AOS_HAL_SPI\n");
	}

	return;
finally:
	hal_spi_finalize(&spi_dev);
}


//#if AOS_HAL_TEST_TIMER
////below is fake data
//#define TIMER_TEST_PORT          (0)
//#define TIMER_TEST_PERIOD        (1000 * 1000) //1s
//#define TIMER_TEST_RELOAD_MODE   (TIMER_RELOAD_AUTO)
//
//static int callback_count = 0;
//static void timer_test_callback(void *arg)
//{
//	callback_count++;
//
//	printf("AOS_HAL_TIMER");
//}
//
//static void timer_cmd()
//{
//	timer_dev_t timer;
//	int32_t ret;
//
//	timer.port = TIMER_TEST_PORT;
//	timer.config.period = TIMER_TEST_PERIOD;
//	timer.config.reload_mode = TIMER_TEST_RELOAD_MODE;
//	timer.config.cb = timer_test_callback;
//	timer.config.arg = &timer;
//
//	ret = hal_timer_init(&timer);
//	if (ret) {
//		printf("hal timer init failed\n");
//		return;
//	}
//
//	ret = hal_timer_start(&timer);
//	if (ret) {
//		printf("hal timer start failed\n");
//		goto finally;
//	}
//
//	AOS_HAL_TEST_SLEEP(500);
//
//	hal_timer_stop(&timer);
//
//	AOS_HAL_TEST_SLEEP(500);
//
//	ret = hal_timer_finalize(&timer);
//	if (ret) {
//		printf("hal timer finanlize failed\n");
//		return;
//	}
//
//	printf("AOS_HAL_TIMER\n");
//	return;
//finally:
//	hal_timer_finalize(&timer);
//}
//
//#endif 

void aos_dry_test(void)
{
	adc_test_cmd();
//	flash_get_info_cmd();
	flash_read_write_cmd();
	flash_secure_cmd();
	gpio_output_cmd();
	gpio_input_cmd();
	gpio_toggle_cmd();
	gpio_irq_enable_cmd();
	gpio_irq_disable_cmd();
	gpio_irq_clear_cmd();
	i2c_master_cmd();
	i2c_memory_cmd();
	rtc_cmd();
	spi_test_cmd();
//	timer_cmd();
}
