#include "../port/port.h"
#include "../internal/mb.h"
#include "../internal/mbframe.h"
#include "../internal/mbport.h"
#include "mbcrc.h"
#include "mbrtu.h"

#define ADU_SER_LENGTH_MIN   4
#define ADU_SER_LENGTH_MAX   256
#define ADU_SER_ADDR_OFF     0
#define ADU_SER_PDU_OFF      1
#define ADU_SER_LENGTH_CRC   2

volatile uint8_t  rtu_send_rev_buf[ADU_SER_LENGTH_MAX];
static volatile uint8_t *send_buf_pos;
static volatile uint16_t send_buf_count;
static volatile uint16_t rev_buf_pos;
static volatile uint16_t cur_send_pdu_len;

void mb_get_pdu_buf(uint8_t **pdu_buf) {
    *pdu_buf = (uint8_t *)&rtu_send_rev_buf[ADU_SER_PDU_OFF];
}

bool mb_set_cur_pdu_len(uint16_t send_pdu_len) {
    cur_send_pdu_len = send_pdu_len;
    return MB_ENOERR;
}

mb_error_code_t mb_rtu_init(uint8_t port, uint32_t baudrate, mb_parity_t parity)
{
    mb_error_code_t    status = MB_ENOERR;
    uint32_t           timer_t35_50us;
    aos_task_t         tx_task;
    aos_task_t         rx_task;
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER();

    if (mb_serial_init(port, baudrate, 8, parity) != true) {
        status = MB_EPORTERR;
        MB_CRITICAL_EXIT();
        return status;
    } 

    aos_task_new_ext(&rx_task, "mbuartrx", mb_rtu_rec_frame, NULL, 1024, 41);
    aos_task_new_ext(&tx_task, "mbuarttx", mb_rtu_transmit_frame, NULL, 1024, 44);
    
    if(baudrate > 19200) {
        timer_t35_50us = 35;       /* 1800us. */
    } else {
        timer_t35_50us = (7UL * 220000UL) / (2UL * baudrate);
    }
    
    if (mb_timer_init((uint16_t )timer_t35_50us) != true) {
        status = MB_EPORTERR;
    }

    MB_CRITICAL_EXIT();
    return status;
}

void mb_rtu_start(void)
{
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER();

    mb_set_run_state(STATE_IDLE);
    mb_serial_enable(false, false);

    MB_CRITICAL_EXIT();
}

void mb_rtu_stop(void)
{
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER();
    mb_serial_enable(false, false);
    mb_timers_disable();
    MB_CRITICAL_EXIT();
}

mb_error_code_t mb_rtu_receive(uint8_t * rcv_address, uint8_t **frame, uint16_t *length)
{
    bool            frame_received = false;
    mb_error_code_t status = MB_ENOERR;
    MB_CRITICAL_ALLOC();

    MB_CRITICAL_ENTER();
    assert(rev_buf_pos < ADU_SER_LENGTH_MAX);

    mb_log(MB_LOG_DEBUG, "rev data len =%d ,data is :\n", rev_buf_pos);
    for (int i = 0; i < rev_buf_pos; i++) {
        mb_log(MB_LOG_DEBUG, "0x%x ", rtu_send_rev_buf[i]);
        if(i == rev_buf_pos - 1)
            mb_log(MB_LOG_DEBUG, "\n");
    }

    if ((rev_buf_pos >= ADU_SER_LENGTH_MIN)
        && (mb_crc16(( uint8_t *) rtu_send_rev_buf, rev_buf_pos) == 0)) {

        *rcv_address = rtu_send_rev_buf[ADU_SER_ADDR_OFF];
        *length = ( uint16_t )( rev_buf_pos - ADU_SER_PDU_OFF - ADU_SER_LENGTH_CRC );

        *frame = ( uint8_t * ) & rtu_send_rev_buf[ADU_SER_PDU_OFF];
        frame_received = true;
    } else {
        mb_log(MB_LOG_ERROR, "frame is too short or CRC error\n");
        status = MB_EIO;
    }

    MB_CRITICAL_EXIT();
    return status;
}

