#include <string.h>
#include <aos/flashpart.h>
#include "ota_port.h"
#include "cmsis.h"
#include "sys_api.h"
#include "rtl8721d_ota.h"

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

int ota_get_running_index(void)
{
    return ota_get_cur_index();
}

int ota_set_user_bootinfo(void *param)
{
    unsigned char buf[8] = { 0, };
    unsigned char signature[8] = { 0x38, 0x31, 0x39, 0x35, 0x38, 0x37, 0x31, 0x31, };
    uint32_t part_id;
    aos_flashpart_ref_t part_ref;
    aos_status_t r;

    /*
     * step1: enable new partition to valid.
     */
    if (ota_get_running_index() == 0)
        part_id = HAL_PARTITION_OTA_TEMP;
    else
        part_id = HAL_PARTITION_APPLICATION;
    printf("enable partition[%d]'s signature to valid\r\n", (int)part_id);
    if (aos_flashpart_get(&part_ref, part_id)) {
        printf("open partition[%d] failed\r\n", (int)part_id);
        return -1;
    }
    r = aos_flashpart_write(&part_ref, 0, signature, sizeof(signature));
step1err:
    /*
     * step1 err: now old partition is valid, so return directly.
     */
    aos_flashpart_put(&part_ref);
    if (r) {
        printf("%s-%d ret %d\r\n" , __func__, __LINE__, (int)r);
        return -1;
    }
    /*
     * step2: enable old partition to invalid.
     */
    if (ota_get_running_index() == 0)
        part_id = HAL_PARTITION_APPLICATION;
    else
        part_id = HAL_PARTITION_OTA_TEMP;
    printf("enable partition[%d]'s signature to invalid\r\n", (int)part_id);
    if (aos_flashpart_get(&part_ref, part_id)) {
        printf("open partition[%d] failed\r\n", (int)part_id);
        goto rollback;
    }
    memset(buf, 0 ,sizeof(buf));
    r = aos_flashpart_write(&part_ref, 0, buf, sizeof(buf));
    if (r) {
        unsigned char tmp[8];
        r = aos_flashpart_read(&part_ref, 0, tmp, sizeof(tmp));
        if (r < 0 || r == AOS_FLASH_ECC_ERROR || !memcmp(tmp, signature, sizeof(tmp))) {
            aos_flashpart_put(&part_ref);
            goto rollback;
        }
    }
    aos_flashpart_put(&part_ref);
    return 0;
    /*
     * step2 open failed or step2 write is not valid:  new partition needs to be rollbacked.
     */
rollback:
    if (ota_get_running_index() == 0)
        part_id = HAL_PARTITION_OTA_TEMP;
    else
        part_id = HAL_PARTITION_APPLICATION;
    printf("rollback partition[%d]\r\n", (int)part_id);
    /*
     * rollback: currently not consider exception.
     */
    if (aos_flashpart_get(&part_ref, part_id))
        return -1;
    (void)aos_flashpart_write(&part_ref, 0, buf, sizeof(buf));
    aos_flashpart_put(&part_ref);
    return -1;
}

int ota_hal_final(void)
{
    return ota_set_user_bootinfo(NULL);
}

int ota_hal_platform_boot_type(void)
{
    return ota_get_boot_type();
}
