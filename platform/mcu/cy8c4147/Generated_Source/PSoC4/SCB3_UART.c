/***************************************************************************//**
* \file SCB3_UART.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  UART mode.
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
#include "SCB3_SPI_UART_PVT.h"
#include "cyapicallbacks.h"

#if (SCB3_UART_WAKE_ENABLE_CONST && SCB3_UART_RX_WAKEUP_IRQ)
    /**
    * \addtogroup group_globals
    * \{
    */
    /** This global variable determines whether to enable Skip Start
    * functionality when SCB3_Sleep() function is called:
    * 0 – disable, other values – enable. Default value is 1.
    * It is only available when Enable wakeup from Deep Sleep Mode is enabled.
    */
    uint8 SCB3_skipStart = 1u;
    /** \} globals */
#endif /* (SCB3_UART_WAKE_ENABLE_CONST && SCB3_UART_RX_WAKEUP_IRQ) */

#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG)

    /***************************************
    *  Configuration Structure Initialization
    ***************************************/

    const SCB3_UART_INIT_STRUCT SCB3_configUart =
    {
        SCB3_UART_SUB_MODE,
        SCB3_UART_DIRECTION,
        SCB3_UART_DATA_BITS_NUM,
        SCB3_UART_PARITY_TYPE,
        SCB3_UART_STOP_BITS_NUM,
        SCB3_UART_OVS_FACTOR,
        SCB3_UART_IRDA_LOW_POWER,
        SCB3_UART_MEDIAN_FILTER_ENABLE,
        SCB3_UART_RETRY_ON_NACK,
        SCB3_UART_IRDA_POLARITY,
        SCB3_UART_DROP_ON_PARITY_ERR,
        SCB3_UART_DROP_ON_FRAME_ERR,
        SCB3_UART_WAKE_ENABLE,
        0u,
        NULL,
        0u,
        NULL,
        SCB3_UART_MP_MODE_ENABLE,
        SCB3_UART_MP_ACCEPT_ADDRESS,
        SCB3_UART_MP_RX_ADDRESS,
        SCB3_UART_MP_RX_ADDRESS_MASK,
        (uint32) SCB3_SCB_IRQ_INTERNAL,
        SCB3_UART_INTR_RX_MASK,
        SCB3_UART_RX_TRIGGER_LEVEL,
        SCB3_UART_INTR_TX_MASK,
        SCB3_UART_TX_TRIGGER_LEVEL,
        (uint8) SCB3_UART_BYTE_MODE_ENABLE,
        (uint8) SCB3_UART_CTS_ENABLE,
        (uint8) SCB3_UART_CTS_POLARITY,
        (uint8) SCB3_UART_RTS_POLARITY,
        (uint8) SCB3_UART_RTS_FIFO_LEVEL,
        (uint8) SCB3_UART_RX_BREAK_WIDTH
    };


    /*******************************************************************************
    * Function Name: SCB3_UartInit
    ****************************************************************************//**
    *
    *  Configures the SCB3 for UART operation.
    *
    *  This function is intended specifically to be used when the SCB3
    *  configuration is set to “Unconfigured SCB3” in the customizer.
    *  After initializing the SCB3 in UART mode using this function,
    *  the component can be enabled using the SCB3_Start() or
    * SCB3_Enable() function.
    *  This function uses a pointer to a structure that provides the configuration
    *  settings. This structure contains the same information that would otherwise
    *  be provided by the customizer settings.
    *
    *  \param config: pointer to a structure that contains the following list of
    *   fields. These fields match the selections available in the customizer.
    *   Refer to the customizer for further description of the settings.
    *
    *******************************************************************************/
    void SCB3_UartInit(const SCB3_UART_INIT_STRUCT *config)
    {
        uint32 pinsConfig;

        if (NULL == config)
        {
            CYASSERT(0u != 0u); /* Halt execution due to bad function parameter */
        }
        else
        {
            /* Get direction to configure UART pins: TX, RX or TX+RX */
            pinsConfig  = config->direction;

        #if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
            /* Add RTS and CTS pins to configure */
            pinsConfig |= (0u != config->rtsRxFifoLevel) ? (SCB3_UART_RTS_PIN_ENABLE) : (0u);
            pinsConfig |= (0u != config->enableCts)      ? (SCB3_UART_CTS_PIN_ENABLE) : (0u);
        #endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

            /* Configure pins */
            SCB3_SetPins(SCB3_SCB_MODE_UART, config->mode, pinsConfig);

            /* Store internal configuration */
            SCB3_scbMode       = (uint8) SCB3_SCB_MODE_UART;
            SCB3_scbEnableWake = (uint8) config->enableWake;
            SCB3_scbEnableIntr = (uint8) config->enableInterrupt;

            /* Set RX direction internal variables */
            SCB3_rxBuffer      =         config->rxBuffer;
            SCB3_rxDataBits    = (uint8) config->dataBits;
            SCB3_rxBufferSize  =         config->rxBufferSize;

            /* Set TX direction internal variables */
            SCB3_txBuffer      =         config->txBuffer;
            SCB3_txDataBits    = (uint8) config->dataBits;
            SCB3_txBufferSize  =         config->txBufferSize;

            /* Configure UART interface */
            if(SCB3_UART_MODE_IRDA == config->mode)
            {
                /* OVS settings: IrDA */
                SCB3_CTRL_REG  = ((0u != config->enableIrdaLowPower) ?
                                                (SCB3_UART_GET_CTRL_OVS_IRDA_LP(config->oversample)) :
                                                (SCB3_CTRL_OVS_IRDA_OVS16));
            }
            else
            {
                /* OVS settings: UART and SmartCard */
                SCB3_CTRL_REG  = SCB3_GET_CTRL_OVS(config->oversample);
            }

            SCB3_CTRL_REG     |= SCB3_GET_CTRL_BYTE_MODE  (config->enableByteMode)      |
                                             SCB3_GET_CTRL_ADDR_ACCEPT(config->multiprocAcceptAddr) |
                                             SCB3_CTRL_UART;

            /* Configure sub-mode: UART, SmartCard or IrDA */
            SCB3_UART_CTRL_REG = SCB3_GET_UART_CTRL_MODE(config->mode);

            /* Configure RX direction */
            SCB3_UART_RX_CTRL_REG = SCB3_GET_UART_RX_CTRL_MODE(config->stopBits)              |
                                        SCB3_GET_UART_RX_CTRL_POLARITY(config->enableInvertedRx)          |
                                        SCB3_GET_UART_RX_CTRL_MP_MODE(config->enableMultiproc)            |
                                        SCB3_GET_UART_RX_CTRL_DROP_ON_PARITY_ERR(config->dropOnParityErr) |
                                        SCB3_GET_UART_RX_CTRL_DROP_ON_FRAME_ERR(config->dropOnFrameErr)   |
                                        SCB3_GET_UART_RX_CTRL_BREAK_WIDTH(config->breakWidth);

            if(SCB3_UART_PARITY_NONE != config->parity)
            {
               SCB3_UART_RX_CTRL_REG |= SCB3_GET_UART_RX_CTRL_PARITY(config->parity) |
                                                    SCB3_UART_RX_CTRL_PARITY_ENABLED;
            }

            SCB3_RX_CTRL_REG      = SCB3_GET_RX_CTRL_DATA_WIDTH(config->dataBits)       |
                                                SCB3_GET_RX_CTRL_MEDIAN(config->enableMedianFilter) |
                                                SCB3_GET_UART_RX_CTRL_ENABLED(config->direction);

            SCB3_RX_FIFO_CTRL_REG = SCB3_GET_RX_FIFO_CTRL_TRIGGER_LEVEL(config->rxTriggerLevel);

            /* Configure MP address */
            SCB3_RX_MATCH_REG     = SCB3_GET_RX_MATCH_ADDR(config->multiprocAddr) |
                                                SCB3_GET_RX_MATCH_MASK(config->multiprocAddrMask);

            /* Configure RX direction */
            SCB3_UART_TX_CTRL_REG = SCB3_GET_UART_TX_CTRL_MODE(config->stopBits) |
                                                SCB3_GET_UART_TX_CTRL_RETRY_NACK(config->enableRetryNack);

            if(SCB3_UART_PARITY_NONE != config->parity)
            {
               SCB3_UART_TX_CTRL_REG |= SCB3_GET_UART_TX_CTRL_PARITY(config->parity) |
                                                    SCB3_UART_TX_CTRL_PARITY_ENABLED;
            }

            SCB3_TX_CTRL_REG      = SCB3_GET_TX_CTRL_DATA_WIDTH(config->dataBits)    |
                                                SCB3_GET_UART_TX_CTRL_ENABLED(config->direction);

            SCB3_TX_FIFO_CTRL_REG = SCB3_GET_TX_FIFO_CTRL_TRIGGER_LEVEL(config->txTriggerLevel);

        #if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
            SCB3_UART_FLOW_CTRL_REG = SCB3_GET_UART_FLOW_CTRL_CTS_ENABLE(config->enableCts) | \
                                            SCB3_GET_UART_FLOW_CTRL_CTS_POLARITY (config->ctsPolarity)  | \
                                            SCB3_GET_UART_FLOW_CTRL_RTS_POLARITY (config->rtsPolarity)  | \
                                            SCB3_GET_UART_FLOW_CTRL_TRIGGER_LEVEL(config->rtsRxFifoLevel);
        #endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

            /* Configure interrupt with UART handler but do not enable it */
            CyIntDisable    (SCB3_ISR_NUMBER);
            CyIntSetPriority(SCB3_ISR_NUMBER, SCB3_ISR_PRIORITY);
            (void) CyIntSetVector(SCB3_ISR_NUMBER, &SCB3_SPI_UART_ISR);

            /* Configure WAKE interrupt */
        #if(SCB3_UART_RX_WAKEUP_IRQ)
            CyIntDisable    (SCB3_RX_WAKE_ISR_NUMBER);
            CyIntSetPriority(SCB3_RX_WAKE_ISR_NUMBER, SCB3_RX_WAKE_ISR_PRIORITY);
            (void) CyIntSetVector(SCB3_RX_WAKE_ISR_NUMBER, &SCB3_UART_WAKEUP_ISR);
        #endif /* (SCB3_UART_RX_WAKEUP_IRQ) */

            /* Configure interrupt sources */
            SCB3_INTR_I2C_EC_MASK_REG = SCB3_NO_INTR_SOURCES;
            SCB3_INTR_SPI_EC_MASK_REG = SCB3_NO_INTR_SOURCES;
            SCB3_INTR_SLAVE_MASK_REG  = SCB3_NO_INTR_SOURCES;
            SCB3_INTR_MASTER_MASK_REG = SCB3_NO_INTR_SOURCES;
            SCB3_INTR_RX_MASK_REG     = config->rxInterruptMask;
            SCB3_INTR_TX_MASK_REG     = config->txInterruptMask;

            /* Configure TX interrupt sources to restore. */
            SCB3_IntrTxMask = LO16(SCB3_INTR_TX_MASK_REG);

            /* Clear RX buffer indexes */
            SCB3_rxBufferHead     = 0u;
            SCB3_rxBufferTail     = 0u;
            SCB3_rxBufferOverflow = 0u;

            /* Clear TX buffer indexes */
            SCB3_txBufferHead = 0u;
            SCB3_txBufferTail = 0u;
        }
    }

