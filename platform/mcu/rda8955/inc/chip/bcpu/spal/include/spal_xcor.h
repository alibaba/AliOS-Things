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


#ifndef SPAL_XCOR_H
#define SPAL_XCOR_H



//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
#include "chip_id.h"

// SPAL XCOR
///@defgroup spal_xcor SPAL Xcor API
///@{

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

// TODO : to be replaced by UINT32
typedef struct
{
    INT16 i;                            //0x00000000
    INT16 q;                            //0x00000002
} SPAL_COMPLEX_T; //Size : 0x4


typedef struct
{
    INT32 i;
    INT32 q;
} SPAL_CMPLX32_T;


// TODO : to be replaced by UINT32
typedef struct
{
    INT16 I; ///< DC Offset on I
    INT16 Q; ///< DC Offset on Q
} SPAL_DC_OFFSET_T;



// DDFSE fixed point configuration
#define SVAL_FRAC_BITS    11


#define SPAL_EGPRS_XCOR_TSC_SIZE  15
#define SPAL_EGPRS_XCOR_TSC_NBR   13
#define SPAL_EGPRS_ISI             8

/// number of taps used in the DDFSE treillis
#define MAIN_TAP_LEN                             (2)
/// number of taps used in the DDFSE equalizer
#define ALL_TAP_LEN                              (6)
/// size of the treillis for 8PSK equal to 8**(MAIN_TAP_LEN-1)
#define STATE_LEN                                (8)
/// length of the training sequence
#define HALF_BURST_LEN                           (61)

// Inter-Symbol Interference
#define SPAL_ISI     5

// Symbol delay induced by ISI for equalization
#define SPAL_DELAY   (SPAL_ISI-1)

// Number/size of Tsc Cross-correlation for time syncronization
#define NB_TSC_XCOR_NBR (8 + SPAL_ISI)
#define SCH_TSC_XCOR_NBR (24 + SPAL_ISI)

#define GSM_TSC_SCH0 0xF020469D
#define GSM_TSC_SCH1 0xD86EA2B4


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//=============================================================================
// spal_XcorResetFchFilterParam
//-----------------------------------------------------------------------------
/// Reset the parameters of FCH filter.
///
//=============================================================================
PUBLIC VOID spal_XcorResetFchFilterParam(VOID);


//=============================================================================
// spal_XcorTscxcNb
//-----------------------------------------------------------------------------
/// This function performs the I and Q correlations of a normal burst training
/// sequence with some received samples. 8 + SPAL_ISI = 13 consecutives correlation
/// are performed.
///
/// @param GsmTscNburst UINT32. INPUT. Training sequence. The correlation only
/// considers 16 bits (bits 5 to 21).
/// @param symb_ptr UINT32*. INPUT. Pointer to the received sample buffer where
/// I and Q samples are 16bits samples (INT16) with I samples on the LSB
/// and Q samples on the MSB. E.g. :
/// - Isample[0]=((INT16*) symb_ptr)[0]; ,
/// - Qsample[0]=((INT16*) symb_ptr)[1]; .
/// .
/// @param out_ptr INT16*. OUTPUT. Pointer to the I and Q correlation with
/// out_ptr[2*t] the I-correlation on the t time slot and out_ptr[2*t+1]
/// the Q-correlation on the t time slot.
//=============================================================================
PUBLIC VOID spal_XcorTscxcNb(UINT32 GsmTscNburst, UINT32* symb_ptr, INT16* out_ptr);


//=============================================================================
// spal_XcorTscxcSch
//-----------------------------------------------------------------------------
/// Training Sequence Xcorrelation mode for SCH burst
/// I and Q results are stored on separated words
///
/// @param symb_ptr UINT32*. ??
/// @param outI_ptr UINT32*. ??
/// @param outQ_ptr UINT32*. ??
//=============================================================================
PUBLIC VOID spal_XcorTscxcSch(UINT32* symb_ptr, UINT32* outI_ptr, UINT32* outQ_ptr);


