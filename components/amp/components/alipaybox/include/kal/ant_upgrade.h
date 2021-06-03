/**
 * ant_upgrade.h
 *
 * 系统及应用升级相关接口
 */
#ifndef __ANT_UPGRADE_H__
#define __ANT_UPGRADE_H__

#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

ant_s32  ant_fota_start(ant_s32 down_size);
ant_s32  ant_fota_write(ant_s32 addr,ant_char *data, ant_s32 len);
ant_s32  ant_fota_upgrade(ant_s32 fota_size);
ant_s32  ant_fota_get_max(void);
ant_s32  ant_fota_block_erase(ant_s32 addr,ant_s32 block_size);
ant_s32  ant_fota_read(ant_s32 addr,ant_char *data, ant_s32 len);
ant_s32  ant_fota_end(void);
ant_s32  ant_fota_get_security(void);

#ifdef __cplusplus
}
#endif

#endif