#else

    /*******************************************************************************
    * Function Name: SCB3_UartInit
    ****************************************************************************//**
    *
    *  Configures the SCB for the UART operation.
    *
    *******************************************************************************/
    void SCB3_UartInit(void)
    {
        /* Configure UART interface */
        SCB3_CTRL_REG = SCB3_UART_DEFAULT_CTRL;

        /* Configure sub-mode: UART, SmartCard or IrDA */
        SCB3_UART_CTRL_REG = SCB3_UART_DEFAULT_UART_CTRL;

        /* Configure RX direction */
        SCB3_UART_RX_CTRL_REG = SCB3_UART_DEFAULT_UART_RX_CTRL;
        SCB3_RX_CTRL_REG      = SCB3_UART_DEFAULT_RX_CTRL;
        SCB3_RX_FIFO_CTRL_REG = SCB3_UART_DEFAULT_RX_FIFO_CTRL;
        SCB3_RX_MATCH_REG     = SCB3_UART_DEFAULT_RX_MATCH_REG;

        /* Configure TX direction */
        SCB3_UART_TX_CTRL_REG = SCB3_UART_DEFAULT_UART_TX_CTRL;
        SCB3_TX_CTRL_REG      = SCB3_UART_DEFAULT_TX_CTRL;
        SCB3_TX_FIFO_CTRL_REG = SCB3_UART_DEFAULT_TX_FIFO_CTRL;

    #if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
        SCB3_UART_FLOW_CTRL_REG = SCB3_UART_DEFAULT_FLOW_CTRL;
    #endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

        /* Configure interrupt with UART handler but do not enable it */
    #if(SCB3_SCB_IRQ_INTERNAL)
        CyIntDisable    (SCB3_ISR_NUMBER);
        CyIntSetPriority(SCB3_ISR_NUMBER, SCB3_ISR_PRIORITY);
        (void) CyIntSetVector(SCB3_ISR_NUMBER, &SCB3_SPI_UART_ISR);
    #endif /* (SCB3_SCB_IRQ_INTERNAL) */

        /* Configure WAKE interrupt */
    #if(SCB3_UART_RX_WAKEUP_IRQ)
        CyIntDisable    (SCB3_RX_WAKE_ISR_NUMBER);
        CyIntSetPriority(SCB3_RX_WAKE_ISR_NUMBER, SCB3_RX_WAKE_ISR_PRIORITY);
        (void) CyIntSetVector(SCB3_RX_WAKE_ISR_NUMBER, &SCB3_UART_WAKEUP_ISR);
    #endif /* (SCB3_UART_RX_WAKEUP_IRQ) */

        /* Configure interrupt sources */
        SCB3_INTR_I2C_EC_MASK_REG = SCB3_UART_DEFAULT_INTR_I2C_EC_MASK;
        SCB3_INTR_SPI_EC_MASK_REG = SCB3_UART_DEFAULT_INTR_SPI_EC_MASK;
        SCB3_INTR_SLAVE_MASK_REG  = SCB3_UART_DEFAULT_INTR_SLAVE_MASK;
        SCB3_INTR_MASTER_MASK_REG = SCB3_UART_DEFAULT_INTR_MASTER_MASK;
        SCB3_INTR_RX_MASK_REG     = SCB3_UART_DEFAULT_INTR_RX_MASK;
        SCB3_INTR_TX_MASK_REG     = SCB3_UART_DEFAULT_INTR_TX_MASK;

        /* Configure TX interrupt sources to restore. */
        SCB3_IntrTxMask = LO16(SCB3_INTR_TX_MASK_REG);

    #if(SCB3_INTERNAL_RX_SW_BUFFER_CONST)
        SCB3_rxBufferHead     = 0u;
        SCB3_rxBufferTail     = 0u;
        SCB3_rxBufferOverflow = 0u;
    #endif /* (SCB3_INTERNAL_RX_SW_BUFFER_CONST) */

    #if(SCB3_INTERNAL_TX_SW_BUFFER_CONST)
        SCB3_txBufferHead = 0u;
        SCB3_txBufferTail = 0u;
    #endif /* (SCB3_INTERNAL_TX_SW_BUFFER_CONST) */
    }
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */


