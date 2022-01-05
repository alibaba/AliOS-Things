/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_I2C_H
#define AOS_I2C_H

#include <aos/device.h>

/**
 * @defgroup i2c_api I2C Master
 * @ingroup driver_api
 * @brief AOS API for I2C master.
 * @{
 */

#define AOS_I2C_F_ADDR_10       ((uint32_t)0x1 << 0)

#define AOS_I2C_MCFG_TX         ((uint32_t)0x0 << 0)
#define AOS_I2C_MCFG_RX         ((uint32_t)0x1 << 0)
#define AOS_I2C_MCFG_ADDR_7     ((uint32_t)0x0 << 1)
#define AOS_I2C_MCFG_ADDR_10    ((uint32_t)0x1 << 1)

typedef aos_dev_ref_t aos_i2c_ref_t;

typedef struct {
    uint32_t flags;
    uint32_t hz;
} aos_i2c_info_t;

typedef struct {
    uint32_t cfg;
    uint16_t addr;
    size_t count;
    void *buf;
} aos_i2c_msg_t;

#define AOS_I2C_MSG_INIT_VAL \
    { \
        .cfg    = 0, \
        .addr   = 0, \
        .count  = 0, \
        .buf    = NULL, \
    }

#define aos_i2c_msg_init(x)     do { *(x) = (aos_i2c_msg_t)AOS_I2C_MSG_INIT_VAL; } while (0)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get an I2C master device.
 * @param   ref I2C ref to operate.
 * @param   id  I2C master device ID.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_i2c_get(aos_i2c_ref_t *ref, uint32_t id);
/**
 * @brief   Release an I2C master device.
 * @param   ref I2C ref to operate.
 * @return  None.
 */
void aos_i2c_put(aos_i2c_ref_t *ref);
/**
 * @brief   Get information about an I2C master device.
 * @param   ref     I2C ref to operate.
 * @param   info    Information structure.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_i2c_get_info(aos_i2c_ref_t *ref, aos_i2c_info_t *info);
/**
 * @brief   Change I2C clock frequency.
 * @param   ref I2C ref to operate.
 * @param   hz  Clock frequency.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_i2c_set_hz(aos_i2c_ref_t *ref, uint32_t hz);
/**
 * @brief   Synchronous I2C data transfer.
 * @param   ref         I2C ref to operate.
 * @param   msgs        Array of messages.
 * @param   num_msgs    Number of messages.
 * @return  0: on success; < 0: on failure.
 */
aos_status_t aos_i2c_transfer(aos_i2c_ref_t *ref, const aos_i2c_msg_t *msgs, size_t num_msgs);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* AOS_I2C_H */
