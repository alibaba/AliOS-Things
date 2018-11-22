#ifndef _MB_PORT_H
#define _MB_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    EV_READY           = 0x1 << 0,
    EV_PDUDATA_READY   = 0x1 << 1,              /* request PUD data is ready*/
    EV_FRAME_SENT      = 0x1 << 2,             /* request frame sent. */
    EV_FRAME_RECEIVED  = 0x1 << 3,            /* response freme received. */
    EV_FRAME_TIMEOUT   = 0x1 << 4,            /* response freme received. */

    EV_RESPOND_ERROR   = 0x1 << 5,
    EV_RESPOND_PROCESS = 0x1 << 6,            /* process response. */
    EV_RESPOND_TIMEOUT = 0x1 << 7,

    EV_REQU_FINISHED   = 0x1 << 8,
} mb_event_t;

bool            mb_event_init( void );
bool            mb_event_post( mb_event_t event );
bool            mb_event_get(mb_event_t * eEvent );
mb_exception_t  mb_wait_slave_respond();


bool            mb_serial_init( uint8_t port, uint32_t baud_rate, uint8_t data_width, mb_parity_t parity );
void            mb_serial_close( void );
void            mb_serial_enable( bool xRxEnable, bool xTxEnable );
bool            mb_serial_rev_byte( int8_t *data );
bool            mb_serial_send_byte( int8_t* data, uint8_t size);

bool            mb_timer_init( uint16_t usTimeOut50us );
void            mb_timer_close( void );
void            mb_t35_timer_enable( );
void            mb_turnaround_timer_enable( );
void            mb_response_timer_enable( );
void            mb_timers_disable( void );

extern          bool(*mb_timer_expired_func) ( void );

#ifdef __cplusplus
}
#endif
#endif
