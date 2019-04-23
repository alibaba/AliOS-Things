/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <breeze_hal_os.h>

#ifdef BUILD_AOS
#include "chip_code.h"
#endif

int os_timer_new(os_timer_t *timer, os_timer_cb_t cb, void *arg, int ms)
{
    return aos_timer_new_ext((aos_timer_t *)timer, cb, arg, ms, 0, 0);
}

int os_timer_start(os_timer_t *timer)
{
    return aos_timer_start((aos_timer_t *)timer);
}

int os_timer_stop(os_timer_t *timer)
{
    return aos_timer_stop((aos_timer_t *)timer);
}

void os_timer_free(os_timer_t *timer)
{
    aos_timer_free((aos_timer_t *)timer);
}

void os_reboot()
{
    aos_reboot();
}

void os_msleep(int ms)
{
    aos_msleep(ms);
}

long long os_now_ms()
{
    return aos_now_ms();
}

int os_kv_set(const char *key, const void *value, int len, int sync)
{
    return aos_kv_set(key, value, len, sync);
}

int os_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return aos_kv_get(key, buffer, buffer_len);
}

int os_kv_del(const char *key)
{
    return aos_kv_del(key);
}

int os_rand(void)
{
    return 0;
}

/*
 * Use this API to acquire os version and vendor info:
 * 1.format on AliOS thing OS:
 * "version number(n bytes):chip code(4bytes)", e.g, "2.1.0:00 00 03 04".
 * 2.For non AliOS things platform, should follow:
 * "non-ali:vendor info", e.g., "non-ali:manufacture1".
 * Vendor should fill info itself with string format.
 * */
static char NON_ALI_INFO[] = "non-ali:vendor1";
int os_get_version_chip_info(char* info, uint8_t* p_len)
{
    if(info == NULL || p_len == NULL){
        printf("Invalid paramters\n");
        return -1;
    }
#ifdef BUILD_AOS
    uint8_t chip_code[4] = { 0 };
    uint8_t chip_id_str[8] = { 0 };
    const char *aostype = "AOS";
    uint8_t suffix_len = 0;
    char t_os_info[20] = { 0 };
    strncpy(t_os_info, (const char *)aos_version_get(), sizeof(t_os_info) - 1);
    char *m_os_type = strtok(t_os_info, "-");
    if (strcmp(aostype, m_os_type) == 0) {
        m_os_type = strtok(NULL, "-");
        m_os_type = strtok(NULL, "-");
        strcat(m_os_type, ":");

        suffix_len = strlen(m_os_type);
        memcpy(info, m_os_type, suffix_len);
        chip_code_st *p_chip_code_obj = get_chip_code(MCU_FAMILY);
        if (p_chip_code_obj != NULL) {
            chip_code[0] = (uint8_t)(p_chip_code_obj->vendor >> 8);
            chip_code[1] = (uint8_t)p_chip_code_obj->vendor;
            chip_code[2] = (uint8_t)(p_chip_code_obj->id >> 8);
            chip_code[3] = (uint8_t)p_chip_code_obj->id;
        }

        hex2string(chip_code, sizeof(chip_code), chip_id_str);
        memcpy(info + suffix_len, chip_id_str, sizeof(chip_id_str));
        suffix_len += sizeof(chip_id_str);
        *p_len = suffix_len;
    }
#else
    memcpy(info, NON_ALI_INFO, strlen(NON_ALI_INFO));
    *p_len = strlen(NON_ALI_INFO);
#endif
    return 0;
}
