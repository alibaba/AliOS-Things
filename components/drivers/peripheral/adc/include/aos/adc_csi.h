/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#ifndef _AOS_ADC_CSI_H
#define _AOS_ADC_CSI_H

#include <aos/adc_core.h>
#include <drv/adc.h>

typedef struct {
    aos_adc_t aos_adc;  /* aos adc device */
    csi_adc_t csi_adc; /* csi adc device */
} aos_adc_csi_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register an CSI ADC device
 *
 * @param[in]   adc      ADC ref to operate. The caller should allocate memory,
 *                       init it and set id for the CSI ADC device.
 *
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_adc_csi_register(aos_adc_csi_t *adc_csi);

/**
 * Unregister an CSI ADC device by id
 *
 * @param[in]   id      CSI ADC device id
 *
 * @return      0: on success; < 0: on failure
 */
aos_status_t aos_adc_csi_unregister(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* _AOS_ADC_CSI_H */
