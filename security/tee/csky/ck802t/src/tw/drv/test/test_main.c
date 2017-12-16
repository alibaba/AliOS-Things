/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 *
 * Filename     : eflash.c
 * Author       : Jun Lin
 * Date Created : 24/12/2015
 * Description  : e flash driver
 */

/*
 ******************************
 *          HEADERS
 ******************************
 */
#include "device.h"
#include "eflash_priv.h"
#include "tee_addr_map.h"
#include "tee_eflash.h"
#include "tee_dbg.h"

/*
 ******************************
 *          MACROS
 ******************************
 */
#define TEE_DRV_EFLASH_NAME       "eflash"
#define TEE_TEST_EFLASH_START_ADDR  EFLASH_START + 8
#define TEE_TEST_EFLASH_SIZE        SECTOR_SIZE + 32

#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

/*
 ******************************
 *          TYPES
 ******************************
 */

/*
 ******************************
 *          VARIABLES
 ******************************
 */

/*
 ******************************
 *          FUNCTIONS
 ******************************
 */
void eflash_test(void)
{
    dev_t *ef_dev = NULL;
    tee_eflash_read_args_t  read_args;
    tee_eflash_write_args_t write_args;
    tee_eflash_erase_args_t erase_args;
    int32_t ret;

    ef_dev = dev_open(TEE_DRV_EFLASH_NAME);
    if (NULL == ef_dev) {
        tee_dbg_print(INF, "dev open elfash drv fail\n");
        return;
    }

    tee_dbg_print(INF, "=========================================write\n");
    write_args.addr = ROUNDDOWN(TEE_TEST_EFLASH_START_ADDR, SECTOR_SIZE);
    write_args.buf = malloc(TEE_TEST_EFLASH_SIZE);
    if (NULL == write_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    *((uint32_t *)write_args.buf) = 0xffeeddcc;
    *((uint32_t *)write_args.buf + 1) = 0xccddeeff;
    tee_dbg_print(INF, "test write pos 0x%08x buffer 0x%08x,data 0x%08x 0x%08x\n",
            write_args.addr, write_args.buf,
            *((uint32_t *)write_args.buf), *((uint32_t *)write_args.buf + 1));
    write_args.size = TEE_TEST_EFLASH_SIZE;
    dev_write(ef_dev, write_args.addr, write_args.buf, write_args.size);
    free(write_args.buf);

    tee_dbg_print(INF, "=========================================read\n");
    read_args.addr = ROUNDDOWN(TEE_TEST_EFLASH_START_ADDR, SECTOR_SIZE);
    read_args.buf = malloc(TEE_TEST_EFLASH_SIZE);
    if (NULL == read_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    read_args.size = TEE_TEST_EFLASH_SIZE;
    ret = dev_read(ef_dev, read_args.addr, read_args.buf, read_args.size);
    if (read_args.size != ret) {
        tee_dbg_print(INF, "test read eflash err 0x%08x", ret);
    }
    tee_dbg_print(INF, "test read eflash pos 0x%08x bufer 0x%08x "
            "data 0x%08x 0x%08x 0x%08x 0x%08x\n",
           read_args.addr, read_args.buf,
           *((uint32_t *)read_args.buf), *((uint32_t *)read_args.buf + 1),
           *((uint32_t *)read_args.buf + 2), *((uint32_t *)read_args.buf + 3));
    free(read_args.buf);

    tee_dbg_print(INF, "=========================================write\n");
    write_args.addr = TEE_TEST_EFLASH_START_ADDR;
    write_args.buf = malloc(TEE_TEST_EFLASH_SIZE);
    if (NULL == write_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    *((uint32_t *)write_args.buf) = 0x12345678;
    *((uint32_t *)write_args.buf + 1) = 0x87654321;
    tee_dbg_print(INF, "test write pos 0x%08x buffer 0x%08x,data 0x%08x 0x%08x\n",
            write_args.addr, write_args.buf,
            *((uint32_t *)write_args.buf), *((uint32_t *)write_args.buf + 1));
    write_args.size = TEE_TEST_EFLASH_SIZE;
    dev_write(ef_dev, write_args.addr, write_args.buf, write_args.size);
    free(write_args.buf);

    tee_dbg_print(INF, "=========================================read\n");
    read_args.addr = ROUNDDOWN(TEE_TEST_EFLASH_START_ADDR, SECTOR_SIZE);
    read_args.buf = malloc(TEE_TEST_EFLASH_SIZE);
    if (NULL == read_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    read_args.size = TEE_TEST_EFLASH_SIZE;
    dev_read(ef_dev, read_args.addr, read_args.buf, read_args.size);
    if (read_args.size != ret) {
        tee_dbg_print(INF, "test read eflash err 0x%08x", ret);
    }
    tee_dbg_print(INF, "test read eflash pos 0x%08x bufer 0x%08x "
            "data 0x%08x 0x%08x 0x%08x 0x%08x\n",
           read_args.addr, read_args.buf,
           *((uint32_t *)read_args.buf), *((uint32_t *)read_args.buf + 1),
           *((uint32_t *)read_args.buf + 2), *((uint32_t *)read_args.buf + 3));
    free(read_args.buf);

    tee_dbg_print(INF, "=========================================write\n");
    write_args.addr = TEE_TEST_EFLASH_START_ADDR + 2*SECTOR_SIZE;
    write_args.buf = malloc(4);
    if (NULL == write_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    *((uint32_t *)write_args.buf) = 0x11223344;
    tee_dbg_print(INF, "test write pos 0x%08x buffer 0x%08x,data 0x%08x\n",
            write_args.addr, write_args.buf, *((uint32_t *)write_args.buf));
    write_args.size = 4;
    dev_write(ef_dev, write_args.addr, write_args.buf, write_args.size);
    free(write_args.buf);

    tee_dbg_print(INF, "=========================================read\n");
    read_args.addr = ROUNDDOWN(TEE_TEST_EFLASH_START_ADDR, SECTOR_SIZE);
    read_args.buf = malloc(TEE_TEST_EFLASH_SIZE);
    if (NULL == read_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    read_args.size = TEE_TEST_EFLASH_SIZE;
    dev_read(ef_dev, read_args.addr, read_args.buf, read_args.size);
    if (read_args.size != ret) {
        tee_dbg_print(INF, "test read eflash err 0x%08x", ret);
    }
    tee_dbg_print(INF, "1111test read eflash pos 0x%08x bufer 0x%08x "
            "data 0x%08x 0x%08x 0x%08x 0x%08x\n",
           read_args.addr, read_args.buf,
           *((uint32_t *)read_args.buf), *((uint32_t *)read_args.buf + 1),
           *((uint32_t *)read_args.buf + 2), *((uint32_t *)read_args.buf + 3));
    free(read_args.buf);

    tee_dbg_print(INF, "=========================================read\n");
    read_args.addr = TEE_TEST_EFLASH_START_ADDR + 2*SECTOR_SIZE;
    read_args.buf = malloc(TEE_TEST_EFLASH_SIZE);
    if (NULL == read_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    read_args.size = TEE_TEST_EFLASH_SIZE;
    dev_read(ef_dev, read_args.addr, read_args.buf, read_args.size);
    if (read_args.size != ret) {
        tee_dbg_print(INF, "test read eflash err 0x%08x", ret);
    }
    tee_dbg_print(INF, "test read eflash pos 0x%08x bufer 0x%08x "
            "data 0x%08x 0x%08x 0x%08x 0x%08x\n",
           read_args.addr, read_args.buf,
           *((uint32_t *)read_args.buf), *((uint32_t *)read_args.buf + 1),
           *((uint32_t *)read_args.buf + 2), *((uint32_t *)read_args.buf + 3));
    free(read_args.buf);

    tee_dbg_print(INF, "=========================================erase\n");
    erase_args.addr = TEE_TEST_EFLASH_START_ADDR;
    erase_args.sec_num = 1;
    dev_ioctl(ef_dev, EFLASH_ERASE, (void *)&erase_args);
    tee_dbg_print(INF, "test erase eflash addr 0x%08x sec_num 0x%08x\n",
                        erase_args.addr, erase_args.sec_num);

    tee_dbg_print(INF, "=========================================read\n");
    read_args.addr = ROUNDDOWN(TEE_TEST_EFLASH_START_ADDR, SECTOR_SIZE);
    read_args.buf = malloc(TEE_TEST_EFLASH_SIZE);
    if (NULL == read_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    read_args.size = TEE_TEST_EFLASH_SIZE;
    dev_read(ef_dev, read_args.addr, read_args.buf, read_args.size);
    if (read_args.size != ret) {
        tee_dbg_print(INF, "test read eflash err 0x%08x", ret);
    }
    tee_dbg_print(INF, "test read eflash pos 0x%08x bufer 0x%08x "
            "data 0x%08x 0x%08x 0x%08x 0x%08x\n",
           read_args.addr, read_args.buf,
           *((uint32_t *)read_args.buf), *((uint32_t *)read_args.buf + 1),
           *((uint32_t *)read_args.buf + 2), *((uint32_t *)read_args.buf + 3));
    free(read_args.buf);

    tee_dbg_print(INF, "=========================================read\n");
    read_args.addr = TEE_TEST_EFLASH_START_ADDR + 2*SECTOR_SIZE;
    read_args.buf = malloc(TEE_TEST_EFLASH_SIZE);
    if (NULL == read_args.buf) {
        tee_dbg_print(INF, "out of memory\n");
    }
    read_args.size = TEE_TEST_EFLASH_SIZE;
    dev_read(ef_dev, read_args.addr, read_args.buf, read_args.size);
    tee_dbg_print(INF, "test read eflash pos 0x%08x bufer 0x%08x "
            "data 0x%08x 0x%08x 0x%08x 0x%08x\n",
           read_args.addr, read_args.buf,
           *((uint32_t *)read_args.buf), *((uint32_t *)read_args.buf + 1),
           *((uint32_t *)read_args.buf + 2), *((uint32_t *)read_args.buf + 3));
    free(read_args.buf);
}
