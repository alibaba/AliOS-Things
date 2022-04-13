/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/i2s.h"


/**
 * Initialises a I2S dev
 *
 * @param[in]  i2s  the dev which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_init(i2s_dev_t *i2s)
{
	return 0;
}

/**
 * Transmit data on a I2S dev
 *
 * @param[in]  i2s      the I2S dev
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_send(i2s_dev_t *i2s, const void *data, uint32_t size, uint32_t timeout)
{
	return 0;
}

/**
 * Receive data on a I2S dev
 *
 * @param[in]   i2s      the I2S dev
 * @param[out]  data     pointer to the buffer which will store incoming data
 * @param[in]   size     number of bytes to receive
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_recv(i2s_dev_t *i2s, void *data, uint32_t size, uint32_t timeout)
{
	return 0;
}

/**
 * Pause a I2S dev
 *
 * @param[in]  i2s  the dev which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_pause(i2s_dev_t *i2s)
{
	return 0;
}

/**
 * Resume a I2S dev
 *
 * @param[in]  i2s  the dev which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_resume(i2s_dev_t *i2s)
{
	return 0;
}

/**
 * Stop a I2S dev
 *
 * @param[in]  i2s  the dev which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_stop(i2s_dev_t *i2s)
{
	return 0;
}

/**
 * Deinitialises a I2S dev
 *
 * @param[in]  i2s  the dev which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_i2s_finalize(i2s_dev_t *i2s)
{
	return 0;
}

