#ifndef _MB_FUNC_H
#define _MB_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif
#if MB_FUNC_OTHER_REP_SLAVEID_BUF > 0
   mb_exception_t eMBFuncReportSlaveID( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_INPUT_ENABLED > 0
eMBException    eMBFuncReadInputRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_HOLDING_ENABLED > 0
mb_exception_t    eMBFuncReadHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
mb_exception_t    eMBFuncWriteHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
mb_exception_t    eMBFuncWriteMultipleHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_COILS_ENABLED > 0
mb_exception_t    eMBFuncReadCoils( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_COIL_ENABLED > 0
mb_exception_t    eMBFuncWriteCoil( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
mb_exception_t    eMBFuncWriteMultipleCoils( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
mb_exception_t    eMBFuncReadDiscreteInputs( UCHAR * pucFrame, USHORT * usLen );
#endif

#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
mb_exception_t    eMBFuncReadWriteMultipleHoldingRegister( UCHAR * pucFrame, USHORT * usLen );
#endif

#ifdef __cplusplus
}
#endif
#endif
