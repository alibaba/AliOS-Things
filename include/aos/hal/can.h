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
 * CAN Operating Mode
 */
typedef enum {
    NORMAL_MODE,
    LOOPBACK_MODE,
    SILENT_MODE,
    SILENT_LOOPBACK_MODE
} hal_can_opt_mode_t;

/*
 * CAN Synchronization Jump Width
 */
typedef enum {
    SJW_1TQ,
    SJW_2TQ,
    SJW_3TQ,
    SJW_4TQ
} hal_can_sync_jump_width_t;

/*
 * CAN Time Quantum in Bit Segment 1
 */
typedef enum {
    BS1_1TQ,
    BS1_2TQ,
    BS1_3TQ,
    BS1_4TQ,
    BS1_5TQ,
    BS1_6TQ,
    BS1_7TQ,
    BS1_8TQ,
    BS1_9TQ,
    BS1_10TQ,
    BS1_11TQ,
    BS1_12TQ,
    BS1_13TQ,
    BS1_14TQ,
    BS1_15TQ,
    BS1_16TQ
} hal_can_time_quantum_bs1_t;

/*
 * CAN Time Quantum in Bit Segment 2
 */
typedef enum {
    BS2_1TQ,
    BS2_2TQ,
    BS2_3TQ,
    BS2_4TQ,
    BS2_5TQ,
    BS2_6TQ,
    BS2_7TQ,
    BS2_8TQ,
    BS2_9TQ,
    BS2_10TQ,
    BS2_11TQ,
    BS2_12TQ,
    BS2_13TQ,
    BS2_14TQ,
    BS2_15TQ,
    BS2_16TQ
} hal_can_time_quantum_bs2_t;

/*
 * CAN Filter Mode
 */
typedef enum {
    IDMASK_FILTERMODE,
    IDLIST_FILTERMODE
} hal_can_filter_mode_t;

/*
 * CAN Filter Scale
 */
typedef enum {
    FILTERSCALE_16BIT,
    FILTERSCALE_32BIT
} hal_can_filter_scale_t;

/*
 * CAN Filter FIFO
 */
typedef enum {
    FILTER_FIFO0,
    FILTER_FIFO1,
    FILTER_FIFO2,
    FILTER_FIFO3
} hal_can_filter_fifo_t;

/*
 * CAN handle configuration
 */
typedef struct {
    uint32_t                   prescaler;
    hal_can_opt_mode_t         opt_mode;
    hal_can_sync_jump_width_t  sync_jump_width;
    hal_can_time_quantum_bs1_t time_quantum_bs1;
    hal_can_time_quantum_bs2_t time_quantum_bs2;

    /* set 1 to enable 0 to disable */
    uint8_t time_triggered_mode;
    uint8_t auto_busOff;
    uint8_t auto_wakeup;
    uint8_t auto_retransmission;
    uint8_t receive_fifo_locked;
    uint8_t transmit_fifo_priority;
} can_config_t;

/*
 * CAN filter configuration
 */
typedef struct {
    uint32_t               filter_id_high;
    uint32_t               filter_id_low;
    uint32_t               filter_mask_id_high;
    uint32_t               filter_mask_id_low;
    hal_can_filter_fifo_t  filter_fifo_assignment;
    uint32_t               filter_bank;
    hal_can_filter_mode_t  filter_mode;
    hal_can_filter_scale_t filter_scale;
    uint8_t                filter_activation;
    uint32_t               slave_start_filter_bank;
} can_filter_config_t;

typedef struct {
    uint32_t std_id;
    uint32_t ext_id;
    uint32_t IDE;
    uint32_t RTR;
    uint32_t DLC;
} can_frameheader_t;

typedef struct {
    uint8_t      port;   /* can port */
    can_config_t config; /* can config */
    void        *priv;   /* priv data */
} can_dev_t;

/**
 * Initialises a CAN interface
 *
 * @param[in]  can  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_init(can_dev_t *can);

/**
 * config a CAN fliter
 *
 * @param[in]  can           the interface which should be initialised
 * @param[in]  FilterConfig  the config of filter
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_filter_init(can_dev_t *can, can_filter_config_t *FilterConfig);

/**
 * Transmit data by CAN
 *
 * @param[in]  can        the can interface
 * @param[in]  tx_header  frame head
 * @param[in]  data       pointer to the start of data
 * @param[in]  size       number of bytes to transmit
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_send(can_dev_t *can, can_frameheader_t *tx_header, const void *data, uint32_t size);

/**
 * Receive data by CAN
 *
 * @param[in]  can        the can interface
 * @param[in]  tx_header  frame head
 * @param[in]  data       pointer to the start of data
 * @param[in]  size       number of bytes to receive
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_recv(can_dev_t *can, can_frameheader_t *rx_header, void *data, uint32_t size);

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
#endif /* __cplusplus */

#endif /* HAL_CAN_H */