mb_error_code_t mb_rtu_send(uint8_t slave_address)
{
    mb_error_code_t status = MB_ENOERR;
    uint16_t        crc_16;

    mb_log(MB_LOG_DEBUG, "slaveaddr = 0x%x\n", slave_address);

    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER();

    if (mb_get_run_state() == STATE_TX_READY) {
        send_buf_pos   = rtu_send_rev_buf;
        send_buf_count = 1;

        send_buf_pos[ADU_SER_ADDR_OFF] = slave_address;
        send_buf_count += cur_send_pdu_len;

        crc_16 = mb_crc16((uint8_t *) send_buf_pos, send_buf_count);
        rtu_send_rev_buf[send_buf_count++] = (uint8_t)(crc_16 & 0xFF);
        rtu_send_rev_buf[send_buf_count++] = (uint8_t)(crc_16 >> 8);

        mb_log(MB_LOG_DEBUG, "start to send data len=%d\n", send_buf_count);
        for (int i=0; i<send_buf_count; i++) {
            mb_log(MB_LOG_DEBUG, "0x%x ", rtu_send_rev_buf[i]);
            if (i == send_buf_count - 1)
                mb_log(MB_LOG_DEBUG, "\n");
        }

        mb_set_run_state(STATE_TX_XMIT);
        mb_serial_enable(false, true);
    } else {
        status = MB_EIO;
    }
    MB_CRITICAL_EXIT();
    return status;
}

bool mb_rtu_rec_frame(void)
{
    bool     xTaskNeedSwitch = false;
    uint8_t  ucByte;
    while(1){
        mb_serial_rev_byte( ( int8_t * ) & ucByte );

        switch (mb_get_run_state()) {
            case STATE_RX_ERROR:
                mb_t35_timer_enable();
                break;

            case STATE_WAIT_RESPONSE:
                rev_buf_pos = 0;
                rtu_send_rev_buf[rev_buf_pos++] = ucByte;
                mb_set_run_state(STATE_RX_RCV);
                mb_t35_timer_enable();
                break;

            case STATE_RX_RCV:
                if (rev_buf_pos < ADU_SER_LENGTH_MAX) {
                    rtu_send_rev_buf[rev_buf_pos++] = ucByte;
                }
                else {
                    mb_set_run_state(STATE_RX_ERROR);
                }
                mb_t35_timer_enable();
                break;

            default :
                mb_log(MB_LOG_DEBUG, "wrong state 0x%x\n", mb_get_run_state());
                break;
        }
    }
    return xTaskNeedSwitch;
}

bool mb_rtu_transmit_frame(void)
{
    bool xNeedPoll = false;
    while(1){
        switch (mb_get_run_state()) {
            case STATE_IDLE:
                //mb_serial_enable(false, false);
                aos_msleep(10);
                break;

            case STATE_TX_XMIT:
                if (send_buf_count != 0) {
                    mb_serial_send_byte(send_buf_pos, send_buf_count);
                    send_buf_pos++;
                    send_buf_count = 0;
                } else {
                    xNeedPoll = mb_event_post(EV_FRAME_SENT);
                    mb_set_run_state(STATE_WAIT_RESPONSE);
                    mb_serial_enable(true, false);
                }
            break;
        }
        aos_msleep(10);
    }
    return xNeedPoll;
}

bool mb_rtu_timer_expired(void)
{
    bool statue = false;

    switch (mb_get_run_state()) {
        case STATE_RX_RCV:
            statue = mb_event_post(EV_FRAME_RECEIVED);
            break;
        case STATE_RX_ERROR:
            statue = mb_event_post(EV_RESPOND_ERROR);
            break;
        case STATE_WAIT_RESPONSE:
            statue = mb_event_post(EV_FRAME_TIMEOUT);
            break;

        case STATE_END:
            mb_event_post(EV_READY);
            break;
        default:
            mb_log(MB_LOG_DEBUG, "not requried state =0x%x\n", mb_get_run_state());
    }

    mb_log(MB_LOG_DEBUG, "state =0x%x\n", mb_get_run_state());

    mb_timers_disable();
    mb_serial_enable(false, false);

    return statue;
}
