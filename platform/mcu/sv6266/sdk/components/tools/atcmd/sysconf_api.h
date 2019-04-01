#ifndef _SYSCONF_API_H_
#define _SYSCONF_API_H_

typedef struct t_SYS_CONF_INFO
{
    u8  nAutoConnect;
    u8  nConnectMode;
} SYS_CONF_INFO;

void init_global_conf();
int do_scan_AP();
int do_wifi_connect();
int do_wifi_disconnect();
int do_wifi_auto_connect();
int set_auto_connect_flag(u8 bEnable);
int get_auto_connect_flag();
void recordAP();

int write_wifi_config();
int read_wifi_config();
int remove_wifi_config();
int write_sys_config();
int read_sys_config();
int remove_sys_config();
#endif
