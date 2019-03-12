/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_CAN_H
#define HAL_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * CAN handle configuration
 */
typedef struct {
    uint32_t baud_rate;           /* baud rate of can */
    uint8_t  ide;                 /* 0:normal can, 1:extend can */
    uint8_t  auto_bus_off;        /* 1:enable auto bus off, 0:disable */
    uint8_t  auto_retry_transmit; /* 1:enable retry transmit, 0:disable */
} can_config_t;

/*
 * CAN device description
 */
typedef struct {
    uint8_t      port;   /* can port */
    can_config_t config; /* can config */
    void        *priv;   /* priv data */
} can_dev_t;

/*
 * CAN frameheader config
 */
typedef struct {
    uint32_t id;  /* id of can */
    uint8_t  rtr; /* 0:data frame, 1:remote frame */
    uint8_t  dlc; /* must <=8 */
} can_frameheader_t;

/*
 * CAN filter_item config
 */
typedef struct{
    uint8_t  rtr;         /* 0:data frame, 1:remote frame */
    uint32_t check_id;    /* the filter identification number */
    uint32_t filter_mask; /* the filter mask number or identification number */
} can_filter_item_t;

/**
 * Initialises a CAN interface
 *
 * @param[in]  can  the interface which should be initialised
 *
 * @return  0 : on success, EINVAL : if an error occurred with any step
 */
int32_t hal_can_init(can_dev_t *can);

/**
 * config a CAN fliter
 *
 * @param[in]  can             the interface which should be initialised
 * @param[in]  filter_grp_cnt  0 will make all id pass. This value must be <=10
 * @param[in]  filter_config   point to a filter config
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_filter_init(can_dev_t *can, const uint8_t filter_grp_cnt, can_filter_item_t *filter_config);

/**
 * Transmit data by CAN
 *
 * @param[in]  can        the can interface
 * @param[in]  tx_header  frame head
 * @param[in]  data       pointer to the start of data
 * @param[in]  timeout    timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                        if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_send(can_dev_t *can, can_frameheader_t *tx_header, const void *data, const uint32_t timeout);

/**
 * Receive data by CAN
 *
 * @param[in]   can        the can interface
 * @param[out]  rx_header  frame head
 * @param[out]  data       pointer to the start of data
 * @param[in]   timeout    timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                         if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_recv(can_dev_t *can, can_frameheader_t *rx_header, void *data, const uint32_t timeout);

/**
 * Deinitialises a CAN interface
 *
 * @param[in]  can  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_finalize(can_dev_t *can);

#ifdef __cplusplus
}
#endif

#endif /* HAL_CAN_H */

