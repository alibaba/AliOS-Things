/**
 * @file interpt.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_INTERPT_H
#define HAL_INTERPT_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_interpt INTERPT
 *  interpt hal API.
 *
 *  @{
 */

#include <stdint.h>

/* interrupt proc handle */
typedef void (*hal_interpt_t)(int32_t vec, void *para);

/**
 * Interrupt vector init
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_interpt_init(void);

/**
 * Mask specified interrupt vector
 *
 *
 * @param[in]  vec  specified interrupt vector
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_interpt_mask(int32_t vec);

/**
 * Unmask specified interrupt vector
 *
 *
 * @param[in]  vec  specified interrupt vector
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_interpt_umask(int32_t vec);

/**
 * Install specified interrupt vector
 *
 *
 * @param[in]  vec      specified interrupt vector
 * @param[in]  handler  interrupt handler
 * @param[in]  para     interrupt handler args
 * @param[in]  vec      specified interrupt vector
 * @param[in]  name     interrupt descript name
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_interpt_install(int32_t vec, hal_interpt_t handler, void *para,  char *name);

#ifdef __cplusplus
}
#endif

#endif /* HAL_INTERPT_H */