//=============================================================================
// spal_XcorFchxc
//-----------------------------------------------------------------------------
// Xcorrelation mode for FCH burst detection
// Power term overwrites symbols,
// CI term is stored at Offset fom Power term
// and CQ term is stored at Offset from CI term.
//
// @param symb_ptr      UINT32*. Pointer to current symbol window.
// @param old_ptr       UINT32*. Pointer to previous symbol window.
// @param out_ptr       UINT32*. Pointer to the output buffer.
// @param size          UINT16.  Size of the window.
// @param computeEnergy UINT16.  Flag that defines if energy needs to be calculated.
//=============================================================================
PUBLIC VOID spal_XcorFchxc( UINT32* symb_ptr,
                            UINT32* old_ptr,
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
                            UINT32* out_ptr,
                            INT32   accu_energy,
                            INT32   accu_corrI,
                            INT32   accu_corrQ,
#endif // (CHIP_XCOR_HAS_SLIDING_WINDOW)
                            UINT16 size,
                            UINT16 computeEnergy);


//=============================================================================
// spal_XcorBiterror
//-----------------------------------------------------------------------------
/// Biterror calculation mode
///
/// @param NbSVwords UINT16. INPUT. Number of softvalue words to be treated.
/// @param sofvalues_ptr UINT32*. INPUT. Pointer to the buffer of soft values
/// coded on 8 bits with 1 sign bit and 7 amplitude bits.
/// @param hardvalues_ptr UINT32*. INPUT. Pointer to the hard decisions after
/// decoding
/// @param biterror UINT16*. Number of soft bits corrected into an hard bit
/// of opposite sign.
//=============================================================================
PUBLIC VOID spal_XcorBiterror(UINT16 NbSVwords, UINT32* sofvalues_ptr, CONST UINT32* hardvalues_ptr, UINT16* biterror);


//=============================================================================
// spal_XcorChest
//-----------------------------------------------------------------------------
// Performs LSE channel estimation. CHIP_CHEST_LEN gives the maximum number of taps that
// can be estimated.
//
// @param RxBuffer INT16*. INPUT. Pointer to the buffer of received samples.
// The samples are complex with RxBuffer[2*t] the inphase part of sample t and
// RxBuffer[2*t+1] the quadrature part of sample t.
// @param ChestTab CONST INT16*. INPUT. channel estimation table
// @param taps INT16*. OUTPUT. Estimated taps of the channel. The taps are complex.
// @param chTapNb UINT8. INPUT. number of channel taps to estimate. Should be smaller or equal to CHIP_CHEST_LEN
// taps[2*t] is the inphase part of taps t,
// taps[2*t+1] is the quadrature part of taps t.
//=============================================================================
PUBLIC VOID spal_XcorChest(INT16* RxBuffer, INT16* ChestTab, INT16* taps, UINT8 chTapNb, INT8  fracBits);


//=============================================================================
// spal_XcorSrec
//-----------------------------------------------------------------------------
/// Reconstruct a reference GMSK burst from channel taps and estimated/known hard bits.
///
/// @param taps INT16*. INPUT. Taps of the channel.
/// @param size UINT8. INPUT. Length of the reconstructed burst.
/// @param HardBits UINT32*. INPUT. Hard bits to be reconstructed.
/// @param refBurst UINT32*. OUTPUT. Pointer to the samples of the reconstructed burst.
/// refBurst[2*t] is the inphase part of sample t,
/// refBurst[2*t+1] is the quadrature part of sample t.
//=============================================================================
PUBLIC VOID spal_XcorSrec(INT16* taps, UINT8 size, UINT32* HardBits,UINT32* refBurst, UINT8 chTapNb);

//=============================================================================
// spal_XcorSrecPSK8
//-----------------------------------------------------------------------------
/// ReconstructS a reference 8-PSK burst from channel taps and estimated/known hard bits.
///
/// @param taps     INT16*.  INPUT.  Pointer to the channel taps.
/// @param size     UINT8.   INPUT.  Length of the reconstructed burst.
/// @param HardBits UINT8*.  INPUT.  Pointert to the Hard decision bits. to be reconstructed.
/// @param refBurst UINT32*. OUTPUT. Pointer to the samples of the reconstructed burst.
/// @param chTapNb  UINT8.   INPUT.  Number of the channel taps.
/// @param regShift UINT8.   INPUT.  Rescale parameter.
///
//=============================================================================
PUBLIC VOID spal_XcorSrecPSK8(INT16* taps, UINT8 size, UINT32* HardBits,UINT32* refBurst, UINT8 chTapNb, UINT8 regShift);

