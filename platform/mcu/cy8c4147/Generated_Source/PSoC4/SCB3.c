/***************************************************************************//**
* \file SCB3.c
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

#include "SCB3_PVT.h"

#if (SCB3_SCB_MODE_I2C_INC)
    #include "SCB3_I2C_PVT.h"
#endif /* (SCB3_SCB_MODE_I2C_INC) */

#if (SCB3_SCB_MODE_EZI2C_INC)
    #include "SCB3_EZI2C_PVT.h"
#endif /* (SCB3_SCB_MODE_EZI2C_INC) */

#if (SCB3_SCB_MODE_SPI_INC || SCB3_SCB_MODE_UART_INC)
    #include "SCB3_SPI_UART_PVT.h"
#endif /* (SCB3_SCB_MODE_SPI_INC || SCB3_SCB_MODE_UART_INC) */


/***************************************
*    Run Time Configuration Vars
***************************************/

/* Stores internal component configuration for Unconfigured mode */
#if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    uint8 SCB3_scbMode = SCB3_SCB_MODE_UNCONFIG;
    uint8 SCB3_scbEnableWake;
    uint8 SCB3_scbEnableIntr;

    /* I2C configuration variables */
    uint8 SCB3_mode;
    uint8 SCB3_acceptAddr;

    /* SPI/UART configuration variables */
    volatile uint8 * SCB3_rxBuffer;
    uint8  SCB3_rxDataBits;
    uint32 SCB3_rxBufferSize;

    volatile uint8 * SCB3_txBuffer;
    uint8  SCB3_txDataBits;
    uint32 SCB3_txBufferSize;

    /* EZI2C configuration variables */
    uint8 SCB3_numberOfAddr;
    uint8 SCB3_subAddrSize;
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Common SCB Vars
***************************************/
/**
* \addtogroup group_general
* \{
*/

/** SCB3_initVar indicates whether the SCB3 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time SCB_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the SCB3_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  SCB3_Init() function can be called before the 
*  SCB3_Start() or SCB3_Enable() function.
*/
uint8 SCB3_initVar = 0u;


#if (! (SCB3_SCB_MODE_I2C_CONST_CFG || \
        SCB3_SCB_MODE_EZI2C_CONST_CFG))
    /** This global variable stores TX interrupt sources after 
    * SCB3_Stop() is called. Only these TX interrupt sources 
    * will be restored on a subsequent SCB3_Enable() call.
    */
    uint16 SCB3_IntrTxMask = 0u;
#endif /* (! (SCB3_SCB_MODE_I2C_CONST_CFG || \
              SCB3_SCB_MODE_EZI2C_CONST_CFG)) */
/** \} globals */

#if (SCB3_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_SCB3_CUSTOM_INTR_HANDLER)
    void (*SCB3_customIntrHandler)(void) = NULL;
#endif /* !defined (CY_REMOVE_SCB3_CUSTOM_INTR_HANDLER) */
#endif /* (SCB3_SCB_IRQ_INTERNAL) */


/***************************************
*    Private Function Prototypes
***************************************/

static void SCB3_ScbEnableIntr(void);
static void SCB3_ScbModeStop(void);
static void SCB3_ScbModePostEnable(void);


/*******************************************************************************
* Function Name: SCB3_Init
****************************************************************************//**
*
*  Initializes the SCB3 component to operate in one of the selected
*  configurations: I2C, SPI, UART or EZI2C.
*  When the configuration is set to "Unconfigured SCB", this function does
*  not do any initialization. Use mode-specific initialization APIs instead:
*  SCB3_I2CInit, SCB3_SpiInit, 
*  SCB3_UartInit or SCB3_EzI2CInit.
*
*******************************************************************************/
void SCB3_Init(void)
{
#if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    if (SCB3_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        SCB3_initVar = 0u;
    }
    else
    {
        /* Initialization was done before this function call */
    }

#elif (SCB3_SCB_MODE_I2C_CONST_CFG)
    SCB3_I2CInit();

