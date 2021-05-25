/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aos/kernel.h"
#include "aos/hal/flash.h"
#include "flash_test.h"

#define TEST_BUFFER_WORDS 512

uint16_t flash_test_buffer[TEST_BUFFER_WORDS];

void hal_flash_test(void)
{
    int      i = 0;
    int      j = 0;
    int      ret = 0;
    uint32_t off_set = 0;

    hal_logic_partition_t * partition_info = NULL;

    printf("*********** flash test start ! ***********\n");

    partition_info = hal_flash_get_info(HAL_PARTITION_TEST);

    if (partition_info == NULL) {
        printf("flash info get error ! test failed\n");
        return;
    } else {
        printf("test partition is %s\n", partition_info->partition_description);
        printf("test partition bytes: %d\n", partition_info->partition_length);
    }

    printf("step1: erase the flash !\n");

    ret = hal_flash_erase(HAL_PARTITION_TEST, 0, partition_info->partition_length);
    if (ret != 0) {
        printf("flash erase return error ! test failed\n");
        return;
    }

    printf("step2: check if the flash is completely erased!\n");

    off_set = 0;

    for (i = 0; i < partition_info->partition_length / (TEST_BUFFER_WORDS * sizeof(uint16_t)); i++)
    {
    	ret = hal_flash_read(HAL_PARTITION_TEST, &off_set, flash_test_buffer, (TEST_BUFFER_WORDS * sizeof(uint16_t)));

        if (ret != 0) {
            printf("flash read error ! test failed\n");
            return;
        }

        for (j = 0; j < TEST_BUFFER_WORDS; j++) {
        	if (flash_test_buffer[j] != 0xFFFF) {
                printf("flash erase data error ! test failed\n");
                return;
        	}
        }
    }

    printf("step3: write the flash !\n");

    off_set = 0;

    for (i = 0; i < partition_info->partition_length / (TEST_BUFFER_WORDS * sizeof(uint16_t)); i++)
    {
        for (j = 0; j < TEST_BUFFER_WORDS; j++) {
        	flash_test_buffer[j] = j + i * TEST_BUFFER_WORDS;
        }

    	ret = hal_flash_write(HAL_PARTITION_TEST, &off_set, flash_test_buffer, (TEST_BUFFER_WORDS * sizeof(uint16_t)));

        if (ret != 0) {
            printf("flash write error ! test failed\n");
            return;
        }
    }

    printf("step4: read the flash to check the data in flash !\n");

    off_set = 0;

    for (i = 0; i < partition_info->partition_length / (TEST_BUFFER_WORDS * sizeof(uint16_t)); i++)
    {
    	ret = hal_flash_read(HAL_PARTITION_TEST, &off_set, flash_test_buffer, (TEST_BUFFER_WORDS * sizeof(uint16_t)));

        if (ret != 0) {
            printf("flash read error ! test failed\n");
            return;
        }

        for (j = 0; j < TEST_BUFFER_WORDS; j++) {
        	if (flash_test_buffer[j] != j + i * TEST_BUFFER_WORDS) {
                printf("flash read data error ! test failed\n");
                return;
        	}
        }
    }

    printf("flash test result: succeed !\n");

    printf("*********** flash test end ! ***********\n");
}
