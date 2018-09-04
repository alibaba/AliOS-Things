#include <string.h>
#include "tst_stub_hal.h"
#include "zx29_efc.h"

extern int32_t printk(const int8_t *fmt, ...);

#define TST_STUB_BASE 0x10000000
#define CONFIG_TST_SIZE (512 * 8)
#define CONFIG_TST_BLKSIZE 512

int32_t tst_stub_get_info(tst_stub_flash_info_t *info)
{
    info->size = CONFIG_TST_SIZE;
    if (CONFIG_TST_BLKSIZE == 2048)
        info->order_of_blk_sz = 11;
    else if (CONFIG_TST_BLKSIZE == 1024)
        info->order_of_blk_sz = 10;
    else if (CONFIG_TST_BLKSIZE == 512)
        info->order_of_blk_sz = 9;
    else
        info->order_of_blk_sz = 9;

    return 0;
}

int32_t _tst_stub_read(uint32_t offset, void *buf, uint32_t count)
{
    uint32_t addr;

    if (buf == NULL) {
        return -1;
    }

    if (offset + count > CONFIG_TST_SIZE) {
        return -1;
    }

    addr = TST_STUB_BASE + offset;

    if (eflash_read(addr, buf, count) >= 0) {
        return count;
    }
    return -1;
}

int32_t _tst_stub_write(uint32_t offset, const void *buf, uint32_t count)
{
    uint32_t addr;

    if (buf == NULL) {
        return -1;
    }

    if (offset + count > CONFIG_TST_SIZE) {
        return -1;
    }

    addr = TST_STUB_BASE + offset;
    if (eflash_write_poll(addr, (uint8_t *)buf, count) >= 0) {
        return count;
    }
    return -1;
}

int32_t _tst_stub_erase(uint32_t start_blk, uint32_t blk_num)
{
    if ((blk_num > CONFIG_TST_SIZE / CONFIG_TST_BLKSIZE) || (blk_num < 0))
        return -1;

    return eflash_erase(TST_STUB_BASE + start_blk * CONFIG_TST_BLKSIZE,
                        blk_num);
}

static tst_stub_hal_ops_t _g_ops = {
    .get_info = tst_stub_get_info,
    .read     = _tst_stub_read,
    .write    = _tst_stub_write,
    .erase    = _tst_stub_erase,
};

extern int32_t tst_stub_init(void);
extern void    tst_stub_cleanup(void);

int32_t hal_stub_init(void)
{
    int32_t ret;

    ret = eflash_init();
    if (ret) {
        return -1;
    }

    ret = tst_stub_hal_reg(&_g_ops);
    if (ret) {
        return -1;
    }

#if 0
printk("tst sub test start\n");
extern void tst_stub_test(void);
tst_stub_test();
printk("tst sub test success\n");

    if (_tst_stub_erase(0, 8)) {
        printk("erase fail \n");
        while(1);
    }
#endif

    ret = tst_stub_init();
    if (ret) {
        return -1;
    }

    return ret;
}

void hal_stub_cleanup(void)
{
    tst_stub_cleanup();
    tst_stub_hal_unreg();
}

static void dump_stub(uint8_t *data, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (!(i % 8))
            printk("\r\n");
        printk("0x%02x ", data[i]);
    }
}

void tst_stub_test(void)
{
    uint8_t buf[64];

    printk("tst stub test ... \n");
    // eflash_init();
    // erase all 3 blocks
    if (_tst_stub_erase(0, 3)) {
        printk("erase fail \n");
        while (1)
            ;
    }

    memset(buf, 0, 64);
    _tst_stub_read(1, buf, 64);
    dump_stub(buf, 64);

    memset(buf, 0x5a, 64);
    _tst_stub_write(0, buf, 64);
    memset(buf, 0, 64);
    _tst_stub_read(0, buf, 64);

    dump_stub(buf, 64);

    memset(buf, 0x5a, 64);
    _tst_stub_write(1, buf, 64);
    memset(buf, 0, 64);
    _tst_stub_read(1, buf, 64);

    dump_stub(buf, 64);

    memset(buf, 0x5a, 64);
    _tst_stub_write(2, buf, 64);
    memset(buf, 0, 64);
    _tst_stub_read(2, buf, 64);

    dump_stub(buf, 64);

    if (_tst_stub_erase(0, 3)) {
        printk("erase fail \n");
        while (1)
            ;
    }

    // while(1);
}
