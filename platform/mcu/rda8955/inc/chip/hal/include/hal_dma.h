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


#ifndef  _HAL_DMA_H_
#define  _HAL_DMA_H_

#include "cs_types.h"
#include "hal_error.h"


/// @defgroup dma HAL DMA Driver
///
/// The maximum transfer size is 65335 bytes.
/// @{







// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// HAL_DMA_IRQ_HANDLER_T
// -----------------------------------------------------------------------------
/// This structure is used to set the user handler called when a transfer
/// is finished.
// =============================================================================
typedef VOID (*HAL_DMA_IRQ_HANDLER_T)(VOID);


// =============================================================================
// HAL_DMA_GEA_CFG_T
// -----------------------------------------------------------------------------
/// This structure is used to configure a transfer with a GEA operation
/// between the source and the destination.
// =============================================================================
typedef struct
{
    /// Source address of the transfer
    CONST UINT8*    srcAddr;

    /// Pointer towards the destination buffer
    UINT8*          dstAddr;

    /// Number of bytes to transfer
    UINT16         transferSize;

    /// Number of the GEA algorithm to use
    UINT8          geaAlgo;

    /// Direction of the GEA transfer
    UINT8          direction;

    /// Value of the lower 32 bits of the KC key
    UINT32         kcLow;

    /// Value of the higher 32 bits of the KC key
    UINT32         kcHigh;

    /// MessKey
    UINT32         messKey;

    /// User handler call once the transfer is finished
    /// NULL for no interruption.
    HAL_DMA_IRQ_HANDLER_T userHandler;
} HAL_DMA_GEA_CFG_T;


// =============================================================================
// HAL_DMA_MODE_T
// -----------------------------------------------------------------------------
/// Those are the different available modes for a DMA transfer
// =============================================================================
typedef enum
{
    /// Normal mode
    HAL_DMA_MODE_NORMAL,
    /// The data are all sent to the same address
    HAL_DMA_MODE_CONST_ADDR,
    /// The pattern value is written at the destination address,
    /// and the source buffer at the secondary destination address.
    HAL_DMA_MODE_ALTERN_ADDR,
    /// Pattern mode: the destination buffer is filled with the pattern
    HAL_DMA_MODE_PATTERN,
    /// Pattern mode written at a constant address
    HAL_DMA_MODE_PATTERN_CONST_ADDR

} HAL_DMA_MODE_T;


// =============================================================================
// HAL_DMA_CFG_T
// -----------------------------------------------------------------------------
/// This structure is used to configure a DMA transfer
// =============================================================================
typedef struct
{
    /// Source address in case of normal or const address
    /// mode
    CONST UINT8*    srcAddr;
    /// (Primary) destination address
    UINT8*          dstAddr;
    /// Secondary (alternate) destination address
    UINT8*          alterDstAddr;
    /// Pattern, used only in case when the pattern is used
    UINT32          pattern;
    /// Size of the transfer to make. Maximum is 256KB.
    UINT32          transferSize;
    /// Transfer mode
    HAL_DMA_MODE_T mode;
    /// User handler called at the end of the transfer if
    /// this field is not NULL
    HAL_DMA_IRQ_HANDLER_T userHandler; //called at end of transfer
} HAL_DMA_CFG_T;


// =============================================================================
// HAL_DMA_FCS_CFG_T
// -----------------------------------------------------------------------------
/// Configuration structure used to calculate the FCS of a source buffer
// =============================================================================
typedef struct
{
    /// Source address of the buffer
    CONST UINT8 * srcAddr;
    /// Buffer size
    UINT16           transferSize;
    /// User handler to call when the FCS is calculated.
    HAL_DMA_IRQ_HANDLER_T userHandler;
} HAL_DMA_FCS_CFG_T;










// =============================================================================
//  FUNCTIONS
// =============================================================================




// =============================================================================
// hal_DmaDone
// -----------------------------------------------------------------------------
/// Test if a DMA transfer is still running.
/// @return \c FALSE if the transfer is still in progress, \c TRUE if it has
/// terminated.
// =============================================================================
PUBLIC BOOL hal_DmaDone(VOID);





// =============================================================================
// hal_DmaStart
// -----------------------------------------------------------------------------
/// Configure the registers and launch the DMA transfer.
///
/// @param pCfg The transfer configuration. Refer to the type descriptions for
/// more details.
/// @return DMA transfer error status: #HAL_ERR_NO, #HAL_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC HAL_ERR_T hal_DmaStart(CONST HAL_DMA_CFG_T *pCfg);



// =============================================================================
// hal_DmaGeaStart
// -----------------------------------------------------------------------------
/// Configure the registers and launch the transfer
/// with GEA. This is a blocking function, that returns only after the
/// transfer is finished.
///
/// @param pCfg The transfer configuration. Refer to the type descriptions for
/// more details.
/// @return DMA transfer error status: #HAL_ERR_NO.
// =============================================================================
PUBLIC HAL_ERR_T hal_DmaGeaStart(CONST HAL_DMA_GEA_CFG_T* pCfg);




// =============================================================================
// hal_DmaFcsStart
// -----------------------------------------------------------------------------
/// Perform a FCS checksum on the data specified by the transfer configuration
/// structure. This is a blocking function, that returns only after the
/// transfer is finished.
///
/// @param pCfg The transfer configuration.
/// @return DMA transfer error status: #HAL_ERR_NO.
// =============================================================================
PUBLIC HAL_ERR_T hal_DmaFcsStart(CONST HAL_DMA_FCS_CFG_T* pCfg);



// =============================================================================
// hal_DmaReadFcs
// -----------------------------------------------------------------------------
/// After a FCS has been started and finished (checked by #hal_DmaDone),
/// this function recovers the FCSdata.
///
/// @param fcs Pointer where the FCS data will be stored
// =============================================================================
PUBLIC VOID hal_DmaReadFcs(UINT8 *fcs);

// =============================================================================
// hal_DmaCheckFcs
// -----------------------------------------------------------------------------
/// After a FCS has been started and finished (checked by #hal_DmaDone),
/// checks the validity of the FCS
/// @return \c TRUE if the FCS is correct, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_DmaCheckFcs(VOID);

/// @} // end of the dma group


#endif // _HAL_DMA_H_