/*******************************************************************************
* Function Name: SCB3_UartPostEnable
****************************************************************************//**
*
*  Restores HSIOM settings for the UART output pins (TX and/or RTS) to be
*  controlled by the SCB UART.
*
*******************************************************************************/
void SCB3_UartPostEnable(void)
{
#if (SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    #if (SCB3_TX_SDA_MISO_PIN)
        if (SCB3_CHECK_TX_SDA_MISO_PIN_USED)
        {
            /* Set SCB UART to drive the output pin */
            SCB3_SET_HSIOM_SEL(SCB3_TX_SDA_MISO_HSIOM_REG, SCB3_TX_SDA_MISO_HSIOM_MASK,
                                           SCB3_TX_SDA_MISO_HSIOM_POS, SCB3_TX_SDA_MISO_HSIOM_SEL_UART);
        }
    #endif /* (SCB3_TX_SDA_MISO_PIN_PIN) */

    #if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
        #if (SCB3_RTS_SS0_PIN)
            if (SCB3_CHECK_RTS_SS0_PIN_USED)
            {
                /* Set SCB UART to drive the output pin */
                SCB3_SET_HSIOM_SEL(SCB3_RTS_SS0_HSIOM_REG, SCB3_RTS_SS0_HSIOM_MASK,
                                               SCB3_RTS_SS0_HSIOM_POS, SCB3_RTS_SS0_HSIOM_SEL_UART);
            }
        #endif /* (SCB3_RTS_SS0_PIN) */
    #endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

#else
    #if (SCB3_UART_TX_PIN)
         /* Set SCB UART to drive the output pin */
        SCB3_SET_HSIOM_SEL(SCB3_TX_HSIOM_REG, SCB3_TX_HSIOM_MASK,
                                       SCB3_TX_HSIOM_POS, SCB3_TX_HSIOM_SEL_UART);
    #endif /* (SCB3_UART_TX_PIN) */

    #if (SCB3_UART_RTS_PIN)
        /* Set SCB UART to drive the output pin */
        SCB3_SET_HSIOM_SEL(SCB3_RTS_HSIOM_REG, SCB3_RTS_HSIOM_MASK,
                                       SCB3_RTS_HSIOM_POS, SCB3_RTS_HSIOM_SEL_UART);
    #endif /* (SCB3_UART_RTS_PIN) */
#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Restore TX interrupt sources. */
    SCB3_SetTxInterruptMode(SCB3_IntrTxMask);
}


