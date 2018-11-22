#include "stdlib.h"
#include "string.h"

#include "port/port.h"

#include "internal/mb.h"
#include "internal/mbframe.h"
#include "internal/mbproto.h"

#if MB_RTU_ENABLED == 1
#include "rtu/mbrtu.h"
#endif

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

static uint8_t                 mb_current_slave_addr;
static volatile mb_run_state_t mb_run_state;
static mb_mode_t               mb_current_mode;

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} mb_state = STATE_NOT_INITIALIZED;

static mb_frame_send_t    mb_frame_send_func;
static mb_frame_start_t   mb_frame_start_func;
static mb_frame_stop_t    mb_frame_stop_func;
static mb_frame_receive_t mb_frame_receive_func;
static mb_frame_close_t   mb_frame_close_func;

bool( *mb_frame_byte_received_func)(void);
bool( *mb_frame_byte_send_func)(void);
bool( *mb_timer_expired_func)(void);

mb_error_code_t mb_init(mb_mode_t mode, uint8_t port, uint32_t baud_rate, mb_parity_t parity)
{
    mb_error_code_t status = MB_ENOERR;

    switch (mode) {
#if MB_RTU_ENABLED > 0
        case MB_RTU:
            mb_frame_start_func      = mb_rtu_start;
            mb_frame_stop_func       = mb_rtu_stop;
            mb_frame_send_func       = mb_rtu_send;
            mb_frame_receive_func    = mb_rtu_receive;
            mb_frame_close_func      = MB_PORT_HAS_CLOSE ? mb_serial_close : NULL;
            mb_frame_byte_received_func = mb_rtu_rec_frame;
            mb_frame_byte_send_func     = mb_rtu_transmit_frame;
            mb_timer_expired_func     = mb_rtu_timer_expired;

            status = mb_rtu_init(port, baud_rate, parity);
            break;
#endif

        default:
            status = MB_EINVAL;
    }

    if (status == MB_ENOERR) {
        if (!mb_event_init()) {
            status = MB_EPORTERR;
        }
        else {
            mb_current_mode = mode;
            mb_state = STATE_DISABLED;
        }
    }
    return status;
}

#if MB_TCP_ENABLED > 0
mb_error_code_t mb_tcp_init( uint16_t tcp_port)
{
    /* reserve for tcp */
}
#endif

mb_error_code_t mb_close( void )
{
    mb_error_code_t status = MB_ENOERR;

    if(mb_state == STATE_DISABLED) {
        if(mb_frame_close_func != NULL) {
            mb_frame_close_func();
        }
    } else {
        status = MB_EILLSTATE;
    }
    return status;
}

mb_error_code_t mb_enable(void)
{
    mb_error_code_t status = MB_ENOERR;

    if(mb_state == STATE_DISABLED) {
        mb_frame_start_func();
        mb_state = STATE_ENABLED;
    } else {
        status = MB_EILLSTATE;
    }
    return status;
}

mb_error_code_t mb_disable( void )
{
    mb_error_code_t  status;

    if (mb_state == STATE_ENABLED) {
        mb_frame_stop_func();
        mb_state = STATE_DISABLED;
        status = MB_ENOERR;
    } else if (mb_state == STATE_DISABLED) {
        status = MB_ENOERR;
    } else {
        status = MB_EILLSTATE;
    }
    return status;
}

mb_error_code_t mb_poll(void)
{
    static uint8_t         *mb_frame;
    static uint8_t          rev_address;
    static uint16_t         length;

    int             i;
    mb_error_code_t status = MB_ENOERR;
    mb_event_t      event;

    if (mb_state != STATE_ENABLED) {
        return MB_EILLSTATE;
    }

    if (mb_event_get(&event) == true) {
        mb_log(MB_LOG_DEBUG, "receive event=0x%x\n", event);

        switch (event) {
            case EV_READY:
                mb_run_state = STATE_IDLE;
                break;
            case EV_PDUDATA_READY:
                mb_run_state = STATE_TX_READY;
                status = mb_frame_send_func(mb_current_slave_addr);
                break;
            
            case EV_FRAME_SENT:
                if (mb_current_slave_addr == SLAVE_ADDR_BROADCAST) {
                    mb_turnaround_timer_enable();
                } else {
                    mb_response_timer_enable();
                }
                break;
            
            case EV_FRAME_RECEIVED:
                status = mb_frame_receive_func(&rev_address, &mb_frame, &length);
                if (status == MB_ENOERR) {
                    if(rev_address == mb_current_slave_addr) {
                        (void)mb_event_post( EV_RESPOND_PROCESS );
                    } else {
                        mb_log(MB_LOG_ERROR, "slave addr is not the sent frame\n");
                        mb_event_post(EV_RESPOND_ERROR);
                    }
                } else {
                    mb_event_post(EV_RESPOND_ERROR);
                }
                break;

            case EV_FRAME_TIMEOUT:
                mb_event_post(EV_RESPOND_TIMEOUT);
                break;

            case EV_REQU_FINISHED:
                mb_set_run_state(STATE_END);
                mb_t35_timer_enable();
                break;
        }
    }
    return MB_ENOERR;
}

void mb_set_cur_slave_address(uint8_t slave_address)
{
    mb_current_slave_addr = slave_address;
}

mb_run_state_t mb_get_run_state()
{
    return mb_run_state;
}

void mb_set_run_state(mb_run_state_t state)
{
    mb_run_state = state;
}

bool mb_run_state_is_idle()
{
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER();
    if(mb_run_state == STATE_IDLE) {
        MB_CRITICAL_EXIT();
        return true;
    }

    MB_CRITICAL_EXIT();
    return false;
}
