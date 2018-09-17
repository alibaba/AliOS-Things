/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 * Filename     : eflash.c
 * Author       : Jun Lin
 * Date Created : 24/12/2015
 * Description  : eflash driver
 */

/*
 ******************************
 *          HEADERS
 ******************************
 */
#include "device.h"
#include "eflash_priv.h"
#include "tee_eflash.h"
#include "tee_dbg.h"
#include "tee_addr_map.h"
#include "hal_memmap.h"

/*
 ******************************
 *          MACROS
 ******************************
 */
#define ROUNDUP(a, b) (((a) + ((b)-1)) & ~((b)-1))
#define ROUNDDOWN(a, b) ((a) & ~((b)-1))

#define TEE_DRV_EFLASH_NAME "eflash"

#ifdef CONFIG_EFLASH_DRV_DBG
#define FLASH_DRV_DBG_E(str, x...)                                      \
    do {                                                                \
        tee_dbg_print(ERR, "%s:%d: " str, __FUNCTION__, __LINE__, ##x); \
    } while (0)

#define FLASH_DRV_DBG_I(str, x...)                                      \
    do {                                                                \
        tee_dbg_print(INF, "%s:%d: " str, __FUNCTION__, __LINE__, ##x); \
    } while (0)
#else
#define FLASH_DRV_DBG_E(str, x...) TEE_ERROR(-3)
#define FLASH_DRV_DBG_I(str, x...)
#endif

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
static dev_t *_g_eflash_dev = NULL;

/*
 ******************************
 *          FUNCTIONS
 ******************************
 */

/* Function: int32_t _eflash_read(dev_t *dev, int32_t pos, void *buf, int32_t
 * length) granularity is 1 bytes
 *
 * pos: the start address of this operation
 * buf: the point to the source buf
 * length: the program length in bytes
 *
 * Return:
 *   -1 --  FAILURE;     >0 -- SUCCESS read size
 */
static int32_t __eflash_read(dev_t *dev, int32_t pos, void *buf, int32_t length)
{
    uint32_t copied   = 0;
    uint32_t word_len = length / sizeof(uint32_t);
    uint32_t left_len = length % sizeof(uint32_t);
    int32_t  i        = 0;
    uint8_t *src = NULL, *dst = NULL;

    FLASH_DRV_DBG_I("read name %s, pos 0x%08x buf 0x%08x size 0x%08x\n",
                    dev->name, pos, buf, length);

    if (pos < FLASH_START || (pos + length) > (FLASH_START + FLASH_SIZE)) {
        FLASH_DRV_DBG_E("bad args\n");
        return EARG;
    }
    if (0 == length) {
        return copied;
    }

    while (copied < word_len) {
        *((uint32_t *)buf + copied) = *((uint32_t *)pos + copied);
        copied++;
    }

    if (left_len) {
        dst = (uint8_t *)((uint32_t *)buf + copied);
        src = (uint8_t *)((uint32_t *)pos + copied);
        for (i = 0; i < left_len; ++i) {
            *(dst + i) = *(src + i);
        }
    }

    return (copied * sizeof(uint32_t) + i);
}

/*
 * erase a page of the flash device
 * granularity is 1 SECTOR
 *
 * addr: start address of the page
 * sec_num: erase sector numboer
 * Return:
 *   0 --  SUCCESS;    -1 -- FAILURE
 */
static int32_t __eflash_erase(dev_t *dev, uint32_t addr, uint32_t sec_num)
{
    unsigned long fbase    = FLASH_CTRL_BASEADDR;
    uint32_t      src_addr = addr;

    if (addr < FLASH_START ||
        (ROUNDDOWN(addr, SECTOR_SIZE) + sec_num * SECTOR_SIZE) >
          (FLASH_START + FLASH_SIZE)) {
        FLASH_DRV_DBG_E("bad args\n");
        return EARG;
    }

    if (0 == sec_num) {
        return SUCCESS;
    }

    while (sec_num--) {
        *(volatile uint32_t *)(fbase + FLASH_ADDR_OFS) = src_addr;
        *(volatile uint32_t *)(fbase + FLASH_PE_OFS)   = 0x1;
        src_addr += SECTOR_SIZE;
    }

    return SUCCESS;
}

static int32_t ___eflash_save(uint32_t *src_addr, uint32_t size,
                              uint32_t *dst_addr)
{
    uint32_t i = 0;

    if (0 != size % 4 || (uint32_t)src_addr < FLASH_START ||
        (uint32_t)src_addr > FLASH_START + FLASH_SIZE) {
        FLASH_DRV_DBG_E("bad args\n");
        return EARG;
    }
    if (0 == size) {
        return SUCCESS;
    } else {
        for (i = 0; i < (size / 4); i++) {
            *(dst_addr + i) = *(src_addr + i);
        }
    }

    return SUCCESS;
}

static int32_t ___do_eflash_program(uint32_t w_addr, uint32_t *buf,
                                    uint32_t b_length)
{
    uint32_t *    address  = (uint32_t *)w_addr;
    uint32_t      word     = 0;
    unsigned long fbase    = FLASH_CTRL_BASEADDR;
    uint32_t      w_length = 0;

    if (0 != b_length % 4 || w_addr < FLASH_START ||
        w_addr > FLASH_START + FLASH_SIZE) {
        FLASH_DRV_DBG_E("bad args\n");
        return EARG;
    }
    if (0 == b_length) {
        return SUCCESS;
    }

    w_length = b_length / 4;
    while (w_length--) {
        word                                            = (uint32_t)(*buf);
        *(volatile uint32_t *)(fbase + FLASH_ADDR_OFS)  = (uint32_t)address;
        *(volatile uint32_t *)(fbase + FLASH_WDATA_OFS) = word;
        *(volatile uint32_t *)(fbase + FLASH_WE_OFS)    = 0x1;
        buf++;
        address++;
    }

    return SUCCESS;
}

/* Function: int32_t _eflash_write(dev_t *dev, int32_t pos, void *buf, int32_t
 * size) program data into the flash device, note that before you call this
 * function; you should call flash_erase first. granularity is 4 bytes
 *
 * w_addr:   the start address of this operation
 * w_buf:    the point to the source buf
 * h_length: the program length in bytes
 *
 * Return:
 *   -1 --  FAILURE;     >0 -- SUCCESS write size
 */
static int32_t __eflash_write(dev_t *dev, int32_t pos, void *buf, int32_t size)
{
    uint32_t addr      = (uint32_t)pos;
    uint32_t pre_space = 0, later_space = 0, nums = 0;
    uint32_t pre_addr = 0, later_addr = 0;
    uint32_t tmp[128];
    int32_t  ret = SUCCESS;

    if (0 != pos % 4 || 0 != size % 4) {
        FLASH_DRV_DBG_E("bad args\n");
        return EARG;
    }

    FLASH_DRV_DBG_I("eflash write name %s, pos 0x%08x buf 0x%08x size 0x%08x\n",
                    dev->name, pos, buf, size);

    pre_addr  = ROUNDDOWN(addr, SECTOR_SIZE);
    pre_space = (addr - pre_addr);

    later_addr  = ROUNDDOWN(addr + size, SECTOR_SIZE);
    later_space = (addr + size - later_addr);

    nums = (size + SECTOR_SIZE - 1) / SECTOR_SIZE;

    FLASH_DRV_DBG_I(
      "nums %d pre addr 0x%08x pre_spaceb 0x%08x later addr 0x%08x "
      "later sapce 0x%08x\n",
      nums, pre_addr, pre_space, later_addr, later_space);
    if (0 == later_space) {
        ret = ___eflash_save((uint32_t *)pre_addr, pre_space, tmp);
        TEE_ASSERT(SUCCESS == ret);
        ret = __eflash_erase(dev, pre_addr, nums);
        TEE_ASSERT(SUCCESS == ret);
        ret = ___do_eflash_program(pre_addr, tmp, pre_space);
        TEE_ASSERT(SUCCESS == ret);
        ret = ___do_eflash_program(addr, (uint32_t *)buf, size);
        TEE_ASSERT(SUCCESS == ret);
    } else {
        if (pre_addr == later_addr) { /* range in 1 SECTOR_SIZE */
            ret = ___eflash_save((uint32_t *)pre_addr, SECTOR_SIZE, tmp);
            TEE_ASSERT(SUCCESS == ret);
            ret = __eflash_erase(dev, pre_addr, nums);
            TEE_ASSERT(SUCCESS == ret);
            ret = ___do_eflash_program(pre_addr, tmp, SECTOR_SIZE);
            TEE_ASSERT(SUCCESS == ret);
        } else {
            /* save pre_addr */
            ret = ___eflash_save((uint32_t *)pre_addr, pre_space, tmp);
            TEE_ASSERT(SUCCESS == ret);
            ret = __eflash_erase(dev, pre_addr, nums);
            TEE_ASSERT(SUCCESS == ret);
            ret = ___do_eflash_program(pre_addr, tmp, pre_space);
            TEE_ASSERT(SUCCESS == ret);

            /* save later_addr */
            ret = ___eflash_save((uint32_t *)(addr + size),
                                 SECTOR_SIZE - later_space, tmp);
            TEE_ASSERT(SUCCESS == ret);
            ret = __eflash_erase(dev, addr + size, 1);
            TEE_ASSERT(SUCCESS == ret);
            ret =
              ___do_eflash_program(addr + size, tmp, SECTOR_SIZE - later_space);
            TEE_ASSERT(SUCCESS == ret);
        }

        ret = ___do_eflash_program(addr, (uint32_t *)buf, size);
        TEE_ASSERT(SUCCESS == ret);
    }

    return (int32_t)size;
}

static int32_t _eflash_open(dev_t *dev)
{
    return SUCCESS;
}

static int32_t _eflash_close(dev_t *dev)
{
    return SUCCESS;
}

static int32_t _eflash_ioctl(dev_t *dev, int32_t cmd, void *arg)
{
    int32_t ret  = SUCCESS;
    void *  args = arg;

    FLASH_DRV_DBG_I("eflash ioctl name %s, cmd 0x%08x, ref 0x%08x\n", dev->name,
                    cmd, dev->dev_ref);
    switch (cmd) {
        case EFLASH_ERASE:
            ret = __eflash_erase(dev, ((tee_eflash_erase_args_t *)args)->addr,
                                 ((tee_eflash_erase_args_t *)args)->sec_num);
            break;
        default:
            TEE_ASSERT(0);
            break;
    }

    return ret;
}

static dev_ops_t _g_eflash_ops = {
    .open    = _eflash_open,
    .close   = _eflash_close,
    .ioctl   = _eflash_ioctl,
    .read    = __eflash_read,
    .write   = __eflash_write,
    .suspend = NULL,
    .resume  = NULL,
};

static int32_t _eflash_init(void)
{
    FLASH_DRV_DBG_I("tee_eflash_probe\n");

    _g_eflash_dev = dev_register(TEE_DRV_EFLASH_NAME, &_g_eflash_ops);
    if (NULL == _g_eflash_dev) {
        FLASH_DRV_DBG_E("tee_eflash_probe: dev regs fail\n");
        return -1;
    }

#if 0
    FLASH_DRV_DBG_I("tee eflash test\n");
    eflash_test();
#endif
    return SUCCESS;
}

DEV_INIT(_eflash_init, (uint8_t *)TEE_DRV_EFLASH_NAME);