/*******************************************************************************
* Function Name: SCB3_UartStop
****************************************************************************//**
*
*  Changes the HSIOM settings for the UART output pins (TX and/or RTS) to keep
*  them inactive after the block is disabled. The output pins are controlled by
*  the GPIO data register. Also, the function disables the skip start feature
*  to not cause it to trigger after the component is enabled.
*
*******************************************************************************/
void SCB3_UartStop(void)
{
#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    #if (SCB3_TX_SDA_MISO_PIN)
        if (SCB3_CHECK_TX_SDA_MISO_PIN_USED)
        {
            /* Set GPIO to drive output pin */
            SCB3_SET_HSIOM_SEL(SCB3_TX_SDA_MISO_HSIOM_REG, SCB3_TX_SDA_MISO_HSIOM_MASK,
                                           SCB3_TX_SDA_MISO_HSIOM_POS, SCB3_TX_SDA_MISO_HSIOM_SEL_GPIO);
        }
    #endif /* (SCB3_TX_SDA_MISO_PIN_PIN) */

    #if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
        #if (SCB3_RTS_SS0_PIN)
            if (SCB3_CHECK_RTS_SS0_PIN_USED)
            {
                /* Set output pin state after block is disabled */
                SCB3_uart_rts_spi_ss0_Write(SCB3_GET_UART_RTS_INACTIVE);

                /* Set GPIO to drive output pin */
                SCB3_SET_HSIOM_SEL(SCB3_RTS_SS0_HSIOM_REG, SCB3_RTS_SS0_HSIOM_MASK,
                                               SCB3_RTS_SS0_HSIOM_POS, SCB3_RTS_SS0_HSIOM_SEL_GPIO);
            }
        #endif /* (SCB3_RTS_SS0_PIN) */
    #endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

#else
    #if (SCB3_UART_TX_PIN)
        /* Set GPIO to drive output pin */
        SCB3_SET_HSIOM_SEL(SCB3_TX_HSIOM_REG, SCB3_TX_HSIOM_MASK,
                                       SCB3_TX_HSIOM_POS, SCB3_TX_HSIOM_SEL_GPIO);
    #endif /* (SCB3_UART_TX_PIN) */

    #if (SCB3_UART_RTS_PIN)
        /* Set output pin state after block is disabled */
        SCB3_rts_Write(SCB3_GET_UART_RTS_INACTIVE);

        /* Set GPIO to drive output pin */
        SCB3_SET_HSIOM_SEL(SCB3_RTS_HSIOM_REG, SCB3_RTS_HSIOM_MASK,
                                       SCB3_RTS_HSIOM_POS, SCB3_RTS_HSIOM_SEL_GPIO);
    #endif /* (SCB3_UART_RTS_PIN) */

#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (SCB3_UART_WAKE_ENABLE_CONST)
    /* Disable skip start feature used for wakeup */
    SCB3_UART_RX_CTRL_REG &= (uint32) ~SCB3_UART_RX_CTRL_SKIP_START;
#endif /* (SCB3_UART_WAKE_ENABLE_CONST) */

    /* Store TX interrupt sources (exclude level triggered). */
    SCB3_IntrTxMask = LO16(SCB3_GetTxInterruptMode() & SCB3_INTR_UART_TX_RESTORE);
}


