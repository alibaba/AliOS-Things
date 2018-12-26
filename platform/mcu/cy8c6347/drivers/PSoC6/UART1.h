/***************************************************************************//**
* \file UART1.h
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

#if !defined(UART1_CY_SCB_UART_PDL_H)
#define UART1_CY_SCB_UART_PDL_H

#include "cyfitter.h"
#include "scb/cy_scb_uart.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*   Initial Parameter Constants
****************************************/

#define UART1_DIRECTION  (3U)
#define UART1_ENABLE_RTS (0U)
#define UART1_ENABLE_CTS (0U)

/* UART direction enum */
#define UART1_RX    (0x1U)
#define UART1_TX    (0x2U)

#define UART1_ENABLE_RX  (0UL != (UART1_DIRECTION & UART1_RX))
#define UART1_ENABLE_TX  (0UL != (UART1_DIRECTION & UART1_TX))


/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
/* Component specific functions. */
void UART1_Start(void);

/* Basic functions */
__STATIC_INLINE cy_en_scb_uart_status_t UART1_Init(cy_stc_scb_uart_config_t const *config);
__STATIC_INLINE void UART1_DeInit(void);
__STATIC_INLINE void UART1_Enable(void);
__STATIC_INLINE void UART1_Disable(void);

/* Register callback. */
__STATIC_INLINE void UART1_RegisterCallback(cy_cb_scb_uart_handle_events_t callback);

/* Configuration change. */
#if (UART1_ENABLE_CTS)
__STATIC_INLINE void UART1_EnableCts(void);
__STATIC_INLINE void UART1_DisableCts(void);
#endif /* (UART1_ENABLE_CTS) */

#if (UART1_ENABLE_RTS)
__STATIC_INLINE void     UART1_SetRtsFifoLevel(uint32_t level);
__STATIC_INLINE uint32_t UART1_GetRtsFifoLevel(void);
#endif /* (UART1_ENABLE_RTS) */

__STATIC_INLINE void UART1_EnableSkipStart(void);
__STATIC_INLINE void UART1_DisableSkipStart(void);

#if (UART1_ENABLE_RX)
/* Low level: Receive direction. */
__STATIC_INLINE uint32_t UART1_Get(void);
__STATIC_INLINE uint32_t UART1_GetArray(void *buffer, uint32_t size);
__STATIC_INLINE void     UART1_GetArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE uint32_t UART1_GetRxFifoStatus(void);
__STATIC_INLINE void     UART1_ClearRxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t UART1_GetNumInRxFifo(void);
__STATIC_INLINE void     UART1_ClearRxFifo(void);
#endif /* (UART1_ENABLE_RX) */

#if (UART1_ENABLE_TX)
/* Low level: Transmit direction. */
__STATIC_INLINE uint32_t UART1_Put(uint32_t data);
__STATIC_INLINE uint32_t UART1_PutArray(void *buffer, uint32_t size);
__STATIC_INLINE void     UART1_PutArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE void     UART1_PutString(char_t const string[]);
__STATIC_INLINE void     UART1_SendBreakBlocking(uint32_t breakWidth);
__STATIC_INLINE uint32_t UART1_GetTxFifoStatus(void);
__STATIC_INLINE void     UART1_ClearTxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t UART1_GetNumInTxFifo(void);
__STATIC_INLINE bool     UART1_IsTxComplete(void);
__STATIC_INLINE void     UART1_ClearTxFifo(void);
#endif /* (UART1_ENABLE_TX) */

#if (UART1_ENABLE_RX)
/* High level: Ring buffer functions. */
__STATIC_INLINE void     UART1_StartRingBuffer(void *buffer, uint32_t size);
__STATIC_INLINE void     UART1_StopRingBuffer(void);
__STATIC_INLINE void     UART1_ClearRingBuffer(void);
__STATIC_INLINE uint32_t UART1_GetNumInRingBuffer(void);

/* High level: Receive direction functions. */
__STATIC_INLINE cy_en_scb_uart_status_t UART1_Receive(void *buffer, uint32_t size);
__STATIC_INLINE void     UART1_AbortReceive(void);
__STATIC_INLINE uint32_t UART1_GetReceiveStatus(void);
__STATIC_INLINE uint32_t UART1_GetNumReceived(void);
#endif /* (UART1_ENABLE_RX) */

