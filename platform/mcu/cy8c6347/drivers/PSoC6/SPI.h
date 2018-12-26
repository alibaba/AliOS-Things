/***************************************************************************//**
* \file SPI.h
* \version 2.0
*
*  This file provides constants and parameter values for the SPI component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(SPI_CY_SCB_SPI_PDL_H)
#define SPI_CY_SCB_SPI_PDL_H

#include "cyfitter.h"
#include "scb/cy_scb_spi.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
/* Component specific functions. */
void SPI_Start(void);

/* Basic functions. */
__STATIC_INLINE cy_en_scb_spi_status_t SPI_Init(cy_stc_scb_spi_config_t const *config);
__STATIC_INLINE void SPI_DeInit(void);
__STATIC_INLINE void SPI_Enable(void);
__STATIC_INLINE void SPI_Disable(void);

/* Register callback. */
__STATIC_INLINE void SPI_RegisterCallback(cy_cb_scb_spi_handle_events_t callback);

/* Bus state. */
__STATIC_INLINE bool SPI_IsBusBusy(void);

/* Slave select control. */
__STATIC_INLINE void SPI_SetActiveSlaveSelect(cy_en_scb_spi_slave_select_t slaveSelect);
__STATIC_INLINE void SPI_SetActiveSlaveSelectPolarity(cy_en_scb_spi_slave_select_t slaveSelect, 
                                                                   cy_en_scb_spi_polarity_t polarity);

/* Low level: read. */
__STATIC_INLINE uint32_t SPI_Read(void);
__STATIC_INLINE uint32_t SPI_ReadArray(void *buffer, uint32_t size);
__STATIC_INLINE uint32_t SPI_GetRxFifoStatus(void);
__STATIC_INLINE void     SPI_ClearRxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t SPI_GetNumInRxFifo(void);
__STATIC_INLINE void     SPI_ClearRxFifo(void);

/* Low level: write. */
__STATIC_INLINE uint32_t SPI_Write(uint32_t data);
__STATIC_INLINE uint32_t SPI_WriteArray(void *buffer, uint32_t size);
__STATIC_INLINE void     SPI_WriteArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE uint32_t SPI_GetTxFifoStatus(void);
__STATIC_INLINE void     SPI_ClearTxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t SPI_GetNumInTxFifo(void);
__STATIC_INLINE bool     SPI_IsTxComplete(void);
__STATIC_INLINE void     SPI_ClearTxFifo(void);

/* Master/slave specific status. */
__STATIC_INLINE uint32_t SPI_GetSlaveMasterStatus(void);
__STATIC_INLINE void     SPI_ClearSlaveMasterStatus(uint32_t clearMask);

/* High level: transfer functions. */
__STATIC_INLINE cy_en_scb_spi_status_t SPI_Transfer(void *txBuffer, void *rxBuffer, uint32_t size);
__STATIC_INLINE void     SPI_AbortTransfer(void);
__STATIC_INLINE uint32_t SPI_GetTransferStatus(void);
__STATIC_INLINE uint32_t SPI_GetNumTransfered(void);

/* Interrupt handler */
__STATIC_INLINE void SPI_Interrupt(void);
/** @} group_general */


/***************************************
*    Variables with External Linkage
***************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t SPI_initVar;
extern cy_stc_scb_spi_config_t const SPI_config;
extern cy_stc_scb_spi_context_t SPI_context;
/** @} group_globals */


/***************************************
*         Preprocessor Macros
***************************************/
/**
* \addtogroup group_macros
* @{
*/
/** The pointer to the base address of the hardware */
#define SPI_HW     ((CySCB_Type *) SPI_SCB__HW)

/** The slave select line 0 constant which takes into account pin placement */
#define SPI_SPI_SLAVE_SELECT0    ( (cy_en_scb_spi_slave_select_t) SPI_SCB__SS0_POSITION)

/** The slave select line 1 constant which takes into account pin placement */
#define SPI_SPI_SLAVE_SELECT1    ( (cy_en_scb_spi_slave_select_t) SPI_SCB__SS1_POSITION)

/** The slave select line 2 constant which takes into account pin placement */
#define SPI_SPI_SLAVE_SELECT2    ( (cy_en_scb_spi_slave_select_t) SPI_SCB__SS2_POSITION)

/** The slave select line 3 constant which takes into account pin placement */
#define SPI_SPI_SLAVE_SELECT3    ((cy_en_scb_spi_slave_select_t) SPI_SCB__SS3_POSITION)
/** @} group_macros */


/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: SPI_Init
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_spi_status_t SPI_Init(cy_stc_scb_spi_config_t const *config)
{
    return Cy_SCB_SPI_Init(SPI_HW, config, &SPI_context);
}


