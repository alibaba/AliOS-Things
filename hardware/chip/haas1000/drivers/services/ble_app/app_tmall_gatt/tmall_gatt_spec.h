#ifndef _TMALL_GATT_SPEC_H_
#define _TMALL_GATT_SPEC_H_

#include "rwip_config.h"     // SW configuration
#define BLE_TMALL_GATT_SPEC     0

#if (BLE_TMALL_GATT_SPEC)

#include "cqueue.h"
#include "cmsis_os.h"

#define TMALL_GATT_SPEC_DATA_LENGTH     240
#define TMALL_GATT_SPEC_BUF_LENGTH      244
#define TMALL_GATT_SEND_DATA_THREAD_SIGNAL  11
#define TMALL_GATT_RECEIVE_MAX_MTU      1024
#define TMALL_GATT_SPEC_TX_Q_LENGTH     2046
#define TMALL_GATT_SPEC_THREAD_WAITTIME 5000

typedef void (*tmall_spec_transmit_data_t)(uint8_t* ptrData, uint32_t dataLen);

enum tmall_gatt_spec_error_code
{
    TMALL_GATT_SPEC_SUCCESS = 0,
    TMALL_GATT_SPEC_TX_ERROR,
};

typedef struct{
    uint8_t     msg_id;
    uint8_t     encry;
    uint8_t     vers;
    uint8_t     opcode;
    uint8_t     len;
    uint8_t     msg_seq;
    uint8_t     seq_max;
} TMALL_GATT_REC_DATA_HEADER_T;

typedef struct{
    uint8_t     opcode;
    uint8_t     param[TMALL_GATT_SPEC_TX_Q_LENGTH];
} TMALL_GATT_DATA_SEND_T;


typedef struct {
    osThreadId   txThreadId;
    osThreadId   rxThreadId;
    CQueue       txQueue;
    CQueue       rxQueue;
    tmall_spec_transmit_data_t  transmitHander;
    uint32_t     currentReceivedMagicCode;
    uint8_t      currentReceivedPacketType;
    uint8_t      txBuf[TMALL_GATT_SPEC_TX_Q_LENGTH];
    uint8_t      rxBuf[TMALL_GATT_RECEIVE_MAX_MTU];
    TMALL_GATT_REC_DATA_HEADER_T assemble_hearder;
    TMALL_GATT_REC_DATA_HEADER_T disassem_hearder;
} TMALL_GATT_SPEC_ENV_T;


#define TMALL_GATT_REC_HEADER_MSG_ID_MSK(x)     ((uint8_t)((x) & 0x0F))
#define TMALL_GATT_REC_HEADER_ENCRY_MSK(x)     ((uint8_t)(((x) >> 4) & 0x01))
#define TMALL_GATT_REC_HEADER_VERS_MSK(x)     ((uint8_t)(((x) >> 5) & 0x07))
#define TMALL_GATT_REC_SEQ_MSK(x)     ((uint8_t)((x) & 0x0F))
#define TMALL_GATT_REC_SEQ_MAX_MSK(x)     ((uint8_t)(((x) >> 4) & 0x0F))
#define TMALL_GATT_ASSEM_HEADER(header_array, msg_id, encry, vers, opcode, len, msg_seq, seq_max)     uint8_t header_array[4] = \
                    { ((msg_id&0x0F) | ((encry&0x01)<<4)) |((vers&0x07)<<5), opcode, ((msg_seq&0x0F) | ((seq_max&0x0F)<<4)), len}

uint8_t tmall_gatt_spec_send_data(TMALL_GATT_DATA_SEND_T *ptrParam, uint32_t paramLenwithOpcode);
void tmall_gatt_spec_init(void);
void tmall_gatt_receive_data_assemble(uint8_t *ptrParam, uint32_t paramLen);
void tmall_gatt_receive_data_clear_header_rxbuf(bool msg_id_clear);


#endif
#endif