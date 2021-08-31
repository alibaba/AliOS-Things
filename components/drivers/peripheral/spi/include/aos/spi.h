/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_SPI_H
#define AOS_SPI_H

#include <aos/device.h>

#define AOS_SPI_MCFG_MODE_MASK  ((uint32_t)0x3 << 0)
#define AOS_SPI_MCFG_MODE_0     ((uint32_t)0x0 << 0)
#define AOS_SPI_MCFG_MODE_1     ((uint32_t)0x1 << 0)
#define AOS_SPI_MCFG_MODE_2     ((uint32_t)0x2 << 0)
#define AOS_SPI_MCFG_MODE_3     ((uint32_t)0x3 << 0)
#define AOS_SPI_MCFG_WIDTH_MASK ((uint32_t)0x3 << 2)
#define AOS_SPI_MCFG_WIDTH_1    ((uint32_t)0x0 << 2)
#define AOS_SPI_MCFG_WIDTH_2    ((uint32_t)0x1 << 2)
#define AOS_SPI_MCFG_WIDTH_4    ((uint32_t)0x2 << 2)
#define AOS_SPI_MCFG_WIDTH_8    ((uint32_t)0x3 << 2)
#define AOS_SPI_MCFG_MSB_FIRST  ((uint32_t)0x0 << 4)
#define AOS_SPI_MCFG_LSB_FIRST  ((uint32_t)0x1 << 4)

typedef aos_dev_ref_t aos_spi_ref_t;

typedef struct {
    uint32_t cfg;
    uint32_t cs;
    uint32_t hz;
    uint32_t pre_cs;
    uint32_t post_cs;
    uint32_t pre_clk;
    uint32_t post_clk;
    size_t count;
    void *rx_buf;
    const void *tx_buf;
} aos_spi_msg_t;

#define AOS_SPI_MSG_INIT_VAL \
    { \
        .cfg            = 0, \
        .cs             = 0, \
        .hz             = 0, \
        .pre_cs         = 0, \
        .post_cs        = 0, \
        .pre_clk        = 0, \
        .post_clk       = 0, \
        .count          = 0, \
        .rx_buf         = NULL, \
        .tx_buf         = NULL, \
    }

#define aos_spi_msg_init(x)     do { *(x) = (aos_spi_msg_t)AOS_SPI_MSG_INIT_VAL; } while (0)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Get a SPI device.
 * @param[out]  ref     SPI ref to operate
 * @param[in]   id      SPI device ID
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_spi_get(aos_spi_ref_t *ref, uint32_t id);
/**
 * @brief       Release a SPI device.
 * @param[in]   ref     SPI ref to operate
 * @return      None
 */
void aos_spi_put(aos_spi_ref_t *ref);
/**
 * @brief       Synchronous SPI data transfer.
 * @param[in]   ref             SPI ref to operate
 * @param[in]   msgs            array of messages
 * @param[in]   num_msgs        number of messages
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_spi_transfer(aos_spi_ref_t *ref, const aos_spi_msg_t *msgs, size_t num_msgs);

#ifdef __cplusplus
}
#endif

#endif /* AOS_SPI_H */
