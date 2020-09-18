#include "2ndboot.h"

void ota_2ndboot_os_entry(void)
{
    return;
}

void ota_2ndboot_image_entry(void)
{
    printf("2ndboot image start \n");

    /* normal start */
    printf("os image start\r\n");
    ota_2ndboot_os_entry();
}

#ifdef AOS_2ND_BOOT_AB
extern int ota_get_ab_boot(void);
extern void ota_switch_ab_boot(void);

int ota_2ndboot_ab_get(void)
{
    return ota_get_ab_boot();
}
void ota_2ndboot_ab_switch(void)
{
    ota_switch_ab_boot();
}
#endif



