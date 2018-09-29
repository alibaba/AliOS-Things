/**
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/* Look this link for the example note: https://github.com/SAML21IOTSK/saml21iot-sk */

#include <atmel_start.h>
#include "mx_debug.h"
#include "mx_common.h"
#include "alicloud_sds.h"

#include "drv_board.h"
#include "main.h"

const emh_alisds_config_t alisds_config =
{
	.product_info = {
		.name			= "microchip-002",
		.module			= "MICROCHIP_LIVING_AIRBOX_MICROCHIP_002",
		.key			= "Dkqt9OjYC0u0DIWGajKP",
		.secret			= "ciubDzkEOKVi0WS2VZzqAGGIgdmW1dsSatitz6Ie",
		.format			= EMH_ARG_ALISDS_FORMAT_JSON,
	},
	.dev_info = {
		.type			= "AIRBOX",
		.category		= "LIVING",
		.manufacture	= "MICROCHIP",
	}
};

void usr_btn_isr(void);
void usr_clicked_handler(void);
void usr_long_pressed_handler(void);

btn_instance_t usr_btn =
{
	.port                   = GPIO_PORTA,
	.pin					= PIN_PA02,
	.io_irq                 = usr_btn_isr,
	.idle					= IOBUTTON_IDLE_STATE_HIGH,
	.long_pressed_timeout	= 5000,
	.pressed_func           = usr_clicked_handler,
	.long_pressed_func		= usr_long_pressed_handler,
};

void usr_btn_isr(void)
{
	button_irq_handler(&usr_btn);
}

void usr_clicked_handler(void)
{
	alisds_provision();
}

void usr_long_pressed_handler(void)
{
	app_log("Restore default settings");

	OLED_ShowStatusString("Restore default");

	alisds_restore();
}


int application_start(int argc, char *argv[])
{
	mx_status err = kNoErr;

	app_log("AliOS started...");

	drv_board_init();
	drv_board_test();

	OLED_ShowString(OLED_DISPLAY_COLUMN_START, OLED_DISPLAY_ROW_1, "Microchip");

	err = alisds_init(&alisds_config, ALI_HANDLE_MAX);
	require_noerr(err, exit);

	rgbled_task_init();
	SHT20_task_init();
	switch_task_init();
	console_task_init();

	button_init(&usr_btn);

	while(1)
	{
		/* Application tick */
		alisds_runloop();
		SHT20_task();
		switch_task();
		button_srv(&usr_btn);
		krhino_task_yield();
	}

exit:
	app_log("App exit reason %d", err);
	while(1);
}


