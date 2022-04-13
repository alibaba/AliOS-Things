#include <stdio.h>
#include "console.h"
#include <aos/hal/gpio.h>
#include <aos/hal/i2c.h>
#include "sunxi-input.h"
#include "input-event-codes.h"
#include "hal_iomux_haas1000.h"
#include "hal_gpio.h"
#include "cmsis_os.h"
#include "tlsc6x.h"

#define TLSC6X_TEST_TASK
//#define DEBUG

#define	TS_MAX_FINGER	2
#define TLSC6X_DEV_NAME	"tlsc6x"
#define INT_GPIO_MUX	6

#define TOUCH_INT HAL_IOMUX_PIN_P3_5
#define TOUCH_RESET HAL_IOMUX_PIN_P3_4
osMutexDef(tp_mutex);
osSemaphoreDef(tp_semaphore);
static ktask_t *g_touch_task;
static ktask_t *g_tptest_task;

//tp config init.
struct tlsc6x_hw_cfg tlsc6x_cfg = {
	.twi_id = 1,
	.addr = 0x2e,
	.int_gpio = TOUCH_INT,
	.reset_gpio = TOUCH_RESET,
	.screen_max_x = 240,
	.screen_max_y = 320,
	.revert_x_flag = 0,
	.revert_y_flag = 1,
	.exchange_x_y_flag = 1,
};

//tp config init.
gpio_dev_t tlsc6x_gpio = {
	.port = TOUCH_INT,
	.config = IRQ_MODE,
};

struct tlsc6x_drv_data *tlsc6x_data = NULL;
int g_is_telink_comp = 1;

int tlsc6x_i2c_read(struct tlsc6x_drv_data *data, char *writebuf, int writelen,
			char* readbuf, int readlen)
{
	uint32_t ret;
	u8 addr = data->config->addr;
	i2c_dev_t tlsc6x_i2c_port = {
    	.port = 0,
    	.config.address_width = 8,
    	.config.freq = 400000,
    	.config.dev_addr = 0x2e,
	};

	//ret = hal_i2c_task_recv(0, addr, writebuf, writelen, readbuf, readlen, 0, NULL);
	ret = hal_i2c_master_recv_vendor(&tlsc6x_i2c_port, addr, writebuf, writelen, readbuf, readlen);
	if (ret != 0) {
		tlsc_err("i2c receive err\n");
		return -1;
	}
#if 0
	else {
		int i = 0;
		hal_trace_printf("writebuf[0]=0x%x, writebuf[1]=0x%x", writebuf[0], writebuf[1]);
		for (i = 0; i < readlen; i++) {
			hal_trace_printf("readbuf[%d]=0x%x", i, readbuf[i]);
		}

	}
#endif
return 0;
}

int tlsc6x_i2c_write(struct tlsc6x_drv_data *data, char *writebuf, int writelen)
{
	uint32_t ret;
	u8 addr = data->config->addr;
	i2c_dev_t tlsc6x_i2c_port = {
    	.port = 0,
    	.config.address_width = 8,
    	.config.freq = 400000,
    	.config.dev_addr = 0x2e,
	};

	ret = hal_i2c_master_send(&tlsc6x_i2c_port, addr, writebuf, writelen, 0);
	if (ret != 0) {
		tlsc_err("i2c send err\n");
		return -1;
	}

	return 0;
}

