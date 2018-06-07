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


#ifndef SPAL_CAP_H
#define SPAL_CAP_H

// SPAL CAP
///@defgroup spal_cap SPAL CAP API
///@{


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

// include
#include "cs_types.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define SPAL_PUNCTURE_TABLE_SIZE 22


#ifdef CS_PLATFORM_SIMU

#define CAP_CTRL_CONFIG(P1,P2,P3,P4,P5,P6,Prsc,NbP,Punct,Preload,mode) (mode)

#else

#if (CHIP_CAP_ON_APB)

// CAP on APB
#define CAP_CTRL_CONFIG(P1,P2,P3,P4,P5,P6,Prsc,NbP,Punct,Preload,mode) ((P1&0xf)     |  \
                                                                        ((P2&0xf)<<4) |  \
                                                                        ((P3&0x7)<<8)  |  \
                                                                        ((P4&0x7)<<11) |  \
                                                                        ((P5&0x7)<<14) |  \
                                                                        ((P6&0x7)<<17) |  \
                                                                        ((Prsc&0x7)<<20)| \
                                                                        ((NbP&0x7)<<23)  |\
                                                                        ((Punct&0x1)<<26)|\
                                                                        ((Preload&0x1)<<27))
#define CS_IS_PUNCTURED(n) ((n) & 1<<26)

#else // (!CHIP_CAP_ON_APB)

// CAP on BBCP2
#define CAP_CTRL_CONFIG(P1,P2,P3,P4,P5,P6,Prsc,NbP,Punct,Preload,mode) ((P1&0x7)      | \
                                                                        ((P2&0x7)<<3) | \
                                                                        ((P3&0x7)<<6) | \
                                                                        ((P4&0x7)<<9) | \
                                                                        ((P5&0x7)<<12)| \
                                                                        ((P6&0x7)<<15)| \
                                                                        ((Prsc&0x7)<<18)| \
                                                                        ((NbP&0x7)<<21)| \
                                                                        ((Punct&0x1)<<24))

#define CS_IS_PUNCTURED(n) ((n) & 1<<24)

#endif // (CHIP_CAP_ON_APB)

#endif // CS_PLATFORM_SIMU


typedef struct
{
    UINT32 capCfg;
    UINT16 nbInBits;
    UINT8  nbOutWords;
} SPAL_CAP_CONV_PARAM_T;


typedef struct
{
    UINT32 capCfg;
    UINT16 nbDecBits;
    UINT16 nbEncBits;
} SPAL_CAP_CRC_PARAM_T;



//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//=============================================================================
// spal_CapEncodeCrc
//-----------------------------------------------------------------------------
/// CAP driver function to encode CRC
///
/// @param param SPAL_CAP_CRC_PARAM_T. Cap configuration for crc.
/// @param In UINT32*. Pointer to the input buffer.
/// @param Out UINT32*. Pointer to the output buffer.
///
//=============================================================================
PUBLIC VOID spal_CapEncodeCrc(SPAL_CAP_CRC_PARAM_T param, UINT32* In, UINT32* Out);

//=============================================================================
// spal_CapDecodeCrc
//-----------------------------------------------------------------------------
/// CAP driver function to decode CRC
///
/// @param param SPAL_CAP_CRC_PARAM_T. Cap configuration for crc.
/// @param In UINT32*. Pointer to the input buffer.
/// @param Out UINT32*. Pointer to the output buffer.
///
//=============================================================================
PUBLIC VOID spal_CapDecodeCrc(SPAL_CAP_CRC_PARAM_T param, UINT32* In, UINT32* Out);

//=============================================================================
// spal_CapConvoluate
//-----------------------------------------------------------------------------
/// CAP driver function to convoluate
///
/// @param param SPAL_CAP_CONV_PARAM_T. Cap configuration for convolution.
/// @param punctTable UINT32*. Pointer to the puncturing table.
/// @param In UINT32*. Pointer to the input buffer.
/// @param Out UINT32*. Pointer to the output buffer.
///
//=============================================================================
PUBLIC VOID spal_CapConvoluate(SPAL_CAP_CONV_PARAM_T param, UINT32* punctTable, UINT32* In, UINT32* Out);

//=============================================================================
// spal_CapConvSrc
//-----------------------------------------------------------------------------
/// CAP driver function to convoluate src
///
/// @param param SPAL_CAP_CONV_PARAM_T. Cap configuration for convolution.
/// @param In UINT32*. Pointer to the input buffer.
/// @param Out UINT32*. Pointer to the output buffer.
///
//=============================================================================
PUBLIC VOID spal_CapConvSrc(SPAL_CAP_CONV_PARAM_T param, UINT32* In, UINT32* Out);

//=============================================================================
// spal_CapFireCorrection
//-----------------------------------------------------------------------------
/// CAP driver function for FIRE code decoding
///
/// @param Out UINT32*. Pointer to the output buffer.
///
/// @return UINT8.
///
//=============================================================================
PUBLIC UINT8 spal_CapFireCorrection(UINT32* Out);

///@}

#endif // SPAL_cap_H
