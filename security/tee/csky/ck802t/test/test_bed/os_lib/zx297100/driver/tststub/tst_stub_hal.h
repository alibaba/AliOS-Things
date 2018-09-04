#ifndef __TST_STUB_HAL_H__
#define __TST_STUB_HAL_H__

#include "tee_types.h"

typedef struct _tst_stub_flash_info_t
{
    uint32_t size; /* blk-aligned */
    /*
     * ... ...
     * 128 = 2 ^ 7; order = 7;
     * 256 = 2 ^ 8; order = 8;
     * 512 = 2 ^ 9; order = 9;
     * ... ...
     */
    uint32_t order_of_blk_sz;
} tst_stub_flash_info_t;

typedef struct _tst_stub_hal_ops_t
{
    int32_t (*get_info)(tst_stub_flash_info_t *info);
    int32_t (*read)(uint32_t offset, void *buf, uint32_t count);
    int32_t (*write)(uint32_t offset, const void *buf, uint32_t count);
    int32_t (*erase)(uint32_t start_blk, uint32_t blk_num);
} tst_stub_hal_ops_t;

/* called by flash drivers, impl-ed by tst-stub. */
extern int32_t tst_stub_hal_reg(tst_stub_hal_ops_t *ops);
extern void    tst_stub_hal_unreg(void);

#endif /* __TST_STUB_HAL_H__ */
