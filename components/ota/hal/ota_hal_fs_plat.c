#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal.h"

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
    return ret;
}

OTA_WEAK int ota_reboot_module(char *module_name)
{
    int ret = 0;
    return ret;
}

OTA_WEAK int ota_subdev_version_get(char *version)
{
    int ret = -1;
    return ret;
}