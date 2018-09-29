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
#include <atmel_start.h>

#include "mx_debug.h"
#include "mx_common.h"
#include "alicloud_sds.h"

#include "drv_board.h"
#include "main.h"

static bool  switch_1 = false;
static bool  switch_2 = false;


mx_status switch_task_init(void)
{
	alisds_attr_t attr;
	switch_1 = gpio_get_pin_level(SW_1);
	switch_2 = gpio_get_pin_level(SW_2);

	attr.name = "switch1";
	attr.type = ALI_ATT_TYPE_BOOL;
	attr.read_func = handle_read_cur_switch_1;
	attr.write_func = NULL;
	alisds_attr_init(ALI_HANDLE_IO_SWITCH_1, attr);

	attr.name = "switch2";
	attr.type = ALI_ATT_TYPE_BOOL;
	attr.read_func = handle_read_cur_switch_2;
	attr.write_func = NULL;
	alisds_attr_init(ALI_HANDLE_IO_SWITCH_2, attr);

	return kNoErr;
}

void switch_task(void)
{
	if (switch_1 != gpio_get_pin_level(SW_1)) {
		switch_1 = gpio_get_pin_level(SW_1);
		alisds_attr_indicate_by_handle(ALI_HANDLE_IO_SWITCH_1);
	}

	if (switch_2 != gpio_get_pin_level(SW_2)) {
		switch_2 = gpio_get_pin_level(SW_2);
		alisds_attr_indicate_by_handle(ALI_HANDLE_IO_SWITCH_2);
	}
}


/* Switch attribute handlers */
mx_status handle_read_cur_switch_1		(alisds_att_val_t *value)
{
	(*value).boolValue = (switch_1==true)? false:true;
	return kNoErr;
}

mx_status handle_read_cur_switch_2		(alisds_att_val_t *value)
{
	(*value).boolValue = (switch_2==true)? false:true;
	return kNoErr;
}