
//////////////////////////////////////////////////////////////////////////////
// Includes
//////////////////////////////////////////////////////////////////////////////

#include "../include/common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "lvgl/src/lv_hal/lv_hal_disp.h"

//////////////////////////////////////////////////////////////////////////////
// ILI9341 requires specific lv_conf resolution and color depth
//////////////////////////////////////////////////////////////////////////////

#if LV_COLOR_DEPTH != 16
#error "modILI9341: LV_COLOR_DEPTH must be set to 16!"
#endif

//////////////////////////////////////////////////////////////////////////////
// ILI9341 Module definitions
//////////////////////////////////////////////////////////////////////////////

typedef struct {
    mp_obj_base_t base;
    spi_device_handle_t spi;
    uint8_t mhz;
    uint8_t spihost;
    uint8_t miso;
    uint8_t mosi;
    uint8_t clk;
    uint8_t cs;
    uint8_t dc;
    uint8_t rst;
    uint8_t backlight;

} ILI9341_t;

// Unfortunately, lvgl doesnt pass user_data to callbacks, so we use this global.
// This means we can have only one active display driver instance, pointed by this global.
STATIC ILI9341_t *g_ILI9341 = NULL;

STATIC mp_obj_t ILI9341_make_new(const mp_obj_type_t *type,
                                 size_t n_args,
                                 size_t n_kw,
                                 const mp_obj_t *all_args);

STATIC mp_obj_t mp_init_ILI9341(mp_obj_t self_in);

STATIC mp_obj_t mp_activate_ILI9341(mp_obj_t self_in)
{
    ILI9341_t *self = MP_OBJ_TO_PTR(self_in);
    g_ILI9341 = self;
    return mp_const_none;
}

STATIC void ili9431_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_init_ILI9341_obj, mp_init_ILI9341);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_activate_ILI9341_obj, mp_activate_ILI9341);
DEFINE_PTR_OBJ(ili9431_flush);

STATIC const mp_rom_map_elem_t ILI9341_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_init_ILI9341_obj) },
    { MP_ROM_QSTR(MP_QSTR_activate), MP_ROM_PTR(&mp_activate_ILI9341_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&PTR_OBJ(ili9431_flush)) },
};

STATIC MP_DEFINE_CONST_DICT(ILI9341_locals_dict, ILI9341_locals_dict_table);

STATIC const mp_obj_type_t ILI9341_type = {
    { &mp_type_type },
    .name = MP_QSTR_ILI9341,
    //.print = ILI9341_print,
    .make_new = ILI9341_make_new,
    .locals_dict = (mp_obj_dict_t*)&ILI9341_locals_dict,
};

STATIC mp_obj_t ILI9341_make_new(const mp_obj_type_t *type,
                                 size_t n_args,
                                 size_t n_kw,
                                 const mp_obj_t *all_args)
{
    enum{
         ARG_mhz,
         ARG_spihost,
         ARG_miso,
         ARG_mosi,
         ARG_clk,
         ARG_cs,
         ARG_dc,
         ARG_rst,
         ARG_backlight,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mhz,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=40}},
        { MP_QSTR_spihost,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=HSPI_HOST}},
        { MP_QSTR_miso,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},             
        { MP_QSTR_mosi,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},
        { MP_QSTR_clk,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},
        { MP_QSTR_cs,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},
        { MP_QSTR_dc,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},
        { MP_QSTR_rst,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},
        { MP_QSTR_backlight,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int=-1}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    ILI9341_t *self = m_new_obj(ILI9341_t);
    self->base.type = type;
    self->spi = NULL;
    self->mhz = args[ARG_mhz].u_int;
    self->spihost = args[ARG_spihost].u_int;
    self->miso = args[ARG_miso].u_int;
    self->mosi = args[ARG_mosi].u_int;
    self->clk = args[ARG_clk].u_int;
    self->cs = args[ARG_cs].u_int;
    self->dc = args[ARG_dc].u_int;
    self->rst = args[ARG_rst].u_int;
    self->backlight = args[ARG_backlight].u_int;

    return MP_OBJ_FROM_PTR(self);
}