static int tlsc6x_update_data(struct tlsc6x_drv_data *parm)
{
	struct tlsc6x_drv_data *data = parm;
	struct tlsc6x_hw_cfg *config = data->config;
	struct ts_event *event = &data->event;
	u8 buf[20] = { 0 };
	int ret = -1;
	int i;
	u16 x, y;
	u8 tlsc_pressure, tlsc_size, updown;

	ret = tlsc6x_i2c_read(data, buf, 1, buf, 18);
	if (ret < 0) {
		tlsc_err("%s read_data i2c_rxdata failed: %d\n", __func__, ret);
		return ret;
	}

	memset(event, 0, sizeof(struct ts_event));
	event->touch_point = buf[2] & 0x07;

	for (i = 0; i < TS_MAX_FINGER; i++) {
        updown = buf[6 * i + 3] & 0xc0;
		if (updown == 0xc0) {
            printf("tlsc6x_update_data finger:%d, updown:0x%02x\n", i, updown);
			continue;
		}
		x = (s16) (buf[6 * i + 3] & 0x0F) << 8 | (s16) buf[6 * i + 4];
		y = (s16) (buf[6 * i + 5] & 0x0F) << 8 | (s16) buf[6 * i + 6];

		if (config->exchange_x_y_flag)
			swap(x, y);
		if (config->revert_x_flag)
			x = config->screen_max_x - x;
		if (config->revert_y_flag)
			y = config->screen_max_y - y;

        printf("tlsc6x_update_data finger:%d, updown:0x%02x, x=%d, y=%d\n", i, updown, x, y);

		tlsc_pressure = buf[6 * i + 7];
		if (tlsc_pressure > 127) {
			tlsc_pressure = 127;
		}
		tlsc_size = (buf[6 * i + 8] >> 4) & 0x0F;

        if (updown == 0x00) { /* touch down */
            valueSecondGlobal = y;
            input_report_abs(data->input_dev, ABS_MT_PRESSURE, x);
        } else if (updown == 0x40) { /* touch up */
            valueSecondGlobal = y;
            input_report_abs(data->input_dev, ABS_MT_POSITION_Y, x);
        } else { /* move */
            valueSecondGlobal = y;
            input_report_abs(data->input_dev, ABS_MT_POSITION_X, x);
        }

        input_sync(data->input_dev);
        //input_report_abs(data->input_dev, ABS_MT_POSITION_X, x);
		//input_report_abs(data->input_dev, ABS_MT_POSITION_Y, y);
#if 0
		if ((buf[6 * i + 3] & 0x40) == 0x0) {
			if (y == 1500) {
				if (x == 40) {
					input_report_key(data->input_dev,
							 KEY_MENU, 1);
				}
				if (x == 80) {
					input_report_key(data->input_dev,
							 KEY_HOMEPAGE, 1);
				}
				if (x == 120) {
					input_report_key(data->input_dev,
							 KEY_BACK, 1);
				}
			} else {
				input_report_abs(data->input_dev,
						 ABS_MT_POSITION_X, x);
				input_report_abs(data->input_dev,
						 ABS_MT_POSITION_Y, y);

				input_report_abs(data->input_dev,
						 ABS_MT_PRESSURE, 15);
				input_report_abs(data->input_dev,
						 ABS_MT_TOUCH_MAJOR, tlsc_size);
				//input_report_key(data->input_dev, BTN_TOUCH, 1);
			}
			input_sync(data->input_dev);
			//tlsc_info("==========x : %d======\n", x);
			//tlsc_info("==========y : %d======\n", y);
		}
#endif

	}
	if (event->touch_point == 0) {
		if (y == 1500) {
			if (x == 40) {
				input_report_key(data->input_dev, KEY_MENU, 0);
			}
			if (x == 80) {
				input_report_key(data->input_dev, KEY_HOMEPAGE,
						 0);
			}
			if (x == 120) {
				input_report_key(data->input_dev, KEY_BACK, 0);
			}

		input_sync(data->input_dev);
		}
		//tlsc6x_clear_report_data(data);
	}

	return 0;

}


void touch_event_handler(void *parm)
{

	tlsc_info("========enter touch_event_handler=====\n");
	struct tlsc6x_drv_data *data = (struct tlsc6x_drv_data *)parm;

	osStatus status = osMutexWait(data->mutex, osWaitForever);
	if (osOK != status) {
		printf("%s tp_mutex wait error = 0x%X!", __func__, status);
		return;
	}

	while (1) {
		//wait irq wakeup.
		osSemaphoreWait(data->irq_sem, osWaitForever);
		tlsc6x_update_data(data);
	}
	osMutexRelease(data->mutex);
}

