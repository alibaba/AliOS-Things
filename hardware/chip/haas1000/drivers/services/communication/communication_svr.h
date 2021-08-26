#ifndef __COMMUNICATION_SVR_H__
#define __COMMUNICATION_SVR_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*communication_receive_func_typedef)(uint8_t *buf, uint8_t len);

void communication_init(void);
int communication_receive_register_callback(communication_receive_func_typedef p);
int communication_send_buf(uint8_t * buf, uint8_t len);


#ifdef KNOWLES_UART_DATA
void uart_audio_init();
void uart_audio_deinit();
#endif


#ifdef __cplusplus
}
#endif

#endif

