#include "soc_types.h"
#include "atcmd.h"
#include "atcmdlib.h"
#include "mac_cfg.h"
#include "sa_cfg.h"
#include "error.h"
#include "osal.h"
#include <string.h>

#define M_CFG_MAC_SIZE      (18)
    //{ATCMD_CFGPARSER,            At_CfgParser,             0},

int At_CfgParser (stParam *param) {
    void *cfg_handle = wifi_cfg_init();
    char test[6];
    wifi_cfg_get_addr1(cfg_handle, test);
    printf("addr1: %02X:%02X:%02X:%02X:%02X:%02X\n", test[0], test[1], test[2], test[3], test[4], test[5]);
    wifi_cfg_get_addr2(cfg_handle, test);
    printf("addr2: %02X:%02X:%02X:%02X:%02X:%02X\n", test[0], test[1], test[2], test[3], test[4], test[5]);
    wifi_cfg_deinit(cfg_handle);
    return ERROR_SUCCESS_NO_RSP;
}


int At_CfgTester (stParam *param) {
    cfg_dump_sa_pre_table();
    printf("\n[%s] data!!!!!!!!!!!!!!!!!!!!!!!!![S]\n", __func__);
    cfg_dump_sa_table(4096);
    printf("\n[%s] data!!!!!!!!!!!!!!!!!!!!!!!!![E]\n", __func__);
    cfg_dump_sa_post_table();

    uint8_t *ptr = (uint8_t *)OS_MemAlloc(sizeof(g_sa_cfg));
    if (ptr == NULL) {
        printf("[%s] no memory, please check\n", __func__);
        return ERROR_MEMORY_FAILED;
    }
    memcpy(ptr, &g_sa_cfg, sizeof(g_sa_cfg));
    int i;
    for (i = 0; i < sizeof(g_sa_cfg); i++) {
        ptr[i]++;
    }
    cfg_sa_write_cfg((struct sa_cfg*)ptr, sizeof(g_sa_cfg));
    OS_MemFree(ptr);
    return ERROR_SUCCESS_NO_RSP;
}

int At_CfgWifiTester (stParam *param) {
    void *cfg_handle = wifi_cfg_init();
    if (cfg_handle == NULL) {
        return ERROR_MEMORY_FAILED;
    }
    unsigned char test[6];
    char buf[36];
    int i;
    wifi_cfg_get_addr1(cfg_handle, test);
    for (i = 0; i < 6; i++) {
        test[i]++;
    }
    snprintf(buf, 36, "%02X:%02X:%02X:%02X:%02X:%02X", test[0], test[1], test[2], test[3], test[4], test[5]);
    wifi_cfg_replace_mem_addr1(cfg_handle, buf);
    wifi_cfg_get_addr2(cfg_handle, test);
    for (i = 0; i < 6; i++) {
        test[i]++;
    }
    snprintf(buf, 36, "%02X:%02X:%02X:%02X:%02X:%02X", test[0], test[1], test[2], test[3], test[4], test[5]);
    wifi_cfg_replace_mem_addr2(cfg_handle, buf);
    wifi_cfg_write_cfg(cfg_handle);
    wifi_cfg_deinit(cfg_handle);
    return ERROR_SUCCESS_NO_RSP;
}

