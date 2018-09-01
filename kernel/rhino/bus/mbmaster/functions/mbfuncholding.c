#include "stdlib.h"
#include "string.h"

#include "../port/port.h"

#include "../internal/mb.h"
#include "../internal/mbframe.h"
#include "../internal/mbproto.h"
#include "../internal/mbconfig.h"

#define MB_PDU_FUNC_READ_ADDR_OFF               ( MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_READ_REGCNT_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE                   ( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX             ( 0x007D )

#define MB_PDU_FUNC_WRITE_ADDR_OFF              ( MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_WRITE_VALUE_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE                  ( 4 )

#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF          ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF        ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN          ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX        ( 0x0078 )

#define MB_PDU_FUNC_READWRITE_READ_ADDR_OFF     ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF   ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF    ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF  ( MB_PDU_DATA_OFF + 6 )
#define MB_PDU_FUNC_READWRITE_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 8 )
#define MB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF  ( MB_PDU_DATA_OFF + 9 )
#define MB_PDU_FUNC_READWRITE_SIZE_MIN          ( 9 )

#define MB_PDU_RESP_READ_ADDR_OFF           ( MB_PDU_DATA_OFF )
#define MB_PDU_RESP_READ_REGCNT_OFF        ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_RESP_READ_VALUE_OFF          ( MB_PDU_DATA_OFF + 1 )

#if MB_FUNC_READ_HOLDING_ENABLED > 0
mb_exception_t
mb_read_holding_reginster(uint8_t slave_addr, uint16_t start_addr, uint16_t quantity,
                       uint8_t *respond_buf, uint8_t *respond_count)
{
    UCHAR       *pPDUBuf;
    mb_exception_t status = MB_EX_NONE;
    UCHAR        RespondCount;
    BOOL         ret;

    if(mb_run_state_is_idle() == FALSE) {
        mb_timer_wait_t35();
    }
        
   if(mb_run_state_is_idle() == FALSE) {
        return MB_EX_MB_BUSY;
    }
    
    if(slave_addr > MB_ADDRESS_MAX) {
        return MB_EX_ILLEGAL_SLAVE_ADDRESS;
    }

    mb_set_cur_slave_address(slave_addr);

    vMBGetPDUBuf(&pPDUBuf);
    pPDUBuf[MB_PDU_FUNC_OFF] = MB_FUNC_READ_HOLDING_REGISTER;
    pPDUBuf[MB_PDU_FUNC_READ_ADDR_OFF]     = htobe(start_addr);
    pPDUBuf[MB_PDU_FUNC_READ_ADDR_OFF+1]   = htobe(start_addr) >> 8;
    pPDUBuf[MB_PDU_FUNC_READ_REGCNT_OFF]   = htobe(quantity);
    pPDUBuf[MB_PDU_FUNC_READ_REGCNT_OFF+1] = htobe(quantity) >> 8;

    xMBSetCurPDULength(MB_PDU_SIZE_MIN+MB_PDU_FUNC_READ_SIZE);

    ret = mb_event_post(EV_PDUDATA_READY);
    if(ret = FALSE) {
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

    if(pPDUBuf[MB_PDU_FUNC_OFF] & 0x80) {
        respond_buf[0] = pPDUBuf[MB_PDU_RESP_READ_REGCNT_OFF];
        mb_event_post(EV_REQU_FINISHED);
        return MB_EX_RESPOND_EXCEPTION;
    }

    *respond_count = pPDUBuf[MB_PDU_RESP_READ_REGCNT_OFF];
    memcpy(respond_buf, &pPDUBuf[MB_PDU_RESP_READ_VALUE_OFF], *respond_count);

    mb_event_post(EV_REQU_FINISHED);
    return status;
}
#endif


