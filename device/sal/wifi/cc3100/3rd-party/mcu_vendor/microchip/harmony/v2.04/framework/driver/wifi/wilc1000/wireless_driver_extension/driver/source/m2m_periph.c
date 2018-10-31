/*******************************************************************************
  WILC1000 Peripherals Application Interface

  File Name:
    m2m_periph.c
  Summary:
    WILC1000 Peripherals Application Interface

  Description:
    WILC1000 Peripherals Application Interface
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#include "driver/include/m2m_periph.h"
#include "driver/source/nmasic.h"
#include "driver/source/m2m_hif.h"

#define GPIO_OP_DIR     0
#define GPIO_OP_SET     1
#define GPIO_OP_GET     2

static sint8 get_gpio_idx(uint8 u8GpioNum)
{
	if(u8GpioNum >= M2M_PERIPH_GPIO_MAX) return -1;
	if(u8GpioNum == M2M_PERIPH_GPIO15) { return 15;
	} else if(u8GpioNum == M2M_PERIPH_GPIO16) { return 16;
	} else if(u8GpioNum == M2M_PERIPH_GPIO18) { return 18;
	} else {
		return -2;
	}
}

/**
 * \brief GPIO read/write skeleton with wakeup/sleep capability. 
 */
static sint8 gpio_ioctl(uint8 op, uint8 u8GpioNum, uint8 u8InVal, uint8 * pu8OutVal)
{
	sint8 ret, gpio;

	ret = hif_chip_wake();
	if(ret != M2M_SUCCESS) goto _EXIT;

	gpio = get_gpio_idx(u8GpioNum);
	if(gpio < 0) goto _EXIT1;

	if(op == GPIO_OP_DIR) {
		ret = set_gpio_dir((uint8)gpio, u8InVal);
	} else if(op == GPIO_OP_SET) {	
		ret = set_gpio_val((uint8)gpio, u8InVal);
	} else if(op == GPIO_OP_GET) {
		ret = get_gpio_val((uint8)gpio, pu8OutVal);
	}
	if(ret != M2M_SUCCESS) goto _EXIT1;

_EXIT1:
	ret = hif_chip_sleep();
_EXIT:
	return ret;
}

sint8 m2m_periph_init(tstrPerphInitParam * param)
{
	return M2M_SUCCESS;
}

sint8 m2m_periph_gpio_set_dir(uint8 u8GpioNum, uint8 u8GpioDir)
{
	return gpio_ioctl(GPIO_OP_DIR, u8GpioNum, u8GpioDir, NULL);
}

sint8 m2m_periph_gpio_set_val(uint8 u8GpioNum, uint8 u8GpioVal)
{
	return gpio_ioctl(GPIO_OP_SET, u8GpioNum, u8GpioVal, NULL);
}

sint8 m2m_periph_gpio_get_val(uint8 u8GpioNum, uint8 * pu8GpioVal)
{
	return gpio_ioctl(GPIO_OP_GET, u8GpioNum, 0, pu8GpioVal);
}

sint8 m2m_periph_gpio_pullup_ctrl(uint8 u8GpioNum, uint8 u8PullupEn)
{
	/* TBD */
	return M2M_SUCCESS;
}

sint8 m2m_periph_i2c_master_init(tstrI2cMasterInitParam * param)
{
	/* TBD */
	return M2M_SUCCESS;
}

sint8 m2m_periph_i2c_master_write(uint8 u8SlaveAddr, uint8 * pu8Buf, uint16 u16BufLen, uint8 flags)
{
	/* TBD */
	return M2M_SUCCESS;
}

sint8 m2m_periph_i2c_master_read(uint8 u8SlaveAddr, uint8 * pu8Buf, uint16 u16BufLen, uint16 * pu16ReadLen, uint8 flags)
{
	/* TBD */
	return M2M_SUCCESS;
}

sint8 m2m_periph_pullup_ctrl(uint32 pinmask, uint8 enable)
{		
	return pullup_ctrl(pinmask, enable);	
}
