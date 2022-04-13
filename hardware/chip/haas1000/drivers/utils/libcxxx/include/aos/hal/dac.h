/**
 * @file dac.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_DAC_H
#define HAL_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_dac DAC
 *  DAC hal API.
 *
 *  @{
 */

#include <stdint.h>

typedef struct {
    uint8_t  port; /**< dac port */
    void    *priv; /**< priv data */
} dac_dev_t;

/**
 * Initialises an dac interface
 *
 * @param[in]  dac  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_init(dac_dev_t *dac);

/**
 * Start output dac
 *
 * @param[in]   dac      the interface which should be started
 * @param[in]   channel  the channel to output dac
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_start(dac_dev_t *dac, uint32_t channel);

/**
 * Stop output dac
 *
 * @param[in]   dac      the interface which should be stopped
 * @param[in]   channel  the channel to output dac
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_stop(dac_dev_t *dac, uint32_t channel);

/**
 * Output a value to an dac interface
 *
 * @param[in]   dac      the interface to set value
 * @param[in]   channel  the channel to output dac
 * @param[in]   data     the value to output
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data);

/**
 * Returns the last data output value of the selected dac channel
 *
 * @param[in]   dac      the interface to get value
 * @param[out]  channel  channel  the channel to output dac
 *
 * @return  dac output value
 */
int32_t hal_dac_get_value(dac_dev_t *dac, uint32_t channel);

/**
 * De-initialises an dac interface, Turns off an dac hardware interface
 *
 * @param[in]  dac  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_dac_finalize(dac_dev_t *dac);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_DAC_H */

