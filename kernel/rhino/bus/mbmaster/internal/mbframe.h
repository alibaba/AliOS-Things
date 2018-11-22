#ifndef _MB_FRAME_H
#define _MB_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

/* frame len and payload offset define */
#define PDU_LENGTH_MAX     253
#define PDU_LENGTH_MIN     1
#define PDU_FUNC_FIELD_OFF 0
#define PDU_DATA_FIELD_OFF 1

typedef void (*mb_frame_start_t)(void);
typedef void (*mb_frame_stop_t)(void);
typedef mb_error_code_t(*mb_frame_receive_t)(uint8_t * pucRcvAddress, uint8_t ** pucFrame,
                                             uint16_t * pusLength);
typedef mb_error_code_t(*mb_frame_send_t)(uint8_t slaveAddress);
typedef void(*mb_frame_close_t)(void);

bool    mb_set_cur_pdu_len(uint16_t send_pdu_len);
void    mb_get_pdu_buf(uint8_t **pdu_buf);

#ifdef __cplusplus
}
#endif
#endif
