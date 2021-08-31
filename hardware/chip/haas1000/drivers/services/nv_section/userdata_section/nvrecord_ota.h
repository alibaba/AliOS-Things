/***************************************************************************
*
*Copyright 2015-2019 BES.
*All rights reserved. All unpublished rights reserved.
*
*No part of this work may be used or reproduced in any form or by any
*means, or stored in a database or retrieval system, without prior written
*permission of BES.
*
*Use of this work is governed by a license granted by BES.
*This work contains confidential and proprietary information of
*BES. which is protected by copyright, trade secret,
*trademark and other intellectual property rights.
*
****************************************************************************/


#ifndef __NVRECORD_OTA_H__
#define __NVRECORD_OTA_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/
#include "nvrecord_extension.h"

/******************************macro defination*****************************/

/******************************type defination******************************/

/****************************function declearation**************************/
#if defined(GSOUND_OTA_ENABLED)&&defined(VOICE_DATAPATH)
void nv_record_otarec_init(void);

void nv_record_otarec_get_ptr(void **ptr);

void nv_record_extension_update_gsound_ota_session(uint8_t gsoundOtaStatus,
                                                   uint32_t totalImageSize,
                                                   const char *session);

void nv_record_extension_update_gsound_ota_progress(uint32_t otaOffset);

void nv_record_extension_update_gsound_info(NV_GSOUND_INFO_T* gsoundInfo);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __NVRECORD_OTA_H__ */