#ifndef __UMESH_H__
#define __UMESH_H__
#include <string.h>
#include <stdint.h>

typedef int (*umesh_ap_info_cb)(const char *ssid, const  char *pwd, const uint8_t *bssid, void *context);

void *umesh_network_init();
int umesh_is_connected(void *hanlde);
int umesh_network_deinit(void *handle);
int umesh_zero_config_request(void *handle);
int umesh_zero_config_set_cb(void *hanlde, umesh_ap_info_cb cb, void *context);
#endif /* __UMESH_H__ */
