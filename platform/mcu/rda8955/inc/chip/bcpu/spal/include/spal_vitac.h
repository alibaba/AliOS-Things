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


#ifndef SPAL_VITAC_H
#define SPAL_VITAC_H


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define EQU_NB_STATES       16
#define EQU_PM_INIT         0x3FFF3FFFU
#define EQU_RESC_THRES      0x3FFFU

#define BACKWARD_TRELLIS    TRUE
#define FORWARD_TRELLIS     FALSE

// DDFSE  defines
#define NODE_MIN        0
#define NODE_MAX        0x0fff
#define ZF_TAP_LEN      (ALL_TAP_LEN - MAIN_TAP_LEN)
#define MULT_SIZE       14

// Number of states in the Viterbi Equalization (VIT_NSTATES=2^(SPAL_ISI-1))
#define VIT_NSTATES 16



// SPAL VITAC
///@defgroup spal_vitac SPAL Vitac API
///@{



/* Errors returned by the VITAC driver functions */
typedef enum
{
    vitac_err_none      = 0,
    vitac_err_busy      = 0xFE,
    vitac_err_badarg    = 0xFF
} SPAL_VITAC_ERROR_T;

/* VITAC types */
typedef enum
{
    VITAC_16_STATES   = 0,
    VITAC_32_STATES   = 1,
    VITAC_64_STATES   = 2
} SPAL_VITAC_STATES_T;

typedef enum
{
    VITAC_RATE_1_2     = 2,
    VITAC_RATE_1_3     = 3,
    VITAC_RATE_1_4     = 4,
    VITAC_RATE_1_5     = 5,
    VITAC_RATE_1_6     = 6
} SPAL_VITAC_RATE_T;

typedef struct
{
    UINT32* in;
    UINT32 H0;
    UINT32 HL;
    UINT16* pm;
    INT16* bmSum;
    UINT8 length;
    UINT8* out;
    UINT8 shift_bm;
    UINT8 shift_sb;
    UINT8 first_state;
} SPAL_VITAC_EQU_PARAM_T;

typedef struct
{
    UINT32 states   : 2; // 2
    UINT32 rate     : 3; // 5
    UINT32 is_punct : 1; // 6
    UINT32 resPoly0 : 3; // 9
    UINT32 resPoly1 : 3; // 12
    UINT32 resPoly2 : 3; // 15
    UINT32 is_header: 1; // 16
    UINT32 nbSymbol : 10; // 25
    //UINT32          : 7; // 32
    //const UINT32 *pPunct;
} SPAL_VITAC_DEC_PARAM_T;



// type for the full vitac decoding
typedef struct
{
    /// Pointer to the input buffer of soft bits coded on 8 bits with 1 sign bit and 7 amplitude bits.
    UINT32*                 inSoftBit;
    /// Pointer to the survivor memory in BB SRAM, used to store the intermediary path matrics.
    UINT32*                 survivorMem;
    /// Pointer to the puncturing table, stored in the BB SRAM (accessible by the Vitac module).
    UINT32*                 punctTable;
    /// 1st convolutional polynome.
    UINT32                  convPoly0;
    /// 2nd convolutional polynome.
    UINT32                  convPoly1;
    /// 3rd convolutional polynome.
    UINT32                  convPoly2;
    /// Configuration for the Vitac module
    SPAL_VITAC_DEC_PARAM_T  vitacDecParam;
} SPAL_VITAC_DEC_CFG_T;



//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_VitacBusy
//-----------------------------------------------------------------------------
/// Checks whether the VITAC is busy ?
///
//=============================================================================
PUBLIC BOOL spal_VitacBusy(VOID);