#if (UART1_ENABLE_TX)
/* High level: Transmit direction functions. */
__STATIC_INLINE cy_en_scb_uart_status_t UART1_Transmit(void *buffer, uint32_t size);
__STATIC_INLINE void     UART1_AbortTransmit(void);
__STATIC_INLINE uint32_t UART1_GetTransmitStatus(void);
__STATIC_INLINE uint32_t UART1_GetNumLeftToTransmit(void);
#endif /* (UART1_ENABLE_TX) */

/* Interrupt handler */
__STATIC_INLINE void UART1_Interrupt(void);
/** @} group_general */


/***************************************
*    Variables with External Linkage
***************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t UART1_initVar;
extern cy_stc_scb_uart_config_t const UART1_config;
extern cy_stc_scb_uart_context_t UART1_context;
/** @} group_globals */


/***************************************
*         Preprocessor Macros
***************************************/
/**
* \addtogroup group_macros
* @{
*/
/** The pointer to the base address of the hardware */
#define UART1_HW     ((CySCB_Type *) UART1_SCB__HW)
/** @} group_macros */


/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: UART1_Init
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t UART1_Init(cy_stc_scb_uart_config_t const *config)
{
   return Cy_SCB_UART_Init(UART1_HW, config, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_DeInit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_DeInit(void)
{
    Cy_SCB_UART_DeInit(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_Enable
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Enable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_Enable(void)
{
    Cy_SCB_UART_Enable(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_Disable
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Disable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_Disable(void)
{
    Cy_SCB_UART_Disable(UART1_HW, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_RegisterCallback
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_RegisterCallback() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_RegisterCallback(cy_cb_scb_uart_handle_events_t callback)
{
    Cy_SCB_UART_RegisterCallback(UART1_HW, callback, &UART1_context);
}


#if (UART1_ENABLE_CTS)
/*******************************************************************************
* Function Name: UART1_EnableCts
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_EnableCts() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_EnableCts(void)
{
    Cy_SCB_UART_EnableCts(UART1_HW);
}


/*******************************************************************************
* Function Name: Cy_SCB_UART_DisableCts
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DisableCts() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_DisableCts(void)
{
    Cy_SCB_UART_DisableCts(UART1_HW);
}
#endif /* (UART1_ENABLE_CTS) */


#if (UART1_ENABLE_RTS)
/*******************************************************************************
* Function Name: UART1_SetRtsFifoLevel
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_SetRtsFifoLevel() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_SetRtsFifoLevel(uint32_t level)
{
    Cy_SCB_UART_SetRtsFifoLevel(UART1_HW, level);
}


/*******************************************************************************
* Function Name: UART1_GetRtsFifoLevel
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetRtsFifoLevel() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetRtsFifoLevel(void)
{
    return Cy_SCB_UART_GetRtsFifoLevel(UART1_HW);
}
#endif /* (UART1_ENABLE_RTS) */


/*******************************************************************************
* Function Name: UART1_EnableSkipStart
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_EnableSkipStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_EnableSkipStart(void)
{
    Cy_SCB_UART_EnableSkipStart(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_DisableSkipStart
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DisableSkipStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_DisableSkipStart(void)
{
    Cy_SCB_UART_DisableSkipStart(UART1_HW);
}


#if (UART1_ENABLE_RX)
/*******************************************************************************
* Function Name: UART1_Get
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Get() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_Get(void)
{
    return Cy_SCB_UART_Get(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_GetArray
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetArray(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_GetArray(UART1_HW, buffer, size);
}


/*******************************************************************************
* Function Name: UART1_GetArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_GetArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_UART_GetArrayBlocking(UART1_HW, buffer, size);
}


/*******************************************************************************
* Function Name: UART1_GetRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetRxFifoStatus(void)
{
    return Cy_SCB_UART_GetRxFifoStatus(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_ClearRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_ClearRxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_UART_ClearRxFifoStatus(UART1_HW, clearMask);
}


/*******************************************************************************
* Function Name: UART1_GetNumInRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetNumInRxFifo(void)
{
    return Cy_SCB_UART_GetNumInRxFifo(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_ClearRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_ClearRxFifo(void)
{
    Cy_SCB_UART_ClearRxFifo(UART1_HW);
}
#endif /* (UART1_ENABLE_RX) */


#if (UART1_ENABLE_TX)
/*******************************************************************************
* Function Name: UART1_Put
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Put() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_Put(uint32_t data)
{
    return Cy_SCB_UART_Put(UART1_HW,data);
}


/*******************************************************************************
* Function Name: UART1_PutArray
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_PutArray(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_PutArray(UART1_HW, buffer, size);
}


/*******************************************************************************
* Function Name: UART1_PutArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_PutArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_UART_PutArrayBlocking(UART1_HW, buffer, size);
}


/*******************************************************************************
* Function Name: UART1_PutString
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutString() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_PutString(char_t const string[])
{
    Cy_SCB_UART_PutString(UART1_HW, string);
}


/*******************************************************************************
* Function Name: UART1_SendBreakBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_SendBreakBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_SendBreakBlocking(uint32_t breakWidth)
{
    Cy_SCB_UART_SendBreakBlocking(UART1_HW, breakWidth);
}


/*******************************************************************************
* Function Name: UART1_GetTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetTxFifoStatus(void)
{
    return Cy_SCB_UART_GetTxFifoStatus(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_ClearTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_ClearTxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_UART_ClearTxFifoStatus(UART1_HW, clearMask);
}


/*******************************************************************************
* Function Name: UART1_GetNumInTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetNumInTxFifo(void)
{
    return Cy_SCB_UART_GetNumInTxFifo(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_IsTxComplete
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_IsTxComplete() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool UART1_IsTxComplete(void)
{
    return Cy_SCB_UART_IsTxComplete(UART1_HW);
}


/*******************************************************************************
* Function Name: UART1_ClearTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_ClearTxFifo(void)
{
    Cy_SCB_UART_ClearTxFifo(UART1_HW);
}
#endif /* (UART1_ENABLE_TX) */


#if (UART1_ENABLE_RX)
/*******************************************************************************
* Function Name: UART1_StartRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_StartRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_StartRingBuffer(void *buffer, uint32_t size)
{
    Cy_SCB_UART_StartRingBuffer(UART1_HW, buffer, size, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_StopRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_StopRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_StopRingBuffer(void)
{
    Cy_SCB_UART_StopRingBuffer(UART1_HW, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_ClearRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_ClearRingBuffer(void)
{
    Cy_SCB_UART_ClearRingBuffer(UART1_HW, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_GetNumInRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetNumInRingBuffer(void)
{
    return Cy_SCB_UART_GetNumInRingBuffer(UART1_HW, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_Receive
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Receive() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t UART1_Receive(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_Receive(UART1_HW, buffer, size, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_GetReceiveStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetReceiveStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetReceiveStatus(void)
{
    return Cy_SCB_UART_GetReceiveStatus(UART1_HW, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_AbortReceive
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_AbortReceive() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_AbortReceive(void)
{
    Cy_SCB_UART_AbortReceive(UART1_HW, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_GetNumReceived
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumReceived() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetNumReceived(void)
{
    return Cy_SCB_UART_GetNumReceived(UART1_HW, &UART1_context);
}
#endif /* (UART1_ENABLE_RX) */


#if (UART1_ENABLE_TX)
/*******************************************************************************
* Function Name: UART1_Transmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Transmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t UART1_Transmit(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_Transmit(UART1_HW, buffer, size, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_GetTransmitStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetTransmitStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetTransmitStatus(void)
{
    return Cy_SCB_UART_GetTransmitStatus(UART1_HW, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_AbortTransmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_AbortTransmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_AbortTransmit(void)
{
    Cy_SCB_UART_AbortTransmit(UART1_HW, &UART1_context);
}


/*******************************************************************************
* Function Name: UART1_GetNumLeftToTransmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumLeftToTransmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t UART1_GetNumLeftToTransmit(void)
{
    return Cy_SCB_UART_GetNumLeftToTransmit(UART1_HW, &UART1_context);
}
#endif /* (UART1_ENABLE_TX) */


/*******************************************************************************
* Function Name: UART1_Interrupt
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Interrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void UART1_Interrupt(void)
{
    Cy_SCB_UART_Interrupt(UART1_HW, &UART1_context);
}

#if defined(__cplusplus)
}
#endif

#endif /* UART1_CY_SCB_UART_PDL_H */


/* [] END OF FILE */