STATIC const mp_rom_map_elem_t ILI9341_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ILI9341) },
        { MP_ROM_QSTR(MP_QSTR_display), (mp_obj_t)&ILI9341_type},
};
         

STATIC MP_DEFINE_CONST_DICT (
    mp_module_ILI9341_globals,
    ILI9341_globals_table
);

const mp_obj_module_t mp_module_ILI9341 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ILI9341_globals
};

//////////////////////////////////////////////////////////////////////////////
// ILI9341 driver implementation
//////////////////////////////////////////////////////////////////////////////

STATIC void disp_spi_init(ILI9341_t *self)
{
	esp_err_t ret;

	spi_bus_config_t buscfg={
		.miso_io_num=self->miso,
		.mosi_io_num=self->mosi,
		.sclk_io_num=self->clk,
		.quadwp_io_num=-1,
		.quadhd_io_num=-1,
		.max_transfer_sz=128*1024,
	};

	spi_device_interface_config_t devcfg={
		.clock_speed_hz=self->mhz*1000*1000, //Clock out at DISP_SPI_MHZ MHz
		.mode=0,                             //SPI mode 0
		.spics_io_num=self->cs,              //CS pin
		.queue_size=1,
		.pre_cb=NULL,
		.post_cb=NULL,
		.flags=SPI_DEVICE_HALFDUPLEX,
		.duty_cycle_pos=128,
	};

	gpio_pad_select_gpio(self->miso);
    gpio_pad_select_gpio(self->mosi);
    gpio_pad_select_gpio(self->clk);

    gpio_set_direction(self->miso, GPIO_MODE_INPUT);
    gpio_set_pull_mode(self->miso, GPIO_PULLUP_ONLY);
    gpio_set_direction(self->mosi, GPIO_MODE_OUTPUT);
    gpio_set_direction(self->clk, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(self->cs);

	//Initialize the SPI bus
	ret=spi_bus_initialize(self->spihost, &buscfg, 1);
    if (ret != ESP_OK) nlr_raise(
        mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed initializing SPI bus")));

	//Attach the LCD to the SPI bus
	ret=spi_bus_add_device(self->spihost, &devcfg, &self->spi);
    if (ret != ESP_OK) nlr_raise(
        mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed adding SPI device")));
}

STATIC void disp_spi_send(ILI9341_t *self, const uint8_t * data, uint16_t length)
{
	if (length == 0) return;           //no need to send anything

	spi_transaction_t t;
    memset(&t, 0, sizeof(t));       	//Zero out the transaction
	t.length = length * 8;              //Length is in bytes, transaction length is in bits.
	t.tx_buffer = data;               	//Data

//	esp_err_t ret;
//	ret=spi_device_transmit(spi, &t);  //Transmit!
//	assert(ret==ESP_OK);            	 //Should have had no issues.

	spi_device_queue_trans(self->spi, &t, portMAX_DELAY);

	spi_transaction_t * rt;
	spi_device_get_trans_result(self->spi, &rt, portMAX_DELAY);
}

STATIC void ili9441_send_cmd(ILI9341_t *self, uint8_t cmd)
{
	gpio_set_level(self->dc, 0);	 /*Command mode*/
	disp_spi_send(self, &cmd, 1);
}

STATIC void ili9341_send_data(ILI9341_t *self, const void * data, uint16_t length)
{
	gpio_set_level(self->dc, 1);	 /*Data mode*/
	disp_spi_send(self, data, length);
}

/*The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct. */
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

STATIC const lcd_init_cmd_t ili_init_cmds[]={
		{0xCF, {0x00, 0x83, 0X30}, 3},
		{0xED, {0x64, 0x03, 0X12, 0X81}, 4},
		{0xE8, {0x85, 0x01, 0x79}, 3},
		{0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
		{0xF7, {0x20}, 1},
		{0xEA, {0x00, 0x00}, 2},
		{0xC0, {0x26}, 1},			/*Power control*/
		{0xC1, {0x11}, 1},			/*Power control */
		{0xC5, {0x35, 0x3E}, 2},	/*VCOM control*/
		{0xC7, {0xBE}, 1},			/*VCOM control*/
		{0x36, {0x48}, 1},			/*Memory Access Control*/
		{0x3A, {0x55}, 1},			/*Pixel Format Set*/
		{0xB1, {0x00, 0x1B}, 2},
		{0xF2, {0x08}, 1},
		{0x26, {0x01}, 1},
		{0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
		{0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
		{0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
		{0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
		{0x2C, {0}, 0},
		{0xB7, {0x07}, 1},
		{0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
		{0x11, {0}, 0x80},
		{0x29, {0}, 0x80},
		{0, {0}, 0xff},
	};

STATIC mp_obj_t mp_init_ILI9341(mp_obj_t self_in)
{
    ILI9341_t *self = MP_OBJ_TO_PTR(self_in);
    mp_activate_ILI9341(self_in);

    disp_spi_init(self);
    gpio_pad_select_gpio(self->dc);

	//Initialize non-SPI GPIOs
	gpio_set_direction(self->dc, GPIO_MODE_OUTPUT);
	gpio_set_direction(self->rst, GPIO_MODE_OUTPUT);
	if (self->backlight != -1) gpio_set_direction(self->backlight, GPIO_MODE_OUTPUT);

	//Reset the display
	gpio_set_level(self->rst, 0);
	vTaskDelay(100 / portTICK_RATE_MS);
	gpio_set_level(self->rst, 1);
	vTaskDelay(100 / portTICK_RATE_MS);


	// printf("ILI9341 initialization.\n");


	//Send all the commands
	uint16_t cmd = 0;
	while (ili_init_cmds[cmd].databytes!=0xff) {
		ili9441_send_cmd(self, ili_init_cmds[cmd].cmd);
		ili9341_send_data(self, ili_init_cmds[cmd].data, ili_init_cmds[cmd].databytes & 0x1F);
		if (ili_init_cmds[cmd].databytes & 0x80) {
			vTaskDelay(100 / portTICK_RATE_MS);
		}
		cmd++;
	}

	///Enable backlight
	//printf("Enable backlight.\n");
	if (self->backlight != -1) gpio_set_level(self->backlight, 1);       
    return mp_const_none;
}


STATIC void ili9431_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	uint8_t data[4];

    ILI9341_t *self = g_ILI9341;

	/*Column addresses*/
	ili9441_send_cmd(self, 0x2A);
	data[0] = (area->x1 >> 8) & 0xFF;
	data[1] = area->x1 & 0xFF;
	data[2] = (area->x2 >> 8) & 0xFF;
	data[3] = area->x2 & 0xFF;
	ili9341_send_data(self, data, 4);

	/*Page addresses*/
	ili9441_send_cmd(self, 0x2B);
	data[0] = (area->y1 >> 8) & 0xFF;
	data[1] = area->y1 & 0xFF;
	data[2] = (area->y2 >> 8) & 0xFF;
	data[3] = area->y2 & 0xFF;
	ili9341_send_data(self, data, 4);

	/*Memory write*/
	ili9441_send_cmd(self, 0x2C);

	uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);

	/*Byte swapping is required*/
	uint32_t i;
	uint8_t * color_u8 = (uint8_t *) color_p;
	uint8_t color_tmp;
	for(i = 0; i < size * 2; i += 2) {
		color_tmp = color_u8[i + 1];
		color_u8[i + 1] = color_u8[i];
		color_u8[i] = color_tmp;
	}

	ili9341_send_data(self, (void*)color_p, size * 2);

	/*
	while(size > LV_HOR_RES) {

		ili9341_send_data((void*)color_p, LV_HOR_RES * 2);
		//vTaskDelay(10 / portTICK_PERIOD_MS);
		size -= LV_HOR_RES;
		color_p += LV_HOR_RES;
	}

	ili9341_send_data((void*)color_p, size * 2);	*/ /*Send the remaining data*/

	lv_disp_flush_ready(disp_drv);
}