static void tlsc6x_irq_handler(void *parm)
{
	struct tlsc6x_drv_data *data = (struct tlsc6x_drv_data *)parm;

	tlsc_info("========enter tlsc6x_irq_handler=====\n");
	if (osOK != osSemaphoreRelease(data->irq_sem)) {
		tlsc_info("tp osSemaphoreRelease failed!");
	}
}


static void tlsc6x_tpd_reset(int reset_gpio)
{
	hal_gpio_pin_set_dir(reset_gpio, HAL_GPIO_DIR_OUT, 0);
	aos_msleep(20);
	hal_gpio_pin_set_dir(reset_gpio, HAL_GPIO_DIR_OUT, 1);
	aos_msleep(30);
}

static int tlsc6x_hw_init(struct tlsc6x_drv_data *data)
{
	int ret;
	int reset_count = 0;
	struct tlsc6x_hw_cfg *config = data->config;

	i2c_dev_t tlsc6x_i2c_port = {
    	.port = 0,
    	.config.address_width = 8,
    	.config.freq = 400000,
    	.config.dev_addr = 0x2e,
	};
	ret = hal_i2c_init(&tlsc6x_i2c_port);

    /* INT pin config */
	gpio_dev_t int_gpio;
    int_gpio.port = (uint8_t)(data->config->int_gpio);
    /* set as interrupt mode */
    int_gpio.config = IRQ_MODE;
    /* configure GPIO with the given settings */
    hal_gpio_init(&int_gpio);

	gpio_dev_t reset_gpio;
	reset_gpio.port = (uint8_t)(data->config->reset_gpio);
	reset_gpio.config = OUTPUT_OPEN_DRAIN_NO_PULL;
	hal_gpio_init(&reset_gpio);

	hal_gpio_output_high(&reset_gpio);

	while(reset_count++ <= 3) {
		tlsc6x_tpd_reset(config->reset_gpio);
		g_is_telink_comp = tlsc6x_tp_dect(data);
		if (g_is_telink_comp)
			break;
	}

	if (g_is_telink_comp) {
		tlsc6x_tpd_reset(config->reset_gpio);
	} else {
		tlsc_err("tlsc6x_tp_dect err\n");
		return -1;
	}

	return 0;
}

static inline void get_tlsc6x_cfg(struct tlsc6x_drv_data *data)
{
	data->config = &tlsc6x_cfg;
	data->irq_num = &tlsc6x_gpio;
}


