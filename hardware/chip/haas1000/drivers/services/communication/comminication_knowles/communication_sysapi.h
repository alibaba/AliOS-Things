#ifndef __COMMUNICATION_SYSAPI_H__
#define __COMMUNICATION_SYSAPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include "stdio.h"

#define TRACE(str, ...)                 do { printf("%s/" str "\n", __FUNCTION__, __VA_ARGS__); } while (0)
#define ASSERT(cond, str, ...)          \
    do { if (!(cond)) { printf("[ASSERT]%s/" str, __FUNCTION__, __VA_ARGS__); while (1); } } while (0)
#define TRACE_TIME(str, ...)            TRACE(str, __VA_ARGS__)

int write_sig_data(const unsigned char *data, unsigned int len);
int write_code_data(const unsigned char *data, unsigned int len);
void programmer_main(void);
#else
#include "hal_trace.h"
#include "hal_timer.h"

#define TRACE_TIME(str, ...)            TRACE("[%05u] " str, TICKS_TO_MS(hal_sys_timer_get()), ##__VA_ARGS__)
#endif

#define UART_OUT_SIGNAL_ID 0x19
enum DOWNLOAD_TRANSPORT {
    TRANSPORT_USB,
    TRANSPORT_UART,
};

enum XFER_TIMEOUT {
    XFER_TIMEOUT_SHORT,
    XFER_TIMEOUT_MEDIUM,
    XFER_TIMEOUT_LONG,
    XFER_TIMEOUT_IDLE,

    XFER_TIMEOUT_QTY
};

enum UART_DMA_STATE {
    UART_DMA_IDLE,
    UART_DMA_START,
    UART_DMA_DONE,
    UART_DMA_ERROR,
};

void init_transport(void);
void deinit_transport(void);
void reinit_transport(void);
void set_send_timeout(uint32_t timeout);
int secure_settings_valid(void);
int secure_boot_enabled(void);
int usb_enabled(void);
int usb_connected(void);
unsigned short get_boot_security_value(void);
unsigned int get_boot_key_index(void);
int send_data(const unsigned char *buf, unsigned int len);
int recv_data(unsigned char *buf, unsigned int len);
int recv_data_dma(unsigned char *buf, unsigned int len, unsigned int expect);
void recv_data_state_get(enum UART_DMA_STATE *state);
void recv_data_reset(void);
int handle_error(void);
int cancel_input(void);

int verify_signature(const unsigned char *key, const unsigned char *sig, const unsigned char *data, unsigned int len);

int debug_read_enabled(void);
int debug_write_enabled(void);

int get_flash_boot_flag(void);
void set_flash_boot_flag(int flag);
void system_reboot(void);
void system_shutdown(void);
void system_flash_boot(void);
void system_set_bootmode(unsigned int bootmode);
void system_clear_bootmode(unsigned int bootmode);
unsigned int system_get_bootmode(void);

void wait_trace_finished(void);

unsigned int get_current_time(void);
uint32_t ama_uart_get_fifo_data(uint8_t *buf);
uint32_t avil_len_of_the_fifo();
void send_message();

#ifdef __cplusplus
}
#endif

#endif

