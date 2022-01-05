/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_I2C_CORE_H
#define AOS_I2C_CORE_H

#include <aos/device_core.h>
#include <aos/i2c.h>

#define AOS_I2C_XF_MSG_HEAD     ((uint32_t)0x1 << 0)
#define AOS_I2C_XF_MSG_TAIL     ((uint32_t)0x1 << 1)
#define AOS_I2C_XF_SEQ_HEAD     ((uint32_t)0x1 << 2)
#define AOS_I2C_XF_SEQ_TAIL     ((uint32_t)0x1 << 3)

#ifndef AOS_I2C_BUF_SIZE
#define AOS_I2C_BUF_SIZE        0
#endif

struct aos_i2c_ops;

typedef struct {
    aos_dev_t dev;

    /* must be initialized before registration */
    const struct aos_i2c_ops *ops;
    uint32_t flags;
    uint32_t hz;

    aos_event_t event;
    struct {
        uint32_t timeout;
        uint32_t flags;
        uint32_t cfg;
        uint16_t addr;
        size_t len;
        size_t head;
        size_t tail;
        void *buf;
    } x;
#if AOS_I2C_BUF_SIZE > 0
    uint8_t buf[2][AOS_I2C_BUF_SIZE];
#endif
} aos_i2c_t;

typedef struct aos_i2c_ops {
    void (*unregister)(aos_i2c_t *);
    aos_status_t (*startup)(aos_i2c_t *);
    void (*shutdown)(aos_i2c_t *);
    aos_status_t (*set_hz)(aos_i2c_t *);
    aos_status_t (*start_xfer)(aos_i2c_t *);
    void (*finish_xfer)(aos_i2c_t *);
    void (*abort_xfer)(aos_i2c_t *);
    aos_status_t (*transfer_sequence)(aos_i2c_t *, const aos_i2c_msg_t *, size_t);
} aos_i2c_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_i2c_register(aos_i2c_t *i2c);
aos_status_t aos_i2c_register_argumented(aos_i2c_t *i2c, uint32_t id, const aos_i2c_ops_t *ops,
                                         uint32_t flags, uint32_t hz);
aos_status_t aos_i2c_unregister(uint32_t id);

size_t aos_i2c_hard_push(aos_i2c_t *i2c, void *tx_buf, size_t count);
bool aos_i2c_hard_pull(aos_i2c_t *i2c, const void *rx_buf, size_t count);
void aos_i2c_hard_fail(aos_i2c_t *i2c);

static inline const void *aos_i2c_hard_tx_inplace(aos_i2c_t *i2c)
{
    if (i2c->x.cfg & AOS_I2C_MCFG_RX)
        return NULL;

    return &((const uint8_t *)i2c->x.buf)[i2c->x.head];
}

static inline void *aos_i2c_hard_rx_inplace(aos_i2c_t *i2c)
{
    if (!(i2c->x.cfg & AOS_I2C_MCFG_RX))
        return NULL;

    return &((uint8_t *)i2c->x.buf)[i2c->x.tail];
}

static inline size_t aos_i2c_hard_remain(aos_i2c_t *i2c)
{
    return i2c->x.head - i2c->x.tail;
}

static inline size_t aos_i2c_xfer_remain(aos_i2c_t *i2c)
{
    return i2c->x.len - i2c->x.head;
}

#ifdef __cplusplus
}
#endif

#endif /* AOS_I2C_CORE_H */
