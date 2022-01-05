/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * File: lightduer_ota_installer.h
 * Auth: Zhong Shuai (zhongshuai@baidu.com)
 * Desc: OTA installer Head File
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_INSTALLER_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_INSTALLER_H

#include "lightduer_mutex.h"
#include "lightduer_ota_package_info.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ERR_MSG_LEN (60)
#define INSTALLER_NAME_LEN 20

typedef struct _duer_ota_installer_s {
    // The name of OTA installer
    char name[INSTALLER_NAME_LEN + 1];
    // The customer data
    void *custom_data;
    // Lock the installer
    duer_mutex_t lock;
    // Last Error message
    char err_msg[ERR_MSG_LEN + 1];

    /*
     * You can get module information
     */
    int (*get_module_info)(
            struct _duer_ota_installer_s *installer,
            void **custom_data,
            duer_ota_module_info_t const *module_info);

    /*
     * Now we notify you, we are ready to start OTA
     * You can do someting before receive data
     * Such as open file, Initialise Flash, Initialise DMA, check chip ...
     */
    int (*notify_ota_begin)(struct _duer_ota_installer_s *installer, void *custom_data);

    /*
     * You can free the resource which you use in notify_ota_begin
     * Set the boot partition ...
     */
    int (*notify_ota_end)(struct _duer_ota_installer_s *installer, void *custom_data);

    /*
     * You can get the file which you upload to Duer Cloud
     */
    int (*get_module_data)(
            struct _duer_ota_installer_s *installer,
            void *custom_data,
            unsigned int offset,
            unsigned char const *data,
            size_t size);

    /*
     * You can verify the data you received
     */
    int (*verify_module_data)(struct _duer_ota_installer_s *installer, void *custom_data);

    /*
     * You can do someting before update the firmware
     * Such as Erase Flash ...
     */
    int (*update_image_begin)(struct _duer_ota_installer_s *installer, void *custom_data);

    /*
     * Update the new firmware
     */
    int (*update_image)(struct _duer_ota_installer_s *installer, void *custom_data);

    /*
     * You can check the new firmware, and select the boot partition
     */
    int (*verify_image)(struct _duer_ota_installer_s *installer, void *custom_data);

    /*
     * Cancel the OTA update, free the resources and revert
     */
    int (*cancel_ota_update)(struct _duer_ota_installer_s *installer, void *custom_data);

    struct _duer_ota_installer_s *next;
} duer_ota_installer_t;

/*
 * Initialise OTA installer module
 *
 * @param void
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_init_installer(void);

/*
 * Get the customr data
 *
 * @param installer: OTA installer
 *
 * @return void *: Success: void *
 *                  Failed: NULL
 */
extern void *duer_ota_installer_get_custom_data(duer_ota_installer_t const *installer);

/*
 * Create a OTA installer
 *
 * @param name: The name of installer
 *
 * @return int: Success: duer_ota_installer_t *
 *              Failed:  NULL
 */
extern duer_ota_installer_t *duer_ota_installer_create_installer(char const *name);

/*
 * Destory a OTA installer
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_destroy_installer(duer_ota_installer_t *installer);

/*
 * Register a OTA installer
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_register_installer(duer_ota_installer_t *installer);

/*
 * Unregister a OTA installer
 *
 * @param name : The name of the OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_unregister_installer(char const *name);

/*
 * Get the OTA installer by name
 *
 * @param name : The name of the OTA installer
 *
 * @return int: Success: duer_ota_installer_t *
 *              Failed:  NULL
 */
extern duer_ota_installer_t *duer_ota_installer_get_installer(char const *name);

/*
 * Notify OTA installer data begin
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_notify_ota_begin(duer_ota_installer_t *installer);

/*
 * Notify OTA installer the module information
 *
 * @param installer: OTA installer
 * @param module_info: module information
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_send_module_info(
        duer_ota_installer_t *installer,
        duer_ota_module_info_t const *module_info);

/*
 * Send OTA installer the module data
 *
 * @param installer: OTA installer
 * @param offset:    The offset of the image
 * @param data:      OTA image data
 * @param size       The size of OTA image data
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_send_module_data(
        duer_ota_installer_t *installer,
        unsigned int offset,
        unsigned char const *data,
        size_t size);

/*
 * Verify the module data
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_verify_module_data(duer_ota_installer_t *installer);

/*
 * Notify OTA installer bigin to update OTA image
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_update_image_begin(duer_ota_installer_t *installer);

/*
 * Notify OTA installer to update OTA image
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_update_image(duer_ota_installer_t *installer);

/*
 * Notify OTA installer to verify OTA image
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_verify_image(duer_ota_installer_t *installer);

/*
 * Notify OTA installer to end the OTA update
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_notify_ota_end(duer_ota_installer_t *installer);

/*
 * Cancel the OTA update
 *
 * @param installer: OTA installer
 *
 * @return int: Success: DUER_OK
 *              Failed:  Other
 */
extern int duer_ota_installer_cancel_ota_update(duer_ota_installer_t *installer);

/*
 * Report error message
 *
 * @param installer: OTA installer
 *          err_msg: error message
 *         err_code: error code
 *
 * @return void:
 */
extern void duer_ota_installer_report_err(
        duer_ota_installer_t *installer,
        char const *err_msg,
        int err_code);

/*
 * Get error message
 *
 * @param installer: OTA installer
 *
 * @return : Success: error message
 *            Failed: NULL
 */
extern const char *duer_ota_installer_get_err_msg(duer_ota_installer_t const *installer);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_OTA_INSTALLER_H
