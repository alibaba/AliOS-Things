#include <string.h>
#include "ota_port.h"



int ota_get_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone)
{
    return 0;
}

int ota_set_bootinfo_crc32value(struct ota_boot_info *info)
{

    return 0;
}

int ota_set_bootinfo(struct ota_boot_info *info, enum bootinfo_zone zone)
{

    return 0;
}

int ota_clear_reboot_count(void)
{
    return 0;
}

int ota_get_boot_type()
{
    int boot_type = 1;
    return boot_type;
}

int ota_set_user_bootinfo(void *param)
{
    return 0;
}

