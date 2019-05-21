#ifndef _APP_LWIP_TCP_H_
#define _APP_LWIP_TCP_H_

#define APP_LWIP_TCP_SERVER_PORT        8050 

UINT32 app_lwip_tcp_init(void);
void app_lwip_tcp_deinit(void);
int app_lwip_tcp_send_packet(UINT8 *data, UINT32 len);

#endif // _APP_LWIP_TCP_H_