#elif (SCB3_SCB_MODE_SPI_CONST_CFG)
    SCB3_SpiInit();

#elif (SCB3_SCB_MODE_UART_CONST_CFG)
    SCB3_UartInit();

#elif (SCB3_SCB_MODE_EZI2C_CONST_CFG)
    SCB3_EzI2CInit();

#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SCB3_Enable
****************************************************************************//**
*
*  Enables SCB3 component operation: activates the hardware and 
*  internal interrupt. It also restores TX interrupt sources disabled after the 
*  SCB3_Stop() function was called (note that level-triggered TX 
*  interrupt sources remain disabled to not cause code lock-up).
*  For I2C and EZI2C modes the interrupt is internal and mandatory for 
*  operation. For SPI and UART modes the interrupt can be configured as none, 
*  internal or external.
*  The SCB3 configuration should be not changed when the component
*  is enabled. Any configuration changes should be made after disabling the 
*  component.
*  When configuration is set to “Unconfigured SCB3”, the component 
*  must first be initialized to operate in one of the following configurations: 
*  I2C, SPI, UART or EZ I2C, using the mode-specific initialization API. 
*  Otherwise this function does not enable the component.
*
*******************************************************************************/
void SCB3_Enable(void)
{
#if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Enable SCB block, only if it is already configured */
    if (!SCB3_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        SCB3_CTRL_REG |= SCB3_CTRL_ENABLED;

        SCB3_ScbEnableIntr();

        /* Call PostEnable function specific to current operation mode */
        SCB3_ScbModePostEnable();
    }
#else
    SCB3_CTRL_REG |= SCB3_CTRL_ENABLED;

    SCB3_ScbEnableIntr();

    /* Call PostEnable function specific to current operation mode */
    SCB3_ScbModePostEnable();
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SCB3_Start
****************************************************************************//**
*
*  Invokes SCB3_Init() and SCB3_Enable().
*  After this function call, the component is enabled and ready for operation.
*  When configuration is set to "Unconfigured SCB", the component must first be
*  initialized to operate in one of the following configurations: I2C, SPI, UART
*  or EZI2C. Otherwise this function does not enable the component.
*
* \globalvars
*  SCB3_initVar - used to check initial configuration, modified
*  on first function call.
*
*******************************************************************************/
void SCB3_Start(void)
{
    if (0u == SCB3_initVar)
    {
        SCB3_Init();
        SCB3_initVar = 1u; /* Component was initialized */
    }

    SCB3_Enable();
}


/*******************************************************************************
* Function Name: SCB3_Stop
****************************************************************************//**
*
*  Disables the SCB3 component: disable the hardware and internal 
*  interrupt. It also disables all TX interrupt sources so as not to cause an 
*  unexpected interrupt trigger because after the component is enabled, the 
*  TX FIFO is empty.
*  Refer to the function SCB3_Enable() for the interrupt 
*  configuration details.
*  This function disables the SCB component without checking to see if 
*  communication is in progress. Before calling this function it may be 
*  necessary to check the status of communication to make sure communication 
*  is complete. If this is not done then communication could be stopped mid 
*  byte and corrupted data could result.
*
*******************************************************************************/
void SCB3_Stop(void)
{
#if (SCB3_SCB_IRQ_INTERNAL)
    SCB3_DisableInt();
#endif /* (SCB3_SCB_IRQ_INTERNAL) */

    /* Call Stop function specific to current operation mode */
    SCB3_ScbModeStop();

    /* Disable SCB IP */
    SCB3_CTRL_REG &= (uint32) ~SCB3_CTRL_ENABLED;

    /* Disable all TX interrupt sources so as not to cause an unexpected
    * interrupt trigger after the component will be enabled because the 
    * TX FIFO is empty.
    * For SCB IP v0, it is critical as it does not mask-out interrupt
    * sources when it is disabled. This can cause a code lock-up in the
    * interrupt handler because TX FIFO cannot be loaded after the block
    * is disabled.
    */
    SCB3_SetTxInterruptMode(SCB3_NO_INTR_SOURCES);

#if (SCB3_SCB_IRQ_INTERNAL)
    SCB3_ClearPendingInt();
#endif /* (SCB3_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: SCB3_SetRxFifoLevel
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
void SCB3_SetRxFifoLevel(uint32 level)
{
    uint32 rxFifoCtrl;

    rxFifoCtrl = SCB3_RX_FIFO_CTRL_REG;

    rxFifoCtrl &= ((uint32) ~SCB3_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    rxFifoCtrl |= ((uint32) (SCB3_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    SCB3_RX_FIFO_CTRL_REG = rxFifoCtrl;
}


/*******************************************************************************
* Function Name: SCB3_SetTxFifoLevel
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
void SCB3_SetTxFifoLevel(uint32 level)
{
    uint32 txFifoCtrl;

    txFifoCtrl = SCB3_TX_FIFO_CTRL_REG;

    txFifoCtrl &= ((uint32) ~SCB3_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    txFifoCtrl |= ((uint32) (SCB3_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    SCB3_TX_FIFO_CTRL_REG = txFifoCtrl;
}


#if (SCB3_SCB_IRQ_INTERNAL)
    /*******************************************************************************
    * Function Name: SCB3_SetCustomInterruptHandler
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
    void SCB3_SetCustomInterruptHandler(void (*func)(void))
    {
    #if !defined (CY_REMOVE_SCB3_CUSTOM_INTR_HANDLER)
        SCB3_customIntrHandler = func; /* Register interrupt handler */
    #else
        if (NULL != func)
        {
            /* Suppress compiler warning */
        }
    #endif /* !defined (CY_REMOVE_SCB3_CUSTOM_INTR_HANDLER) */
    }
#endif /* (SCB3_SCB_IRQ_INTERNAL) */


/*******************************************************************************
* Function Name: SCB3_ScbModeEnableIntr
****************************************************************************//**
*
*  Enables an interrupt for a specific mode.
*
*******************************************************************************/
static void SCB3_ScbEnableIntr(void)
{
#if (SCB3_SCB_IRQ_INTERNAL)
    /* Enable interrupt in NVIC */
    #if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
        if (0u != SCB3_scbEnableIntr)
        {
            SCB3_EnableInt();
        }

    #else
        SCB3_EnableInt();

    #endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */
#endif /* (SCB3_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: SCB3_ScbModePostEnable
****************************************************************************//**
*
*  Calls the PostEnable function for a specific operation mode.
*
*******************************************************************************/
static void SCB3_ScbModePostEnable(void)
{
#if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
#if (!SCB3_CY_SCBIP_V1)
    if (SCB3_SCB_MODE_SPI_RUNTM_CFG)
    {
        SCB3_SpiPostEnable();
    }
    else if (SCB3_SCB_MODE_UART_RUNTM_CFG)
    {
        SCB3_UartPostEnable();
    }
    else
    {
        /* Unknown mode: do nothing */
    }
#endif /* (!SCB3_CY_SCBIP_V1) */

#elif (SCB3_SCB_MODE_SPI_CONST_CFG)
    SCB3_SpiPostEnable();

#elif (SCB3_SCB_MODE_UART_CONST_CFG)
    SCB3_UartPostEnable();

#else
    /* Unknown mode: do nothing */
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SCB3_ScbModeStop
****************************************************************************//**
*
*  Calls the Stop function for a specific operation mode.
*
*******************************************************************************/
static void SCB3_ScbModeStop(void)
{
#if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    if (SCB3_SCB_MODE_I2C_RUNTM_CFG)
    {
        SCB3_I2CStop();
    }
    else if (SCB3_SCB_MODE_EZI2C_RUNTM_CFG)
    {
        SCB3_EzI2CStop();
    }
#if (!SCB3_CY_SCBIP_V1)
    else if (SCB3_SCB_MODE_SPI_RUNTM_CFG)
    {
        SCB3_SpiStop();
    }
    else if (SCB3_SCB_MODE_UART_RUNTM_CFG)
    {
        SCB3_UartStop();
    }
#endif /* (!SCB3_CY_SCBIP_V1) */
    else
    {
        /* Unknown mode: do nothing */
    }
#elif (SCB3_SCB_MODE_I2C_CONST_CFG)
    SCB3_I2CStop();

#elif (SCB3_SCB_MODE_EZI2C_CONST_CFG)
    SCB3_EzI2CStop();

#elif (SCB3_SCB_MODE_SPI_CONST_CFG)
    SCB3_SpiStop();

#elif (SCB3_SCB_MODE_UART_CONST_CFG)
    SCB3_UartStop();

#else
    /* Unknown mode: do nothing */
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: SCB3_SetPins
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
    void SCB3_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask)
    {
        uint32 pinsDm[SCB3_SCB_PINS_NUMBER];
        uint32 i;
        
    #if (!SCB3_CY_SCBIP_V1)
        uint32 pinsInBuf = 0u;
    #endif /* (!SCB3_CY_SCBIP_V1) */
        
        uint32 hsiomSel[SCB3_SCB_PINS_NUMBER] = 
        {
            SCB3_RX_SCL_MOSI_HSIOM_SEL_GPIO,
            SCB3_TX_SDA_MISO_HSIOM_SEL_GPIO,
            0u,
            0u,
            0u,
            0u,
            0u,
        };

    #if (SCB3_CY_SCBIP_V1)
        /* Supress compiler warning. */
        if ((0u == subMode) || (0u == uartEnableMask))
        {
        }
    #endif /* (SCB3_CY_SCBIP_V1) */

        /* Set default HSIOM to GPIO and Drive Mode to Analog Hi-Z */
        for (i = 0u; i < SCB3_SCB_PINS_NUMBER; i++)
        {
            pinsDm[i] = SCB3_PIN_DM_ALG_HIZ;
        }

        if ((SCB3_SCB_MODE_I2C   == mode) ||
            (SCB3_SCB_MODE_EZI2C == mode))
        {
        #if (SCB3_RX_SCL_MOSI_PIN)
            hsiomSel[SCB3_RX_SCL_MOSI_PIN_INDEX] = SCB3_RX_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [SCB3_RX_SCL_MOSI_PIN_INDEX] = SCB3_PIN_DM_OD_LO;
        #elif (SCB3_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[SCB3_RX_WAKE_SCL_MOSI_PIN_INDEX] = SCB3_RX_WAKE_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [SCB3_RX_WAKE_SCL_MOSI_PIN_INDEX] = SCB3_PIN_DM_OD_LO;
        #else
        #endif /* (SCB3_RX_SCL_MOSI_PIN) */
        
        #if (SCB3_TX_SDA_MISO_PIN)
            hsiomSel[SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_TX_SDA_MISO_HSIOM_SEL_I2C;
            pinsDm  [SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_PIN_DM_OD_LO;
        #endif /* (SCB3_TX_SDA_MISO_PIN) */
        }
    #if (!SCB3_CY_SCBIP_V1)
        else if (SCB3_SCB_MODE_SPI == mode)
        {
        #if (SCB3_RX_SCL_MOSI_PIN)
            hsiomSel[SCB3_RX_SCL_MOSI_PIN_INDEX] = SCB3_RX_SCL_MOSI_HSIOM_SEL_SPI;
        #elif (SCB3_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[SCB3_RX_WAKE_SCL_MOSI_PIN_INDEX] = SCB3_RX_WAKE_SCL_MOSI_HSIOM_SEL_SPI;
        #else
        #endif /* (SCB3_RX_SCL_MOSI_PIN) */
        
        #if (SCB3_TX_SDA_MISO_PIN)
            hsiomSel[SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_TX_SDA_MISO_HSIOM_SEL_SPI;
        #endif /* (SCB3_TX_SDA_MISO_PIN) */
        
        #if (SCB3_CTS_SCLK_PIN)
            hsiomSel[SCB3_CTS_SCLK_PIN_INDEX] = SCB3_CTS_SCLK_HSIOM_SEL_SPI;
        #endif /* (SCB3_CTS_SCLK_PIN) */

            if (SCB3_SPI_SLAVE == subMode)
            {
                /* Slave */
                pinsDm[SCB3_RX_SCL_MOSI_PIN_INDEX] = SCB3_PIN_DM_DIG_HIZ;
                pinsDm[SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_PIN_DM_STRONG;
                pinsDm[SCB3_CTS_SCLK_PIN_INDEX] = SCB3_PIN_DM_DIG_HIZ;

            #if (SCB3_RTS_SS0_PIN)
                /* Only SS0 is valid choice for Slave */
                hsiomSel[SCB3_RTS_SS0_PIN_INDEX] = SCB3_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm  [SCB3_RTS_SS0_PIN_INDEX] = SCB3_PIN_DM_DIG_HIZ;
            #endif /* (SCB3_RTS_SS0_PIN) */

            #if (SCB3_TX_SDA_MISO_PIN)
                /* Disable input buffer */
                 pinsInBuf |= SCB3_TX_SDA_MISO_PIN_MASK;
            #endif /* (SCB3_TX_SDA_MISO_PIN) */
            }
            else 
            {
                /* (Master) */
                pinsDm[SCB3_RX_SCL_MOSI_PIN_INDEX] = SCB3_PIN_DM_STRONG;
                pinsDm[SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_PIN_DM_DIG_HIZ;
                pinsDm[SCB3_CTS_SCLK_PIN_INDEX] = SCB3_PIN_DM_STRONG;

            #if (SCB3_RTS_SS0_PIN)
                hsiomSel [SCB3_RTS_SS0_PIN_INDEX] = SCB3_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm   [SCB3_RTS_SS0_PIN_INDEX] = SCB3_PIN_DM_STRONG;
                pinsInBuf |= SCB3_RTS_SS0_PIN_MASK;
            #endif /* (SCB3_RTS_SS0_PIN) */

            #if (SCB3_SS1_PIN)
                hsiomSel [SCB3_SS1_PIN_INDEX] = SCB3_SS1_HSIOM_SEL_SPI;
                pinsDm   [SCB3_SS1_PIN_INDEX] = SCB3_PIN_DM_STRONG;
                pinsInBuf |= SCB3_SS1_PIN_MASK;
            #endif /* (SCB3_SS1_PIN) */

            #if (SCB3_SS2_PIN)
                hsiomSel [SCB3_SS2_PIN_INDEX] = SCB3_SS2_HSIOM_SEL_SPI;
                pinsDm   [SCB3_SS2_PIN_INDEX] = SCB3_PIN_DM_STRONG;
                pinsInBuf |= SCB3_SS2_PIN_MASK;
            #endif /* (SCB3_SS2_PIN) */

            #if (SCB3_SS3_PIN)
                hsiomSel [SCB3_SS3_PIN_INDEX] = SCB3_SS3_HSIOM_SEL_SPI;
                pinsDm   [SCB3_SS3_PIN_INDEX] = SCB3_PIN_DM_STRONG;
                pinsInBuf |= SCB3_SS3_PIN_MASK;
            #endif /* (SCB3_SS3_PIN) */

                /* Disable input buffers */
            #if (SCB3_RX_SCL_MOSI_PIN)
                pinsInBuf |= SCB3_RX_SCL_MOSI_PIN_MASK;
            #elif (SCB3_RX_WAKE_SCL_MOSI_PIN)
                pinsInBuf |= SCB3_RX_WAKE_SCL_MOSI_PIN_MASK;
            #else
            #endif /* (SCB3_RX_SCL_MOSI_PIN) */

            #if (SCB3_CTS_SCLK_PIN)
                pinsInBuf |= SCB3_CTS_SCLK_PIN_MASK;
            #endif /* (SCB3_CTS_SCLK_PIN) */
            }
        }
        else /* UART */
        {
            if (SCB3_UART_MODE_SMARTCARD == subMode)
            {
                /* SmartCard */
            #if (SCB3_TX_SDA_MISO_PIN)
                hsiomSel[SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_TX_SDA_MISO_HSIOM_SEL_UART;
                pinsDm  [SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_PIN_DM_OD_LO;
            #endif /* (SCB3_TX_SDA_MISO_PIN) */
            }
            else /* Standard or IrDA */
            {
                if (0u != (SCB3_UART_RX_PIN_ENABLE & uartEnableMask))
                {
                #if (SCB3_RX_SCL_MOSI_PIN)
                    hsiomSel[SCB3_RX_SCL_MOSI_PIN_INDEX] = SCB3_RX_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [SCB3_RX_SCL_MOSI_PIN_INDEX] = SCB3_PIN_DM_DIG_HIZ;
                #elif (SCB3_RX_WAKE_SCL_MOSI_PIN)
                    hsiomSel[SCB3_RX_WAKE_SCL_MOSI_PIN_INDEX] = SCB3_RX_WAKE_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [SCB3_RX_WAKE_SCL_MOSI_PIN_INDEX] = SCB3_PIN_DM_DIG_HIZ;
                #else
                #endif /* (SCB3_RX_SCL_MOSI_PIN) */
                }

                if (0u != (SCB3_UART_TX_PIN_ENABLE & uartEnableMask))
                {
                #if (SCB3_TX_SDA_MISO_PIN)
                    hsiomSel[SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_TX_SDA_MISO_HSIOM_SEL_UART;
                    pinsDm  [SCB3_TX_SDA_MISO_PIN_INDEX] = SCB3_PIN_DM_STRONG;
                    
                    /* Disable input buffer */
                    pinsInBuf |= SCB3_TX_SDA_MISO_PIN_MASK;
                #endif /* (SCB3_TX_SDA_MISO_PIN) */
                }

            #if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
                if (SCB3_UART_MODE_STD == subMode)
                {
                    if (0u != (SCB3_UART_CTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* CTS input is multiplexed with SCLK */
                    #if (SCB3_CTS_SCLK_PIN)
                        hsiomSel[SCB3_CTS_SCLK_PIN_INDEX] = SCB3_CTS_SCLK_HSIOM_SEL_UART;
                        pinsDm  [SCB3_CTS_SCLK_PIN_INDEX] = SCB3_PIN_DM_DIG_HIZ;
                    #endif /* (SCB3_CTS_SCLK_PIN) */
                    }

                    if (0u != (SCB3_UART_RTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* RTS output is multiplexed with SS0 */
                    #if (SCB3_RTS_SS0_PIN)
                        hsiomSel[SCB3_RTS_SS0_PIN_INDEX] = SCB3_RTS_SS0_HSIOM_SEL_UART;
                        pinsDm  [SCB3_RTS_SS0_PIN_INDEX] = SCB3_PIN_DM_STRONG;
                        
                        /* Disable input buffer */
                        pinsInBuf |= SCB3_RTS_SS0_PIN_MASK;
                    #endif /* (SCB3_RTS_SS0_PIN) */
                    }
                }
            #endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */
            }
        }
    #endif /* (!SCB3_CY_SCBIP_V1) */

    /* Configure pins: set HSIOM, DM and InputBufEnable */
    /* Note: the DR register settings do not effect the pin output if HSIOM is other than GPIO */

    #if (SCB3_RX_SCL_MOSI_PIN)
        SCB3_SET_HSIOM_SEL(SCB3_RX_SCL_MOSI_HSIOM_REG,
                                       SCB3_RX_SCL_MOSI_HSIOM_MASK,
                                       SCB3_RX_SCL_MOSI_HSIOM_POS,
                                        hsiomSel[SCB3_RX_SCL_MOSI_PIN_INDEX]);

        SCB3_uart_rx_i2c_scl_spi_mosi_SetDriveMode((uint8) pinsDm[SCB3_RX_SCL_MOSI_PIN_INDEX]);

        #if (!SCB3_CY_SCBIP_V1)
            SCB3_SET_INP_DIS(SCB3_uart_rx_i2c_scl_spi_mosi_INP_DIS,
                                         SCB3_uart_rx_i2c_scl_spi_mosi_MASK,
                                         (0u != (pinsInBuf & SCB3_RX_SCL_MOSI_PIN_MASK)));
        #endif /* (!SCB3_CY_SCBIP_V1) */
    
    #elif (SCB3_RX_WAKE_SCL_MOSI_PIN)
        SCB3_SET_HSIOM_SEL(SCB3_RX_WAKE_SCL_MOSI_HSIOM_REG,
                                       SCB3_RX_WAKE_SCL_MOSI_HSIOM_MASK,
                                       SCB3_RX_WAKE_SCL_MOSI_HSIOM_POS,
                                       hsiomSel[SCB3_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        SCB3_uart_rx_wake_i2c_scl_spi_mosi_SetDriveMode((uint8)
                                                               pinsDm[SCB3_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        SCB3_SET_INP_DIS(SCB3_uart_rx_wake_i2c_scl_spi_mosi_INP_DIS,
                                     SCB3_uart_rx_wake_i2c_scl_spi_mosi_MASK,
                                     (0u != (pinsInBuf & SCB3_RX_WAKE_SCL_MOSI_PIN_MASK)));

         /* Set interrupt on falling edge */
        SCB3_SET_INCFG_TYPE(SCB3_RX_WAKE_SCL_MOSI_INTCFG_REG,
                                        SCB3_RX_WAKE_SCL_MOSI_INTCFG_TYPE_MASK,
                                        SCB3_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS,
                                        SCB3_INTCFG_TYPE_FALLING_EDGE);
    #else
    #endif /* (SCB3_RX_WAKE_SCL_MOSI_PIN) */

    #if (SCB3_TX_SDA_MISO_PIN)
        SCB3_SET_HSIOM_SEL(SCB3_TX_SDA_MISO_HSIOM_REG,
                                       SCB3_TX_SDA_MISO_HSIOM_MASK,
                                       SCB3_TX_SDA_MISO_HSIOM_POS,
                                        hsiomSel[SCB3_TX_SDA_MISO_PIN_INDEX]);

        SCB3_uart_tx_i2c_sda_spi_miso_SetDriveMode((uint8) pinsDm[SCB3_TX_SDA_MISO_PIN_INDEX]);

    #if (!SCB3_CY_SCBIP_V1)
        SCB3_SET_INP_DIS(SCB3_uart_tx_i2c_sda_spi_miso_INP_DIS,
                                     SCB3_uart_tx_i2c_sda_spi_miso_MASK,
                                    (0u != (pinsInBuf & SCB3_TX_SDA_MISO_PIN_MASK)));
    #endif /* (!SCB3_CY_SCBIP_V1) */
    #endif /* (SCB3_RX_SCL_MOSI_PIN) */

    #if (SCB3_CTS_SCLK_PIN)
        SCB3_SET_HSIOM_SEL(SCB3_CTS_SCLK_HSIOM_REG,
                                       SCB3_CTS_SCLK_HSIOM_MASK,
                                       SCB3_CTS_SCLK_HSIOM_POS,
                                       hsiomSel[SCB3_CTS_SCLK_PIN_INDEX]);

        SCB3_uart_cts_spi_sclk_SetDriveMode((uint8) pinsDm[SCB3_CTS_SCLK_PIN_INDEX]);

        SCB3_SET_INP_DIS(SCB3_uart_cts_spi_sclk_INP_DIS,
                                     SCB3_uart_cts_spi_sclk_MASK,
                                     (0u != (pinsInBuf & SCB3_CTS_SCLK_PIN_MASK)));
    #endif /* (SCB3_CTS_SCLK_PIN) */

    #if (SCB3_RTS_SS0_PIN)
        SCB3_SET_HSIOM_SEL(SCB3_RTS_SS0_HSIOM_REG,
                                       SCB3_RTS_SS0_HSIOM_MASK,
                                       SCB3_RTS_SS0_HSIOM_POS,
                                       hsiomSel[SCB3_RTS_SS0_PIN_INDEX]);

        SCB3_uart_rts_spi_ss0_SetDriveMode((uint8) pinsDm[SCB3_RTS_SS0_PIN_INDEX]);

        SCB3_SET_INP_DIS(SCB3_uart_rts_spi_ss0_INP_DIS,
                                     SCB3_uart_rts_spi_ss0_MASK,
                                     (0u != (pinsInBuf & SCB3_RTS_SS0_PIN_MASK)));
    #endif /* (SCB3_RTS_SS0_PIN) */

    #if (SCB3_SS1_PIN)
        SCB3_SET_HSIOM_SEL(SCB3_SS1_HSIOM_REG,
                                       SCB3_SS1_HSIOM_MASK,
                                       SCB3_SS1_HSIOM_POS,
                                       hsiomSel[SCB3_SS1_PIN_INDEX]);

        SCB3_spi_ss1_SetDriveMode((uint8) pinsDm[SCB3_SS1_PIN_INDEX]);

        SCB3_SET_INP_DIS(SCB3_spi_ss1_INP_DIS,
                                     SCB3_spi_ss1_MASK,
                                     (0u != (pinsInBuf & SCB3_SS1_PIN_MASK)));
    #endif /* (SCB3_SS1_PIN) */

    #if (SCB3_SS2_PIN)
        SCB3_SET_HSIOM_SEL(SCB3_SS2_HSIOM_REG,
                                       SCB3_SS2_HSIOM_MASK,
                                       SCB3_SS2_HSIOM_POS,
                                       hsiomSel[SCB3_SS2_PIN_INDEX]);

        SCB3_spi_ss2_SetDriveMode((uint8) pinsDm[SCB3_SS2_PIN_INDEX]);

        SCB3_SET_INP_DIS(SCB3_spi_ss2_INP_DIS,
                                     SCB3_spi_ss2_MASK,
                                     (0u != (pinsInBuf & SCB3_SS2_PIN_MASK)));
    #endif /* (SCB3_SS2_PIN) */

    #if (SCB3_SS3_PIN)
        SCB3_SET_HSIOM_SEL(SCB3_SS3_HSIOM_REG,
                                       SCB3_SS3_HSIOM_MASK,
                                       SCB3_SS3_HSIOM_POS,
                                       hsiomSel[SCB3_SS3_PIN_INDEX]);

        SCB3_spi_ss3_SetDriveMode((uint8) pinsDm[SCB3_SS3_PIN_INDEX]);

        SCB3_SET_INP_DIS(SCB3_spi_ss3_INP_DIS,
                                     SCB3_spi_ss3_MASK,
                                     (0u != (pinsInBuf & SCB3_SS3_PIN_MASK)));
    #endif /* (SCB3_SS3_PIN) */
    }

#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */


#if (SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: SCB3_I2CSlaveNackGeneration
    ****************************************************************************//**
    *
    *  Sets command to generate NACK to the address or data.
    *
    *******************************************************************************/
    void SCB3_I2CSlaveNackGeneration(void)
    {
        /* Check for EC_AM toggle condition: EC_AM and clock stretching for address are enabled */
        if ((0u != (SCB3_CTRL_REG & SCB3_CTRL_EC_AM_MODE)) &&
            (0u == (SCB3_I2C_CTRL_REG & SCB3_I2C_CTRL_S_NOT_READY_ADDR_NACK)))
        {
            /* Toggle EC_AM before NACK generation */
            SCB3_CTRL_REG &= ~SCB3_CTRL_EC_AM_MODE;
            SCB3_CTRL_REG |=  SCB3_CTRL_EC_AM_MODE;
        }

        SCB3_I2C_SLAVE_CMD_REG = SCB3_I2C_SLAVE_CMD_S_NACK;
    }
#endif /* (SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */


/* [] END OF FILE */
