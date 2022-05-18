#include "esp_system.h"

void aos_hal_reboot(void)
{
    esp_restart();
}

char g_chip_id[32];
const char *aos_get_device_name(void)
{
    uint8_t mac[32] = {0};
    extern esp_err_t esp_read_mac(uint8_t *mac, esp_mac_type_t type);
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    memset(g_chip_id, 0, 32);
    snprintf(g_chip_id, 32, "haas_%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return g_chip_id;
}

