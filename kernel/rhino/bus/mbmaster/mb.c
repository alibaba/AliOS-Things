/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port/port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "internal/mb.h"
#include "internal/mbconfig.h"
#include "internal/mbframe.h"
#include "internal/mbproto.h"
#include "internal/mbfunc.h"

#if MB_RTU_ENABLED == 1
#include "rtu/mbrtu.h"
#endif
#if MB_ASCII_ENABLED == 1
#include "rtu/mbascii.h"
#endif
#if MB_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

static UCHAR                   mb_current_slave_addr;
static volatile mb_run_state_t mb_run_state;
static mb_mode_t               mb_current_mode;

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
}mb_state = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 */
static peMBFrameSend    mb_frame_send_func;
static pvMBFrameStart   mb_frame_start_func;
static pvMBFrameStop    mb_frame_stop_func;
static peMBFrameReceive mb_frame_receive_func;
static pvMBFrameClose   mb_frame_close_func;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 */
BOOL( *mb_frame_byte_received_func) (void);
BOOL( *mb_frame_byte_send_func) (void);
BOOL( *mb_timer_expired_func) (void);

BOOL( *pxMBFrameCBReceiveFSMCur ) (void);
BOOL( *pxMBFrameCBTransmitFSMCur ) (void);

mb_error_code_t mb_init( mb_mode_t mode, uint8_t port, uint32_t baud_rate, mb_parity_t parity)
{
    mb_error_code_t eStatus = MB_ENOERR;

    switch ( mode ) {
#if MB_RTU_ENABLED > 0
        case MB_RTU:
            mb_frame_start_func      = eMBRTUStart;
            mb_frame_stop_func       = eMBRTUStop;
            mb_frame_send_func       = eMBRTUSend;
            mb_frame_receive_func    = eMBRTUReceive;
            mb_frame_close_func      = MB_PORT_HAS_CLOSE ? mb_serial_close : NULL;
            mb_frame_byte_received_func = xMBRTUReceiveFSM;
            mb_frame_byte_send_func     = xMBRTUTransmitFSM;
            mb_timer_expired_func     = xMBRTUTimerExpired;

            eStatus = eMBRTUInit(port, baud_rate, parity );
            break;
#endif
#if MB_ASCII_ENABLED > 0
        case MB_ASCII:
            mb_frame_start_func      = eMBASCIIStart;
            mb_frame_stop_func       = eMBASCIIStop;
            mb_frame_send_func       = eMBASCIISend;
            mb_frame_receive_func    = eMBASCIIReceive;
            mb_frame_close_func      = MB_PORT_HAS_CLOSE ? mb_serial_close : NULL;
            mb_frame_byte_received_func = xMBASCIIReceiveFSM;
            mb_frame_byte_send_func     = xMBASCIITransmitFSM;
            mb_timer_expired_func     = xMBASCIITimerT1SExpired;

            eStatus = eMBASCIIInit( ucMBAddress, ucPort, ulBaudRate, eParity );
            break;
#endif
        default:
            eStatus = MB_EINVAL;
    }

    if( eStatus == MB_ENOERR ) {
        if( !mb_event_init(  ) )
        {
            /* port dependent event module initalization failed. */
            eStatus = MB_EPORTERR;
        }
        else
        {
            mb_current_mode = mode;
            mb_state = STATE_DISABLED;
        }
    }
    return eStatus;
}

#if MB_TCP_ENABLED > 0
mb_error_code_t mb_tcp_init( USHORT tcp_port)
{
    /* reserve for tcp */
}
#endif

mb_error_code_t mb_close( void )
{
    mb_error_code_t eStatus = MB_ENOERR;

    if( mb_state == STATE_DISABLED )
    {
        if( mb_frame_close_func != NULL )
        {
            mb_frame_close_func(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

mb_error_code_t mb_enable( void )
{
    mb_error_code_t eStatus = MB_ENOERR;

    if( mb_state == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        mb_frame_start_func(  );
        mb_state = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

mb_error_code_t mb_disable( void )
{
    mb_error_code_t    eStatus;

    if( mb_state == STATE_ENABLED )
    {
        mb_frame_stop_func(  );
        mb_state = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( mb_state == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

mb_error_code_t mb_poll( void )
{
    static UCHAR         *mb_frame;
    static UCHAR          rev_address;
    static USHORT         length;

    int             i;
    mb_error_code_t eStatus = MB_ENOERR;
    mb_event_t      eEvent;

    /* Check if the protocol stack is ready. */
    if( mb_state != STATE_ENABLED )
    {
        return MB_EILLSTATE;
    }

    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( mb_event_get( &eEvent ) == TRUE )
    {
        mb_log(MB_LOG_DEBUG, "receive event=0x%x\n", eEvent);

        switch ( eEvent )
        {
        case EV_READY:
            mb_run_state = STATE_IDLE;
            break;
        case EV_PDUDATA_READY:
            mb_run_state = STATE_TX_READY;
            eStatus = mb_frame_send_func(mb_current_slave_addr);
            break;
            
        case EV_FRAME_SENT:
            if( mb_current_slave_addr == MB_ADDRESS_BROADCAST) {
                mb_turnaround_timer_enable();
            }
            else {
                mb_response_timer_enable();
            }
            break;
            
        case EV_FRAME_RECEIVED:
            eStatus = mb_frame_receive_func( &rev_address, &mb_frame, &length);
            if( eStatus == MB_ENOERR )
            {
                /* Check if the frame is for us. If not ignore the frame. */
                if( rev_address == mb_current_slave_addr )
                {
                    ( void )mb_event_post( EV_RESPOND_PROCESS );
                }
                else {
                    mb_log(MB_LOG_ERROR, "slave addr is not the sent frame\n");
                    mb_event_post(EV_RESPOND_ERROR);
                }
            }
            else {
                mb_event_post(EV_RESPOND_ERROR);
            }
            break;

        case EV_FRAME_TIMEOUT:
            mb_event_post( EV_RESPOND_TIMEOUT );
            break;

        case EV_REQU_FINISHED:
            mb_set_run_state(STATE_END);
            mb_t35_timer_enable();
            break;
        }
    }
    return MB_ENOERR;
}

/* Set current slave address. */
void mb_set_cur_slave_address( UCHAR slave_address ){
    mb_current_slave_addr = slave_address;
}

mb_run_state_t mb_get_run_state(){
    return mb_run_state;
}

void mb_set_run_state(mb_run_state_t state){
    mb_run_state = state;
}

BOOL mb_run_state_is_idle(){
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER();
    if(mb_run_state == STATE_IDLE) {
        MB_CRITICAL_EXIT();
        return TRUE;
    }

    MB_CRITICAL_EXIT();
    return FALSE;
}

