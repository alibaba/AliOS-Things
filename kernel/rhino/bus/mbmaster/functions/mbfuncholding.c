#include "stdlib.h"
#include "string.h"

#include "../port/port.h"

#include "../internal/mb.h"
#include "../internal/mbframe.h"
#include "../internal/mbproto.h"

#define PDU_REQ_READ_ADDR_OFF          ( PDU_DATA_FIELD_OFF  + 0)
#define PDU_REQ_READ_REGCNT_OFF        ( PDU_DATA_FIELD_OFF  + 2 )
#define PDU_REQ_READ_SIZE              ( 4 )
#define PDU_REQ_READ_REGCNT_MAX        ( 0x007D )

#define PDU_RESP_READ_REGCNT_OFF       ( PDU_DATA_FIELD_OFF + 0 )
#define PDU_RESP_READ_VALUE_OFF        ( PDU_DATA_FIELD_OFF + 1 )
 
#if MB_FUNC_READ_HOLDING_ENABLED > 0
mb_exception_t mb_read_holding_reginster(uint8_t slave_addr, uint16_t start_addr,
                                         uint16_t quantity, uint8_t *respond_buf,
                                         uint8_t *respond_count)
{
    uint8_t       *pdu_buf;
    mb_exception_t status = MB_EX_NONE;
    mb_exception_t ret;
    uint8_t expect_count;

    if(mb_run_state_is_idle() == false) {
        mb_timer_wait_t35();
    }
        
   if(mb_run_state_is_idle() == false) {
        return MB_EX_MB_BUSY;
    }
    
    if(slave_addr > SLAVE_ADDR_MAX) {
        return MB_EX_ILLEGAL_SLAVE_ADDRESS;
    }

    expect_count = quantity * 2;

    mb_set_cur_slave_address(slave_addr);

    mb_get_pdu_buf(&pdu_buf);
    pdu_buf[PDU_FUNC_FIELD_OFF] = FUNC_CODE_READ_HOLDING_REGISTERS;
    pdu_buf[PDU_REQ_READ_ADDR_OFF]     = htobe(start_addr);
    pdu_buf[PDU_REQ_READ_ADDR_OFF+1]   = htobe(start_addr) >> 8;
    pdu_buf[PDU_REQ_READ_REGCNT_OFF]   = htobe(quantity);
    pdu_buf[PDU_REQ_READ_REGCNT_OFF+1] = htobe(quantity) >> 8;

    mb_set_cur_pdu_len(PDU_LENGTH_MIN + PDU_REQ_READ_SIZE);

    ret = mb_event_post(EV_PDUDATA_READY);
    if(ret == false) {
        mb_log(MB_LOG_ERROR, "sent event error\n");
    }

    /* wait slave respond */
    mb_log(MB_LOG_DEBUG, "waiting for respond\n");
    
    status = mb_wait_slave_respond();

    if(status != MB_EX_NONE) {
        mb_event_post(EV_REQU_FINISHED);
        return status;
    }

    if((respond_buf == NULL) || (respond_count == NULL)) {
        mb_event_post(EV_REQU_FINISHED);
        return MB_EX_INVALID_PARAM;
    }

    if(pdu_buf[PDU_FUNC_FIELD_OFF] & 0x80) {
        respond_buf[0] = pdu_buf[PDU_RESP_READ_REGCNT_OFF];
        mb_event_post(EV_REQU_FINISHED);
        return MB_EX_RESPOND_EXCEPTION;
    }

    *respond_count = pdu_buf[PDU_RESP_READ_REGCNT_OFF];
    if(expect_count != (*respond_count)) {
        mb_log(MB_LOG_ERROR, "frame length is error\n");
        mb_event_post(EV_REQU_FINISHED);
        return MB_EX_RESPOND_ERROR;
    }

    memcpy(respond_buf, &pdu_buf[PDU_RESP_READ_VALUE_OFF], *respond_count);

    mb_event_post(EV_REQU_FINISHED);
    return status;
}
#endif


