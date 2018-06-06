/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/




#ifndef SPAL_EXCOR_H
#define SPAL_EXCOR_H



//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "chip_id.h"
#include "spal_xcor.h"


#if ( (CHIP_EDGE_SUPPORTED==1) || (defined CHIP_DIE_8955) )
#include "excor.h"

// -----------------------------------------
// Constant
#define SVAL_ZERO                          0
#define SVAL_ONE                        2047
#define SVAL_NEGONE                     2048
#define SVAL_INVSQRT                    1448
#define SVAL_COS3PI8                     784
#define SVAL_SIN3PI8                    1892







// -----------------------------------------
// type
typedef enum
{
    IR_COMBINE_NO_COMBINE               =   0,
    IR_COMBINE_SAME_MCS_CPS             =   1,
    IR_COMBINE_MCS6PAD_TO_MCS8          =   2,
    IR_COMBINE_MCS8_TO_MCS6PAD          =   3,
    IR_COMBINE_OHTER_MCS                =   4
} SPAL_IR_MCS6PAD_COMBINE_T;


//-----------------------------------------
// function declaration
PUBLIC VOID spal_XcorBmmlZf( INT16* taps,
                             INT16* metric,
                             INT8             fracBits);

PUBLIC UINT32 spal_XcorComplexPower16(    INT16 *seq,
        INT16 *out,
        UINT16 len,
        INT8 fracBits);

PUBLIC UINT32 spal_XcorComplexPower32 (    INT16 *seq,
        UINT32 *out,
        UINT16 len,
        INT8 fracBits);

PUBLIC VOID spal_XcorDerotate ( INT16* in,
                                INT16* out,
                                UINT16 Length,
                                UINT8 ModType );

PUBLIC VOID spal_XcorDcIqCompensate(     INT16* inOut,
        INT16* dc_compens,
        INT16* iq_compens,
        UINT16 length,
        INT8 fracBits);

PUBLIC VOID spal_XcorSrecPSK8(    INT16* taps,
                                  UINT8 size,
                                  UINT32* HardBits,
                                  UINT32* refBurst,
                                  UINT8 chTapNb,
                                  UINT8 regShift);

PUBLIC VOID spal_XcorComplexConvolution (    INT16* Xseq,
        UINT16 Xlen,
        INT16* Yseq,
        UINT16 Ylen,
        INT16* Zseq,
        UINT16 Zoff,
        UINT16 Zlen,
        INT8 fracBits   );




//====================================================================================
//    spal_XcorComplexMatrixMultiply
//
//    This function is used to compute double convolution in the DC IQ estimation function.
//
//  @param inVector  INT16*  input data
//  @param inMatrix  INT16*  input matrix
//  @param outMatrix INT16*  output matrix (multiplication result)
//  @param columnLen UINT8             number of input matrix columns
//  @param rowLen    UINT8             number of input matrix rows
//  @param fracBits  UINT8             fractional bits of the data format
//  @return          SPAL_CMPLX32_T   the last accumulation result
//
//  @author Yordan Tabakov
//
//  @date 26/11/07
//====================================================================================

PUBLIC SPAL_CMPLX32_T spal_XcorComplexMatrixMultiply(
    INT16* inVector,
    INT16* inMatrix,
    INT16* outMatrix,
    UINT8   columnLen,
    UINT8   rowLen,
    INT8    fracBits);

//=============================================================================
// spal_XcorFastConvolution
//-----------------------------------------------------------------------------
// real vectors convolution with enable rescale
//
// @param Xseq INT16*.INPUT.The first input sequence.
// @param Yseq INT16*.INPUT.The second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence.
// @param Xlen UINT16 .INPUT. The length of the first input sequence, 2(Ylen -1) zreos added orignal vector .
// @param Ylen UINT16.INPUT.The length of the second input sequence.
// @param frac_bits0 UINT8.INPUT.Rescale parameter.
// @param frac_bits1 UINT8.INPUT.Rescale parameter.
//=============================================================================
PUBLIC VOID spal_XcorFastConvolution (    INT16* Xseq,
        INT16* Yseq,
        INT16* Zseq,
        UINT16  Xlen,
        UINT16  Ylen,
        UINT8 fracBits0,
        UINT8 fracBits1);
//====================================================================================
//    spal_XcorFastMatrixMultiply
//
//    This function is used to compute matrix multiply.
//
//  @param inVector  real value  input vector A (m,n)
//  @param inMatrix  real value  input vector B (n,l)
//  @param outMatrix real value  output matrix (multiplication result)
//  @param rowALen UINT8             number of input matrix A  rows
//  @param  columnBLen    UINT8          number of input matrix B column
//  @param  LoopABLen    UINT8            loop number of input matrix A,B
//  @param fracBits0  UINT8             fractional bits of the data format
//  @param fracBits1  UINT8             fractional bits of the data format
//====================================================================================

