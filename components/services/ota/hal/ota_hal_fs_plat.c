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
    char store_file_name[64] = {0};
    ota_service_t *tmp_ctx = NULL;

    if((dev_info == NULL) || (ver == NULL) || (size == 0)) {
        OTA_LOG_E("upgrade subdev input param err\n");
        ret = OTA_INVALID_PARAMETER;
        return ret;
    }
    tmp_ctx = (ota_service_t*)dev_info;
    OTA_LOG_I("subdev_name = %s, iamge_len = %d, ver = %s\n",
             tmp_ctx->dn, size, ver);
    memset(store_file_name, 0x00, sizeof(store_file_name));
    ret = ota_get_storefile_subdev_name(store_file_name);
    printf("ret = %d, file_name = %s\r\n", ret, store_file_name);
    if(ret == 0) {
        ;
    }
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
#if 0
    int i = 0;
    FILE *fptr = NULL;
    int upack_path_len = 0;
    char upack_path[128] = {0};
    char file_name[10] = "app.json";
    char read_buf[64];
    int read_len = 0;
    char *pos = 0;
    if(version != NULL) {
        ret = 0;
        memset(upack_path, 0x00,sizeof(upack_path));
        HAL_Get_User_Dir(upack_path);
        upack_path_len = strlen(upack_path);
        if(upack_path[upack_path_len] != '/') {
            upack_path[upack_path_len++] = '/'; 
        }
        memcpy(&upack_path[upack_path_len], file_name, 8);
        fptr = ota_fopen(upack_path, "r");
        if(fptr == NULL) {
            OTA_LOG_I("can't find app.json file\r\n");
            strcpy(version, "0.0.0");//app.json is null default ver = 0.0.0
        }
        else {
            memset(read_buf, 0x00, sizeof(read_buf));
            read_len = ota_fread(read_buf, 1, sizeof(read_buf), fptr);
            if(read_len > 13) {
                pos = strstr(read_buf, "\"version\":");
                if(pos != NULL) {
                    pos += 10;
                    while(pos[0] != '"') {
                        pos++;
                    }
                    pos++;
                    while(pos[0] != '"') {
                        version[i++] = pos[0];
                        pos++;
                    }
                    version[i] = 0;
                }
            }
        }
        if(fptr != NULL) {
            ota_fclose(fptr);

        }
    }
#endif
    return ret;
}