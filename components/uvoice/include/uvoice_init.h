/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_INIT_H__
#define __UVOICE_INIT_H__

/** @defgroup uvoice_init_api uvoice_init
 *  @ingroup uvoice_aos_api
 * @{
 */

/**
 * Init the uvoice module.
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int uvoice_init(void);

/**
 * DeInit the uvoice module.
 *
 * @retrun 0 on success, otherwise will be failed.
 */
int uvoice_free(void);

/**
 * @}
 */

#endif /* __UVOICE_INIT_H__ */

