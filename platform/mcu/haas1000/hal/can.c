/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/can.h"


/**
 * Initialises a CAN interface
 *
 * @param[in]  can  the interface which should be initialised
 *
 * @return  0 : on success, EINVAL : if an error occurred with any step
 */
int32_t hal_can_init(can_dev_t *can)
{
	return 0;
}

/**
 * config a CAN fliter
 *
 * @param[in]  can             the interface which should be initialised
 * @param[in]  filter_grp_cnt  0 will make all id pass. This value must be <=10
 * @param[in]  filter_config   point to a filter config
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_filter_init(can_dev_t *can, const uint8_t filter_grp_cnt, can_filter_item_t *filter_config)
{
	return 0;
}

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
int32_t hal_can_send(can_dev_t *can, can_frameheader_t *tx_header, const void *data, const uint32_t timeout)
{
	return 0;
}

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
int32_t hal_can_recv(can_dev_t *can, can_frameheader_t *rx_header, void *data, const uint32_t timeout)
{
	return 0;
}


/**
 * Deinitialises a CAN interface
 *
 * @param[in]  can  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_can_finalize(can_dev_t *can)
{
	return 0;
}

