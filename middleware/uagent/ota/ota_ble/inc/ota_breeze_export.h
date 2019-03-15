#ifndef OTA_BREEZE_EXPORT_H
#define OTA_BREEZE_EXPORT_H
#include "breeze_export.h"

#define OTA_BREEZE_FW_VER_LEN              (8)


typedef void (*ota_breeze_get_data_t) (breeze_otainfo_t* breeze_info);

typedef struct _ota_ble_version_{
    unsigned char fw_ver[OTA_BREEZE_FW_VER_LEN];
    unsigned int fw_ver_len;
}ota_breeze_version_t;

typedef struct _ota_ble_service_dat{
    unsigned char is_ota_enable;
    ota_breeze_version_t verison;
    ota_breeze_get_data_t get_dat_cb;
}ota_breeze_service_manage_t;


int ota_breeze_service_init(ota_breeze_service_manage_t* ota_manage);

#endif /*OTA_BREEZE_EXPORT_H*/
