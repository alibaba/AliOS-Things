// Copyright 2020-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include "unity.h"
#include "unity_config.h"
#include "spi_bus.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SPI_SCK_IO 18
#define SPI_MOSI_IO 23
#define SPI_MISO_IO 19

void spi_bus_init_deinit_test()
{
    spi_bus_handle_t bus_handle = NULL;
    spi_config_t bus_conf = {
        .miso_io_num = SPI_MISO_IO,
        .mosi_io_num = SPI_MOSI_IO,
        .sclk_io_num = SPI_SCK_IO,
    };
    bus_handle = spi_bus_create(SPI2_HOST, &bus_conf);
    TEST_ASSERT(bus_handle != NULL);
    TEST_ASSERT(ESP_OK == spi_bus_delete(&bus_handle));
    TEST_ASSERT(bus_handle == NULL);
    bus_handle = spi_bus_create(SPI3_HOST, &bus_conf);
    TEST_ASSERT(bus_handle != NULL);
    TEST_ASSERT(ESP_OK == spi_bus_delete(&bus_handle));
    TEST_ASSERT(bus_handle == NULL);
}

/* connect mosi with miso for transfer test */
void spi_bus_transfer_test()
{
    spi_bus_handle_t bus_handle = NULL;
    spi_config_t bus_conf = {
        .miso_io_num = SPI_MISO_IO,
        .mosi_io_num = SPI_MOSI_IO,
        .sclk_io_num = SPI_SCK_IO,
    };

    spi_device_config_t device_conf = {
        .cs_io_num = NULL_SPI_CS_PIN,
        .mode = 0,
        .clock_speed_hz = 20 * 1000 * 1000,
    };

    bus_handle = spi_bus_create(SPI2_HOST, &bus_conf);
    TEST_ASSERT(bus_handle != NULL);
    spi_bus_device_handle_t device_handle = NULL;
    device_handle = spi_bus_device_create(bus_handle, &device_conf);
    TEST_ASSERT(device_handle != NULL);

    printf("************byte transfer test***************\n");
    for (uint8_t i = 0; i < 200; i++) {
        uint8_t in = 0;
        TEST_ASSERT(ESP_OK == spi_bus_transfer_byte(device_handle, i, &in));
        TEST_ASSERT_EQUAL_UINT8(i, in);
        printf("in=%u\n", in);
    }

    vTaskDelay(2);
    printf("************bytes transfer test***************\n");
    uint8_t data[200] = {0};
    uint8_t data_in[200] = {0};
    for (uint8_t i = 0; i < 200; i++) {
        data[i] = i;
    }
    TEST_ASSERT(ESP_OK == spi_bus_transfer_bytes(device_handle, data, data_in, 200));
    for (uint8_t i = 0; i < 200; i++) {
        printf("%u ", data_in[i]);
    }
    printf("\n");

    vTaskDelay(2);
    printf("************reg16 transfer test***************\n");
    for (uint16_t i = (0xffff - 200); i < 0xffff; i++) {
        uint16_t in = 0;
        TEST_ASSERT(ESP_OK == spi_bus_transfer_reg16(device_handle, i, &in));
        TEST_ASSERT_EQUAL_UINT16(i, in);
        printf("in=%u\n", in);
    }

    vTaskDelay(2);
    printf("************reg32 transfer test***************\n");
    for (uint32_t i = (0xffffffff - 200); i < 0xffffffff; i++) {
        uint32_t in = 0;
        TEST_ASSERT(ESP_OK == spi_bus_transfer_reg32(device_handle, i, &in));
        TEST_ASSERT_EQUAL_UINT32(i, in);
        printf("in=%x\n", in);
    }

    TEST_ASSERT(ESP_OK == spi_bus_device_delete(&device_handle));
    TEST_ASSERT(device_handle == NULL);
    TEST_ASSERT(ESP_OK == spi_bus_delete(&bus_handle));
    TEST_ASSERT(bus_handle == NULL);
}

TEST_CASE("spi bus init-deinit test", "[bus]spi_bus]")
{
    spi_bus_init_deinit_test();
}

TEST_CASE("spi bus transfer test", "[bus][spi_bus]")
{
    spi_bus_transfer_test();
}
