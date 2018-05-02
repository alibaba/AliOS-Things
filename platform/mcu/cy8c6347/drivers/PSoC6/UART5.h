/***************************************************************************//**
* \file UART5.h
* \version 2.0
*
*  This file provides constants and parameter values for the UART component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(UART5_CY_SCB_UART_PDL_H)
#define UART5_CY_SCB_UART_PDL_H

#include "cyfitter.h"
#include "scb/cy_scb_uart.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*   Initial Parameter Constants
****************************************/

#define UART5_DIRECTION  (3U)
#define UART5_ENABLE_RTS (0U)
#define UART5_ENABLE_CTS (0U)

/* UART direction enum */
#define UART5_RX    (0x1U)
#define UART5_TX    (0x2U)

#define UART5_ENABLE_RX  (0UL != (UART5_DIRECTION & UART5_RX))
#define UART5_ENABLE_TX  (0UL != (UART5_DIRECTION & UART5_TX))


/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
/* Component specific functions. */
void UART5_Start(void);

/* Basic functions */
__STATIC_INLINE cy_en_scb_uart_status_t UART5_Init(cy_stc_scb_uart_config_t const *config);
__STATIC_INLINE void UART5_DeInit(void);
__STATIC_INLINE void UART5_Enable(void);
__STATIC_INLINE void UART5_Disable(void);

/* Register callback. */
__STATIC_INLINE void UART5_RegisterCallback(cy_cb_scb_uart_handle_events_t callback);

/* Configuration change. */
#if (UART5_ENABLE_CTS)
__STATIC_INLINE void UART5_EnableCts(void);
__STATIC_INLINE void UART5_DisableCts(void);
#endif /* (UART5_ENABLE_CTS) */

#if (UART5_ENABLE_RTS)
__STATIC_INLINE void     UART5_SetRtsFifoLevel(uint32_t level);
__STATIC_INLINE uint32_t UART5_GetRtsFifoLevel(void);
#endif /* (UART5_ENABLE_RTS) */

__STATIC_INLINE void UART5_EnableSkipStart(void);
__STATIC_INLINE void UART5_DisableSkipStart(void);

#if (UART5_ENABLE_RX)
/* Low level: Receive direction. */
__STATIC_INLINE uint32_t UART5_Get(void);
__STATIC_INLINE uint32_t UART5_GetArray(void *buffer, uint32_t size);
__STATIC_INLINE void     UART5_GetArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE uint32_t UART5_GetRxFifoStatus(void);
__STATIC_INLINE void     UART5_ClearRxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t UART5_GetNumInRxFifo(void);
__STATIC_INLINE void     UART5_ClearRxFifo(void);
#endif /* (UART5_ENABLE_RX) */

#if (UART5_ENABLE_TX)
/* Low level: Transmit direction. */
__STATIC_INLINE uint32_t UART5_Put(uint32_t data);
__STATIC_INLINE uint32_t UART5_PutArray(void *buffer, uint32_t size);
__STATIC_INLINE void     UART5_PutArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE void     UART5_PutString(char_t const string[]);
__STATIC_INLINE void     UART5_SendBreakBlocking(uint32_t breakWidth);
__STATIC_INLINE uint32_t UART5_GetTxFifoStatus(void);
__STATIC_INLINE void     UART5_ClearTxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t UART5_GetNumInTxFifo(void);
__STATIC_INLINE bool     UART5_IsTxComplete(void);
__STATIC_INLINE void     UART5_ClearTxFifo(void);
#endif /* (UART5_ENABLE_TX) */

#if (UART5_ENABLE_RX)
/* High level: Ring buffer functions. */
__STATIC_INLINE void     UART5_StartRingBuffer(void *buffer, uint32_t size);
__STATIC_INLINE void     UART5_StopRingBuffer(void);
__STATIC_INLINE void     UART5_ClearRingBuffer(void);
__STATIC_INLINE uint32_t UART5_GetNumInRingBuffer(void);

/* High level: Receive direction functions. */
__STATIC_INLINE cy_en_scb_uart_status_t UART5_Receive(void *buffer, uint32_t size);
__STATIC_INLINE void     UART5_AbortReceive(void);
__STATIC_INLINE uint32_t UART5_GetReceiveStatus(void);
__STATIC_INLINE uint32_t UART5_GetNumReceived(void);
#endif /* (UART5_ENABLE_RX) */