//=============================================================================
// spal_XcorBmsum
//-----------------------------------------------------------------------------
/// This functions computes the sum tables required by the viterbi.
/// It receives channel taps and computes for any register state,
/// the corresponding reference sample.
///
/// The functions computes the table for a 16 states viterbi (channel of length 5).
/// Therefore the functions returns 16 complex reference samples for forward
/// treillis and 16 complex reference samples for backward treillis.
///
/// @param taps INT16*. INPUT. Pointer to the channel taps with:
/// - taps[2*t] I part of the complex tap t,
/// - taps[2*t+1] Q part of the complex tap t.
/// .
/// @param SumTable INT16*. OUTPUT. Pointer to the output buffer.
/// The first 16 complex samples are the reference samples
/// for forward treillis, the last 16 complex samples are the reference samples for
/// backward treillis with:
/// - SumTable[2*t] I part of the complex sample t,
/// - SumTable[2*t+1] Q part of the complex sample t.
/// .
//=============================================================================
PUBLIC VOID spal_XcorBmsum(INT16* taps, INT16* SumTable);


//=============================================================================
// spal_XcorErrCalc
//-----------------------------------------------------------------------------
/// Estimates noise from a reference received sequence to an actually received
/// sequence.
///
/// @param Original UINT32*. INPUT. Pointer to the received sequence samples.
/// Original[2*t] is the inphase part of received sample t,
/// Original[2*t+1] is the quadrature part of received sample t.
/// @param Reference UINT32*. INPUT. Pointer to the ideal sequence samples.
/// Reference[2*t] is the inphase part of reference sample t,
/// Reference[2*t+1] is the quadrature part of reference sample t.
/// @param size UINT8. INPUT. Number of samples for noise estimation.
///
/// @return UINT32. OUTPUT. Total noise power (linear).
//=============================================================================
PUBLIC UINT32 spal_XcorErrCalc(UINT32* Original, UINT32* Reference, UINT8 size);


//=============================================================================
// spal_XcorIcorCalc
//-----------------------------------------------------------------------------
/// Performs a complex multiplications of a sequence with the conjugate of
/// another sequence and returns the sum of the imaginary parts
/// of the result sequence.
///
/// With the input below, the functions returns:
/// return = Sum(Original(2*t)*Reference(2*t+1)-Original(2*t+1)*Reference(2*t));
///
/// @param Original UINT32*. INPUT. Pointer to the complex sequence of received
/// samples where:
/// - ((INT16*)Original)[2*t] is the real part (inphase) of sample t,
/// - ((INT16*)Original)[2*t+1] is the imaginary part (quadrature) of sample t.
/// @param Reference UINT32*. INPUT. Pointer to the complex sequence of reconstructed
/// samples (samples rebuilt after equalization) where:
/// - ((INT16*)Reference)[2*t] is the real part (inphase) of sample t,
/// - ((INT16*)Reference)[2*t+1] is the imaginary part (quadrature) of sample t.
/// @param size UINT8. INPUT. The length of both sequences.
///
/// @return INT32. The sum of the imaginary part of the multiplication of
/// Original samples with conjugate Reference samples.
//=============================================================================
PUBLIC INT32 spal_XcorIcorCalc(UINT32* Original, UINT32* Reference, UINT8 size);


//=============================================================================
// spal_XcorHvextract
//-----------------------------------------------------------------------------
/// This function reads a buffer of soft bits and outputs the corresponding
/// hard bit.
///
/// @param softvalues UINT8*. INPUT. Pointer to the soft bit buffer. The soft
/// bits shall be coded on 8 bits with 1 sign bit (MSB) and 7 amplitude bits.
/// @param size UINT8. INPUT. Length of the soft bit input buffer SEEN AS A
/// BUFFER OF 32 BITS WORDS !!! For example if we want to extract 4 hard bits
/// from the corresponding soft bits, size=1, in other words
/// size=nb_soft_bits*8/32.
/// @param hardvalues UINT32*. OUTPUT. Pointer to the output buffer presented
/// as a buffer of 32 bits words. The first hard bit is on the LSB of the first
/// 32 bits word, i.e.
/// hard(softvalues[1])=hardvalues[0] & 0x00000001;
//=============================================================================
PUBLIC VOID spal_XcorHvextract(UINT8* softvalues, UINT8 size, UINT32* hardvalues);


