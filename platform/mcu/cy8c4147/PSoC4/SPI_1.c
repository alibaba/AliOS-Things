/***************************************************************************//**
* \file SPI_1.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SPI_1_PVT.h"

#if (SPI_1_SCB_MODE_I2C_INC)
    #include "SPI_1_I2C_PVT.h"
#endif /* (SPI_1_SCB_MODE_I2C_INC) */

#if (SPI_1_SCB_MODE_EZI2C_INC)
    #include "SPI_1_EZI2C_PVT.h"
#endif /* (SPI_1_SCB_MODE_EZI2C_INC) */

#if (SPI_1_SCB_MODE_SPI_INC || SPI_1_SCB_MODE_UART_INC)
    #include "SPI_1_SPI_UART_PVT.h"
#endif /* (SPI_1_SCB_MODE_SPI_INC || SPI_1_SCB_MODE_UART_INC) */


/***************************************
*    Run Time Configuration Vars
***************************************/

/* Stores internal component configuration for Unconfigured mode */
#if (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    uint8 SPI_1_scbMode = SPI_1_SCB_MODE_UNCONFIG;
    uint8 SPI_1_scbEnableWake;
    uint8 SPI_1_scbEnableIntr;

    /* I2C configuration variables */
    uint8 SPI_1_mode;
    uint8 SPI_1_acceptAddr;

    /* SPI/UART configuration variables */
    volatile uint8 * SPI_1_rxBuffer;
    uint8  SPI_1_rxDataBits;
    uint32 SPI_1_rxBufferSize;

    volatile uint8 * SPI_1_txBuffer;
    uint8  SPI_1_txDataBits;
    uint32 SPI_1_txBufferSize;

    /* EZI2C configuration variables */
    uint8 SPI_1_numberOfAddr;
    uint8 SPI_1_subAddrSize;
#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Common SCB Vars
***************************************/
/**
* \addtogroup group_general
* \{
*/

/** SPI_1_initVar indicates whether the SPI_1 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time SCB_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the SPI_1_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  SPI_1_Init() function can be called before the 
*  SPI_1_Start() or SPI_1_Enable() function.
*/
uint8 SPI_1_initVar = 0u;


#if (! (SPI_1_SCB_MODE_I2C_CONST_CFG || \
        SPI_1_SCB_MODE_EZI2C_CONST_CFG))
    /** This global variable stores TX interrupt sources after 
    * SPI_1_Stop() is called. Only these TX interrupt sources 
    * will be restored on a subsequent SPI_1_Enable() call.
    */
    uint16 SPI_1_IntrTxMask = 0u;
#endif /* (! (SPI_1_SCB_MODE_I2C_CONST_CFG || \
              SPI_1_SCB_MODE_EZI2C_CONST_CFG)) */
/** \} globals */

#if (SPI_1_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_SPI_1_CUSTOM_INTR_HANDLER)
    void (*SPI_1_customIntrHandler)(void) = NULL;
#endif /* !defined (CY_REMOVE_SPI_1_CUSTOM_INTR_HANDLER) */
#endif /* (SPI_1_SCB_IRQ_INTERNAL) */


/***************************************
*    Private Function Prototypes
***************************************/

static void SPI_1_ScbEnableIntr(void);
static void SPI_1_ScbModeStop(void);
static void SPI_1_ScbModePostEnable(void);


