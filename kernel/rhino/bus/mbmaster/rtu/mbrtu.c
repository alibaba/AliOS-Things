/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "../port/port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "../internal/mb.h"
#include "mbrtu.h"
#include "../internal/mbframe.h"

#include "mbcrc.h"
#include "../internal/mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

/* ----------------------- Static variables ---------------------------------*/


volatile UCHAR  ucRTUBuf[MB_SER_PDU_SIZE_MAX];

static volatile UCHAR *pucSndBufferCur;
static volatile USHORT usSndBufferCount;

static volatile USHORT usRcvBufferPos;

static volatile USHORT usCurSendPDULen;


/* ----------------------- Start implementation -----------------------------*/
mb_error_code_t
eMBRTUInit(UCHAR ucPort, ULONG ulBaudRate, mb_parity_t eParity )
{
    mb_error_code_t    eStatus = MB_ENOERR;
    ULONG           usTimerT35_50us;
    MB_CRITICAL_ALLOC();

    MB_CRITICAL_ENTER( );

    /* Modbus RTU uses 8 Databits. */
    if( mb_serial_init( ucPort, ulBaudRate, 8, eParity ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }
    else
    {
        /* If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if( ulBaudRate > 19200 )
        {
            usTimerT35_50us = 35;       /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
            usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
        }
        if( mb_timer_init( ( USHORT ) usTimerT35_50us ) != TRUE )
        {
            eStatus = MB_EPORTERR;
        }
    }
    MB_CRITICAL_EXIT( );

    return eStatus;
}

void
eMBRTUStart( void )
{
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER( );
    /* Initially the receiver is in the state STATE_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    mb_set_run_state(STATE_IDLE);
    mb_serial_enable( FALSE, FALSE );

    MB_CRITICAL_EXIT( );
}

void
eMBRTUStop( void )
{
    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER(  );
    mb_serial_enable( FALSE, FALSE );
    mb_timers_disable(  );
    MB_CRITICAL_EXIT(  );
}

mb_error_code_t
eMBRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    BOOL            xFrameReceived = FALSE;
    mb_error_code_t    eStatus = MB_ENOERR;
    MB_CRITICAL_ALLOC();

    MB_CRITICAL_ENTER(  );
    assert( usRcvBufferPos < MB_SER_PDU_SIZE_MAX );

    mb_log(MB_LOG_DEBUG, "rev data len =%d ,data is :\n", usRcvBufferPos);
    for(int i = 0; i < usRcvBufferPos; i++) {
        mb_log(MB_LOG_DEBUG, "0x%x ", ucRTUBuf[i]);
        if(i == usRcvBufferPos - 1)
            mb_log(MB_LOG_DEBUG, "\n");
    }

    /* Length and CRC check */
    if(( usRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
        && ( usMBCRC16( ( UCHAR * ) ucRTUBuf, usRcvBufferPos ) == 0 ))    /* liano for test */
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = ucRTUBuf[MB_SER_PDU_ADDR_OFF];

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
        *pusLength = ( USHORT )( usRcvBufferPos - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC );

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = ( UCHAR * ) & ucRTUBuf[MB_SER_PDU_PDU_OFF];
        xFrameReceived = TRUE;
    }
    else
    {
        mb_log(MB_LOG_ERROR, "frame is too short or CRC error\n");
        eStatus = MB_EIO;
    }

    MB_CRITICAL_EXIT(  );
    return eStatus;
}

mb_error_code_t
eMBRTUSend( UCHAR ucSlaveAddress)
{
    mb_error_code_t    eStatus = MB_ENOERR;
    USHORT          usCRC16;

    mb_log(MB_LOG_DEBUG, "slaveaddr = 0x%x\n", ucSlaveAddress);

    MB_CRITICAL_ALLOC();
    MB_CRITICAL_ENTER(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( mb_get_run_state() == STATE_TX_READY)
    {
        /* First byte before the Modbus-PDU is the slave address. */
        pucSndBufferCur = ucRTUBuf;
        usSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usSndBufferCount += usCurSendPDULen;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) pucSndBufferCur, usSndBufferCount );
        ucRTUBuf[usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        ucRTUBuf[usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        mb_log(MB_LOG_DEBUG, "start to send data len=%d\n", usSndBufferCount);
        for(int i=0;i<usSndBufferCount;i++) {
            mb_log(MB_LOG_DEBUG, "0x%x ", ucRTUBuf[i]);
            if(i == usSndBufferCount - 1)
                mb_log(MB_LOG_DEBUG, "\n");
        }

        /* Activate the transmitter. */
        mb_set_run_state(STATE_TX_XMIT);
        mb_serial_enable( FALSE, TRUE );
    }
    else
    {
        eStatus = MB_EIO;
    }
    MB_CRITICAL_EXIT(  );
    return eStatus;
}

BOOL
xMBRTUReceiveFSM( void )
{
    BOOL            xTaskNeedSwitch = FALSE;
    UCHAR           ucByte;

    /* Always read the character. */
    ( void )mb_serial_rev_byte( ( CHAR * ) & ucByte );

    switch ( mb_get_run_state())
    {
        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_RX_ERROR:
        mb_t35_timer_enable(  );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE.
         */
    case STATE_WAIT_RESPONSE:
        usRcvBufferPos = 0;
        ucRTUBuf[usRcvBufferPos++] = ucByte;
        mb_set_run_state(STATE_RX_RCV);

        /* Enable t3.5 timers. */
        mb_t35_timer_enable( );
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * number of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if( usRcvBufferPos < MB_SER_PDU_SIZE_MAX )
        {
            ucRTUBuf[usRcvBufferPos++] = ucByte;
        }
        else
        {
            mb_set_run_state(STATE_RX_ERROR);
        }
        mb_t35_timer_enable(  );
        break;

     default :
        mb_log(MB_LOG_DEBUG, "wrong state 0x%x\n", mb_get_run_state());
        break;
    }
    return xTaskNeedSwitch;
}

BOOL
xMBRTUTransmitFSM( void )
{
    BOOL xNeedPoll = FALSE;

    switch ( mb_get_run_state() )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_IDLE:
        /* enable receiver/disable transmitter. */
        mb_serial_enable( FALSE, FALSE );
        break;

    case STATE_TX_XMIT:
        /* check if we are finished. */
        if( usSndBufferCount != 0 )
        {
            mb_serial_send_byte( ( CHAR )*pucSndBufferCur );
            pucSndBufferCur++;  /* next byte in sendbuffer. */
            usSndBufferCount--;
        }
        else
        {
            xNeedPoll = mb_event_post( EV_FRAME_SENT );
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            mb_set_run_state(STATE_WAIT_RESPONSE);
            mb_serial_enable(TRUE, FALSE );
        }
        break;
    }

    return xNeedPoll;
}

BOOL
xMBRTUTimerExpired( void )
{
    BOOL            xNeedPoll = FALSE;

    switch ( mb_get_run_state() )
    {
        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_RX_RCV:
        xNeedPoll = mb_event_post( EV_FRAME_RECEIVED );
        break;

        /* An error occured while receiving the frame. */
    case STATE_RX_ERROR:
        xNeedPoll = mb_event_post( EV_RESPOND_ERROR );
        break;

    case STATE_WAIT_RESPONSE:
        xNeedPoll = mb_event_post(EV_FRAME_TIMEOUT);
        break;

    case STATE_END:
        mb_event_post(EV_READY);
        break;
    /* Function called in an illegal state. */
    default:
        mb_log(MB_LOG_DEBUG, "not requried state =0x%x\n", mb_get_run_state());;
    }

    mb_log(MB_LOG_DEBUG, "state =0x%x\n", mb_get_run_state());

    mb_timers_disable(  );
    mb_serial_enable( FALSE, FALSE );

    return xNeedPoll;
}

void vMBGetPDUBuf(UCHAR **pucPDUBuf) {
    *pucPDUBuf = (UCHAR *)&ucRTUBuf[MB_SER_PDU_PDU_OFF];
}

BOOL xMBSetCurPDULength(USHORT usSendPDULen) {
    usCurSendPDULen = usSendPDULen;
}


