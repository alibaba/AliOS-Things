#ifndef _HAL_LMDMA_H_
#define _HAL_LMDMA_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define LMDMA_VER (0x0200)

typedef enum _LMDMA_STAT_E {
    LMDMA_STAT_Idle         = 0,        ///< The current state of the DMA channel: Idle (value after a "reset" command)
    LMDMA_STAT_Run          = 1,        ///< The current state of the DMA channel: Run
    LMDMA_STAT_Pended       = 2,        ///< The current state of the DMA channel: Pended
    LMDMA_STAT_Stop_Error   = 3,        ///< The current state of the DMA channel: Stop/Error
    LMDMA_STAT_Complete     = 4         ///< The current state of the DMA channel: Complete
} LMDMA_STAT_E;

typedef enum _LMDMA_FSM_E {
    LMDMA_FSM_USE_ACMD      = 0,        ///< In the action-command mode.
    LMDMA_FSM_USE_TCNT      = 1,        ///< In the fast-start mode and uses the DMA_TCNT register as the trigger register.
    LMDMA_FSM_USE_ISADDR    = 2,        ///< In the fast-start mode and uses the DMA_ISADDR register as the trigger register.
    LMDMA_FSM_USE_ESADDR    = 3         ///< In the fast-start mode and uses the DMA_ESADDR register as the trigger register.
} LMDMA_FSM_E;

typedef enum _LMDMA_CHAN_E {
    LMDMA_CHAN_0            = 0,        ///< Channel 0 is selected.
    LMDMA_CHAN_1            = 1,        ///< Channel 1 is selected.
    LMDMA_CHAN_MAX          = 2,        ///< Maximum number of channels.
    LMDMA_CHAN_NOT_FOUND    = -14       ///< Free channel was not found.
} LMDMA_CHAN_E;

typedef enum _LMDMA_ACMD_E {
    LMDMA_ACMD_NOP          = 0,        ///< Main Action Command: No operation
    LMDMA_ACMD_Start        = 1,        ///< Main Action Command: Start the DMA transfer
    LMDMA_ACMD_Stop         = 2,        ///< Main Action Command: Stop the DMA transfer
    LMDMA_ACMD_Reset        = 3         ///< Main Action Command: Reset DMA status/error
} LMDMA_ACMD_E;

typedef enum _LMDMA_SCMD_E {
    LMDMA_SCMD_NoCond       = 0,        ///< Sub Action Command: It stands for "No Condition".
    LMDMA_SCMD_EnQ          = 1,        ///< Sub Action Command: It stands for "enqueue".
    LMDMA_SCMD_WaitDBS      = 2,        ///< Sub Action Command: It stands for "Wait for DLM Bank Switch".
    LMDMA_SCMD_EnQWait      = 3         ///< Sub Action Command: It stands for "Enqueue and Wait (for DLM Bank Switch)".
} LMDMA_SCMD_E;

typedef enum _LMDMA_LM_E {
    LMDMA_ILM               = 0,        ///< ILM (Instruction Local Memory) is selected for DMA transfer.
    LMDMA_DLM               = 1         ///< DLM (Data Local Memory) is selected for DMA transfer.
} LMDMA_LM_E;

typedef enum _LMDMA_TDIR_E {
    LMDMA_TDIR_EM2LM        = 0,        ///< From EM (External Memory) to LM (Local Memory).
    LMDMA_TDIR_LM2EM        = 1         ///< From LM (Local Memory) to EM (External Memory).
} LMDMA_TDIR_E;

typedef enum _LMDMA_TES_E {
    LMDMA_TES_1             = 0,        ///< 1 byte  transfer element size.
    LMDMA_TES_2             = 1,        ///< 2 bytes transfer element size.
    LMDMA_TES_4             = 2         ///< 4 bytes transfer element size.
} LMDMA_TES_E;

typedef enum _LMDMA_IE_E {
    LMDMA_CIE               = 0,        ///< Interrupt Enable on Completion.
    LMDMA_SIE               = 1,        ///< Interrupt Enable on explicit Stop.
    LMDMA_EIE               = 2         ///< Interrupt Enable on Error.
} LMDMA_IE_E;

typedef enum _LMDMA_BLOCKING_MODE_E {
    LMDMA_NON_BLOCKING      = 0,        ///< Set the DMA HAL driver to operate in non-blocking mode.
    LMDMA_BLOCKING          = 1         ///< Set the DMA HAL driver to operate in blocking mode.
} LMDMA_BLOCKING_MODE_E;

