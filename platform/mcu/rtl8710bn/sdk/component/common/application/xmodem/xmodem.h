/*
	X-Modem Header File

	1999/09/03	sprite, support Xmode Tx & Rx
*/

#ifndef	_XMODE_H_
#define	_XMODE_H_

#include <basic_types.h>

/*****************
 * X-Modem status
 *****************/
#define	XMODEM_OK		1
#define	XMODEM_CANCEL	2
#define	XMODEM_ACK		3
#define	XMODEM_NAK		4
#define	XMODEM_COMPLETE	5
#define	XMODEM_NO_SESSION	6
#define	XMODEM_ABORT	7
#define	XMODEM_TIMEOUT	8

/****************************
 * flow control character
 ****************************/
#define	SOH	0x01		/* Start of header */
#define	STX	0x02		/* Start of header XModem-1K */
#define	EOT	0x04		/* End of transmission */
#define	ACK	0x06		/* Acknowledge */
#define	NAK	0x15		/* Not acknowledge */
#define	CAN	0x18		/* Cancel */
#define	ESC	0x1b		/* User Break */

/****************************
 * Xmode paramters
 ****************************/
#define	FRAME_SIZE	132	/* X-modem structure */
#define	FRAME_SIZE_1K	1028	/* X-modem structure */
#define	XM_BUFFER_SIZE	1024	/* X-modem buffer */
#define	TIMEOUT		180	/* max timeout */
#define	RETRY_COUNT	20	/* Try times */
#define	xWAITTIME	0x00400000	/* waitiing time */
#define	WAIT_FRAME_TIME     (10000*100)	/* 10 sec, wait frame timeout */
#define	WAIT_CHAR_TIME      (1000*100)	/* 1 sec, wait char timeout */

/***********************
 * frame structure
 ***********************/
typedef	struct
{
	 unsigned char	soh;
	 unsigned char	recordNo;
	 unsigned char	recordNoInverted;
	 unsigned char	buffer[XM_BUFFER_SIZE];
	 unsigned char	CRC;
} XMODEM_FRAME;

typedef struct _XMODEM_COM_PORT_ {
    char (*poll) (void);
    char (*get)(void);
    void (*put)(char c);
}XMODEM_COM_PORT, *PXMODEM_COM_PORT;

typedef struct _XMODEM_CTRL_ {
    u16 xMUsing;
    u16 currentFrame; /* current frame number */
    u16 previousFrame;    /* previous frame number */
    u16 expected;
    s16 rStatus;
    s32 rFinish;
    u32 total_frame;
    u32 rx_len;
    char *pXFrameBuf;    
    u32 (*RxFrameHandler)(char *ptr,  u32 frame_num, u32 frame_size);
    XMODEM_COM_PORT ComPort;
}XMODEM_CTRL, *PXMODEM_CTRL;

typedef u32 (*RxFrameHandler_t)(char *ptr,  u32 frame_num, u32 frame_size);

extern s16 xModemStart(XMODEM_CTRL *pXMCtrl, char *FrameBuf, RxFrameHandler_t RxFrameHdl);
extern s16 xModemEnd(XMODEM_CTRL *pXMCtrl);
extern s32 xModemRxBuffer(XMODEM_CTRL *pXMCtrl, s32 MaxSize);

#endif /* _XMODE_H_ */

