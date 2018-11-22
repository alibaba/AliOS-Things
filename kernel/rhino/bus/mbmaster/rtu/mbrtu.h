#ifndef _MB_RTU_H
#define _MB_RTU_H

#ifdef __cplusplus
extern "C" {
#endif

mb_error_code_t mb_rtu_init(uint8_t port, uint32_t baudrate, mb_parity_t parity);
void            mb_rtu_start(void);
void            mb_rtu_stop(void);
mb_error_code_t mb_rtu_receive( uint8_t * rcv_address, uint8_t **frame, uint16_t *length);
mb_error_code_t mb_rtu_send(uint8_t slave_address);
bool            mb_rtu_rec_frame(void);
bool            mb_rtu_transmit_frame(void);
bool            mb_rtu_timer_expired(void);

#ifdef __cplusplus
}
#endif
#endif
