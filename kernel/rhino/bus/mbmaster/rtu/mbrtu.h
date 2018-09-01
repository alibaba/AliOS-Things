#ifndef _MB_RTU_H
#define _MB_RTU_H

#ifdef __cplusplus
extern "C" {
#endif
    mb_error_code_t eMBRTUInit(UCHAR ucPort, ULONG ulBaudRate,
                             mb_parity_t eParity );
void            eMBRTUStart( void );
void            eMBRTUStop( void );
mb_error_code_t    eMBRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength );
mb_error_code_t    eMBRTUSend( UCHAR slaveAddress);
BOOL            xMBRTUReceiveFSM( void );
BOOL            xMBRTUTransmitFSM( void );
BOOL            xMBRTUTimerT15Expired( void );
BOOL            xMBRTUTimerExpired( void );

#ifdef __cplusplus
}
#endif
#endif
