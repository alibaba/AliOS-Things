/**
 * @file adc.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_ADC_H
#define HAL_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_adc ADC
 *  ADC hal API.
 *
 *  @{
 */

#include <stdint.h>

/* Define the wait forever timeout macro */
#define HAL_WAIT_FOREVER 0xFFFFFFFFU

/* Define ADC config args */
typedef struct {
    uint32_t sampling_cycle; /**< sampling period in number of ADC clock cycles */
} adc_config_t;

/* Define ADC dev hal handle */
typedef struct {
    uint8_t      port;   /**< adc port */
    adc_config_t config; /**< adc config */
    void        *priv;   /**< priv data */
} adc_dev_t;

/**
 * Initialises an ADC interface, Prepares an ADC hardware interface for sampling
 *
 * @param[in]  adc  the interface which should be initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_adc_init(adc_dev_t *adc);

/**
 * Takes a single sample from an ADC interface
 *
 * @note If the ADC is configured with mutiple channels, the result of each channel is
 *       copied to output buffer one by one according to the sequence of the channel registered,
 *       and each result takes sizeof(uint32_t) bytes.
 *
 * @param[in]   adc      the interface which should be sampled
 * @param[out]  output   pointer to a variable which will receive the sample
 * @param[in]   timeout  ms timeout
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout);

/**
 * De-initialises an ADC interface, Turns off an ADC hardware interface
 *
 * @param[in]  adc  the interface which should be de-initialised
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_adc_finalize(adc_dev_t *adc);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_H */

