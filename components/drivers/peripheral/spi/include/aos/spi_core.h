/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_SPI_CORE_H
#define AOS_SPI_CORE_H

#include <aos/device_core.h>
#include <aos/spi.h>

#define AOS_SPI_XF_MSG_HEAD     ((uint32_t)0x1 << 0)
#define AOS_SPI_XF_MSG_TAIL     ((uint32_t)0x1 << 1)
#define AOS_SPI_XF_SEQ_HEAD     ((uint32_t)0x1 << 2)
#define AOS_SPI_XF_SEQ_TAIL     ((uint32_t)0x1 << 3)
#define AOS_SPI_XF_RX           ((uint32_t)0x1 << 4)
#define AOS_SPI_XF_TX           ((uint32_t)0x1 << 5)

#ifndef AOS_SPI_BUF_SIZE
#define AOS_SPI_BUF_SIZE        0
#endif

struct aos_spi_ops;

typedef struct {
    aos_dev_t dev;

    /* must be initialized before registration */
    const struct aos_spi_ops *ops;
    uint32_t flags;
    uint32_t num_cs;
    uint32_t min_hz;
    uint32_t max_hz;

    aos_event_t event;
    struct {
        uint32_t timeout;
        uint32_t flags;
        uint32_t cfg;
        uint32_t cs;
        uint32_t hz;
        uint32_t pre_cs;
        uint32_t post_cs;
        uint32_t pre_clk;
        uint32_t post_clk;
        size_t len;
        size_t head;
        size_t tail;
        void *rx_buf;
        const void *tx_buf;
    } x;
#if AOS_SPI_BUF_SIZE > 0
    uint8_t rx_buf[2][AOS_SPI_BUF_SIZE];
    uint8_t tx_buf[2][AOS_SPI_BUF_SIZE];
#endif
} aos_spi_t;

typedef struct aos_spi_ops {
    void (*unregister)(aos_spi_t *);
    aos_status_t (*startup)(aos_spi_t *);
    void (*shutdown)(aos_spi_t *);
    aos_status_t (*start_xfer)(aos_spi_t *);
    void (*finish_xfer)(aos_spi_t *);
    void (*abort_xfer)(aos_spi_t *);
    aos_status_t (*transfer_sequence)(aos_spi_t *, const aos_spi_msg_t *, size_t);
} aos_spi_ops_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_spi_register(aos_spi_t *spi);
aos_status_t aos_spi_register_argumented(aos_spi_t *spi, uint32_t id, const aos_spi_ops_t *ops, uint32_t flags,
                                         uint32_t num_cs, uint32_t min_hz, uint32_t max_hz);
aos_status_t aos_spi_unregister(uint32_t id);

size_t aos_spi_hard_push(aos_spi_t *spi, void *tx_buf, size_t count);
bool aos_spi_hard_pull(aos_spi_t *spi, const void *rx_buf, size_t count);
void aos_spi_hard_fail(aos_spi_t *spi);

static inline const void *aos_spi_hard_tx_inplace(aos_spi_t *spi)
{
    if (!(spi->x.flags & AOS_SPI_XF_TX))
        return NULL;

    return &((const uint8_t *)spi->x.tx_buf)[spi->x.head];
}

static inline void *aos_spi_hard_rx_inplace(aos_spi_t *spi)
{
    if (!(spi->x.flags & AOS_SPI_XF_RX))
        return NULL;

    return &((uint8_t *)spi->x.rx_buf)[spi->x.tail];
}

static inline size_t aos_spi_hard_remain(aos_spi_t *spi)
{
    return spi->x.head - spi->x.tail;
}

static inline size_t aos_spi_xfer_remain(aos_spi_t *spi)
{
    return spi->x.len - spi->x.head;
}

#ifdef __cplusplus
}
#endif

#endif /* AOS_SPI_CORE_H */
