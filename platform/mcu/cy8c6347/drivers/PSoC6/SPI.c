/***************************************************************************//**
* \file SPI.c
* \version 2.0
*
*  This file provides the source code to the API for the SPI Component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SPI.h"
#include "sysint/cy_sysint.h"
#include "cyfitter_sysint.h"
#include "cyfitter_sysint_cfg.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*     Global variables
***************************************/

/** SPI_initVar indicates whether the SPI
*  component has been initialized. The variable is initialized to 0
*  and set to 1 the first time SPI_Start() is called.
*  This allows  the component to restart without reinitialization
*  after the first call to the SPI_Start() routine.
*
*  If re-initialization of the component is required, then the
*  SPI_Init() function can be called before the
*  SPI_Start() or SPI_Enable() function.
*/
uint8_t SPI_initVar = 0U;

/** The instance-specific configuration structure.
* The pointer to this structure should be passed to Cy_SCB_SPI_Init function
* to initialize component with GUI selected settings.
*/
cy_stc_scb_spi_config_t const SPI_config =
{
    .spiMode  = CY_SCB_SPI_MASTER,
    .subMode  = CY_SCB_SPI_MOTOROLA,
    .sclkMode = CY_SCB_SPI_CPHA0_CPOL0,

    .oversample = 16UL,

    .rxDataWidth              = 4UL,
    .txDataWidth              = 4UL,
    .enableMsbFirst           = true,
    .enableInputFilter        = false,

    .enableFreeRunSclk        = false,
    .enableMisoLateSample     = false,
    .enableTransferSeperation = false,
    .ssPolarity               = ((((uint32_t) CY_SCB_SPI_ACTIVE_LOW) << SPI_SPI_SLAVE_SELECT0) | \
                                 (((uint32_t) CY_SCB_SPI_ACTIVE_LOW) << SPI_SPI_SLAVE_SELECT1) | \
                                 (((uint32_t) CY_SCB_SPI_ACTIVE_LOW) << SPI_SPI_SLAVE_SELECT2) | \
                                 (((uint32_t) CY_SCB_SPI_ACTIVE_LOW) << SPI_SPI_SLAVE_SELECT3)),

    .enableWakeFromSleep = false,

    .rxFifoTriggerLevel  = 0UL,
    .rxFifoIntEnableMask = 0x0UL,

    .txFifoTriggerLevel  = 0UL,
    .txFifoIntEnableMask = 0x0UL,

    .masterSlaveIntEnableMask = 0x0UL
};

/** The instance-specific context structure.
* It is used while the driver operation for internal configuration and
* data keeping for the SPI. The user should not modify anything in this 
* structure.
*/
cy_stc_scb_spi_context_t SPI_context;


/*******************************************************************************
* Function Name: SPI_Start
****************************************************************************//**
*
* Invokes SPI_Init() and SPI_Enable().
* Also configures interrupt if it is internal.
* After this function call the component is enabled and ready for operation.
* This is the preferred method to begin component operation.
*
* \globalvars
* \ref SPI_initVar - used to check initial configuration,
* modified  on first function call.
*
*******************************************************************************/
void SPI_Start(void)
{
    if (0U == SPI_initVar)
    {
        /* Configure component */
        (void) Cy_SCB_SPI_Init(SPI_HW, &SPI_config, &SPI_context);

        /* Set active slave select to line 0 */
        Cy_SCB_SPI_SetActiveSlaveSelect(SPI_HW, SPI_SPI_SLAVE_SELECT0);

        /* Hook interrupt service routine */
    #if defined(SPI_SCB_IRQ__INTC_ASSIGNED)
        (void) Cy_SysInt_Init(&SPI_SCB_IRQ_cfg, &SPI_Interrupt);
    #endif /* (SPI_SCB_IRQ__INTC_ASSIGNED) */

        /* Component is configured */
        SPI_initVar = 1U;
    }

    /* Enable interrupt in NVIC */
#if defined(SPI_SCB_IRQ__INTC_ASSIGNED)
    NVIC_EnableIRQ((IRQn_Type) SPI_SCB_IRQ_cfg.intrSrc);
#endif /* (SPI_SCB_IRQ__INTC_ASSIGNED) */

    Cy_SCB_SPI_Enable(SPI_HW);
}

#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */
