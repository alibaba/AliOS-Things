/*
 * Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <driver/i2c.h>
#include "hal/atca_hal.h"
#include "esp_err.h"
#include "esp_log.h"

#define SDA_PIN                            16
#define SCL_PIN                            17
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */

#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL                    ESP_LOG_INFO
#endif

#define MAX_I2C_BUSES 2  //ESP32 has 2 I2C bus

typedef struct atcaI2Cmaster
{
    int id;
    int ref_ct;
    int bus_index;
} ATCAI2CMaster_t;

ATCAI2CMaster_t *i2c_hal_data[MAX_I2C_BUSES];
int i2c_bus_ref_ct = 0;
i2c_config_t conf;

const char* TAG = "HAL_I2C";

void hal_i2c_change_baud(ATCAIface iface, uint32_t speed)
{
    esp_err_t rc;
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;

    conf.master.clk_speed = speed;
    rc = i2c_param_config(i2c_hal_data[bus]->id, &conf);
    if (rc == ESP_OK)
    {
//        ESP_LOGD(TAG, "Baudrate Changed");
    }
    else
    {
//        ESP_LOGW(TAG, "Baudrate Change Failed");
    }
}

ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    esp_err_t rc;
    int bus = cfg->atcai2c.bus;
    ATCAHAL_t *phal = (ATCAHAL_t*)hal;

    if (i2c_bus_ref_ct == 0)
    {
        for (int i = 0; i < MAX_I2C_BUSES; i++)
        {
            i2c_hal_data[i] = NULL;
        }
    }

    i2c_bus_ref_ct++;

    if (bus >= 0 && bus < MAX_I2C_BUSES)
    {
        if (i2c_hal_data[bus] == NULL)
        {
            i2c_hal_data[bus] = malloc(sizeof(ATCAI2CMaster_t));
            i2c_hal_data[bus]->ref_ct = 1;

            switch (bus)
            {
            case 0:
                i2c_hal_data[bus]->id = I2C_NUM_0;
                break;
            case 1:
                i2c_hal_data[bus]->id = I2C_NUM_1;
                break;
            }

            conf.mode = I2C_MODE_MASTER;
            conf.sda_io_num = SDA_PIN;
            conf.scl_io_num = SCL_PIN;
            conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
            conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
            conf.master.clk_speed = 100000; //cfg->atcai2c.baud;
//            ESP_LOGI(TAG, "Configuring I2C");
            rc = i2c_param_config(i2c_hal_data[bus]->id, &conf);
//            ESP_LOGD(TAG, "I2C Param Config: %s", esp_err_to_name(rc));
            rc = i2c_driver_install(i2c_hal_data[bus]->id, I2C_MODE_MASTER, 0, 0, 0);
//            ESP_LOGD(TAG, "I2C Driver Install; %s", esp_err_to_name(rc));
            i2c_hal_data[bus]->bus_index = bus;
        }
        else
        {
            i2c_hal_data[bus]->ref_ct++;
        }

        phal->hal_data = i2c_hal_data[bus];

        return ATCA_SUCCESS;
    }
//    ESP_LOGE(TAG, "I2C init failed");
    return ATCA_COMM_FAIL;
}

ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = iface->mIfaceCFG;
    esp_err_t rc;

    txdata[0] = 0x03;              //Word Address value, Command Token as per datasheet of ATECC508A
    txlength++;
//    ESP_LOGD(TAG, "txdata: %p , txlength: %d", txdata, txlength);
//    ESP_LOG_BUFFER_HEXDUMP(TAG, txdata, txlength, 3);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    (void)i2c_master_start(cmd);
    (void)i2c_master_write_byte(cmd, cfg->atcai2c.slave_address | I2C_MASTER_WRITE, ACK_CHECK_EN);
    (void)i2c_master_write(cmd, txdata, txlength, ACK_CHECK_EN);
    (void)i2c_master_stop(cmd);
    rc = i2c_master_cmd_begin(cfg->atcai2c.bus, cmd, 10);
    (void)i2c_cmd_link_delete(cmd);

    if (ESP_OK != rc)
    {
        return ATCA_COMM_FAIL;
    }
    else
    {
        return ATCA_SUCCESS;
    }
}

ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = iface->mIfaceCFG;
    esp_err_t rc;
    i2c_cmd_handle_t cmd;
    int high = 0;
    int low = 0;

    cmd = i2c_cmd_link_create();
    (void)i2c_master_start(cmd);
    (void)i2c_master_write_byte(cmd, cfg->atcai2c.slave_address | I2C_MASTER_READ, ACK_CHECK_EN);
    (void)i2c_master_read_byte(cmd, rxdata, ACK_VAL);
    rc = i2c_master_cmd_begin(cfg->atcai2c.bus, cmd, 10);
    (void)i2c_cmd_link_delete(cmd);

    if (ESP_OK != rc)
    {
        return ATCA_COMM_FAIL;
    }

    *rxlength = rxdata[0];

    if (*rxlength > 1)
    {
        cmd = i2c_cmd_link_create();
        if (*rxlength > 2)
        {
            (void)i2c_master_read(cmd, &rxdata[1], (*rxlength) - 2, ACK_VAL);
        }
        (void)i2c_master_read_byte(cmd, rxdata + (*rxlength) - 1, NACK_VAL);
        (void)i2c_master_stop(cmd);
        rc = i2c_master_cmd_begin(cfg->atcai2c.bus, cmd, 10);
        (void)i2c_cmd_link_delete(cmd);
    }
    else
    {
        cmd = i2c_cmd_link_create();
        (void)i2c_master_stop(cmd);
        rc = i2c_master_cmd_begin(cfg->atcai2c.bus, cmd, 10);
        (void)i2c_cmd_link_delete(cmd);
    }

