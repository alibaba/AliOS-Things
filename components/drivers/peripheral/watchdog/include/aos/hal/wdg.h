/**
 * @file wdg.h
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef HAL_WDG_H
#define HAL_WDG_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup hal_wdg WDG
 *  wdg hal API.
 *
 *  @{
 */

#include <stdint.h>

/* Define wdt expired time */
typedef struct {
    uint32_t timeout; /**< Watchdag timeout */
} wdg_config_t;

/* Define wdg dev handle */
typedef struct {
    uint8_t       port;   /**< wdg port */
    wdg_config_t  config; /**< wdg config */
    void         *priv;   /**< priv data */
} wdg_dev_t;

/**
 * This function will initialize the on board CPU hardware watch dog
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_wdg_init(wdg_dev_t *wdg);

/**
 * Reload watchdog counter.
 *
 * @param[in]  wdg  the watch dog device
 */
void hal_wdg_reload(wdg_dev_t *wdg);

/**
 * This function performs any platform-specific cleanup needed for hardware watch dog.
 *
 * @param[in]  wdg  the watch dog device
 *
 * @return  0 : on success,  otherwise is error
 */
int32_t hal_wdg_finalize(wdg_dev_t *wdg);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HAL_WDG_H */