PUBLIC VOID spal_XcorFastMatrixMultiply(
    INT16* inVectorA,
    INT16* inVectorB,
    INT16* outMatrix,
    UINT8   rowALen,
    UINT8   columnBLen,
    UINT8   LoopABLen,
    UINT8    fracBits0,
    UINT8    fracBits1);



//====================================================================================
//    spal_XcorFastMatrixMultiplyS
//-----------------------------------------------------------------------------
//    This function is used to compute matrix multiply.
//
//  @param inVectorA  real value  input vector A (m,n)
//  @param inVectorB  real value  input vector B (n,l)
//  @param outMatrix real value  output matrix (multiplication result)
//  @param ctrl  UINT32           ctrl register configuration value.
//====================================================================================

PUBLIC  VOID spal_XcorFastMatrixMultiplyS(
    INT16* inVectorA,
    INT16* inVectorB,
    INT16* outMatrix,
    UINT32  ctrl);
//====================================================================================
//    spal_XcorFastMatrixMultiplyInit
//-----------------------------------------------------------------------------
//    This function is used to compute matrix multiply.
//
//  @param inVectorA  real value  input vector A (m,n)
//  @param inVectorB  real value  input vector B (n,l)
//  @param outMatrix real value  output matrix (multiplication result)
//  @param ctrl  UINT32           ctrl register configuration value.
//====================================================================================

PUBLIC  VOID spal_XcorFastMatrixMultiplyInit(
    INT16* inVectorA,
    INT16* inVectorB,
    INT16* outMatrix,
    UINT32   ctrl);

//====================================================================================
//    spal_XcorFastMatrixMultiplySet
//-----------------------------------------------------------------------------
//    This function is used to compute matrix multiply.
//
//  @param inVectorA  real value  input vector A (m,n)
//  @param inVectorB  real value  input vector B (n,l)
//  @param outMatrix real value  output matrix (multiplication result)
//  @param rowALen UINT8             number of input matrix A  rows
//  @param  columnBLen    UINT8     number of input matrix B column
//  @param  LoopABLen    UINT8      loop number of input matrix A,B calculation
//  @param fracBits0  UINT8            Rescale bit
//  @param fracBits1  UINT8            Rescale bit
//====================================================================================

PUBLIC  VOID spal_XcorFastMatrixMultiplySet(INT16* inVectorA,
        INT16* inVectorB,
        INT16* outMatrix,
        UINT8   rowALen,
        UINT8   columnBLen,
        UINT8   LoopABLen,
        UINT8    fracBits0,
        UINT8    fracBits1);

//=============================================================================
// spal_XcorFastConvolutionSet
//-----------------------------------------------------------------------------
// real vectors convolution with enable rescale
//
// @param Xseq INT16*.INPUT.The first input sequence.
// @param Yseq INT16*.INPUT.The second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence.
// @param ctrl UINT32.INPUT. ctrl register configuration.
//=============================================================================

PUBLIC  VOID spal_XcorFastConvolutionSet (INT16* Xseq,
        INT16* Yseq,
        INT16* Zseq,
        UINT32 ctrl
                                         );

//=============================================================================
// spal_XcorFastConvolutionInit
//-----------------------------------------------------------------------------
// real vectors convolution with enable rescale
//
// @param Xseq INT16*.INPUT.The first input sequence.
// @param Yseq INT16*.INPUT.The second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence.
// @param ctrl UINT32.INPUT. ctrl register configuration.
//=============================================================================

PUBLIC  VOID spal_XcorFastConvolutionInit (INT16* Xseq,
        INT16* Yseq,
        INT16* Zseq,
        UINT32 ctrl);




PUBLIC VOID spal_ExcorIrRecombine( UINT8*  inBuf0,
                                   UINT32* inOutPps0,
                                   UINT8*  inBuf1,
                                   UINT8*  inBuf1_2nd,
                                   UINT32* inPps1,
                                   SPAL_IR_MCS6PAD_COMBINE_T flag,
                                   UINT16  len,
                                   UINT8*  outBuf);

PUBLIC VOID spal_ExcorIQShift(UINT32* IQ_ADDR,UINT16 len,INT8 fracBits);

PUBLIC VOID spal_ExcorEgprsRotate ( INT16* in,
                                    INT16* out,
                                    UINT8 offset,
                                    UINT16 Length);

#endif

#endif