//=============================================================================
// spal_VitacEqualize
//-----------------------------------------------------------------------------
/// Run Equalization Viterbi Process on a half burst without traceback.
///
/// @param trellis BOOL. INPUT. This parameters makes the Viterbi equalize the
/// backward part of the treillis of the forward part of the treillis. Two defines
/// have been specifically designed for this purpose:
/// - BACKWARD_TRELLIS (defined as TRUE in spal_vitac.h) is used for backward operation,
/// - FORWARD_TRELLIS (defined as FALSE in spal_vitac.h) is used for forward operation.
///
/// @param param SPAL_VITAC_EQU_PARAM_T*. INPUT/OUTPUT. param sets the working buffer,
/// the operation mode, provides the input as well as the output.
/// Parameters:
/// - param.H0. UINT32. INPUT. Is the value of the first complex channel tap where
/// ((INT16*)param.H0)[0] is the real part (inphase) of the channel coefficient and
/// ((INT16*)param.H0)[1] is the imaginary part (quadrature) of the channel coefficient.
/// - param.HL. UINT32. INPUT. Is the value of the last (5th) complex channel tap where
/// ((INT16*)param.HL)[0] is the real part (inphase) of the channel coefficient and
/// ((INT16*)param.HL)[1] is the imaginary part (quadrature) of the channel coefficient.
/// - param.bmSum. INT16*. INPUT. The complex branch metric for a specific state of the
/// register are precalculated by the spal_XcorBmsum function and stored in a
/// INT16 buffer. ((INT16*)param.bmSum)[2*i] is the real part (inphase) of the branch metric
/// corresponding to the ith register state and ((INT16*)param.bmSum)[2*i+1] is the
/// imaginary part (quadrature) of the branch metric corresponding to the ith register
/// state.
/// - param.length. UINT8. INPUT. Number of samples in half burst including tail samples.
/// - param.shift_bm. UINT8. INPUT. Branch metric shift value. The path metric are
/// accumulated by adding branch metric. Each computed branch metric is shifted by
/// param.shift_bm before being added to the path metric. param.shift_bm is computed outside
/// of the viterbi equalizer and depends on the SNR. Hardware limitations impose that
/// param->shift_bm should belong to [4,13],
/// - param.shift_sb. UINT8. INPUT. Soft bit shift value. The soft bits are computed as
/// a difference between two path metrics. The soft bits are on 8 bits whereas the
/// path metrics are on 16 bits, therefore a shift is sometimes required to determine
/// the most appropriate 8 bits to output as a soft bit. param.shift_sb is computed outside
/// of the viterbi equalizer and depends on the SNR. In practice param.shift_sb=0 until
/// param.shift_bm is maximum (13). Hardware limitations impose that param->shift_sb
/// should belong to [0,9],
/// - first_state. UINT8. INPUT. The initial state of the register is determined by
/// the first and last bits of the training sequence for respectively the backward
/// and forward treillis equalization.
/// .
/// Working buffers:
/// - param.pm UINT16*. INPUT/OUTPUT. Sets the buffer where path metrics (a.k.a. node
/// metrics) are stored. When the VITAC finishes equalization, the buffer can
/// be used to select the most likely end state when the tail bits do not close
/// the treillis.
/// .
/// Input/Output buffers:
/// - in. UINT32*. INPUT. Pointer to the input buffer. It contains the complex
/// received samples where ((INT16*)in)[2*i] is the real part (inphase) of the
/// ith complex received sample and ((INT16*)in)[2*i+1] is the imaginary part
/// (quadrature) of the ith complex received sample.
/// - out. UINT8*. OUTPUT. Pointer to the output buffer. It contains the 8 bits
/// softbits for each state, for each sample (before trace back) coded as 1 sign
/// bit and 7 amplitude bits and where path_diff[n+(m*VIT_NSTATES)] is the soft
/// bit for the nth state for the mth sample.
/// .
///
/// @return SPAL_VITAC_ERROR_T. Indicates valid or invalid VITAC processing.
//=============================================================================
PUBLIC UINT8 spal_VitacEqualize(BOOL trellis, SPAL_VITAC_EQU_PARAM_T* param);

//=============================================================================
// spal_VitacFullDecoding
//-----------------------------------------------------------------------------
// This function performs the Viterbi decoding of a coded block. It receives
// the soft bits and coding scheme and outputs the decoded bits. The parameters
// corresponding to each coding scheme are stored inside SPAL internal
// variables.
//
// @param cs_type SPAL_CODING_SCHEME_T. INPUT. SPAL coding scheme definition.
// FACCH_H, FACCH_F, SACCH, SDCCH, BCCH and AGCH are not included as they are
// considered as CS_CS1
// @param punct_ptr UINT32*. Pointer to the puncturing table
// @param survivorMem_ptr UINT32*. Pointer to the survivor memory, allocated in an upper layer
// @param softBit_ptr UINT32*. INPUT. Pointer to the soft bit buffer. The soft
// bit are coded on 8 bits with 1 sign bit and 7 amplitude bits.
// @param output_ptr UINT32*. OUTPUT. Pointer to the decoded bits buffer.
//
// @return SPAL_VITAC_ERROR_T. Indicates valid or invalid VITAC processing.
//=============================================================================
PUBLIC UINT8 spal_VitacFullDecoding(SPAL_VITAC_DEC_CFG_T *pCfg, UINT32* output_ptr);

//=============================================================================
// spal_VitacIntClear
//-----------------------------------------------------------------------------
/// Clear the processing done interrupt
///
//=============================================================================
PUBLIC VOID spal_VitacIntClear(VOID);


//=============================================================================
// spal_VitacDdfseHalfBurst()
//-----------------------------------------------------------------------------
/// Calculate the partial metrics used for the branch metrics and the ZF.
///
/// @param pSample SPAL_COMPLEX_T*     address of input samples buffer.
/// @param length UINT8                 length og the equalization.
/// @param inc INT8                     direction of the equalization.
/// @param StartHistory PUINT8          history for the metric initialization
/// @param BranchMetric SPAL_COMPLEX_T Partial branch metrics
/// @param SoftOutput INT16             soft-bit output
/// @author Tabakov Yordan
/// @date 16/08/06
///
//=============================================================================

PUBLIC VOID spal_VitacDdfseHalfBurst(     UINT32* pSample,
        UINT8   length,
        INT8    inc,
        UINT8   logNoisePwr,
        PUINT8  StartHistory,
        UINT32* BmmlMetric,
        UINT32* ZfMetric,
        UINT8*  SoftOutput);






///@}

#endif  /* SPAL_VITAC_H */





