//=============================================================================
// spal_XcorDcoc
//-----------------------------------------------------------------------------
/// DC offset correction function, derotation and power measurement function.
///
/// @param RxBuffer UINT32*. INPUT. Pointer to the received samples buffer.
/// @param size UINT8. INPUT. Length of the burst.
/// @param derotate BOOL. INPUT. Triggers derotation on and off:
/// - TRUE, derotation applied,
/// - FALSE, derotation not applied.
/// .
/// @param bypassDcoc BOOL. INPUT. Switch DC Offset Correction on/off.
/// @param Dco SPAL_DC_OFFSET_T*. OUTPUT. Pointer to the following structure:
/// - Dco.I, DC offset on I,
/// - Dco.Q, DC offset on Q.
/// .
///
/// @return UINT32. Return the power of the burst after DC offset correction.
//=============================================================================
PUBLIC UINT32 spal_XcorDcoc(UINT32* RxBuffer, UINT8 size, BOOL derotate, BOOL bypassDcoc, SPAL_DC_OFFSET_T* Dco);


//=============================================================================
// spal_XcorDcocHburst
//-----------------------------------------------------------------------------
/// DC offset correction and derotation for second part of the burst when using
/// the EQU_HBURST_MODE.
///
/// @param RxBuffer UINT32*. INPUT. Pointer to the received samples buffer.
/// @param size UINT8. INPUT. Length of the burst.
/// @param derotate BOOL. INPUT. Triggers derotation on and off:
/// - TRUE, derotation applied,
/// - FALSE, derotation not applied.
/// Note that the number of samples derotated during first part of the burst
/// must be multiple of 4 in order to have relevant rotation on second part
/// of the burst.
/// .
/// @param bypassDcoc BOOL. INPUT. Switch DC Offset Correction on/off.
/// @param Dco SPAL_DC_OFFSET_T*. INPUT. Pointer to the following structure:
/// - Dco.I, DC offset on I,
/// - Dco.Q, DC offset on Q.
/// This DC value (calculated on first part of the burst) will be corrected.
/// .
///
/// @return UINT32. Return the power of the burst after DC offset correction.
//=============================================================================
PUBLIC UINT32 spal_XcorDcocHburst(UINT32* RxBuffer, UINT8 size, BOOL derotate, BOOL bypassDcoc, SPAL_DC_OFFSET_T Dco);



//=============================================================================
// spal_XcorBmmlZf()
//-----------------------------------------------------------------------------
/// Calculate the partial metrics used for the branch metrics and the ZF.
///
/// @param taps INT16*. address of input taps array.
/// @param metrics INT16. array containing the metrics.
/// @author Tabakov Yordan
/// @date 16/08/06
///
//=============================================================================
// compute the BMML or partial ZF (two taps)
PUBLIC VOID spal_XcorBmmlZf( INT16* taps,
                             INT16* metric,
                             INT8             fracBits);

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
PUBLIC SPAL_CMPLX32_T spal_XcorComplexMatrixMultiply(   INT16* inVector,
        INT16* inMatrix,
        INT16* outMatrix,
        UINT8   columnLen,
        UINT8   rowLen,
        INT8    fracBits);

//=============================================================================
// spal_XcorComplexConvolution
//-----------------------------------------------------------------------------
// Complex convolution with enable rescale
//
// @param Xseq INT16.INPUT.The first input sequence.
// @param Xlen UINT16 .INPUT. The length of the first input sequence.
// @param Yseq INT16.INPUT.The second input sequence.
// @param Ylen UINT16.INPUT.The length of the second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence (16 or 32-bit complex).
// @param Zoff UINT16.INPUT. Offset in the ouptut sequence
// @param Zlen UINT16.INPUT. Max length of the output sequence (discard if negative)
// @param frac_bits INT8.INPUT.Rescale parameter.
//
// @author Yordan Tabakov
// @date 22/11/2007
//=============================================================================
PUBLIC VOID spal_XcorComplexConvolution (    INT16* Xseq,
        UINT16 Xlen,
        INT16* Yseq,
        UINT16 Ylen,
        INT16* Zseq,
        UINT16 Zoff,
        UINT16 Zlen,
        INT8 fracBits   );


