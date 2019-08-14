/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <jse_osal.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jse_osal.h"
#include "be_upgrader.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "ota_download.h"

#define BE_CLI_REPLY_SUCCESS "#be-cli-reply#success\n"
#define BE_CLI_REPLY_FAILED "#be-cli-reply#failed\n"

#define OTA_BREAKPOINT -1
#define OTA_FINISH 0

static jse_write_flash_cb_t be_upgd_write_callback;
static be_upgd_finish_cb_t be_upgd_finish_callbak;

/* ota handle */
static esp_ota_handle_t ota_handle  = NULL;
static esp_partition_t *ota_part    = NULL;
static esp_partition_t *spiffs_part = NULL;
static size_t spiffs_offset         = 0;
static int ota_offset               = 0;
static int be_rom_size              = 0;

int be_str_to_upper(char *src, int len) {
    int i = 0;

    if (src == NULL) {
        return -1;
    }

    for (i = 0; i < len; i++) {
        src[i] = toupper(src[i]);
    }
    return 0;
}

/**
 *
 * ota write flash
 * esp_err_t esp_ota_write(esp_ota_handle_thandle, const void *data, size_t
 * size) esp_err_t esp_ota_end(esp_ota_handle_thandle) esp_err_t
 * esp_ota_set_boot_partition(constesp_partition_t *partition)
 *
 */

static int be_upgrade_ota_write_cb(int32_t writed_size, uint8_t *buf,
                                   int32_t buf_len, int type) {
    esp_err_t err = 0;
    if (ota_handle && buf_len > 0 && buf) {
        err = esp_ota_write(ota_handle, buf, buf_len);
    }

    ota_offset = ota_offset + buf_len;
    jse_debug("OTA write %d / %d", ota_offset, be_rom_size);

    /* report progress to webIDE */
    static int last_len = 0;
    char *msg[64];
    sprintf(msg, "%d/%d", ota_offset, be_rom_size);
    if (ota_offset - last_len > 4096) {
        last_len = ota_offset;
        bone_websocket_send_frame("/ide/updateimg_process", 200, msg);
    }

    return err;
}

static int be_upgrade_ota_finish_cb(int finished_result, void *updated_type) {
    esp_err_t err = 0;
    if (ota_handle && ota_part && (OTA_FINISH == finished_result)) {
        err = esp_ota_set_boot_partition(ota_part);
    }

    jse_debug("OTA update success .");

    return err;
}

/**
 *
 * spiffs write
 * esp_err_t esp_partition_write(const esp_partition_t* partition,
                             size_t dst_offset, const void* src, size_t size);
 *  * @param partition Pointer to partition structure obtained using
 *                  esp_partition_find_first or esp_partition_get.
 *                  Must be non-NULL.
 *
 * esp_err_t esp_partition_erase_range(const esp_partition_t* partition,
                                    uint32_t start_addr, uint32_t size);
 * @param start_addr Address where erase operation should start. Must be aligned
 *                   to 4 kilobytes.
 * @param size Size of the range which should be erased, in bytes.
 *                   Must be divisible by 4 kilobytes.
 *
 *
*/
static int be_upgrade_direct_flash_write_cb(int32_t writed_size, uint8_t *buf,
                                            int32_t buf_len, int type) {
    esp_err_t err;
    if (buf_len > 0 && buf && spiffs_part) {
        err = esp_partition_write(spiffs_part, spiffs_offset, buf, buf_len);
        spiffs_offset += buf_len;
    }

    jse_warn("write SPIFFS  offset %d  len = %d", (int)spiffs_offset,
            buf_len);

    return spiffs_offset;
}

static int be_upgrade_direct_flash_finish_cb(int finished_result,
                                             void *updated_type) {
    if (OTA_FINISH == finished_result)
        jse_debug("Update SPIFFS success .");
    else
        jse_debug("Update SPIFFS failed .");
    return 0;
}

static void be_upgrade_set_callback(jse_write_flash_cb_t wcb,
                                    be_upgd_finish_cb_t fcb) {
    be_upgd_write_callback = wcb;
    be_upgd_finish_callbak = fcb;
}

static jse_write_flash_cb_t be_upgrade_get_write_cb_fn() {
    return be_upgd_write_callback;
}

static be_upgd_finish_cb_t be_upgrade_get_finish_cb_fn() {
    return be_upgd_finish_callbak;
}

