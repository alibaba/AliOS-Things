#ifndef TLSC6X_H
#define TLSC6X_H

#include <stdio.h>

#include "console.h"
#include "sunxi-input.h"
#include "input-event-codes.h"
#include "hal_iomux_haas1000.h"

#ifdef DEBUG
#define tlsc_info(x...) hal_trace_printf("[tlsc] " x)
#define TLSC_FUNC_ENTER() hal_trace_printf("[tlsc]%s: Enter\n", __func__)
#define TLSC_FUNC_EXIT() hal_trace_printf("[tlsc]%s: Exit\n", __func__)
#else
#define tlsc_info(x...)
#define tlsc_err(x...)
#define TLSC_FUNC_ENTER()
#define TLSC_FUNC_EXIT()
#endif

#define tlsc_err(x...) printf("[tlsc][error] " x)

#define swap(a, b) \
	do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

int tlsc6x_init(void);

struct tlsc6x_hw_cfg {
	int twi_id;
	int addr;
	int int_gpio;
	int reset_gpio;
	int screen_max_x;
	int screen_max_y;
	int revert_x_flag;
	int revert_y_flag;
	int exchange_x_y_flag;
};
struct ts_event {
	u16 x1;
	u16 y1;
	u16 x2;
	u16 y2;
	u16 x3;
	u16 y3;
	u16 x4;
	u16 y4;
	u16 x5;
	u16 y5;
	u16 pressure;
	u8 touch_point;
};
struct tlsc6x_drv_data {
	struct tlsc6x_hw_cfg	*config;
	struct gpio_dev_t *irq_num;

	osSemaphoreId	irq_sem;
	osMutexId	mutex;

	struct sunxi_input_dev *input_dev;
	struct ts_event		event;

};

int tlsc6x_i2c_read(struct tlsc6x_drv_data *data, char *writebuf, int writelen,
			char* readbuf, int readlen);

int tlsc6x_i2c_write(struct tlsc6x_drv_data *data, char *writebuf, int writelen);

#endif  /*TLSC6X_H*/
