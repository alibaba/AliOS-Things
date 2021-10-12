/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_OTA_H
#define AOS_OTA_H

/**
 * sys ota upgrade thread function.
 *
 * @param[in]  ctx   device information.
 *
 * @return  0 on success, negative error on failure.
 */
int interval_sys_upgrade_start(void *ctx);

/**
 * submodule ota cb.
 *
 *
 * @param[in]      ctx     submodule information.
 *
 *
 * @return  0 on success, negative error on failure.
 */
int interval_module_upgrade_start(void *ctx);

/**
 * amp_fota_image_local_copy.
 *
 *
 * @param[in]      image_name     image name.
 * @param[in]      image_size     image size.
 *
 *
 * @return  0 on success, negative error on failure.
 */
int aos_fota_image_local_copy(char *image_name, int image_size);

#endif /* AOS_OTA_H */
