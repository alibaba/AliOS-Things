/**
 ******************************************************************************
 * @file    mx_hal_i2c.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   I2C sensor rw driver
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#include <stdio.h>
#include "mx_hal.h"

#include <driver_init.h>
#include <hal_i2c_m_sync.h>

/******************************************************************************
 *                                 Structures
 ******************************************************************************/

struct _i2c_instance {
    uint32_t place_holder;
};

/******************************************************************************
 *                              Variable Definitions
 ******************************************************************************/

extern struct i2c_m_sync_desc I2C_0;
/* NULL handler for this driver, complete in future */
static struct _i2c_instance i2c;

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

void* mx_hal_i2c_init(void *config)
{
    UNUSED_PARAMETER(config);

	I2C_0_init();
	i2c_m_sync_set_baudrate(&I2C_0, 0, 100);
	i2c_m_sync_enable(&I2C_0);

    return &i2c;
}


/**
 * \brief Sync version of i2c write command
 */
int32_t mx_hal_i2c_cmd_write(void *instance, uint16_t slave_addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    UNUSED_PARAMETER(instance);
	
    mx_status err = kNoErr;
	i2c_m_sync_set_slaveaddr(&I2C_0, slave_addr, 1);
	err = i2c_m_sync_cmd_write(&I2C_0, reg, buffer, length);
	require_noerr(err, exit);
	
exit:
	return err;	
}

int32_t mx_hal_i2c_cmd_read(void *instance, uint16_t slave_addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
	UNUSED_PARAMETER(instance);

	mx_status err = kNoErr;
	i2c_m_sync_set_slaveaddr(&I2C_0, slave_addr, 1);
		
	err = i2c_m_sync_cmd_read(&I2C_0, reg, buffer, length);
	require_noerr(err, exit);

exit:
	return err;
}
