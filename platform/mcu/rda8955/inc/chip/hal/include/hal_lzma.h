/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef  _HAL_LZMA_H_
#define  _HAL_LZMA_H_


#include "cs_types.h"

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_LzmaInit
// -----------------------------------------------------------------------------
/// Initialize lzma module
/// @param VOID
// =============================================================================
PUBLIC VOID hal_LzmaInit(VOID);

// =============================================================================
// hal_LzmaSetIrqMask
// -----------------------------------------------------------------------------
/// Set Irq Mask
/// @param UINT32
///   bit 0: Decode done IRQ mask
///   bit 1: Decode error IRQ mask
///   bit 2: AXI Bus error IRQ mask
// =============================================================================
PUBLIC VOID hal_LzmaSetIrqMask(UINT32 mask);

// =============================================================================
// hal_LzmaSetDecDoneIrqMask
// -----------------------------------------------------------------------------
/// Set Irq Mask
/// @param BOOL
///   TRUE: set Decode done IRQ mask
///   FALSE: clear  Decode done IRQ mask
// =============================================================================
PUBLIC VOID hal_LzmaSetDecDoneIrqMask(BOOL mask);

// =============================================================================
// hal_LzmaSetDecErrIrqMask
// -----------------------------------------------------------------------------
/// Set Irq Mask
/// @param BOOL
///   TRUE: set Decode error IRQ mask
///   FALSE: clear  Decode error IRQ mask
// =============================================================================
PUBLIC VOID hal_LzmaSetDecErrIrqMask(BOOL mask);

// =============================================================================
// hal_LzmaSetAxiErrIrqMask
// -----------------------------------------------------------------------------
/// Set Irq Mask
/// @param BOOL
///   TRUE: set AXI Bus error IRQ mask
///   FALSE: clear AXI Bus error IRQ mask
// =============================================================================
PUBLIC VOID hal_LzmaSetAxiErrIrqMask(BOOL mask);

// =============================================================================
// hal_LzmaStart
// -----------------------------------------------------------------------------
/// Start Lzma
/// @param VOID
// =============================================================================
PUBLIC VOID hal_LzmaStart(VOID);

// =============================================================================
// hal_LzmaConfig
// -----------------------------------------------------------------------------
/// Set Lzma configuration registers
/// @param UINT32 blk_size : block size
/// @param UINT32 dic_size : directary size, default 1024
/// @param UINT32 stream_size : stream size
// =============================================================================
PUBLIC VOID hal_LzmaConfig(UINT32 blk_size, UINT32 dic_size, UINT32 stream_size);

// =============================================================================
// hal_LzmaSetSrcBuf
// -----------------------------------------------------------------------------
/// Set Lzma DMA source address
/// @param UINT8* src_Addr : lzma DMA source address
// =============================================================================
PUBLIC VOID hal_LzmaSetSrcBuf(UINT8* srcAddr);

// =============================================================================
// hal_LzmaSetDstBuf
// -----------------------------------------------------------------------------
/// Set lzma DMA distnation address
/// @param UINT8* dstAddr : Decompress output address
// =============================================================================
PUBLIC VOID hal_LzmaSetDstBuf(UINT8* dstAddr);

// =============================================================================
// hal_LzmaGetReg1Status
// -----------------------------------------------------------------------------
/// Get Status Register 0x4 value
/// @param VOID
/// @return UINT32
// =============================================================================
PUBLIC UINT32 hal_LzmaGetReg1Status(VOID);



#endif //  _HAL_LZMA_H_
