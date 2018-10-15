/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include "fs_ctrl.h"
#include "driver/chip/sdmmc/hal_sdhost.h"
#include "common/framework/sys_ctrl/sys_ctrl.h"
#include "fs/fatfs/ff.h"
#include "driver/chip/sdmmc/sdmmc.h"

#define FS_DBG_ON	0
#define FS_INF_ON	1
#define FS_WRN_ON	1
#define FS_ERR_ON	1

#define FS_LOG(flags, fmt, arg...)  \
    do {                            \
        if (flags)                  \
            printf(fmt, ##arg);     \
    } while (0)

#define FS_DBG(fmt, arg...) FS_LOG(FS_DBG_ON, "[FS DBG] "fmt, ##arg)
#define FS_INF(fmt, arg...) FS_LOG(FS_INF_ON, "[FS INF] "fmt, ##arg)
#define FS_WRN(fmt, arg...) FS_LOG(FS_WRN_ON, "[FS WRN] "fmt, ##arg)
#define FS_ERR(fmt, arg...)                         \
    do {                                            \
        FS_LOG(FS_ERR_ON, "[FS ERR] %s():%d, "fmt,  \
               __func__, __LINE__, ##arg);          \
    } while (0)

typedef struct
{
	OS_Mutex_t mutex;
	FATFS *fs;
} fs_ctrl_private;

static fs_ctrl_private fs_ctrl;

#define FS_CTRL_LOCK()   OS_RecursiveMutexLock(&fs_ctrl.mutex, OS_WAIT_FOREVER)
#define FS_CTRL_UNLOCK() OS_RecursiveMutexUnlock(&fs_ctrl.mutex)

/*
 * @brief Init the device and mount the file system
 * @param[in] dev_type Device type
 * @param[in] dev_id Device ID
 * @return 0 on success, -1 on failure
 */
int fs_ctrl_mount(enum fs_mnt_dev_type dev_type, uint32_t dev_id)
{
	enum fs_mnt_status status = FS_MNT_STATUS_MOUNT_FAIL;

	FS_CTRL_LOCK();

	if (fs_ctrl.fs != NULL) {
		status = FS_MNT_STATUS_MOUNT_OK;
		goto out; /* already mounted, nothing to do */
	}

	if (mmc_card_create(dev_id) != 0) {
		FS_ERR("mmc create fail\n");
		goto notify;
	}
	struct mmc_card *card = mmc_card_open(dev_id);
	if (card == NULL) {
		FS_ERR("mmc open fail\n");
		goto err_card;
	}
	if (!mmc_card_present(card)) {
		int mmc_ret = mmc_rescan(card, 0);
		if (mmc_ret != 0) {
			FS_ERR("mmc scan fail\n");
			mmc_card_close(dev_id);
			goto err_card;
		} else {
			FS_INF("mmc init\n");
		}
	}
	mmc_card_close(dev_id);

	FATFS *fs = malloc(sizeof(FATFS));
	if (fs == NULL) {
		FS_ERR("no mem\n");
		goto err_fs;
	}
	FRESULT fs_ret = f_mount(fs, "", 0);
	if (fs_ret != FR_OK) {
		FS_ERR("mount fail, err %d\n", fs_ret);
		free(fs);
		goto err_fs;
	} else {
		FS_INF("mount success\n");
		fs_ctrl.fs = fs;
		status = FS_MNT_STATUS_MOUNT_OK;
	}
	goto notify;

err_fs:
	if (mmc_card_present(card))
		mmc_card_deinit(card);

err_card:
	mmc_card_delete(dev_id, 0);

notify:
	if (sys_event_send(CTRL_MSG_TYPE_FS,
	                   FS_CTRL_MSG_FS_MNT,
	                   FS_MNT_MSG_PARAM(dev_type, dev_id, status),
	                   0) != 0) {
		FS_ERR("send event fail\n");
	}

out:
	FS_CTRL_UNLOCK();
	return (status == FS_MNT_STATUS_MOUNT_OK ? 0 : -1);
}

/*
 * @brief Unmount the file system, and deinit the device
 * @param[in] dev_type Device type
 * @param[in] dev_id Device ID
 * @return 0 on success, -1 on failure
 */
int fs_ctrl_unmount(enum fs_mnt_dev_type dev_type, uint32_t dev_id)
{
	int ret = 0;

	FS_CTRL_LOCK();

	if (fs_ctrl.fs == NULL) {
		goto out; /* unmounted, nothing to do */
	}

	FRESULT fs_ret = f_mount(NULL, "", 0);
	if (fs_ret != FR_OK) {
		FS_ERR("unmount fail, err %d\n", fs_ret);
		ret = -1;
	} else {
		free(fs_ctrl.fs);
		fs_ctrl.fs = NULL;
	}

	struct mmc_card *card = mmc_card_open(dev_id);
	if (card == NULL) {
		FS_ERR("card open fail\n");
	} else {
		if (mmc_card_present(card)) {
			mmc_card_deinit(card);
		}
		mmc_card_close(dev_id);
		mmc_card_delete(dev_id, 0);
	}

	if (ret == 0) {
		if (sys_event_send(CTRL_MSG_TYPE_FS,
		                   FS_CTRL_MSG_FS_MNT,
		                   FS_MNT_MSG_PARAM(dev_type, dev_id,
		                                    FS_MNT_STATUS_UNMOUNT),
		                   0) != 0) {
			FS_ERR("send event fail\n");
		}
	}

out:
	FS_CTRL_UNLOCK();
	return ret;
}

static void fs_ctrl_msg_process(uint32_t event, uint32_t data, void *arg)
{
	switch (EVENT_SUBTYPE(event)) {
		case SD_CARD_MSG_INSERT:
			fs_ctrl_mount(FS_MNT_DEV_TYPE_SDCARD, 0);
			break;
		case SD_CARD_MSG_REMOVE:
			fs_ctrl_unmount(FS_MNT_DEV_TYPE_SDCARD, 0);
			break;
		default:
			break;
	}
}

/*
 * @brief Init file system control module
 * @return 0 on success, -1 on failure
 */
int fs_ctrl_init(void)
{
	observer_base *base = sys_callback_observer_create(CTRL_MSG_TYPE_SDCARD,
	                                                   SD_CARD_MSG_ALL,
	                                                   fs_ctrl_msg_process,
	                                                   NULL);
	if (base == NULL) {
		FS_ERR("create fail\n");
		return -1;
	}

	if (sys_ctrl_attach(base) != 0) {
		FS_ERR("attach fail\n");
		return -1;
	}

	if (OS_MutexCreate(&fs_ctrl.mutex) != OS_OK) {
		FS_ERR("create mtx fail\n");
		return -1;
	}

	return 0;
}

/*
 * @brief SD card detect callback fucntion
 * @param[in] present 1 for card inserted, 0 for card removed
 * @return none
 */
void sdcard_detect_callback(uint32_t present)
{
	uint16_t subtype;

	if (present) {
		FS_INF("card insert\n");
		subtype = SD_CARD_MSG_INSERT;
	} else {
		FS_INF("card remove\n");
		subtype = SD_CARD_MSG_REMOVE;
	}

	if (sys_event_send(CTRL_MSG_TYPE_SDCARD, subtype, 0, 0) != 0) {
		FS_ERR("send event fail\n");
	}
}
