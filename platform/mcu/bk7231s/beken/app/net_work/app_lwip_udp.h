#ifndef __APP_LWIP_UDP_H__
#define __APP_LWIP_UDP_H__

UINT32 app_lwip_udp_init(void);
void app_lwip_udp_deinit(void);
int app_lwip_udp_send_packet (UINT8 *data, UINT32 len);
void app_lwip_disconnect_cmd_udp(void);

#endif
// eof

