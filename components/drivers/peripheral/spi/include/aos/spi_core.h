/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_SPI_CORE_H
#define AOS_SPI_CORE_H

#include <aos/device_core.h>
#include <aos/spi.h>

#define AOS_SPI_F_MODE_0        ((uint32_t)0x1 << 0)
#define AOS_SPI_F_MODE_1        ((uint32_t)0x1 << 1)
#define AOS_SPI_F_MODE_2        ((uint32_t)0x1 << 2)
#define AOS_SPI_F_MODE_3        ((uint32_t)0x1 << 3)
#define AOS_SPI_F_RX_1          ((uint32_t)0x1 << 4)
#define AOS_SPI_F_RX_2          ((uint32_t)0x1 << 5)
#define AOS_SPI_F_RX_4          ((uint32_t)0x1 << 6)
#define AOS_SPI_F_RX_8          ((uint32_t)0x1 << 7)
#define AOS_SPI_F_TX_1          ((uint32_t)0x1 << 8)
#define AOS_SPI_F_TX_2          ((uint32_t)0x1 << 9)
#define AOS_SPI_F_TX_4          ((uint32_t)0x1 << 10)
#define AOS_SPI_F_TX_8          ((uint32_t)0x1 << 11)
#define AOS_SPI_F_FULL_DUPLEX_1 ((uint32_t)0x1 << 12)
#define AOS_SPI_F_FULL_DUPLEX_2 ((uint32_t)0x1 << 13)
#define AOS_SPI_F_FULL_DUPLEX_4 ((uint32_t)0x1 << 14)
#define AOS_SPI_F_FULL_DUPLEX_8 ((uint32_t)0x1 << 15)
#define AOS_SPI_F_MSB_FIRST     ((uint32_t)0x1 << 16)
#define AOS_SPI_F_LSB_FIRST     ((uint32_t)0x1 << 17)
#define AOS_SPI_F_NO_CS         ((uint32_t)0x1 << 18)

#define AOS_SPI_XF_MSG_HEAD     ((uint32_t)0x1 << 0)
#define AOS_SPI_XF_MSG_TAIL     ((uint32_t)0x1 << 1)
#define AOS_SPI_XF_SEQ_HEAD     ((uint32_t)0x1 << 2)
#define AOS_SPI_XF_SEQ_TAIL     ((uint32_t)0x1 << 3)
#define AOS_SPI_XF_RX           ((uint32_t)0x1 << 4)
#define AOS_SPI_XF_TX           ((uint32_t)0x1 << 5)
#define AOS_SPI_XF_BUF_IDX      ((uint32_t)0x1 << 6)

#ifndef AOS_SPI_BUF_SIZE
#define AOS_SPI_BUF_SIZE        256
#endif

struct aos_spi;

typedef struct {
    void (*unregister)(struct aos_spi *);
    aos_status_t (*startup)(struct aos_spi *);
    void (*shutdown)(struct aos_spi *);
    aos_status_t (*start_xfer)(struct aos_spi *);
    void (*finish_xfer)(struct aos_spi *);
    void (*abort_xfer)(struct aos_spi *);
} aos_spi_ops_t;

typedef struct aos_spi {
    aos_dev_t dev;

    /* must be initialized before registration */
    const aos_spi_ops_t *ops;
    uint32_t flags;
    uint32_t num_cs;
    uint32_t min_hz;
    uint32_t max_hz;

    aos_spinlock_t lock;
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
        size_t pos[2];
    } x;
    uint8_t rx_buf[2][AOS_SPI_BUF_SIZE];
    uint8_t tx_buf[2][AOS_SPI_BUF_SIZE];
} aos_spi_t;

#ifdef __cplusplus
extern "C" {
#endif

aos_status_t aos_spi_register(aos_spi_t *spi);
aos_status_t aos_spi_unregister(uint32_t id);

size_t aos_spi_hard_push(aos_spi_t *spi, void *tx_buf, size_t count);
bool aos_spi_hard_pull(aos_spi_t *spi, const void *rx_buf, size_t count);
void aos_spi_hard_fail(aos_spi_t *spi);

static inline size_t aos_spi_hard_remain(aos_spi_t *spi)
{
    return spi->x.pos[0] - spi->x.pos[1];
}

static inline size_t aos_spi_xfer_remain(aos_spi_t *spi)
{
    return spi->x.len - spi->x.pos[0];
}

#ifdef __cplusplus
}
#endif

#endif /* AOS_SPI_CORE_H */