//=============================================================================
// spal_XcorComplexPower
//-----------------------------------------------------------------------------
// Complex convolution with enable rescale
//
// @param seq INT16*.INPUT. input sequence.
// @param out INT16*.OUTPUT. output sequence (may be null).
// @param len UINT16 .INPUT. The length of the input sequence.
// @param frac_bits INT8.INPUT.Rescale parameter.
// @return INT32.Power of the complex sequence
//
// @author Yordan Tabakov
// @date 22/11/2007
//=============================================================================
PUBLIC UINT32 spal_XcorComplexPower (    INT16 *seq,
        INT16 *out,
        UINT16 len,
        INT8 fracBits);


//======================================================================
// spal_XcorDerotate
//----------------------------------------------------------------------
// This function is used for 8PSK or GMSK signal phase derotate
//
// @param in        INT16*  INPUT.  Pointer to the input sequence.
// @param out       INT16*  OUTPUT. Pointer to the output sequence.
// @param Length    UINT16            INPUT.  Length of the input sequence.
// @param fracBits  INT8              INPUT.  Rescale parameter.
// @param ModType   UINT8             INPUT.  Defines the modulation type.
//
// @author Zhao Yang, modified by Nadia Touliou
// @date 30/11/07
//======================================================================
PUBLIC VOID spal_XcorDerotate ( INT16* in,
                                INT16* out,
                                UINT16 Length,
                                UINT8 ModType );



//======================================================================
// spal_XcorDcIqCompensate
//----------------------------------------------------------------------
// This function is used in order to calculate from the 13 correlations
//
// @param inOut          INT16*    INPUT/OUTPUT.   Input/Output Data Buffer
// @param dc_compens     INT16*    INPUT.          Dc compensate
// @param iq_compens     INT16*    INPUT.          Iq compensate
// @param length         UINT16    INPUT.          Length of the Input/Output sequence.
// @param fracBits       INT8      INPUT           shift performed on the outut samples
//
// @author Nadia Touliou
// @date 10/12/07
//======================================================================
PUBLIC VOID spal_XcorDcIqCompensate(     INT16* inOut,
        INT16* dc_compens,
        INT16* iq_compens,
        UINT16 length,
        INT8 fracBits);


PUBLIC VOID spal_XcorFchFilter(UINT32* pold, UINT32* RxBuffer, UINT8 CntFlag,
                               UINT32* accu_outBuf,
                               INT32   accu_energy,
                               INT32   accu_corrQ);

PUBLIC VOID spal_XcorFchxc_AutoCorr(UINT32* symb_ptr, UINT16 XOffset, UINT16 size);

PUBLIC VOID spal_XcorFchxc_Delay( UINT32* symb_ptr,
                                  UINT32* old_ptr,
                                  UINT32* out_ptr,
                                  INT32   accu_energy,
                                  INT32   accu_corrI,
                                  INT32   accu_corrQ,
                                  UINT16  size,
                                  UINT16  computeEnergy);


PUBLIC VOID spal_XcorFchFilterAsm(UINT32 *Inpt, UINT8 CntFlag, UINT32 *Xpt, UINT32 *Ypt);

///@}

//=============================================================================
// spal_XcorChest8taps
//-----------------------------------------------------------------------------
// channel estimation function
//
// @param RxBuffer      INT16*. Pointer to received samples.
// @param ChestTab     INT16*. channel estimation tabel used in LS.
// @param taps           INT16*. Pointer to the output channel estimation results.
// @param chTapNb      UINT8.  Number of channel estimation taps.
// @param fracBits        INT8.  Shift indicator.
//=============================================================================

PUBLIC VOID spal_XcorChest8taps(INT16* RxBuffer, INT16 *ChestTab, INT16* taps, UINT8 chTapNb, INT8  fracBits);

#endif // SPAL_XCOR_H





















