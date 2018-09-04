#include "stdlib.h"
#include "string.h"

#include "../port/port.h"

#include "../internal/mb.h"
#include "../internal/mbframe.h"
#include "../internal/mbproto.h"
#include "../internal/mbconfig.h"

#define MB_PDU_FUNC_READ_ADDR_OFF           ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_READ_COILCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE               ( 4 )
#define MB_PDU_FUNC_READ_COILCNT_MAX        ( 0x07D0 )

#define MB_PDU_FUNC_WRITE_ADDR_OFF          ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_VALUE_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE              ( 4 )

#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF      ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF   ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF   ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF    ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN      ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_MAX   ( 0x07B0 )

#define MB_PDU_RESP_READ_ADDR_OFF           ( MB_PDU_DATA_OFF )
#define MB_PDU_RESP_READ_COILCNT_OFF        ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_RESP_READ_VALUE_OFF          ( MB_PDU_DATA_OFF + 1 )

#if MB_FUNC_READ_COILS_ENABLED > 0

mb_exception_t
eMBReadCoils(UCHAR ucSlaveAddr, USHORT usStartAddr, USHORT Quantity,
             UCHAR *pucRespondBuf, UCHAR *ucRespondCount)
{
    UCHAR       *pPDUBuf;
    mb_exception_t status = MB_EX_NONE;
    UCHAR        RespondCount;

     if(mb_run_state_is_idle() == FALSE) {
         mb_timer_wait_t35();
     }
         
    if(mb_run_state_is_idle() == FALSE) {
         return MB_EX_MB_BUSY;
     }

    if(ucSlaveAddr > MB_ADDRESS_MAX) {
        return MB_EX_ILLEGAL_SLAVE_ADDRESS;
    }

    mb_set_cur_slave_address(ucSlaveAddr);

    vMBGetPDUBuf(&pPDUBuf);
    pPDUBuf[MB_PDU_FUNC_OFF] = MB_FUNC_READ_COILS;
    pPDUBuf[MB_PDU_FUNC_READ_ADDR_OFF] = htobe(usStartAddr);
    pPDUBuf[MB_PDU_FUNC_READ_ADDR_OFF+1] = htobe(usStartAddr) >> 8;
    pPDUBuf[MB_PDU_FUNC_READ_COILCNT_OFF] = htobe(Quantity);
    pPDUBuf[MB_PDU_FUNC_READ_COILCNT_OFF+1] = htobe(Quantity) >> 8;

    xMBSetCurPDULength(MB_PDU_SIZE_MIN+MB_PDU_FUNC_READ_SIZE);

    mb_event_post(EV_PDUDATA_READY);

    /* wait slave respond */
    status = mb_wait_slave_respond();

    if(status != MB_EX_NONE) {
        mb_event_post(EV_REQU_FINISHED);
        return status;
    }

    if((pucRespondBuf == NULL) || (ucRespondCount == NULL)) {
        mb_event_post(EV_REQU_FINISHED);
        return MB_EX_INVALID_PARAM;
    }

    if(pPDUBuf[MB_PDU_FUNC_OFF] & 0x80) {
        pucRespondBuf[0] = pPDUBuf[MB_PDU_RESP_READ_COILCNT_OFF];
        mb_event_post(EV_REQU_FINISHED);
        return MB_EX_RESPOND_EXCEPTION;
    }

    *ucRespondCount = pPDUBuf[MB_PDU_RESP_READ_COILCNT_OFF];
    memcpy(pucRespondBuf, &pPDUBuf[MB_PDU_RESP_READ_VALUE_OFF], *ucRespondCount);

    mb_event_post(EV_REQU_FINISHED);
    return status;
}
#endif

