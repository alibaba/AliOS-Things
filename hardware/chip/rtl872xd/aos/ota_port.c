#include <string.h>
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
    int ret = -1;
    char dev_str[16] = {0};
    int partition_id =5;
    int fd;
    unsigned char buf[8] = {0};
    unsigned char signature[8] = {0x38, 0x31, 0x39, 0x35, 0x38, 0x37, 0x31, 0x31};
    (void *)param;
    /*
        step1: enable new partition to valid.
    */
    if(ota_get_running_index()==0)
        partition_id = MTD_PART_ID_KERNEL2;
    else
        partition_id = MTD_PART_ID_KERNEL;
    printf("enable partition[%d]'s signature to valid\r\n", partition_id);
    memset(dev_str, 0, sizeof(dev_str));
    snprintf(dev_str, 15, "/dev/mtdblock%d", partition_id);
    fd = open(dev_str, 0);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_str);
        return -1;
    }
    lseek(fd, 0 ,SEEK_SET);
    ret = write(fd, signature, sizeof(signature));
step1err:
    /*
        step1 err: now old partition is valid, so return directly.
     */
    close(fd);
    if(ret != sizeof(signature)) {
        printf("%s-%d ret %d\r\n" , __FUNCTION__, __LINE__, ret);
        return -1;
    }
    /*
        step2: enable old partition to invalid.
    */
    if(ota_get_running_index()==0)
        partition_id = MTD_PART_ID_KERNEL;
    else
        partition_id = MTD_PART_ID_KERNEL2;
    printf("enable  partition[%d]'s signature to invalid\r\n", partition_id);
    memset(dev_str, 0 , sizeof(dev_str));
    snprintf(dev_str, 15, "/dev/mtdblock%d", partition_id);
    fd = open(dev_str, 0);
    if (fd < 0) {
        printf("open %s failed\r\n", dev_str);
        goto rollback;
    }
    lseek(fd, 0 ,SEEK_SET);
    memset(buf, 0 ,sizeof(buf));
    ret = write(fd, buf, sizeof(buf));
    if(ret != sizeof(buf)) {
       unsigned char tmp[8];
       lseek(fd, 0, SEEK_SET);
       read(fd, tmp, sizeof(tmp));
       if(!memcmp(tmp, signature, sizeof(tmp))) {
            close(fd);
            goto rollback;
       }
    }
    close(fd);
    return 0;
    /*
        step2 open failed or step2 write is not valid:  new partition needs to be rollbacked.
     */
rollback:
    if(ota_get_running_index()==0)
        partition_id = MTD_PART_ID_KERNEL2;
    else
        partition_id = MTD_PART_ID_KERNEL;
    printf("rollback partition[%d]\r\n", partition_id);
    snprintf(dev_str, 15, "/dev/mtdblock%d", partition_id);
    /*
        rollback: currently not consider exception.
     */
    fd = open(dev_str, 0);
    lseek(fd, 0 ,SEEK_SET);
    write(fd, buf, sizeof(buf));
    close(fd);
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