/*******************************************************************************
* Function Name: SPI_DeInit
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_DeInit(void)
{
    Cy_SCB_SPI_DeInit(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_Enable
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Enable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_Enable(void)
{
    Cy_SCB_SPI_Enable(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_Disable
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Disable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_Disable(void)
{
    Cy_SCB_SPI_Disable(SPI_HW, &SPI_context);
}


/*******************************************************************************
* Function Name: SPI_RegisterCallback
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_RegisterCallback() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_RegisterCallback(cy_cb_scb_spi_handle_events_t callback)
{
    Cy_SCB_SPI_RegisterCallback(SPI_HW, callback, &SPI_context);
}


/*******************************************************************************
* Function Name: SPI_IsBusBusy
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_IsBusBusy() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool SPI_IsBusBusy(void)
{
    return Cy_SCB_SPI_IsBusBusy(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_SetActiveSlaveSelect
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_SetActiveSlaveSelect() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_SetActiveSlaveSelect(cy_en_scb_spi_slave_select_t slaveSelect)
{
    Cy_SCB_SPI_SetActiveSlaveSelect(SPI_HW, slaveSelect);
}


/*******************************************************************************
* Function Name: SPI_SetActiveSlaveSelectPolarity
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_SetActiveSlaveSelectPolarity() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_SetActiveSlaveSelectPolarity(cy_en_scb_spi_slave_select_t slaveSelect, 
                                                                   cy_en_scb_spi_polarity_t polarity)
{
    Cy_SCB_SPI_SetActiveSlaveSelectPolarity(SPI_HW, slaveSelect, polarity);
}


/*******************************************************************************
* Function Name: SPI_Read
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Read() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_Read(void)
{
    return Cy_SCB_SPI_Read(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_ReadArray
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ReadArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_ReadArray(void *buffer, uint32_t size)
{
    return Cy_SCB_SPI_ReadArray(SPI_HW, buffer, size);
}


/*******************************************************************************
* Function Name: SPI_GetRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_GetRxFifoStatus(void)
{
    return Cy_SCB_SPI_GetRxFifoStatus(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_ClearRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_ClearRxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_SPI_ClearRxFifoStatus(SPI_HW, clearMask);
}


/*******************************************************************************
* Function Name: SPI_GetNumInRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_GetNumInRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_GetNumInRxFifo(void)
{
    return Cy_SCB_GetNumInRxFifo(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_ClearRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_ClearRxFifo(void)
{
    Cy_SCB_SPI_ClearRxFifo(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_Write
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Write() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_Write(uint32_t data)
{
    return Cy_SCB_SPI_Write(SPI_HW, data);
}


/*******************************************************************************
* Function Name: SPI_WriteArray
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_WriteArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_WriteArray(void *buffer, uint32_t size)
{
    return Cy_SCB_SPI_WriteArray(SPI_HW, buffer, size);
}


/*******************************************************************************
* Function Name: SPI_WriteArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_WriteArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_WriteArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_SPI_WriteArrayBlocking(SPI_HW, buffer, size);
}


/*******************************************************************************
* Function Name: SPI_GetTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_GetTxFifoStatus(void)
{
    return Cy_SCB_SPI_GetTxFifoStatus(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_ClearTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_ClearTxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_SPI_ClearTxFifoStatus(SPI_HW, clearMask);
}


/*******************************************************************************
* Function Name: SPI_GetNumInTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetNumInTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_GetNumInTxFifo(void)
{
    return Cy_SCB_SPI_GetNumInTxFifo(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_IsTxComplete
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_IsTxComplete() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool SPI_IsTxComplete(void)
{
    return Cy_SCB_SPI_IsTxComplete(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_ClearTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_ClearTxFifo(void)
{
    Cy_SCB_SPI_ClearTxFifo(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_GetSlaveMasterStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetSlaveMasterStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_GetSlaveMasterStatus(void)
{
    return Cy_SCB_SPI_GetSlaveMasterStatus(SPI_HW);
}


/*******************************************************************************
* Function Name: SPI_ClearSlaveMasterStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_ClearSlaveMasterStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_ClearSlaveMasterStatus(uint32_t clearMask)
{
    Cy_SCB_SPI_ClearSlaveMasterStatus(SPI_HW, clearMask);
}


/*******************************************************************************
* Function Name: SPI_Transfer
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Transfer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_spi_status_t SPI_Transfer(void *txBuffer, void *rxBuffer, uint32_t size)
{
    return Cy_SCB_SPI_Transfer(SPI_HW, txBuffer, rxBuffer, size, &SPI_context);
}

/*******************************************************************************
* Function Name: SPI_AbortTransfer
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_AbortTransfer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_AbortTransfer(void)
{
    Cy_SCB_SPI_AbortTransfer(SPI_HW, &SPI_context);
}


/*******************************************************************************
* Function Name: SPI_GetTransferStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetTransferStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_GetTransferStatus(void)
{
    return Cy_SCB_SPI_GetTransferStatus(SPI_HW, &SPI_context);
}


/*******************************************************************************
* Function Name: SPI_GetNumTransfered
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_GetNumTransfered() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t SPI_GetNumTransfered(void)
{
    return Cy_SCB_SPI_GetNumTransfered(SPI_HW, &SPI_context);
}


/*******************************************************************************
* Function Name: SPI_Interrupt
****************************************************************************//**
*
* Invokes the Cy_SCB_SPI_Interrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void SPI_Interrupt(void)
{
    Cy_SCB_SPI_Interrupt(SPI_HW, &SPI_context);
}


#if defined(__cplusplus)
}
#endif

#endif /* SPI_CY_SCB_SPI_PDL_H */


/* [] END OF FILE */