//    ESP_LOG_BUFFER_HEX(TAG, rxdata, *rxlength);

    if (ESP_OK != rc)
    {
        return ATCA_COMM_FAIL;
    }
    else
    {
        return ATCA_SUCCESS;
    }
}

ATCA_STATUS hal_i2c_release(void *hal_data)
{
    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    i2c_bus_ref_ct--;
    if (hal && --(hal->ref_ct) <= 0 && i2c_hal_data[hal->bus_index] != NULL)
    {
        i2c_driver_delete(hal->bus_index);
        free(i2c_hal_data[hal->bus_index]);
        i2c_hal_data[hal->bus_index] = NULL;
    }
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
//    uint32_t bdrt = cfg->atcai2c.baud;
    uint16_t rxlen;
    uint8_t data[4] = { 0 };
    const uint8_t expected[4] = { 0x04, 0x11, 0x33, 0x43 };

//    if (bdrt != 100000) {
//        hal_i2c_change_baud(iface, 100000);
//    }

    // 0x00 as wake up pulse
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    (void)i2c_master_start(cmd);
    (void)i2c_master_write_byte(cmd, I2C_MASTER_WRITE, ACK_CHECK_DIS);
    (void)i2c_master_stop(cmd);
    (void)i2c_master_cmd_begin(cfg->atcai2c.bus, cmd, 10);
    (void)i2c_cmd_link_delete(cmd);

    atca_delay_ms(10);   // wait tWHI + tWLO which is configured based on device type and configuration structure

//    if (bdrt != 100000)
//    {
//        hal_i2c_change_baud(iface, cfg->atcai2c.baud);
//    }

    rxlen = 4;
    hal_i2c_receive(iface, data, &rxlen);
    if (memcmp(data, expected, 4) == 0)
    {
//        ESP_LOGI(TAG, "I2C wake successful");
        return ATCA_SUCCESS;
    }
//    ESP_LOGW(TAG, "I2C wake failed");
    return ATCA_COMM_FAIL;
}

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t idle_data = 0x02;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    (void)i2c_master_start(cmd);
    (void)i2c_master_write_byte(cmd, cfg->atcai2c.slave_address | I2C_MASTER_WRITE, ACK_CHECK_EN);
    (void)i2c_master_write(cmd, &idle_data, 1, ACK_CHECK_DIS);
    (void)i2c_master_stop(cmd);
    (void)i2c_master_cmd_begin(cfg->atcai2c.bus, cmd, 10);
    (void)i2c_cmd_link_delete(cmd);
//    ESP_LOGI(TAG, "IDLE Command Sent");
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t sleep_data = 0x01;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    (void)i2c_master_start(cmd);
    (void)i2c_master_write_byte(cmd, cfg->atcai2c.slave_address | I2C_MASTER_WRITE, ACK_CHECK_EN);
    (void)i2c_master_write(cmd, &sleep_data, 1, ACK_CHECK_DIS);
    (void)i2c_master_stop(cmd);
    (void)i2c_master_cmd_begin(cfg->atcai2c.bus, cmd, 10);
    (void)i2c_cmd_link_delete(cmd);
//    ESP_LOGI(TAG, "hal_i2c_sleep");

    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
//    ESP_LOGI(TAG, "hal_i2c_discover_buses");
    return ATCA_UNIMPLEMENTED;
}

ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg *cfg, int *found)
{
//    ESP_LOGI(TAG, "hal_i2c_discover_devices");
    return ATCA_UNIMPLEMENTED;
}

