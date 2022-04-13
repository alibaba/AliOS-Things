#ifndef __SYS_API_USB_CDC_H__
#define __SYS_API_USB_CDC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tool_msg.h"
#include "hal_trace.h"
#include "hal_timer.h"

#define TRACE_TIME(str, ...)            TRACE("[%05u] " str, TICKS_TO_MS(hal_sys_timer_get()), ##__VA_ARGS__)

extern const unsigned int default_recv_timeout_short;
extern const unsigned int default_recv_timeout_idle;
extern const unsigned int default_recv_timeout_4k_data;
extern const unsigned int default_send_timeout;

void reset_transport(void);

void set_recv_timeout(unsigned int timeout);
void set_send_timeout(unsigned int timeout);

int send_data(const unsigned char *buf, size_t len);
int recv_data_ex(unsigned char *buf, size_t len, size_t expect, size_t *rlen);
int handle_error(void);
int cancel_input(void);

void system_reboot(void);
void system_shutdown(void);
void system_flash_boot(void);
void system_set_bootmode(unsigned int bootmode);
void system_clear_bootmode(unsigned int bootmode);
unsigned int system_get_bootmode(void);

#ifdef __cplusplus
}
#endif

#endif