/*******************************************************************************
* Function Name: SCB3_UartSetRxAddress
****************************************************************************//**
*
*  Sets the hardware detectable receiver address for the UART in the
*  Multiprocessor mode.
*
*  \param address: Address for hardware address detection.
*
*******************************************************************************/
void SCB3_UartSetRxAddress(uint32 address)
{
     uint32 matchReg;

    matchReg = SCB3_RX_MATCH_REG;

    matchReg &= ((uint32) ~SCB3_RX_MATCH_ADDR_MASK); /* Clear address bits */
    matchReg |= ((uint32)  (address & SCB3_RX_MATCH_ADDR_MASK)); /* Set address  */

    SCB3_RX_MATCH_REG = matchReg;
}


/*******************************************************************************
* Function Name: SCB3_UartSetRxAddressMask
****************************************************************************//**
*
*  Sets the hardware address mask for the UART in the Multiprocessor mode.
*
*  \param addressMask: Address mask.
*   - Bit value 0 – excludes bit from address comparison.
*   - Bit value 1 – the bit needs to match with the corresponding bit
*     of the address.
*
*******************************************************************************/
void SCB3_UartSetRxAddressMask(uint32 addressMask)
{
    uint32 matchReg;

    matchReg = SCB3_RX_MATCH_REG;

    matchReg &= ((uint32) ~SCB3_RX_MATCH_MASK_MASK); /* Clear address mask bits */
    matchReg |= ((uint32) (addressMask << SCB3_RX_MATCH_MASK_POS));

    SCB3_RX_MATCH_REG = matchReg;
}


