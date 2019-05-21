#ifndef _HOSTAPD_INTF_PUB_H_
#define _HOSTAPD_INTF_PUB_H_

extern int hapd_intf_ioctl(unsigned long arg);
extern void hapd_intf_ke_rx_handle(INT32 dummy);
extern int hapd_intf_set_ap(void *beacon, int bcn_len, int head_len);

extern void wpa_buffer_scan_results(void);
extern void wpa_clear_scan_results(void);
extern void wpa_enable_traffic_port_at_opensystem(void);

#endif
// eof