/*******************************************************************************
* Function Name: SPI_1_Init
****************************************************************************//**
*
*  Initializes the SPI_1 component to operate in one of the selected
*  configurations: I2C, SPI, UART or EZI2C.
*  When the configuration is set to "Unconfigured SCB", this function does
*  not do any initialization. Use mode-specific initialization APIs instead:
*  SPI_1_I2CInit, SPI_1_SpiInit, 
*  SPI_1_UartInit or SPI_1_EzI2CInit.
*
*******************************************************************************/
void SPI_1_Init(void)
{
#if (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
    if (SPI_1_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        SPI_1_initVar = 0u;
    }
    else
    {
        /* Initialization was done before this function call */
    }

#elif (SPI_1_SCB_MODE_I2C_CONST_CFG)
    SPI_1_I2CInit();

#elif (SPI_1_SCB_MODE_SPI_CONST_CFG)
    SPI_1_SpiInit();

#elif (SPI_1_SCB_MODE_UART_CONST_CFG)
    SPI_1_UartInit();

#elif (SPI_1_SCB_MODE_EZI2C_CONST_CFG)
    SPI_1_EzI2CInit();

#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SPI_1_Enable
****************************************************************************//**
*
*  Enables SPI_1 component operation: activates the hardware and 
*  internal interrupt. It also restores TX interrupt sources disabled after the 
*  SPI_1_Stop() function was called (note that level-triggered TX 
*  interrupt sources remain disabled to not cause code lock-up).
*  For I2C and EZI2C modes the interrupt is internal and mandatory for 
*  operation. For SPI and UART modes the interrupt can be configured as none, 
*  internal or external.
*  The SPI_1 configuration should be not changed when the component
*  is enabled. Any configuration changes should be made after disabling the 
*  component.
*  When configuration is set to “Unconfigured SPI_1”, the component 
*  must first be initialized to operate in one of the following configurations: 
*  I2C, SPI, UART or EZ I2C, using the mode-specific initialization API. 
*  Otherwise this function does not enable the component.
*
*******************************************************************************/
void SPI_1_Enable(void)
{
#if (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Enable SCB block, only if it is already configured */
    if (!SPI_1_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        SPI_1_CTRL_REG |= SPI_1_CTRL_ENABLED;

        SPI_1_ScbEnableIntr();

        /* Call PostEnable function specific to current operation mode */
        SPI_1_ScbModePostEnable();
    }
#else
    SPI_1_CTRL_REG |= SPI_1_CTRL_ENABLED;

    SPI_1_ScbEnableIntr();

    /* Call PostEnable function specific to current operation mode */
    SPI_1_ScbModePostEnable();
#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SPI_1_Start
****************************************************************************//**
*
*  Invokes SPI_1_Init() and SPI_1_Enable().
*  After this function call, the component is enabled and ready for operation.
*  When configuration is set to "Unconfigured SCB", the component must first be
*  initialized to operate in one of the following configurations: I2C, SPI, UART
*  or EZI2C. Otherwise this function does not enable the component.
*
* \globalvars
*  SPI_1_initVar - used to check initial configuration, modified
*  on first function call.
*
*******************************************************************************/
void SPI_1_Start(void)
{
    if (0u == SPI_1_initVar)
    {
        SPI_1_Init();
        SPI_1_initVar = 1u; /* Component was initialized */
    }

    SPI_1_Enable();
}


/*******************************************************************************
* Function Name: SPI_1_Stop
****************************************************************************//**
*
*  Disables the SPI_1 component: disable the hardware and internal 
*  interrupt. It also disables all TX interrupt sources so as not to cause an 
*  unexpected interrupt trigger because after the component is enabled, the 
*  TX FIFO is empty.
*  Refer to the function SPI_1_Enable() for the interrupt 
*  configuration details.
*  This function disables the SCB component without checking to see if 
*  communication is in progress. Before calling this function it may be 
*  necessary to check the status of communication to make sure communication 
*  is complete. If this is not done then communication could be stopped mid 
*  byte and corrupted data could result.
*
*******************************************************************************/
void SPI_1_Stop(void)
{
#if (SPI_1_SCB_IRQ_INTERNAL)
    SPI_1_DisableInt();
#endif /* (SPI_1_SCB_IRQ_INTERNAL) */

    /* Call Stop function specific to current operation mode */
    SPI_1_ScbModeStop();

    /* Disable SCB IP */
    SPI_1_CTRL_REG &= (uint32) ~SPI_1_CTRL_ENABLED;

    /* Disable all TX interrupt sources so as not to cause an unexpected
    * interrupt trigger after the component will be enabled because the 
    * TX FIFO is empty.
    * For SCB IP v0, it is critical as it does not mask-out interrupt
    * sources when it is disabled. This can cause a code lock-up in the
    * interrupt handler because TX FIFO cannot be loaded after the block
    * is disabled.
    */
    SPI_1_SetTxInterruptMode(SPI_1_NO_INTR_SOURCES);

#if (SPI_1_SCB_IRQ_INTERNAL)
    SPI_1_ClearPendingInt();
#endif /* (SPI_1_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: SPI_1_SetRxFifoLevel
****************************************************************************//**
*
*  Sets level in the RX FIFO to generate a RX level interrupt.
*  When the RX FIFO has more entries than the RX FIFO level an RX level
*  interrupt request is generated.
*
*  \param level: Level in the RX FIFO to generate RX level interrupt.
*   The range of valid level values is between 0 and RX FIFO depth - 1.
*
*******************************************************************************/
void SPI_1_SetRxFifoLevel(uint32 level)
{
    uint32 rxFifoCtrl;

    rxFifoCtrl = SPI_1_RX_FIFO_CTRL_REG;

    rxFifoCtrl &= ((uint32) ~SPI_1_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    rxFifoCtrl |= ((uint32) (SPI_1_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    SPI_1_RX_FIFO_CTRL_REG = rxFifoCtrl;
}


/*******************************************************************************
* Function Name: SPI_1_SetTxFifoLevel
****************************************************************************//**
*
*  Sets level in the TX FIFO to generate a TX level interrupt.
*  When the TX FIFO has less entries than the TX FIFO level an TX level
*  interrupt request is generated.
*
*  \param level: Level in the TX FIFO to generate TX level interrupt.
*   The range of valid level values is between 0 and TX FIFO depth - 1.
*
*******************************************************************************/
void SPI_1_SetTxFifoLevel(uint32 level)
{
    uint32 txFifoCtrl;

    txFifoCtrl = SPI_1_TX_FIFO_CTRL_REG;

    txFifoCtrl &= ((uint32) ~SPI_1_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    txFifoCtrl |= ((uint32) (SPI_1_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    SPI_1_TX_FIFO_CTRL_REG = txFifoCtrl;
}


#if (SPI_1_SCB_IRQ_INTERNAL)
    /*******************************************************************************
    * Function Name: SPI_1_SetCustomInterruptHandler
    ****************************************************************************//**
    *
    *  Registers a function to be called by the internal interrupt handler.
    *  First the function that is registered is called, then the internal interrupt
    *  handler performs any operation such as software buffer management functions
    *  before the interrupt returns.  It is the user's responsibility not to break
    *  the software buffer operations. Only one custom handler is supported, which
    *  is the function provided by the most recent call.
    *  At the initialization time no custom handler is registered.
    *
    *  \param func: Pointer to the function to register.
    *        The value NULL indicates to remove the current custom interrupt
    *        handler.
    *
    *******************************************************************************/
    void SPI_1_SetCustomInterruptHandler(void (*func)(void))
    {
    #if !defined (CY_REMOVE_SPI_1_CUSTOM_INTR_HANDLER)
        SPI_1_customIntrHandler = func; /* Register interrupt handler */
    #else
        if (NULL != func)
        {
            /* Suppress compiler warning */
        }
    #endif /* !defined (CY_REMOVE_SPI_1_CUSTOM_INTR_HANDLER) */
    }
#endif /* (SPI_1_SCB_IRQ_INTERNAL) */


/*******************************************************************************
* Function Name: SPI_1_ScbModeEnableIntr
****************************************************************************//**
*
*  Enables an interrupt for a specific mode.
*
*******************************************************************************/
static void SPI_1_ScbEnableIntr(void)
{
#if (SPI_1_SCB_IRQ_INTERNAL)
    /* Enable interrupt in NVIC */
    #if (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
        if (0u != SPI_1_scbEnableIntr)
        {
            SPI_1_EnableInt();
        }

    #else
        SPI_1_EnableInt();

    #endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */
#endif /* (SPI_1_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: SPI_1_ScbModePostEnable
****************************************************************************//**
*
*  Calls the PostEnable function for a specific operation mode.
*
*******************************************************************************/
static void SPI_1_ScbModePostEnable(void)
{
#if (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
#if (!SPI_1_CY_SCBIP_V1)
    if (SPI_1_SCB_MODE_SPI_RUNTM_CFG)
    {
        SPI_1_SpiPostEnable();
    }
    else if (SPI_1_SCB_MODE_UART_RUNTM_CFG)
    {
        SPI_1_UartPostEnable();
    }
    else
    {
        /* Unknown mode: do nothing */
    }
#endif /* (!SPI_1_CY_SCBIP_V1) */

#elif (SPI_1_SCB_MODE_SPI_CONST_CFG)
    SPI_1_SpiPostEnable();

#elif (SPI_1_SCB_MODE_UART_CONST_CFG)
    SPI_1_UartPostEnable();

#else
    /* Unknown mode: do nothing */
#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SPI_1_ScbModeStop
****************************************************************************//**
*
*  Calls the Stop function for a specific operation mode.
*
*******************************************************************************/
static void SPI_1_ScbModeStop(void)
{
#if (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
    if (SPI_1_SCB_MODE_I2C_RUNTM_CFG)
    {
        SPI_1_I2CStop();
    }
    else if (SPI_1_SCB_MODE_EZI2C_RUNTM_CFG)
    {
        SPI_1_EzI2CStop();
    }
#if (!SPI_1_CY_SCBIP_V1)
    else if (SPI_1_SCB_MODE_SPI_RUNTM_CFG)
    {
        SPI_1_SpiStop();
    }
    else if (SPI_1_SCB_MODE_UART_RUNTM_CFG)
    {
        SPI_1_UartStop();
    }
#endif /* (!SPI_1_CY_SCBIP_V1) */
    else
    {
        /* Unknown mode: do nothing */
    }
#elif (SPI_1_SCB_MODE_I2C_CONST_CFG)
    SPI_1_I2CStop();

#elif (SPI_1_SCB_MODE_EZI2C_CONST_CFG)
    SPI_1_EzI2CStop();

#elif (SPI_1_SCB_MODE_SPI_CONST_CFG)
    SPI_1_SpiStop();

#elif (SPI_1_SCB_MODE_UART_CONST_CFG)
    SPI_1_UartStop();

#else
    /* Unknown mode: do nothing */
#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: SPI_1_SetPins
    ****************************************************************************//**
    *
    *  Sets the pins settings accordingly to the selected operation mode.
    *  Only available in the Unconfigured operation mode. The mode specific
    *  initialization function calls it.
    *  Pins configuration is set by PSoC Creator when a specific mode of operation
    *  is selected in design time.
    *
    *  \param mode:      Mode of SCB operation.
    *  \param subMode:   Sub-mode of SCB operation. It is only required for SPI and UART
    *             modes.
    *  \param uartEnableMask: enables TX or RX direction and RTS and CTS signals.
    *
    *******************************************************************************/
    void SPI_1_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask)
    {
        uint32 pinsDm[SPI_1_SCB_PINS_NUMBER];
        uint32 i;
        
    #if (!SPI_1_CY_SCBIP_V1)
        uint32 pinsInBuf = 0u;
    #endif /* (!SPI_1_CY_SCBIP_V1) */
        
        uint32 hsiomSel[SPI_1_SCB_PINS_NUMBER] = 
        {
            SPI_1_RX_SCL_MOSI_HSIOM_SEL_GPIO,
            SPI_1_TX_SDA_MISO_HSIOM_SEL_GPIO,
            0u,
            0u,
            0u,
            0u,
            0u,
        };

    #if (SPI_1_CY_SCBIP_V1)
        /* Supress compiler warning. */
        if ((0u == subMode) || (0u == uartEnableMask))
        {
        }
    #endif /* (SPI_1_CY_SCBIP_V1) */

        /* Set default HSIOM to GPIO and Drive Mode to Analog Hi-Z */
        for (i = 0u; i < SPI_1_SCB_PINS_NUMBER; i++)
        {
            pinsDm[i] = SPI_1_PIN_DM_ALG_HIZ;
        }

        if ((SPI_1_SCB_MODE_I2C   == mode) ||
            (SPI_1_SCB_MODE_EZI2C == mode))
        {
        #if (SPI_1_RX_SCL_MOSI_PIN)
            hsiomSel[SPI_1_RX_SCL_MOSI_PIN_INDEX] = SPI_1_RX_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [SPI_1_RX_SCL_MOSI_PIN_INDEX] = SPI_1_PIN_DM_OD_LO;
        #elif (SPI_1_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[SPI_1_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_1_RX_WAKE_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [SPI_1_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_1_PIN_DM_OD_LO;
        #else
        #endif /* (SPI_1_RX_SCL_MOSI_PIN) */
        
        #if (SPI_1_TX_SDA_MISO_PIN)
            hsiomSel[SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_TX_SDA_MISO_HSIOM_SEL_I2C;
            pinsDm  [SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_PIN_DM_OD_LO;
        #endif /* (SPI_1_TX_SDA_MISO_PIN) */
        }
    #if (!SPI_1_CY_SCBIP_V1)
        else if (SPI_1_SCB_MODE_SPI == mode)
        {
        #if (SPI_1_RX_SCL_MOSI_PIN)
            hsiomSel[SPI_1_RX_SCL_MOSI_PIN_INDEX] = SPI_1_RX_SCL_MOSI_HSIOM_SEL_SPI;
        #elif (SPI_1_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[SPI_1_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_1_RX_WAKE_SCL_MOSI_HSIOM_SEL_SPI;
        #else
        #endif /* (SPI_1_RX_SCL_MOSI_PIN) */
        
        #if (SPI_1_TX_SDA_MISO_PIN)
            hsiomSel[SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_TX_SDA_MISO_HSIOM_SEL_SPI;
        #endif /* (SPI_1_TX_SDA_MISO_PIN) */
        
        #if (SPI_1_CTS_SCLK_PIN)
            hsiomSel[SPI_1_CTS_SCLK_PIN_INDEX] = SPI_1_CTS_SCLK_HSIOM_SEL_SPI;
        #endif /* (SPI_1_CTS_SCLK_PIN) */

            if (SPI_1_SPI_SLAVE == subMode)
            {
                /* Slave */
                pinsDm[SPI_1_RX_SCL_MOSI_PIN_INDEX] = SPI_1_PIN_DM_DIG_HIZ;
                pinsDm[SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_PIN_DM_STRONG;
                pinsDm[SPI_1_CTS_SCLK_PIN_INDEX] = SPI_1_PIN_DM_DIG_HIZ;

            #if (SPI_1_RTS_SS0_PIN)
                /* Only SS0 is valid choice for Slave */
                hsiomSel[SPI_1_RTS_SS0_PIN_INDEX] = SPI_1_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm  [SPI_1_RTS_SS0_PIN_INDEX] = SPI_1_PIN_DM_DIG_HIZ;
            #endif /* (SPI_1_RTS_SS0_PIN) */

            #if (SPI_1_TX_SDA_MISO_PIN)
                /* Disable input buffer */
                 pinsInBuf |= SPI_1_TX_SDA_MISO_PIN_MASK;
            #endif /* (SPI_1_TX_SDA_MISO_PIN) */
            }
            else 
            {
                /* (Master) */
                pinsDm[SPI_1_RX_SCL_MOSI_PIN_INDEX] = SPI_1_PIN_DM_STRONG;
                pinsDm[SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_PIN_DM_DIG_HIZ;
                pinsDm[SPI_1_CTS_SCLK_PIN_INDEX] = SPI_1_PIN_DM_STRONG;

            #if (SPI_1_RTS_SS0_PIN)
                hsiomSel [SPI_1_RTS_SS0_PIN_INDEX] = SPI_1_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm   [SPI_1_RTS_SS0_PIN_INDEX] = SPI_1_PIN_DM_STRONG;
                pinsInBuf |= SPI_1_RTS_SS0_PIN_MASK;
            #endif /* (SPI_1_RTS_SS0_PIN) */

            #if (SPI_1_SS1_PIN)
                hsiomSel [SPI_1_SS1_PIN_INDEX] = SPI_1_SS1_HSIOM_SEL_SPI;
                pinsDm   [SPI_1_SS1_PIN_INDEX] = SPI_1_PIN_DM_STRONG;
                pinsInBuf |= SPI_1_SS1_PIN_MASK;
            #endif /* (SPI_1_SS1_PIN) */

            #if (SPI_1_SS2_PIN)
                hsiomSel [SPI_1_SS2_PIN_INDEX] = SPI_1_SS2_HSIOM_SEL_SPI;
                pinsDm   [SPI_1_SS2_PIN_INDEX] = SPI_1_PIN_DM_STRONG;
                pinsInBuf |= SPI_1_SS2_PIN_MASK;
            #endif /* (SPI_1_SS2_PIN) */

            #if (SPI_1_SS3_PIN)
                hsiomSel [SPI_1_SS3_PIN_INDEX] = SPI_1_SS3_HSIOM_SEL_SPI;
                pinsDm   [SPI_1_SS3_PIN_INDEX] = SPI_1_PIN_DM_STRONG;
                pinsInBuf |= SPI_1_SS3_PIN_MASK;
            #endif /* (SPI_1_SS3_PIN) */

                /* Disable input buffers */
            #if (SPI_1_RX_SCL_MOSI_PIN)
                pinsInBuf |= SPI_1_RX_SCL_MOSI_PIN_MASK;
            #elif (SPI_1_RX_WAKE_SCL_MOSI_PIN)
                pinsInBuf |= SPI_1_RX_WAKE_SCL_MOSI_PIN_MASK;
            #else
            #endif /* (SPI_1_RX_SCL_MOSI_PIN) */

            #if (SPI_1_CTS_SCLK_PIN)
                pinsInBuf |= SPI_1_CTS_SCLK_PIN_MASK;
            #endif /* (SPI_1_CTS_SCLK_PIN) */
            }
        }
        else /* UART */
        {
            if (SPI_1_UART_MODE_SMARTCARD == subMode)
            {
                /* SmartCard */
            #if (SPI_1_TX_SDA_MISO_PIN)
                hsiomSel[SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_TX_SDA_MISO_HSIOM_SEL_UART;
                pinsDm  [SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_PIN_DM_OD_LO;
            #endif /* (SPI_1_TX_SDA_MISO_PIN) */
            }
            else /* Standard or IrDA */
            {
                if (0u != (SPI_1_UART_RX_PIN_ENABLE & uartEnableMask))
                {
                #if (SPI_1_RX_SCL_MOSI_PIN)
                    hsiomSel[SPI_1_RX_SCL_MOSI_PIN_INDEX] = SPI_1_RX_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [SPI_1_RX_SCL_MOSI_PIN_INDEX] = SPI_1_PIN_DM_DIG_HIZ;
                #elif (SPI_1_RX_WAKE_SCL_MOSI_PIN)
                    hsiomSel[SPI_1_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_1_RX_WAKE_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [SPI_1_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_1_PIN_DM_DIG_HIZ;
                #else
                #endif /* (SPI_1_RX_SCL_MOSI_PIN) */
                }

                if (0u != (SPI_1_UART_TX_PIN_ENABLE & uartEnableMask))
                {
                #if (SPI_1_TX_SDA_MISO_PIN)
                    hsiomSel[SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_TX_SDA_MISO_HSIOM_SEL_UART;
                    pinsDm  [SPI_1_TX_SDA_MISO_PIN_INDEX] = SPI_1_PIN_DM_STRONG;
                    
                    /* Disable input buffer */
                    pinsInBuf |= SPI_1_TX_SDA_MISO_PIN_MASK;
                #endif /* (SPI_1_TX_SDA_MISO_PIN) */
                }

            #if !(SPI_1_CY_SCBIP_V0 || SPI_1_CY_SCBIP_V1)
                if (SPI_1_UART_MODE_STD == subMode)
                {
                    if (0u != (SPI_1_UART_CTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* CTS input is multiplexed with SCLK */
                    #if (SPI_1_CTS_SCLK_PIN)
                        hsiomSel[SPI_1_CTS_SCLK_PIN_INDEX] = SPI_1_CTS_SCLK_HSIOM_SEL_UART;
                        pinsDm  [SPI_1_CTS_SCLK_PIN_INDEX] = SPI_1_PIN_DM_DIG_HIZ;
                    #endif /* (SPI_1_CTS_SCLK_PIN) */
                    }

                    if (0u != (SPI_1_UART_RTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* RTS output is multiplexed with SS0 */
                    #if (SPI_1_RTS_SS0_PIN)
                        hsiomSel[SPI_1_RTS_SS0_PIN_INDEX] = SPI_1_RTS_SS0_HSIOM_SEL_UART;
                        pinsDm  [SPI_1_RTS_SS0_PIN_INDEX] = SPI_1_PIN_DM_STRONG;
                        
                        /* Disable input buffer */
                        pinsInBuf |= SPI_1_RTS_SS0_PIN_MASK;
                    #endif /* (SPI_1_RTS_SS0_PIN) */
                    }
                }
            #endif /* !(SPI_1_CY_SCBIP_V0 || SPI_1_CY_SCBIP_V1) */
            }
        }
    #endif /* (!SPI_1_CY_SCBIP_V1) */

    /* Configure pins: set HSIOM, DM and InputBufEnable */
    /* Note: the DR register settings do not effect the pin output if HSIOM is other than GPIO */

    #if (SPI_1_RX_SCL_MOSI_PIN)
        SPI_1_SET_HSIOM_SEL(SPI_1_RX_SCL_MOSI_HSIOM_REG,
                                       SPI_1_RX_SCL_MOSI_HSIOM_MASK,
                                       SPI_1_RX_SCL_MOSI_HSIOM_POS,
                                        hsiomSel[SPI_1_RX_SCL_MOSI_PIN_INDEX]);

        SPI_1_uart_rx_i2c_scl_spi_mosi_SetDriveMode((uint8) pinsDm[SPI_1_RX_SCL_MOSI_PIN_INDEX]);

        #if (!SPI_1_CY_SCBIP_V1)
            SPI_1_SET_INP_DIS(SPI_1_uart_rx_i2c_scl_spi_mosi_INP_DIS,
                                         SPI_1_uart_rx_i2c_scl_spi_mosi_MASK,
                                         (0u != (pinsInBuf & SPI_1_RX_SCL_MOSI_PIN_MASK)));
        #endif /* (!SPI_1_CY_SCBIP_V1) */
    
    #elif (SPI_1_RX_WAKE_SCL_MOSI_PIN)
        SPI_1_SET_HSIOM_SEL(SPI_1_RX_WAKE_SCL_MOSI_HSIOM_REG,
                                       SPI_1_RX_WAKE_SCL_MOSI_HSIOM_MASK,
                                       SPI_1_RX_WAKE_SCL_MOSI_HSIOM_POS,
                                       hsiomSel[SPI_1_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        SPI_1_uart_rx_wake_i2c_scl_spi_mosi_SetDriveMode((uint8)
                                                               pinsDm[SPI_1_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        SPI_1_SET_INP_DIS(SPI_1_uart_rx_wake_i2c_scl_spi_mosi_INP_DIS,
                                     SPI_1_uart_rx_wake_i2c_scl_spi_mosi_MASK,
                                     (0u != (pinsInBuf & SPI_1_RX_WAKE_SCL_MOSI_PIN_MASK)));

         /* Set interrupt on falling edge */
        SPI_1_SET_INCFG_TYPE(SPI_1_RX_WAKE_SCL_MOSI_INTCFG_REG,
                                        SPI_1_RX_WAKE_SCL_MOSI_INTCFG_TYPE_MASK,
                                        SPI_1_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS,
                                        SPI_1_INTCFG_TYPE_FALLING_EDGE);
    #else
    #endif /* (SPI_1_RX_WAKE_SCL_MOSI_PIN) */

    #if (SPI_1_TX_SDA_MISO_PIN)
        SPI_1_SET_HSIOM_SEL(SPI_1_TX_SDA_MISO_HSIOM_REG,
                                       SPI_1_TX_SDA_MISO_HSIOM_MASK,
                                       SPI_1_TX_SDA_MISO_HSIOM_POS,
                                        hsiomSel[SPI_1_TX_SDA_MISO_PIN_INDEX]);

        SPI_1_uart_tx_i2c_sda_spi_miso_SetDriveMode((uint8) pinsDm[SPI_1_TX_SDA_MISO_PIN_INDEX]);

    #if (!SPI_1_CY_SCBIP_V1)
        SPI_1_SET_INP_DIS(SPI_1_uart_tx_i2c_sda_spi_miso_INP_DIS,
                                     SPI_1_uart_tx_i2c_sda_spi_miso_MASK,
                                    (0u != (pinsInBuf & SPI_1_TX_SDA_MISO_PIN_MASK)));
    #endif /* (!SPI_1_CY_SCBIP_V1) */
    #endif /* (SPI_1_RX_SCL_MOSI_PIN) */

    #if (SPI_1_CTS_SCLK_PIN)
        SPI_1_SET_HSIOM_SEL(SPI_1_CTS_SCLK_HSIOM_REG,
                                       SPI_1_CTS_SCLK_HSIOM_MASK,
                                       SPI_1_CTS_SCLK_HSIOM_POS,
                                       hsiomSel[SPI_1_CTS_SCLK_PIN_INDEX]);

        SPI_1_uart_cts_spi_sclk_SetDriveMode((uint8) pinsDm[SPI_1_CTS_SCLK_PIN_INDEX]);

        SPI_1_SET_INP_DIS(SPI_1_uart_cts_spi_sclk_INP_DIS,
                                     SPI_1_uart_cts_spi_sclk_MASK,
                                     (0u != (pinsInBuf & SPI_1_CTS_SCLK_PIN_MASK)));
    #endif /* (SPI_1_CTS_SCLK_PIN) */

    #if (SPI_1_RTS_SS0_PIN)
        SPI_1_SET_HSIOM_SEL(SPI_1_RTS_SS0_HSIOM_REG,
                                       SPI_1_RTS_SS0_HSIOM_MASK,
                                       SPI_1_RTS_SS0_HSIOM_POS,
                                       hsiomSel[SPI_1_RTS_SS0_PIN_INDEX]);

        SPI_1_uart_rts_spi_ss0_SetDriveMode((uint8) pinsDm[SPI_1_RTS_SS0_PIN_INDEX]);

        SPI_1_SET_INP_DIS(SPI_1_uart_rts_spi_ss0_INP_DIS,
                                     SPI_1_uart_rts_spi_ss0_MASK,
                                     (0u != (pinsInBuf & SPI_1_RTS_SS0_PIN_MASK)));
    #endif /* (SPI_1_RTS_SS0_PIN) */

    #if (SPI_1_SS1_PIN)
        SPI_1_SET_HSIOM_SEL(SPI_1_SS1_HSIOM_REG,
                                       SPI_1_SS1_HSIOM_MASK,
                                       SPI_1_SS1_HSIOM_POS,
                                       hsiomSel[SPI_1_SS1_PIN_INDEX]);

        SPI_1_spi_ss1_SetDriveMode((uint8) pinsDm[SPI_1_SS1_PIN_INDEX]);

        SPI_1_SET_INP_DIS(SPI_1_spi_ss1_INP_DIS,
                                     SPI_1_spi_ss1_MASK,
                                     (0u != (pinsInBuf & SPI_1_SS1_PIN_MASK)));
    #endif /* (SPI_1_SS1_PIN) */

    #if (SPI_1_SS2_PIN)
        SPI_1_SET_HSIOM_SEL(SPI_1_SS2_HSIOM_REG,
                                       SPI_1_SS2_HSIOM_MASK,
                                       SPI_1_SS2_HSIOM_POS,
                                       hsiomSel[SPI_1_SS2_PIN_INDEX]);

        SPI_1_spi_ss2_SetDriveMode((uint8) pinsDm[SPI_1_SS2_PIN_INDEX]);

        SPI_1_SET_INP_DIS(SPI_1_spi_ss2_INP_DIS,
                                     SPI_1_spi_ss2_MASK,
                                     (0u != (pinsInBuf & SPI_1_SS2_PIN_MASK)));
    #endif /* (SPI_1_SS2_PIN) */

    #if (SPI_1_SS3_PIN)
        SPI_1_SET_HSIOM_SEL(SPI_1_SS3_HSIOM_REG,
                                       SPI_1_SS3_HSIOM_MASK,
                                       SPI_1_SS3_HSIOM_POS,
                                       hsiomSel[SPI_1_SS3_PIN_INDEX]);

        SPI_1_spi_ss3_SetDriveMode((uint8) pinsDm[SPI_1_SS3_PIN_INDEX]);

        SPI_1_SET_INP_DIS(SPI_1_spi_ss3_INP_DIS,
                                     SPI_1_spi_ss3_MASK,
                                     (0u != (pinsInBuf & SPI_1_SS3_PIN_MASK)));
    #endif /* (SPI_1_SS3_PIN) */
    }

#endif /* (SPI_1_SCB_MODE_UNCONFIG_CONST_CFG) */


#if (SPI_1_CY_SCBIP_V0 || SPI_1_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: SPI_1_I2CSlaveNackGeneration
    ****************************************************************************//**
    *
    *  Sets command to generate NACK to the address or data.
    *
    *******************************************************************************/
    void SPI_1_I2CSlaveNackGeneration(void)
    {
        /* Check for EC_AM toggle condition: EC_AM and clock stretching for address are enabled */
        if ((0u != (SPI_1_CTRL_REG & SPI_1_CTRL_EC_AM_MODE)) &&
            (0u == (SPI_1_I2C_CTRL_REG & SPI_1_I2C_CTRL_S_NOT_READY_ADDR_NACK)))
        {
            /* Toggle EC_AM before NACK generation */
            SPI_1_CTRL_REG &= ~SPI_1_CTRL_EC_AM_MODE;
            SPI_1_CTRL_REG |=  SPI_1_CTRL_EC_AM_MODE;
        }

        SPI_1_I2C_SLAVE_CMD_REG = SPI_1_I2C_SLAVE_CMD_S_NACK;
    }
#endif /* (SPI_1_CY_SCBIP_V0 || SPI_1_CY_SCBIP_V1) */


/* [] END OF FILE */
