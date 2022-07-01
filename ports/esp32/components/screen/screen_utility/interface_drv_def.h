// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _INTERFACE_DRV_DEF_H_
#define _INTERFACE_DRV_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**< Define the function of interface instance */
#define LCD_WRITE_CMD(cmd)      g_lcd_handle.interface_drv->write_cmd(g_lcd_handle.interface_drv, (cmd))
#define LCD_WRITE_DATA(data)    g_lcd_handle.interface_drv->write_data(g_lcd_handle.interface_drv, (data))
#define LCD_WRITE(data, length) g_lcd_handle.interface_drv->write(g_lcd_handle.interface_drv, (data), (length))
#define LCD_READ(data, length)  g_lcd_handle.interface_drv->read(g_lcd_handle.interface_drv, (data), (length))
#define LCD_IFACE_ACQUIRE()     g_lcd_handle.interface_drv->bus_acquire(g_lcd_handle.interface_drv)
#define LCD_IFACE_RELEASE()     g_lcd_handle.interface_drv->bus_release(g_lcd_handle.interface_drv)

static inline esp_err_t LCD_WRITE_REG(uint16_t cmd, uint16_t data)
{
    esp_err_t ret;
    ret = LCD_WRITE_CMD(cmd);
    ret |= LCD_WRITE_DATA(data);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

#ifdef __cplusplus
}
#endif

#endif