int tlsc6x_init(void)
{
	int ret;
	struct sunxi_input_dev *input_dev;

    tlsc_info("tlsc6x_init start\n");

	tlsc6x_data = malloc(sizeof(struct tlsc6x_drv_data));
	if (NULL == tlsc6x_data) {
		tlsc_err("malloc tlsc6x_data err\n");
		return -1;
	}


	get_tlsc6x_cfg(tlsc6x_data);
	ret = tlsc6x_hw_init(tlsc6x_data);
	if (ret < 0) {
		tlsc_err("tlsc6x_hw_init err\n");
		goto err_free_data;
	}

	ret = hal_gpio_enable_irq(tlsc6x_data->irq_num, IRQ_TRIGGER_FALLING_EDGE, tlsc6x_irq_handler,
			 tlsc6x_data);
	if (ret < 0) {
		tlsc_err("irq request err\n");
		goto err_free_data;
	}
	if (!(tlsc6x_data->mutex)) {
		tlsc6x_data->mutex = osMutexCreate((osMutex(tp_mutex)));
		if (NULL == tlsc6x_data->mutex) {
			tlsc_err("mutex init err\n");
			goto err_disable_irq;
		}
	}
	if (!(tlsc6x_data->irq_sem)) {
		tlsc6x_data->irq_sem = osSemaphoreCreate(osSemaphore(tp_semaphore), 0);
		if (NULL == tlsc6x_data->irq_sem) {
			tlsc_err("irq_sem init err\n");
			goto err_free_mutex;
		}
	}

	input_dev = sunxi_input_allocate_device();
	if (NULL == input_dev) {
		tlsc_err("input dev alloc err\n");
		goto err_free_sem;

	}
	input_dev->name = TLSC6X_DEV_NAME;

    input_set_capability(input_dev, EV_ABS, ABS_MT_PRESSURE);
	input_set_capability(input_dev, EV_ABS, ABS_MT_POSITION_X);
	input_set_capability(input_dev, EV_ABS, ABS_MT_POSITION_Y);
	input_set_capability(input_dev, EV_ABS, ABS_MT_TOUCH_MAJOR);
	input_set_capability(input_dev, EV_ABS, ABS_MT_WIDTH_MAJOR);
	input_set_capability(input_dev, EV_KEY, KEY_MENU);
	input_set_capability(input_dev, EV_KEY, KEY_BACK);
	input_set_capability(input_dev, EV_KEY, KEY_HOMEPAGE);
	sunxi_input_register_device(input_dev);
	tlsc6x_data->input_dev = input_dev;

	kstat_t task_ret;
	task_ret = krhino_task_dyn_create(&g_touch_task, "touch_handle_task", tlsc6x_data, 33, 0, (1024), (task_entry_t)touch_event_handler, 1);
	if (task_ret != RHINO_SUCCESS) {
		tlsc_err("input dev alloc err\n");
		goto err_free_sem;

	}
	tlsc_info("tlsc6x_data6x_init success\n");

	return 0;

err_free_sem:
	osSemaphoreDelete(tlsc6x_data->irq_sem);

err_free_mutex:
	osMutexDelete(tlsc6x_data->mutex);

err_disable_irq:
	hal_gpio_disable_irq(tlsc6x_data->irq_num);

err_free_irq:
	hal_gpio_clear_irq(tlsc6x_data->irq_num);

err_free_data:
	free(tlsc6x_data);
	tlsc6x_data = NULL;

	return -1;
}

int tlsc6x_deinit(void)
{
	if (NULL == tlsc6x_data) {
		tlsc_err("tlsc6x is not init\n");
		return -1;
	}

	osSemaphoreDelete(tlsc6x_data->irq_sem);

	osMutexDelete(tlsc6x_data->mutex);

	hal_gpio_irq_disable(tlsc6x_data->irq_num);

	hal_gpio_irq_free(tlsc6x_data->irq_num);

	free(tlsc6x_data);

	tlsc6x_data = NULL;

	return 0;
}


#ifdef TLSC6X_TEST_TASK

void tp_test_task(void *parm)
{
	int fd;
	int x = -1, y = -1;
	struct sunxi_input_event event;
	hal_trace_printf("tp_test_task run");
	memset(&event, 0, sizeof(struct sunxi_input_event));

	fd = sunxi_input_open(TLSC6X_DEV_NAME);
	if (fd < 0) {
		printf("gpio key open err\n");
		krhino_task_dyn_del(NULL);
	}

	while(1) {
		sunxi_input_read(fd, &event, sizeof(struct sunxi_input_event));

		if (event.type != EV_ABS) {
			continue;
		}
		switch (event.code) {
			case ABS_MT_POSITION_X:
				x = event.value;
				break;
			case ABS_MT_POSITION_Y:
				y = event.value;
				break;

		}
		if (x >= 0 && y >= 0) {
			x = -1;
			y = -1;
		}
	}
	krhino_task_dyn_del(NULL);
}

//static
void cmd_tlsc6x_test(void)
{
#if 0
	int ret = -1;

	ret = tlsc6x_init();
	if (ret < 0)
		printf("tlsc6x init fail\n");
#endif
	kstat_t task_ret;
	task_ret = krhino_task_dyn_create(&g_tptest_task, "tp_test_task", 0, 33, 0, (1024), (task_entry_t)tp_test_task, 1);
	if (task_ret != RHINO_SUCCESS) {
		printf("create tp read task err\n");
	}
}
//FINSH_FUNCTION_EXPORT_CMD(cmd_tlsc6x_test, tp_test, tlsc6x-test);
#endif