typedef union _LMDMA_CFG {
    //
    // Individual bit fields
    //
    struct {
        uint32_t NCHN       : 2;        ///< The number of DMA channels implemented.
        uint32_t UNEA       : 1;        ///< Indicates if the Un-aligned External Address transfer feature is implemented.
        uint32_t _2DET      : 1;        ///< Indicates if the 2-D Element Transfer feature is implemented.
        uint32_t Reserved   : 12;       ///< Reserved by the Andes spec (not used).
        uint32_t VER        : 16;       ///< Indicates the DMA architecture and implementation version.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} LMDMA_CFG;

typedef union _LMDMA_GCSW {
    //
    // Individual bit fields
    //
    struct {
        uint32_t C0STAT     : 3;        ///< The current state for the DMA channel 0.
        uint32_t C1STAT     : 3;        ///< The current state for the DMA channel 1.
        uint32_t Reserved1  : 6;        ///< Reserved by the Andes spec (not used).
        uint32_t C0INT      : 1;        ///< Interrupt flag for the DMA channel 0.
        uint32_t C1INT      : 1;        ///< Interrupt flag for the DMA channel 1.
        uint32_t Reserved2  : 2;        ///< Reserved by the Andes spec (not used).
        uint32_t HCHAN      : 2;        ///< Head channel number.
        uint32_t FSM        : 2;        ///< Fast-start mode field.
        uint32_t SCMD       : 2;        ///< DMA Sub-Action Command used in the fast-start mode.
        uint32_t Reserved3  : 6;        ///< Reserved by the Andes spec (not used).
        uint32_t SDBE       : 1;        ///< Shadow DBM and DBB bits Enable control.
        uint32_t DBM        : 1;        ///< Double-Buffer Mode enable control for the data local memory.
        uint32_t DBB        : 1;        ///< Double-Buffer Bank which can be accessed by the processor.
        uint32_t EN         : 1;        ///< DMA engine global enable bit.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} LMDMA_GCSW;

typedef union _LMDMA_ACT {
    //
    // Individual bit fields
    //
    struct {
        uint32_t ACMD       : 2;        ///< DMA Main Action Command.
        uint32_t SCMD       : 2;        ///< DMA Sub-action Command.
        uint32_t Reserved   : 28;       ///< Reserved by the Andes spec (not used).
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} LMDMA_ACT;

typedef union _LMDMA_SETUP {
    //
    // Individual bit fields
    //
    struct {
        uint32_t LM         : 1;        ///< Local Memory Selection.
        uint32_t TDIR       : 1;        ///< Transfer Direction.
        uint32_t TES        : 2;        ///< Transfer Element Size.
        uint32_t ESTR       : 12;       ///< External memory transfer Stride.
        uint32_t CIE        : 1;        ///< Interrupt Enable on Completion.
        uint32_t SIE        : 1;        ///< Interrupt Enable on explicit Stop.
        uint32_t EIE        : 1;        ///< Interrupt Enable on Error.
        uint32_t UE         : 1;        ///< Enable the Un-aligned External Address feature. (optional)
        uint32_t _2DE       : 1;        ///< Enable the 2-D External Transfer feature. (optional)
        uint32_t CA         : 3;        ///< Cacheability attribute of external memory access.
        uint32_t Reserved   : 8;        ///< Reserved by the Andes spec (not used).
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} LMDMA_SETUP;

typedef union _LMDMA_STATUS {
    //
    // Individual bit fields
    //
    struct {
        uint32_t STAT       : 3;        ///< The current state of the DMA channel.
        uint32_t STUNA      : 1;        ///< Indicates if an Un-aligned error has occurred on the External Stride value.
        uint32_t DERR       : 1;        ///< DMA Transfer Disruption Error.
        uint32_t EUNA       : 1;        ///< Indicates if an Un-aligned error has occurred on the address to the External bus.
        uint32_t IUNA       : 1;        ///< Indicates if an Un-aligned error has occurred on the address for local memory.
        uint32_t IOOR       : 1;        ///< Indicates if an Out-Of-Range error has occurred on the address for local memory.
        uint32_t EBUS       : 1;        ///< Indicates if a Bus Error has occurred on a DMA transfer.
        uint32_t ESUP       : 1;        ///< Indicates if a DMA setup error has occurred.
        uint32_t ELM        : 1;        ///< Indicates if a Local Memory error has occurred during the DMA transfer.
        uint32_t EECC       : 1;        ///< Indicates if an uncorrectable parity/ECC error has occurred during the DMA transfer.
        uint32_t Reserved   : 19;       ///< Reserved by the Andes spec (not used).
        uint32_t WE         : 1;        ///< Wait event. Indicates which event the channel is waiting for in the "Pended" state.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} LMDMA_STATUS;

typedef union _LMDMA_2DSET {
    //
    // Individual bit fields
    //
    struct {
        uint32_t WECNT      : 16;       ///< The Width Element Count for a 2-D transfer region.
        uint32_t HTSTR      : 16;       ///< The Height Stride for a 2-D transfer region.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} LMDMA_2DSET;

#if (defined(LMDMA_VER) && (LMDMA_VER >= 0x0200))   // {
typedef union _LMDMA_HSTATUS {
    //
    // Individual bit fields
    //
    struct {
        uint32_t STAT       : 3;        ///< The current state of the DMA channel.
        uint32_t STUNA      : 1;        ///< Indicates if an Un-aligned error has occurred on the External Stride value.
        uint32_t DERR       : 1;        ///< DMA Transfer Disruption Error.
        uint32_t EUNA       : 1;        ///< Indicates if an Un-aligned error has occurred on the address to the External bus.
        uint32_t IUNA       : 1;        ///< Indicates if an Un-aligned error has occurred on the address for local memory.
        uint32_t IOOR       : 1;        ///< Indicates if an Out-Of-Range error has occurred on the address for local memory.
        uint32_t EBUS       : 1;        ///< Indicates if a Bus Error has occurred on a DMA transfer.
        uint32_t ESUP       : 1;        ///< Indicates if a DMA setup error has occurred.
        uint32_t Reserved   : 21;       ///< Reserved by the Andes spec (not used).
        uint32_t WE         : 1;        ///< Wait event. Indicates which event the channel is waiting for in the "Pended" state.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} LMDMA_HSTATUS;
#endif  // }

typedef union _MMU_ILMB {
    //
    // Individual bit fields
    //
    struct {
        uint32_t IEN        : 1;        ///< Instruction Local Memory enable bit.
        uint32_t ILMSZ      : 4;        ///< Indicates the size of ILM. The size has to be power of 2.
        uint32_t Reserved1  : 2;        ///< Reserved by the Andes spec (not used).
        uint32_t ECCEN      : 2;        ///< Parity/ECC enable control.
        uint32_t Reserved2  : 1;        ///< Reserved by the Andes spec (not used).
        uint32_t IBPA       : 22;       ///< The base physical address of ILM.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} MMU_ILMB;

typedef union _MMU_DLMB {
    //
    // Individual bit fields
    //
    struct {
        uint32_t DEN        : 1;        ///< Data Local Memory enable bit.
        uint32_t DLMSZ      : 4;        ///< Indicates the size of DLM. The size has to be power of 2.
        uint32_t DBM        : 1;        ///< Enable Double-Buffer Mode for data local memory.
        uint32_t DBB        : 1;        ///< Double-buffer bank which can be accessed by the processor.
        uint32_t ECCEN      : 2;        ///< Parity/ECC enable control.
        uint32_t Reserved   : 1;        ///< Reserved by the Andes spec (not used).
        uint32_t DBPA       : 22;       ///< The base physical address of DLM.
    } Bits;
    //
    // All bit fields as a 32-bit value
    //
    uint32_t Reg32;
} MMU_DLMB;

/**
 * Prototype for the Local Memory DMA interrupt service routine.
 *
 * @param   dma_chan                    DMA Channel Number.
 * @param   dma_int_type                The type of DMA interrupt enable.
 */
typedef
void
(*LMDMA_ISR) (
    LMDMA_CHAN_E            dma_chan,
    LMDMA_IE_E              dma_int_type
    );

/**
 * Registers an interrupt service routine to be called from the Local Memory DMA interrupt handler.
 *
 * If dma_isr is NULL, then the Local Memory DMA interrupt is disabled.
 *
 * @param   dma_chan                    DMA Channel Selection.
 * @param   dma_int_enable              Select the type of DMA interrupt enable.
 * @param   dma_isr                     A pointer to an ISR function that is called when a DMA interrupt handler occurs.
 *                                      NULL to disable interrupt.
 *
 * @retval  -2                          The dma_chan or dma_int_enable of the parameters are incorrect.
 * @retval   0                          The operation completed successfully.
 */
int32_t
hal_lmdma_register_isr (
    LMDMA_CHAN_E            dma_chan,
    LMDMA_IE_E              dma_int_enable,
    LMDMA_ISR               dma_isr
    );

/**
 * Initialize the Local Memory DMA hardware.
 */
void
hal_lmdma_init (
    void
    );

/**
 * Deinitialize the Local Memory DMA hardware.
 */
void
hal_lmdma_deinit (
    void
    );

/**
 * Start the Local Memory DMA transfer.
 *
 * @param   dma_isaddr                  DMA Internal Start Address.
 * @param   dma_esaddr                  DMA External Start Address.
 * @param   dma_tcnt                    DMA Transfer Element Count.
 * @param   dma_chan                    DMA Channel Selection.
 * @param   dma_tdir                    DMA Transfer Direction.
 * @param   dma_tes                     DMA Transfer Element Size.
 *
 * @retval  131                         The address for local memory was out of range.
 * @retval    0                         The operation completed successfully.
 */
int32_t
hal_lmdma_start (
    uint8_t                 *dma_isaddr,
    uint8_t                 *dma_esaddr,
    uint32_t                dma_tcnt,
    LMDMA_CHAN_E            dma_chan,
    LMDMA_TDIR_E            dma_tdir,
    LMDMA_TES_E             dma_tes
    );

/**
 * Stop the Local Memory DMA transfer.
 *
 * @param   dma_chan                    DMA Channel Selection.
 * @param   dma_blocking_mode           Sets the DMA HAL driver to operate in blocking or non-blocking mode.
 *
 * @retval  -2                          The dma_chan of the parameters is incorrect.
 * @retval   0                          The operation completed successfully.
 */
int32_t
hal_lmdma_stop (
    LMDMA_CHAN_E            dma_chan,
    LMDMA_BLOCKING_MODE_E   dma_blocking_mode
    );

/**
 * Reset the Local Memory DMA status or error.
 *
 * @param   dma_chan                    DMA Channel Selection.
 * @param   dma_blocking_mode           Sets the DMA HAL driver to operate in blocking or non-blocking mode.
 *
 * @retval  -2                          The dma_chan of the parameters is incorrect.
 * @retval   0                          The operation completed successfully.
 */
int32_t
hal_lmdma_reset (
    LMDMA_CHAN_E            dma_chan,
    LMDMA_BLOCKING_MODE_E   dma_blocking_mode
    );

/**
 * Sets the Local Memory DMA to 32-bit memory copy operation.
 *
 * @param   dma_isaddr                  DMA Internal Start Address.
 * @param   dma_esaddr                  DMA External Start Address.
 * @param   cnt                         Number of bytes to copy.
 * @param   dma_chan                    DMA Channel Selection.
 * @param   dma_tdir                    DMA Transfer Direction.
 *
 * @retval  -19                         The DMA has not been started.
 * @retval  131                         The address for local memory was out of range.
 * @retval    0                         The operation completed successfully.
 */
int32_t
hal_lmdma_copy32 (
    uint8_t                 *dma_isaddr,
    uint8_t                 *dma_esaddr,
    uint32_t                cnt,
    LMDMA_CHAN_E            dma_chan,
    LMDMA_TDIR_E            dma_tdir
    );

/**
 * Sets the Local Memory DMA to 8-bit memory copy operation.
 *
 * @param   dma_isaddr                  DMA Internal Start Address.
 * @param   dma_esaddr                  DMA External Start Address.
 * @param   cnt                         Number of bytes to copy.
 * @param   dma_chan                    DMA Channel Selection.
 * @param   dma_tdir                    DMA Transfer Direction.
 *
 * @retval  -19                         The DMA has not been started.
 * @retval  131                         The address for local memory was out of range.
 * @retval    0                         The operation completed successfully.
 */
int32_t
hal_lmdma_copy8 (
    uint8_t                 *dma_isaddr,
    uint8_t                 *dma_esaddr,
    uint32_t                cnt,
    LMDMA_CHAN_E            dma_chan,
    LMDMA_TDIR_E            dma_tdir
    );

/**
 * This service finds a free Local Memory DMA channel.
 *
 * @param   dma_blocking_mode           Sets the DMA HAL driver to operate in blocking or non-blocking mode.
 *
 * @retval  LMDMA_CHAN_NOT_FOUND        The free channel was not found.
 * @return  Returns the free channel number.
 */
LMDMA_CHAN_E
hal_lmdma_find_free_channel (
    LMDMA_BLOCKING_MODE_E   dma_blocking_mode
    );

/**
 * This service reads the Local Memory DMA configuration value.
 *
 * @return  Returns the DMA configuration value.
 */
uint32_t
hal_lmdma_get_cfg (
    void
    );

/**
 * This service reads the Local Memory DMA channel status value.
 *
 * @param   dma_chan                    DMA Channel Selection.
 *
 * @retval  -2                          The dma_chan of the parameters is incorrect.
 * @return  Returns the DMA channel status value.
 */
uint32_t
hal_lmdma_get_status (
    LMDMA_CHAN_E            dma_chan
    );

#endif  // #ifndef _HAL_LMDMA_H_