#if (UART5_ENABLE_TX)
/* High level: Transmit direction functions. */
__STATIC_INLINE cy_en_scb_uart_status_t UART5_Transmit(void *buffer, uint32_t size);
__STATIC_INLINE void     UART5_AbortTransmit(void);
__STATIC_INLINE uint32_t UART5_GetTransmitStatus(void);
__STATIC_INLINE uint32_t UART5_GetNumLeftToTransmit(void);
#endif /* (UART5_ENABLE_TX) */

/* Interrupt handler */
__STATIC_INLINE void UART5_Interrupt(void);
/** @} group_general */


/***************************************
*    Variables with External Linkage
***************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t UART5_initVar;
extern cy_stc_scb_uart_config_t const UART5_config;
extern cy_stc_scb_uart_context_t UART5_context;
/** @} group_globals */


/***************************************
*         Preprocessor Macros
***************************************/
/**
* \addtogroup group_macros
* @{
*/
/** The pointer to the base address of the hardware */
#define UART5_HW     ((CySCB_Type *) UART5_SCB__HW)
/** @} group_macros */


/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: UART5_Init
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t UART5_Init(cy_stc_scb_uart_config_t const *config)
{
   return Cy_SCB_UART_Init(UART5_HW, config, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_DeInit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_DeInit(void)
{
    Cy_SCB_UART_DeInit(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_Enable
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Enable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_Enable(void)
{
    Cy_SCB_UART_Enable(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_Disable
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Disable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_Disable(void)
{
    Cy_SCB_UART_Disable(UART5_HW, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_RegisterCallback
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_RegisterCallback() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_RegisterCallback(cy_cb_scb_uart_handle_events_t callback)
{
    Cy_SCB_UART_RegisterCallback(UART5_HW, callback, &UART5_context);
}


#if (UART5_ENABLE_CTS)
/*******************************************************************************
* Function Name: UART5_EnableCts
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_EnableCts() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_EnableCts(void)
{
    Cy_SCB_UART_EnableCts(UART5_HW);
}


/*******************************************************************************
* Function Name: Cy_SCB_UART_DisableCts
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DisableCts() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_DisableCts(void)
{
    Cy_SCB_UART_DisableCts(UART5_HW);
}
#endif /* (UART5_ENABLE_CTS) */


#if (UART5_ENABLE_RTS)
/*******************************************************************************
* Function Name: UART5_SetRtsFifoLevel
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_SetRtsFifoLevel() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_SetRtsFifoLevel(uint32_t level)
{
    Cy_SCB_UART_SetRtsFifoLevel(UART5_HW, level);
}


/*******************************************************************************
* Function Name: UART5_GetRtsFifoLevel
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetRtsFifoLevel() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetRtsFifoLevel(void)
{
    return Cy_SCB_UART_GetRtsFifoLevel(UART5_HW);
}
#endif /* (UART5_ENABLE_RTS) */


/*******************************************************************************
* Function Name: UART5_EnableSkipStart
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_EnableSkipStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_EnableSkipStart(void)
{
    Cy_SCB_UART_EnableSkipStart(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_DisableSkipStart
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DisableSkipStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_DisableSkipStart(void)
{
    Cy_SCB_UART_DisableSkipStart(UART5_HW);
}


#if (UART5_ENABLE_RX)
/*******************************************************************************
* Function Name: UART5_Get
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Get() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_Get(void)
{
    return Cy_SCB_UART_Get(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_GetArray
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetArray(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_GetArray(UART5_HW, buffer, size);
}


/*******************************************************************************
* Function Name: UART5_GetArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_GetArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_UART_GetArrayBlocking(UART5_HW, buffer, size);
}


/*******************************************************************************
* Function Name: UART5_GetRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetRxFifoStatus(void)
{
    return Cy_SCB_UART_GetRxFifoStatus(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_ClearRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_ClearRxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_UART_ClearRxFifoStatus(UART5_HW, clearMask);
}


/*******************************************************************************
* Function Name: UART5_GetNumInRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetNumInRxFifo(void)
{
    return Cy_SCB_UART_GetNumInRxFifo(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_ClearRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_ClearRxFifo(void)
{
    Cy_SCB_UART_ClearRxFifo(UART5_HW);
}
#endif /* (UART5_ENABLE_RX) */


#if (UART5_ENABLE_TX)
/*******************************************************************************
* Function Name: UART5_Put
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Put() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_Put(uint32_t data)
{
    return Cy_SCB_UART_Put(UART5_HW,data);
}


/*******************************************************************************
* Function Name: UART5_PutArray
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_PutArray(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_PutArray(UART5_HW, buffer, size);
}


/*******************************************************************************
* Function Name: UART5_PutArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_PutArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_UART_PutArrayBlocking(UART5_HW, buffer, size);
}


/*******************************************************************************
* Function Name: UART5_PutString
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutString() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_PutString(char_t const string[])
{
    Cy_SCB_UART_PutString(UART5_HW, string);
}


/*******************************************************************************
* Function Name: UART5_SendBreakBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_SendBreakBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_SendBreakBlocking(uint32_t breakWidth)
{
    Cy_SCB_UART_SendBreakBlocking(UART5_HW, breakWidth);
}


/*******************************************************************************
* Function Name: UART5_GetTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetTxFifoStatus(void)
{
    return Cy_SCB_UART_GetTxFifoStatus(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_ClearTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_ClearTxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_UART_ClearTxFifoStatus(UART5_HW, clearMask);
}


/*******************************************************************************
* Function Name: UART5_GetNumInTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetNumInTxFifo(void)
{
    return Cy_SCB_UART_GetNumInTxFifo(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_IsTxComplete
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_IsTxComplete() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool UART5_IsTxComplete(void)
{
    return Cy_SCB_UART_IsTxComplete(UART5_HW);
}


/*******************************************************************************
* Function Name: UART5_ClearTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_ClearTxFifo(void)
{
    Cy_SCB_UART_ClearTxFifo(UART5_HW);
}
#endif /* (UART5_ENABLE_TX) */


#if (UART5_ENABLE_RX)
/*******************************************************************************
* Function Name: UART5_StartRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_StartRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_StartRingBuffer(void *buffer, uint32_t size)
{
    Cy_SCB_UART_StartRingBuffer(UART5_HW, buffer, size, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_StopRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_StopRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_StopRingBuffer(void)
{
    Cy_SCB_UART_StopRingBuffer(UART5_HW, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_ClearRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_ClearRingBuffer(void)
{
    Cy_SCB_UART_ClearRingBuffer(UART5_HW, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_GetNumInRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetNumInRingBuffer(void)
{
    return Cy_SCB_UART_GetNumInRingBuffer(UART5_HW, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_Receive
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Receive() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t UART5_Receive(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_Receive(UART5_HW, buffer, size, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_GetReceiveStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetReceiveStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetReceiveStatus(void)
{
    return Cy_SCB_UART_GetReceiveStatus(UART5_HW, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_AbortReceive
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_AbortReceive() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_AbortReceive(void)
{
    Cy_SCB_UART_AbortReceive(UART5_HW, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_GetNumReceived
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumReceived() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetNumReceived(void)
{
    return Cy_SCB_UART_GetNumReceived(UART5_HW, &UART5_context);
}
#endif /* (UART5_ENABLE_RX) */


#if (UART5_ENABLE_TX)
/*******************************************************************************
* Function Name: UART5_Transmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Transmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t UART5_Transmit(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_Transmit(UART5_HW, buffer, size, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_GetTransmitStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetTransmitStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetTransmitStatus(void)
{
    return Cy_SCB_UART_GetTransmitStatus(UART5_HW, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_AbortTransmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_AbortTransmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_AbortTransmit(void)
{
    Cy_SCB_UART_AbortTransmit(UART5_HW, &UART5_context);
}


/*******************************************************************************
* Function Name: UART5_GetNumLeftToTransmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumLeftToTransmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART5_GetNumLeftToTransmit(void)
{
    return Cy_SCB_UART_GetNumLeftToTransmit(UART5_HW, &UART5_context);
}
#endif /* (UART5_ENABLE_TX) */


/*******************************************************************************
* Function Name: UART5_Interrupt
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Interrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART5_Interrupt(void)
{
    Cy_SCB_UART_Interrupt(UART5_HW, &UART5_context);
}

#if defined(__cplusplus)
}
#endif

#endif /* UART5_CY_SCB_UART_PDL_H */


/* [] END OF FILE */
