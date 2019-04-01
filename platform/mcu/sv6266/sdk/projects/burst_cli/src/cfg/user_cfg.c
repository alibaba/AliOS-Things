#include <stdint.h>
struct wifi_cfg {
    char mac_addr[6];
};

const struct wifi_cfg g_user_cfg __attribute__((section(".user_data"))) = {
    .mac_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};
