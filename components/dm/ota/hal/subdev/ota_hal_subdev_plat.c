#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "ota_log.h"
#include "aos/kernel.h"
#include "aos/hal/flash.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota/ota_hal.h"

OTA_WEAK int ota_hal_sudev_init(ota_boot_param_t *param)
{
    int ret = 0;
    return ret;
}

OTA_WEAK int ota_recvd_from_subdev(void *device, unsigned char *buf, unsigned int len)
{
    int ret = 0;
    return ret;
}

OTA_WEAK int ota_send_to_subdev(void *device, unsigned char* buf, unsigned int len)
{
    int ret = 0;
    return ret;
}

OTA_WEAK int ota_subdev_upgrade_start(void *dev_info, unsigned int size, char *ver)
{
    int ret = 0;
    void *fptr = NULL;
    char file_name[64] = {0};
    ota_service_t *tmp_ctx = NULL;

    if((dev_info == NULL) || (ver == NULL) || (size == 0)) {
        OTA_LOG_E("upgrade subdev input param err\n");
        ret = OTA_INVALID_PARAMETER;
        return ret;
    }
    tmp_ctx = (ota_service_t*)dev_info;
    OTA_LOG_I("subdev_name = %s, iamge_len = %d, ver = %s\n",
             tmp_ctx->dn, size, ver);
    
    if(ota_get_storefile_name(file_name) != 0) {
        ret = OTA_MCU_UPGRADE_FAIL;
        OTA_LOG_E("ota store file = %s failed\n", file_name);
        return ret;
    }
    OTA_LOG_I("want send file = %s\n", file_name);
    fptr = ota_fopen(file_name, "rb");
    (void)ota_fclose(fptr);

    return ret;
}