static void be_upgrade_register_callback(be_upgd_type_t type) {
    esp_err_t err;
    if (type == BE_UPGD_KERNEL_TYPE) {
        /* get ota flash partition */
        ota_part = esp_ota_get_next_update_partition(NULL);
        if (ota_part == NULL) {
            jse_error("[%s][%d] OTA partition not found \n", __FUNCTION__,
                   __LINE__);
            return;
        }
        /* erase ota partition */
        err = esp_ota_begin(ota_part, 0, &ota_handle);
        if (err != ESP_OK) {
            jse_error("[%s][%d] erase ota part failed, err = 0x%x \n",
                   __FUNCTION__, __LINE__, err);
            return;
        }
        jse_debug("[%s][%d] Format OTA partition success. \n", __FUNCTION__,
               __LINE__);
        ota_offset = 0;
        be_upgrade_set_callback(be_upgrade_ota_write_cb,
                                be_upgrade_ota_finish_cb);
    } else if (type == BE_UPGD_SPIFFS_TYPE) {
        /* get spiffs flash partition */
        esp_partition_subtype_t subtype = ESP_PARTITION_SUBTYPE_DATA_SPIFFS;
        spiffs_part =
            esp_partition_find_first(ESP_PARTITION_TYPE_DATA, subtype, NULL);
        if (spiffs_part == NULL) {
            jse_error("[%s][%d] spiffs partition not found \n", __FUNCTION__,
                   __LINE__);
            return;
        }
        jse_debug("Format SPIFFS partition %s at 0x%x, size 0x%x\n",
               spiffs_part->label, spiffs_part->address, spiffs_part->size);
        /* erase all partition */
        esp_partition_erase_range(spiffs_part, 0, spiffs_part->size);
        spiffs_offset = 0;

        be_upgrade_set_callback(be_upgrade_direct_flash_write_cb,
                                be_upgrade_direct_flash_finish_cb);
    } else {
        jse_error("unknown type=%d", type);
    }
}

void be_upgrader_handler(be_upgd_param_t *param) {
    int ret                          = 0;
    jse_write_flash_cb_t jse_write_cb  = NULL;
    be_upgd_finish_cb_t be_finish_cb = NULL;
    be_upgd_type_t upgrade_type      = BE_UPGD_KERNEL_TYPE;

    if (param == NULL) {
        return;
    }

    jse_debug("md5=%s,type=%d,url=%s,romsize=%d", param->md5, param->type,
             param->url, param->total_size);

    be_rom_size  = param->total_size;
    upgrade_type = param->type;
    be_upgrade_register_callback(upgrade_type);
    jse_write_cb  = be_upgrade_get_write_cb_fn();
    be_finish_cb = be_upgrade_get_finish_cb_fn();

    /* ota_set_update_breakpoint(0);

       if (BE_UPGD_KERNEL_TYPE == upgrade_type) {
           be_upgrade_hal_init(ota_get_update_breakpoint());
       } */

    ret = ota_download(param->url, jse_write_cb, param->md5);
    jse_debug("ota download ret =%d", ret);

    if (ret <= 0) {
        jse_error("ota download error");
        be_finish_cb(OTA_BREAKPOINT, &upgrade_type);
        goto done;
    }

    if (ret == OTA_DOWNLOAD_CANCEL) {
        jse_error("ota download cancel");
        be_finish_cb(OTA_BREAKPOINT, &upgrade_type);
        goto done;
    }

    be_str_to_upper(param->md5, MD5_LEN);
    ret = check_md5(param->md5, MD5_LEN);

    jse_debug("check_md5 return %d ", ret);

    if (ret == 0) {
        jse_debug("be_upgrader_handler update ok");
        jse_debug(BE_CLI_REPLY_SUCCESS);
        bone_websocket_send_frame("/device/updateimg_reply", 200, "success");
        jse_osal_delay(200);

        be_finish_cb(OTA_FINISH, &upgrade_type);

        /* ota_set_update_breakpoint(0); */
        jse_system_reboot();
        return;
    }

done:
    jse_free(param->url);
    jse_free(param);
    jse_debug(BE_CLI_REPLY_FAILED);
    jse_error("be_upgrader_handler fail");
    bone_websocket_send_frame("/device/updateimg_reply", 205, "failed");
    jse_osal_delay(200);
    jse_osal_exit_task(0);
}
