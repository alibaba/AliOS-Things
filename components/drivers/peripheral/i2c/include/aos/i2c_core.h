/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_I2C_CORE_H
#define AOS_I2C_CORE_H

#include <aos/device_core.h>
#include <aos/i2c.h>

#define AOS_I2C_F_ADDR_10       ((uint32_t)0x1 << 0)

#define AOS_I2C_XF_MSG_HEAD     ((uint32_t)0x1 << 0)
#define AOS_I2C_XF_MSG_TAIL     ((uint32_t)0x1 << 1)
#define AOS_I2C_XF_SEQ_HEAD     ((uint32_t)0x1 << 2)
#define AOS_I2C_XF_SEQ_TAIL     ((uint32_t)0x1 << 3)
#define AOS_I2C_XF_BUF_IDX      ((uint32_t)0x1 << 4)

#ifndef AOS_I2C_BUF_SIZE
#define AOS_I2C_BUF_SIZE        256
#endif

struct aos_i2c;

typedef struct {
    void (*unregister)(struct aos_i2c *);
    aos_status_t (*startup)(struct aos_i2c *);
    void (*shutdown)(struct aos_i2c *);
    aos_status_t (*start_xfer)(struct aos_i2c *);
    void (*finish_xfer)(struct aos_i2c *);
    void (*abort_xfer)(struct aos_i2c *);
} aos_i2c_ops_t;

typedef struct aos_i2c {
    aos_dev_t dev;

    /* must be initialized before registration */
    const aos_i2c_ops_t *ops;
    uint32_t flags;
    uint32_t hz;

    aos_spinlock_t lock;
    aos_event_t event;
    struct {
        uint32_t timeout;
        uint32_t flags;
        uint32_t cfg;
        uint16_t addr;
        size_t len;
        size_t pos[2];
    } x;
    uint8_t buf[2][AOS_I2C_BUF_SIZE];
} aos_i2c_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_i2c_register(aos_i2c_t *i2c);
aos_status_t aos_i2c_unregister(uint32_t id);

size_t aos_i2c_hard_push(aos_i2c_t *i2c, void *tx_buf, size_t count);
bool aos_i2c_hard_pull(aos_i2c_t *i2c, const void *rx_buf, size_t count);
void aos_i2c_hard_fail(aos_i2c_t *i2c);

static inline size_t aos_i2c_hard_remain(aos_i2c_t *i2c)
{
    return i2c->x.pos[0] - i2c->x.pos[1];
}

static inline size_t aos_i2c_xfer_remain(aos_i2c_t *i2c)
{
    return i2c->x.len - i2c->x.pos[0];
}

#ifdef __cplusplus
}
#endif

#endif /* AOS_I2C_CORE_H */
