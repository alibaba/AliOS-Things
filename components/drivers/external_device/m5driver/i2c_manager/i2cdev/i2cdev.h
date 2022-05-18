/*

SPDX-License-Identifier: MIT

MIT License

Copyright (c) 2021 Rop Gonggrijp.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


#ifndef __I2CDEV_H__
#define __I2CDEV_H__

#ifdef __cplusplus
extern "C" {
#endif


// i2cdev-compatible I2C device descriptor.
// Everything but the port and addr is ignored, port properties handled by I2C Manager
typedef struct
{
    i2c_port_t port;
    i2c_config_t cfg;
    uint8_t addr;
    SemaphoreHandle_t mutex;
    uint32_t timeout_ticks;
} i2c_dev_t;

esp_err_t i2cdev_init();
esp_err_t i2cdev_done();
esp_err_t i2c_dev_create_mutex(i2c_dev_t *dev);
esp_err_t i2c_dev_delete_mutex(i2c_dev_t *dev);
esp_err_t i2c_dev_take_mutex(i2c_dev_t *dev);
esp_err_t i2c_dev_give_mutex(i2c_dev_t *dev);
esp_err_t i2c_dev_read(const i2c_dev_t *dev, const void *out_data,
        size_t out_size, void *in_data, size_t in_size);
esp_err_t i2c_dev_write(const i2c_dev_t *dev, const void *out_reg,
        size_t out_reg_size, const void *out_data, size_t out_size);
esp_err_t i2c_dev_read_reg(const i2c_dev_t *dev, uint8_t reg,
        void *in_data, size_t in_size);
esp_err_t i2c_dev_write_reg(const i2c_dev_t *dev, uint8_t reg,
        const void *out_data, size_t out_size);


#ifdef __cplusplus
}
#endif

#endif /* __I2CDEV_H__ */