#if(SCB3_UART_RX_DIRECTION)
    /*******************************************************************************
    * Function Name: SCB3_UartGetChar
    ****************************************************************************//**
    *
    *  Retrieves next data element from receive buffer.
    *  This function is designed for ASCII characters and returns a char where
    *  1 to 255 are valid characters and 0 indicates an error occurred or no data
    *  is present.
    *  - RX software buffer is disabled: Returns data element retrieved from RX
    *    FIFO.
    *  - RX software buffer is enabled: Returns data element from the software
    *    receive buffer.
    *
    *  \return
    *   Next data element from the receive buffer. ASCII character values from
    *   1 to 255 are valid. A returned zero signifies an error condition or no
    *   data available.
    *
    *  \sideeffect
    *   The errors bits may not correspond with reading characters due to
    *   RX FIFO and software buffer usage.
    *   RX software buffer is enabled: The internal software buffer overflow
    *   is not treated as an error condition.
    *   Check SCB3_rxBufferOverflow to capture that error condition.
    *
    *******************************************************************************/
    uint32 SCB3_UartGetChar(void)
    {
        uint32 rxData = 0u;

        /* Reads data only if there is data to read */
        if (0u != SCB3_SpiUartGetRxBufferSize())
        {
            rxData = SCB3_SpiUartReadRxData();
        }

        if (SCB3_CHECK_INTR_RX(SCB3_INTR_RX_ERR))
        {
            rxData = 0u; /* Error occurred: returns zero */
            SCB3_ClearRxInterruptSource(SCB3_INTR_RX_ERR);
        }

        return (rxData);
    }


    /*******************************************************************************
    * Function Name: SCB3_UartGetByte
    ****************************************************************************//**
    *
    *  Retrieves the next data element from the receive buffer, returns the
    *  received byte and error condition.
    *   - The RX software buffer is disabled: returns the data element retrieved
    *     from the RX FIFO. Undefined data will be returned if the RX FIFO is
    *     empty.
    *   - The RX software buffer is enabled: returns data element from the
    *     software receive buffer.
    *
    *  \return
    *   Bits 7-0 contain the next data element from the receive buffer and
    *   other bits contain the error condition.
    *   - SCB3_UART_RX_OVERFLOW - Attempt to write to a full
    *     receiver FIFO.
    *   - SCB3_UART_RX_UNDERFLOW    Attempt to read from an empty
    *     receiver FIFO.
    *   - SCB3_UART_RX_FRAME_ERROR - UART framing error detected.
    *   - SCB3_UART_RX_PARITY_ERROR - UART parity error detected.
    *
    *  \sideeffect
    *   The errors bits may not correspond with reading characters due to
    *   RX FIFO and software buffer usage.
    *   RX software buffer is enabled: The internal software buffer overflow
    *   is not treated as an error condition.
    *   Check SCB3_rxBufferOverflow to capture that error condition.
    *
    *******************************************************************************/
    uint32 SCB3_UartGetByte(void)
    {
        uint32 rxData;
        uint32 tmpStatus;

        #if (SCB3_CHECK_RX_SW_BUFFER)
        {
            SCB3_DisableInt();
        }
        #endif

        if (0u != SCB3_SpiUartGetRxBufferSize())
        {
            /* Enables interrupt to receive more bytes: at least one byte is in
            * buffer.
            */
            #if (SCB3_CHECK_RX_SW_BUFFER)
            {
                SCB3_EnableInt();
            }
            #endif

            /* Get received byte */
            rxData = SCB3_SpiUartReadRxData();
        }
        else
        {
            /* Reads a byte directly from RX FIFO: underflow is raised in the
            * case of empty. Otherwise the first received byte will be read.
            */
            rxData = SCB3_RX_FIFO_RD_REG;


            /* Enables interrupt to receive more bytes. */
            #if (SCB3_CHECK_RX_SW_BUFFER)
            {

                /* The byte has been read from RX FIFO. Clear RX interrupt to
                * not involve interrupt handler when RX FIFO is empty.
                */
                SCB3_ClearRxInterruptSource(SCB3_INTR_RX_NOT_EMPTY);

                SCB3_EnableInt();
            }
            #endif
        }

        /* Get and clear RX error mask */
        tmpStatus = (SCB3_GetRxInterruptSource() & SCB3_INTR_RX_ERR);
        SCB3_ClearRxInterruptSource(SCB3_INTR_RX_ERR);

        /* Puts together data and error status:
        * MP mode and accept address: 9th bit is set to notify mark.
        */
        rxData |= ((uint32) (tmpStatus << 8u));

        return (rxData);
    }


    #if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
        /*******************************************************************************
        * Function Name: SCB3_UartSetRtsPolarity
        ****************************************************************************//**
        *
        *  Sets active polarity of RTS output signal.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        *  \param polarity: Active polarity of RTS output signal.
        *   - SCB3_UART_RTS_ACTIVE_LOW  - RTS signal is active low.
        *   - SCB3_UART_RTS_ACTIVE_HIGH - RTS signal is active high.
        *
        *******************************************************************************/
        void SCB3_UartSetRtsPolarity(uint32 polarity)
        {
            if(0u != polarity)
            {
                SCB3_UART_FLOW_CTRL_REG |= (uint32)  SCB3_UART_FLOW_CTRL_RTS_POLARITY;
            }
            else
            {
                SCB3_UART_FLOW_CTRL_REG &= (uint32) ~SCB3_UART_FLOW_CTRL_RTS_POLARITY;
            }
        }


        /*******************************************************************************
        * Function Name: SCB3_UartSetRtsFifoLevel
        ****************************************************************************//**
        *
        *  Sets level in the RX FIFO for RTS signal activation.
        *  While the RX FIFO has fewer entries than the RX FIFO level the RTS signal
        *  remains active, otherwise the RTS signal becomes inactive.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        *  \param level: Level in the RX FIFO for RTS signal activation.
        *   The range of valid level values is between 0 and RX FIFO depth - 1.
        *   Setting level value to 0 disables RTS signal activation.
        *
        *******************************************************************************/
        void SCB3_UartSetRtsFifoLevel(uint32 level)
        {
            uint32 uartFlowCtrl;

            uartFlowCtrl = SCB3_UART_FLOW_CTRL_REG;

            uartFlowCtrl &= ((uint32) ~SCB3_UART_FLOW_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
            uartFlowCtrl |= ((uint32) (SCB3_UART_FLOW_CTRL_TRIGGER_LEVEL_MASK & level));

            SCB3_UART_FLOW_CTRL_REG = uartFlowCtrl;
        }
    #endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

#endif /* (SCB3_UART_RX_DIRECTION) */


#if(SCB3_UART_TX_DIRECTION)
    /*******************************************************************************
    * Function Name: SCB3_UartPutString
    ****************************************************************************//**
    *
    *  Places a NULL terminated string in the transmit buffer to be sent at the
    *  next available bus time.
    *  This function is blocking and waits until there is a space available to put
    *  requested data in transmit buffer.
    *
    *  \param string: pointer to the null terminated string array to be placed in the
    *   transmit buffer.
    *
    *******************************************************************************/
    void SCB3_UartPutString(const char8 string[])
    {
        uint32 bufIndex;

        bufIndex = 0u;

        /* Blocks the control flow until all data has been sent */
        while(string[bufIndex] != ((char8) 0))
        {
            SCB3_UartPutChar((uint32) string[bufIndex]);
            bufIndex++;
        }
    }


    /*******************************************************************************
    * Function Name: SCB3_UartPutCRLF
    ****************************************************************************//**
    *
    *  Places byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) in the transmit buffer.
    *  This function is blocking and waits until there is a space available to put
    *  all requested data in transmit buffer.
    *
    *  \param txDataByte: the data to be transmitted.
    *
    *******************************************************************************/
    void SCB3_UartPutCRLF(uint32 txDataByte)
    {
        SCB3_UartPutChar(txDataByte);  /* Blocks control flow until all data has been sent */
        SCB3_UartPutChar(0x0Du);       /* Blocks control flow until all data has been sent */
        SCB3_UartPutChar(0x0Au);       /* Blocks control flow until all data has been sent */
    }


    #if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
        /*******************************************************************************
        * Function Name: SCB3SCB_UartEnableCts
        ****************************************************************************//**
        *
        *  Enables usage of CTS input signal by the UART transmitter.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        *******************************************************************************/
        void SCB3_UartEnableCts(void)
        {
            SCB3_UART_FLOW_CTRL_REG |= (uint32)  SCB3_UART_FLOW_CTRL_CTS_ENABLE;
        }


        /*******************************************************************************
        * Function Name: SCB3_UartDisableCts
        ****************************************************************************//**
        *
        *  Disables usage of CTS input signal by the UART transmitter.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        *******************************************************************************/
        void SCB3_UartDisableCts(void)
        {
            SCB3_UART_FLOW_CTRL_REG &= (uint32) ~SCB3_UART_FLOW_CTRL_CTS_ENABLE;
        }


        /*******************************************************************************
        * Function Name: SCB3_UartSetCtsPolarity
        ****************************************************************************//**
        *
        *  Sets active polarity of CTS input signal.
        *  Only available for PSoC 4100 BLE / PSoC 4200 BLE / PSoC 4100M / PSoC 4200M /
        *  PSoC 4200L / PSoC 4000S / PSoC 4100S / PSoC Analog Coprocessor devices.
        *
        * \param
        * polarity: Active polarity of CTS output signal.
        *   - SCB3_UART_CTS_ACTIVE_LOW  - CTS signal is active low.
        *   - SCB3_UART_CTS_ACTIVE_HIGH - CTS signal is active high.
        *
        *******************************************************************************/
        void SCB3_UartSetCtsPolarity(uint32 polarity)
        {
            if (0u != polarity)
            {
                SCB3_UART_FLOW_CTRL_REG |= (uint32)  SCB3_UART_FLOW_CTRL_CTS_POLARITY;
            }
            else
            {
                SCB3_UART_FLOW_CTRL_REG &= (uint32) ~SCB3_UART_FLOW_CTRL_CTS_POLARITY;
            }
        }
    #endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */


    /*******************************************************************************
    * Function Name: SCB3_UartSendBreakBlocking
    ****************************************************************************//**
    *
    * Sends a break condition (logic low) of specified width on UART TX line.
    * Blocks until break is completed. Only call this function when UART TX FIFO
    * and shifter are empty.
    *
    * \param breakWidth
    * Width of break condition. Valid range is 4 to 16 bits.
    *
    * \note
    * Before sending break all UART TX interrupt sources are disabled. The state
    * of UART TX interrupt sources is restored before function returns.
    *
    * \sideeffect
    * If this function is called while there is data in the TX FIFO or shifter that
    * data will be shifted out in packets the size of breakWidth.
    *
    *******************************************************************************/
    void SCB3_UartSendBreakBlocking(uint32 breakWidth)
    {
        uint32 txCtrlReg;
        uint32 txIntrReg;

        /* Disable all UART TX interrupt source and clear UART TX Done history */
        txIntrReg = SCB3_GetTxInterruptMode();
        SCB3_SetTxInterruptMode(0u);
        SCB3_ClearTxInterruptSource(SCB3_INTR_TX_UART_DONE);

        /* Store TX CTRL configuration */
        txCtrlReg = SCB3_TX_CTRL_REG;

        /* Set break width */
        SCB3_TX_CTRL_REG = (SCB3_TX_CTRL_REG & (uint32) ~SCB3_TX_CTRL_DATA_WIDTH_MASK) |
                                        SCB3_GET_TX_CTRL_DATA_WIDTH(breakWidth);

        /* Generate break */
        SCB3_TX_FIFO_WR_REG = 0u;

        /* Wait for break completion */
        while (0u == (SCB3_GetTxInterruptSource() & SCB3_INTR_TX_UART_DONE))
        {
        }

        /* Clear all UART TX interrupt sources to  */
        SCB3_ClearTxInterruptSource(SCB3_INTR_TX_ALL);

        /* Restore TX interrupt sources and data width */
        SCB3_TX_CTRL_REG = txCtrlReg;
        SCB3_SetTxInterruptMode(txIntrReg);
    }
#endif /* (SCB3_UART_TX_DIRECTION) */


#if (SCB3_UART_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: SCB3_UartSaveConfig
    ****************************************************************************//**
    *
    *  Clears and enables an interrupt on a falling edge of the Rx input. The GPIO
    *  interrupt does not track in the active mode, therefore requires to be
    *  cleared by this API.
    *
    *******************************************************************************/
    void SCB3_UartSaveConfig(void)
    {
    #if (SCB3_UART_RX_WAKEUP_IRQ)
        /* Set SKIP_START if requested (set by default). */
        if (0u != SCB3_skipStart)
        {
            SCB3_UART_RX_CTRL_REG |= (uint32)  SCB3_UART_RX_CTRL_SKIP_START;
        }
        else
        {
            SCB3_UART_RX_CTRL_REG &= (uint32) ~SCB3_UART_RX_CTRL_SKIP_START;
        }

        /* Clear RX GPIO interrupt status and pending interrupt in NVIC because
        * falling edge on RX line occurs while UART communication in active mode.
        * Enable interrupt: next interrupt trigger should wakeup device.
        */
        SCB3_CLEAR_UART_RX_WAKE_INTR;
        SCB3_RxWakeClearPendingInt();
        SCB3_RxWakeEnableInt();
    #endif /* (SCB3_UART_RX_WAKEUP_IRQ) */
    }


    /*******************************************************************************
    * Function Name: SCB3_UartRestoreConfig
    ****************************************************************************//**
    *
    *  Disables the RX GPIO interrupt. Until this function is called the interrupt
    *  remains active and triggers on every falling edge of the UART RX line.
    *
    *******************************************************************************/
    void SCB3_UartRestoreConfig(void)
    {
    #if (SCB3_UART_RX_WAKEUP_IRQ)
        /* Disable interrupt: no more triggers in active mode */
        SCB3_RxWakeDisableInt();
    #endif /* (SCB3_UART_RX_WAKEUP_IRQ) */
    }


    #if (SCB3_UART_RX_WAKEUP_IRQ)
        /*******************************************************************************
        * Function Name: SCB3_UART_WAKEUP_ISR
        ****************************************************************************//**
        *
        *  Handles the Interrupt Service Routine for the SCB UART mode GPIO wakeup
        *  event. This event is configured to trigger on a falling edge of the RX line.
        *
        *******************************************************************************/
        CY_ISR(SCB3_UART_WAKEUP_ISR)
        {
        #ifdef SCB3_UART_WAKEUP_ISR_ENTRY_CALLBACK
            SCB3_UART_WAKEUP_ISR_EntryCallback();
        #endif /* SCB3_UART_WAKEUP_ISR_ENTRY_CALLBACK */

            SCB3_CLEAR_UART_RX_WAKE_INTR;

        #ifdef SCB3_UART_WAKEUP_ISR_EXIT_CALLBACK
            SCB3_UART_WAKEUP_ISR_ExitCallback();
        #endif /* SCB3_UART_WAKEUP_ISR_EXIT_CALLBACK */
        }
    #endif /* (SCB3_UART_RX_WAKEUP_IRQ) */
#endif /* (SCB3_UART_RX_WAKEUP_IRQ) */


/* [] END OF FILE */
