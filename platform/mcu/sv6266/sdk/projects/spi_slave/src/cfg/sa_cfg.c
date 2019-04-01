#include <stdint.h>
struct wifi_cfg {
    char mac_addr[6];
};

extern const struct wifi_cfg g_sa_cfg __attribute__((section(".sa_mp_data")));
const struct wifi_cfg g_sa_cfg __attribute__((section(".sa_mp_data"))) = {
    .mac_addr = {0x12, 0x23, 0x34, 0x45, 0x56, 0x67},
};
