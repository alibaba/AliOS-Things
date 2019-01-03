/***************************************************************************//**
* \file SCB3.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values for the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_SCB3_H)
#define CY_SCB_SCB3_H

#include <cydevice_trm.h>
#include <cyfitter.h>
#include <cytypes.h>
#include <CyLib.h>

/* SCB IP block v0 is available in PSoC 4100/PSoC 4200 */
#define SCB3_CY_SCBIP_V0    (CYIPBLOCK_m0s8scb_VERSION == 0u)
/* SCB IP block v1 is available in PSoC 4000 */
#define SCB3_CY_SCBIP_V1    (CYIPBLOCK_m0s8scb_VERSION == 1u)
/* SCB IP block v2 is available in all other devices */
#define SCB3_CY_SCBIP_V2    (CYIPBLOCK_m0s8scb_VERSION >= 2u)

/** Component version major.minor */
#define SCB3_COMP_VERSION_MAJOR    (4)
#define SCB3_COMP_VERSION_MINOR    (0)
    
#define SCB3_SCB_MODE           (4u)

/* SCB modes enum */
#define SCB3_SCB_MODE_I2C       (0x01u)
#define SCB3_SCB_MODE_SPI       (0x02u)
#define SCB3_SCB_MODE_UART      (0x04u)
#define SCB3_SCB_MODE_EZI2C     (0x08u)
#define SCB3_SCB_MODE_UNCONFIG  (0xFFu)

/* Condition compilation depends on operation mode: Unconfigured implies apply to all modes */
#define SCB3_SCB_MODE_I2C_CONST_CFG       (SCB3_SCB_MODE_I2C       == SCB3_SCB_MODE)
#define SCB3_SCB_MODE_SPI_CONST_CFG       (SCB3_SCB_MODE_SPI       == SCB3_SCB_MODE)
#define SCB3_SCB_MODE_UART_CONST_CFG      (SCB3_SCB_MODE_UART      == SCB3_SCB_MODE)
#define SCB3_SCB_MODE_EZI2C_CONST_CFG     (SCB3_SCB_MODE_EZI2C     == SCB3_SCB_MODE)
#define SCB3_SCB_MODE_UNCONFIG_CONST_CFG  (SCB3_SCB_MODE_UNCONFIG  == SCB3_SCB_MODE)

/* Condition compilation for includes */
#define SCB3_SCB_MODE_I2C_INC      (0u !=(SCB3_SCB_MODE_I2C   & SCB3_SCB_MODE))
#define SCB3_SCB_MODE_EZI2C_INC    (0u !=(SCB3_SCB_MODE_EZI2C & SCB3_SCB_MODE))
#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_SCB_MODE_SPI_INC  (0u !=(SCB3_SCB_MODE_SPI   & SCB3_SCB_MODE))
    #define SCB3_SCB_MODE_UART_INC (0u !=(SCB3_SCB_MODE_UART  & SCB3_SCB_MODE))
#else
    #define SCB3_SCB_MODE_SPI_INC  (0u)
    #define SCB3_SCB_MODE_UART_INC (0u)
#endif /* (!SCB3_CY_SCBIP_V1) */

/* Interrupts remove options */
#define SCB3_REMOVE_SCB_IRQ             (0u)
#define SCB3_SCB_IRQ_INTERNAL           (0u == SCB3_REMOVE_SCB_IRQ)

#define SCB3_REMOVE_UART_RX_WAKEUP_IRQ  (1u)
#define SCB3_UART_RX_WAKEUP_IRQ         (0u == SCB3_REMOVE_UART_RX_WAKEUP_IRQ)

/* SCB interrupt enum */
#define SCB3_SCB_INTR_MODE_NONE     (0u)
#define SCB3_SCB_INTR_MODE_INTERNAL (1u)
#define SCB3_SCB_INTR_MODE_EXTERNAL (2u)

/* Internal clock remove option */
#define SCB3_REMOVE_SCB_CLK     (0u)
#define SCB3_SCB_CLK_INTERNAL   (0u == SCB3_REMOVE_SCB_CLK)


/***************************************
*       Includes
****************************************/

#include "SCB3_PINS.h"

#if (SCB3_SCB_CLK_INTERNAL)
    #include "SCB3_SCBCLK.h"
#endif /* (SCB3_SCB_CLK_INTERNAL) */


/***************************************
*       Type Definitions
***************************************/

typedef struct
{
    uint8 enableState;
} SCB3_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

/**
* \addtogroup group_general
* @{
*/

/* Start and Stop APIs */
void SCB3_Init(void);
void SCB3_Enable(void);
void SCB3_Start(void);
void SCB3_Stop(void);

/** @} general */

/**
* \addtogroup group_power
* @{
*/
/* Sleep and Wakeup APis */
void SCB3_Sleep(void);
void SCB3_Wakeup(void);
/** @} power */ 

/**
* \addtogroup group_interrupt
* @{
*/
#if (SCB3_SCB_IRQ_INTERNAL)
    /* Custom interrupt handler */
    void SCB3_SetCustomInterruptHandler(void (*func)(void));
#endif /* (SCB3_SCB_IRQ_INTERNAL) */
/** @} interrupt */

/* Interface to internal interrupt component */
#if (SCB3_SCB_IRQ_INTERNAL)
    /**
    * \addtogroup group_interrupt
    * @{
    */    
    /*******************************************************************************
    * Function Name: SCB3_EnableInt
    ****************************************************************************//**
    *
    *  When using an Internal interrupt, this enables the interrupt in the NVIC. 
    *  When using an external interrupt the API for the interrupt component must 
    *  be used to enable the interrupt.
    *
    *******************************************************************************/
    #define SCB3_EnableInt()    CyIntEnable(SCB3_ISR_NUMBER)
    
    
    /*******************************************************************************
    * Function Name: SCB3_DisableInt
    ****************************************************************************//**
    *
    *  When using an Internal interrupt, this disables the interrupt in the NVIC. 
    *  When using an external interrupt the API for the interrupt component must 
    *  be used to disable the interrupt.
    *
    *******************************************************************************/    
    #define SCB3_DisableInt()   CyIntDisable(SCB3_ISR_NUMBER)
    /** @} interrupt */

    /*******************************************************************************
    * Function Name: SCB3_ClearPendingInt
    ****************************************************************************//**
    *
    *  This function clears the interrupt pending status in the NVIC. 
    *
    *******************************************************************************/
    #define SCB3_ClearPendingInt()  CyIntClearPending(SCB3_ISR_NUMBER)
#endif /* (SCB3_SCB_IRQ_INTERNAL) */

#if (SCB3_UART_RX_WAKEUP_IRQ)
    /*******************************************************************************
    * Function Name: SCB3_RxWakeEnableInt
    ****************************************************************************//**
    *
    *  This function enables the interrupt (RX_WAKE) pending status in the NVIC. 
    *
    *******************************************************************************/    
    #define SCB3_RxWakeEnableInt()  CyIntEnable(SCB3_RX_WAKE_ISR_NUMBER)
    

    /*******************************************************************************
    * Function Name: SCB3_RxWakeDisableInt
    ****************************************************************************//**
    *
    *  This function disables the interrupt (RX_WAKE) pending status in the NVIC.  
    *
    *******************************************************************************/
    #define SCB3_RxWakeDisableInt() CyIntDisable(SCB3_RX_WAKE_ISR_NUMBER)
    
    
    /*******************************************************************************
    * Function Name: SCB3_RxWakeClearPendingInt
    ****************************************************************************//**
    *
    *  This function clears the interrupt (RX_WAKE) pending status in the NVIC. 
    *
    *******************************************************************************/    
    #define SCB3_RxWakeClearPendingInt()  CyIntClearPending(SCB3_RX_WAKE_ISR_NUMBER)
#endif /* (SCB3_UART_RX_WAKEUP_IRQ) */

/**
* \addtogroup group_interrupt
* @{
*/
/* Get interrupt cause */
/*******************************************************************************
* Function Name: SCB3_GetInterruptCause
****************************************************************************//**
*
*  Returns a mask of bits showing the source of the current triggered interrupt. 
*  This is useful for modes of operation where an interrupt can be generated by 
*  conditions in multiple interrupt source registers.
*
*  \return
*   Mask with the OR of the following conditions that have been triggered.
*    - SCB3_INTR_CAUSE_MASTER - Interrupt from Master
*    - SCB3_INTR_CAUSE_SLAVE - Interrupt from Slave
*    - SCB3_INTR_CAUSE_TX - Interrupt from TX
*    - SCB3_INTR_CAUSE_RX - Interrupt from RX
*
*******************************************************************************/
#define SCB3_GetInterruptCause()    (SCB3_INTR_CAUSE_REG)


/* APIs to service INTR_RX register */
/*******************************************************************************
* Function Name: SCB3_GetRxInterruptSource
****************************************************************************//**
*
*  Returns RX interrupt request register. This register contains current status 
*  of RX interrupt sources.
*
*  \return
*   Current status of RX interrupt sources.
*   Each constant is a bit field value. The value returned may have multiple 
*   bits set to indicate the current status.
*   - SCB3_INTR_RX_FIFO_LEVEL - The number of data elements in the 
      RX FIFO is greater than the value of RX FIFO level.
*   - SCB3_INTR_RX_NOT_EMPTY - Receiver FIFO is not empty.
*   - SCB3_INTR_RX_FULL - Receiver FIFO is full.
*   - SCB3_INTR_RX_OVERFLOW - Attempt to write to a full 
*     receiver FIFO.
*   - SCB3_INTR_RX_UNDERFLOW - Attempt to read from an empty 
*     receiver FIFO.
*   - SCB3_INTR_RX_FRAME_ERROR - UART framing error detected.
*   - SCB3_INTR_RX_PARITY_ERROR - UART parity error detected.
*
*******************************************************************************/
#define SCB3_GetRxInterruptSource() (SCB3_INTR_RX_REG)


/*******************************************************************************
* Function Name: SCB3_SetRxInterruptMode
****************************************************************************//**
*
*  Writes RX interrupt mask register. This register configures which bits from 
*  RX interrupt request register will trigger an interrupt event.
*
*  \param interruptMask: RX interrupt sources to be enabled (refer to 
*   SCB3_GetRxInterruptSource() function for bit fields values).
*
*******************************************************************************/
#define SCB3_SetRxInterruptMode(interruptMask)     SCB3_WRITE_INTR_RX_MASK(interruptMask)


/*******************************************************************************
* Function Name: SCB3_GetRxInterruptMode
****************************************************************************//**
*
*  Returns RX interrupt mask register This register specifies which bits from 
*  RX interrupt request register will trigger an interrupt event.
*
*  \return 
*   RX interrupt sources to be enabled (refer to 
*   SCB3_GetRxInterruptSource() function for bit fields values).
*
*******************************************************************************/
#define SCB3_GetRxInterruptMode()   (SCB3_INTR_RX_MASK_REG)


/*******************************************************************************
* Function Name: SCB3_GetRxInterruptSourceMasked
****************************************************************************//**
*
*  Returns RX interrupt masked request register. This register contains logical
*  AND of corresponding bits from RX interrupt request and mask registers.
*  This function is intended to be used in the interrupt service routine to 
*  identify which of enabled RX interrupt sources cause interrupt event.
*
*  \return 
*   Current status of enabled RX interrupt sources (refer to 
*   SCB3_GetRxInterruptSource() function for bit fields values).
*
*******************************************************************************/
#define SCB3_GetRxInterruptSourceMasked()   (SCB3_INTR_RX_MASKED_REG)


/*******************************************************************************
* Function Name: SCB3_ClearRxInterruptSource
****************************************************************************//**
*
*  Clears RX interrupt sources in the interrupt request register.
*
*  \param interruptMask: RX interrupt sources to be cleared (refer to 
*   SCB3_GetRxInterruptSource() function for bit fields values).
*
*  \sideeffects 
*   The side effects are listed in the table below for each 
*   affected interrupt source. Refer to section RX FIFO interrupt sources for 
*   detailed description.
*   - SCB3_INTR_RX_FIFO_LEVEL Interrupt source is not cleared when 
*     the receiver FIFO has more entries than level.
*   - SCB3_INTR_RX_NOT_EMPTY Interrupt source is not cleared when
*     receiver FIFO is not empty.
*   - SCB3_INTR_RX_FULL Interrupt source is not cleared when 
*      receiver FIFO is full.
*
*******************************************************************************/
#define SCB3_ClearRxInterruptSource(interruptMask)  SCB3_CLEAR_INTR_RX(interruptMask)


/*******************************************************************************
* Function Name: SCB3_SetRxInterrupt
****************************************************************************//**
*
*  Sets RX interrupt sources in the interrupt request register.
*
*  \param interruptMask: RX interrupt sources to set in the RX interrupt request 
*   register (refer to SCB3_GetRxInterruptSource() function for bit 
*   fields values).
*
*******************************************************************************/
#define SCB3_SetRxInterrupt(interruptMask)  SCB3_SET_INTR_RX(interruptMask)

void SCB3_SetRxFifoLevel(uint32 level);


/* APIs to service INTR_TX register */
/*******************************************************************************
* Function Name: SCB3_GetTxInterruptSource
****************************************************************************//**
*
*  Returns TX interrupt request register. This register contains current status 
*  of TX interrupt sources.
* 
*  \return 
*   Current status of TX interrupt sources.
*   Each constant is a bit field value. The value returned may have multiple 
*   bits set to indicate the current status.
*   - SCB3_INTR_TX_FIFO_LEVEL - The number of data elements in the 
*     TX FIFO is less than the value of TX FIFO level.
*   - SCB3_INTR_TX_NOT_FULL - Transmitter FIFO is not full.
*   - SCB3_INTR_TX_EMPTY - Transmitter FIFO is empty.
*   - SCB3_INTR_TX_OVERFLOW - Attempt to write to a full 
*     transmitter FIFO.
*   - SCB3_INTR_TX_UNDERFLOW - Attempt to read from an empty 
*     transmitter FIFO.
*   - SCB3_INTR_TX_UART_NACK - UART received a NACK in SmartCard 
*   mode.
*   - SCB3_INTR_TX_UART_DONE - UART transfer is complete. 
*     All data elements from the TX FIFO are sent.
*   - SCB3_INTR_TX_UART_ARB_LOST - Value on the TX line of the UART
*     does not match the value on the RX line.
*
*******************************************************************************/
#define SCB3_GetTxInterruptSource() (SCB3_INTR_TX_REG)


/*******************************************************************************
* Function Name: SCB3_SetTxInterruptMode
****************************************************************************//**
*
*  Writes TX interrupt mask register. This register configures which bits from 
*  TX interrupt request register will trigger an interrupt event.
*
*  \param interruptMask: TX interrupt sources to be enabled (refer to 
*   SCB3_GetTxInterruptSource() function for bit field values).
*
*******************************************************************************/
#define SCB3_SetTxInterruptMode(interruptMask)  SCB3_WRITE_INTR_TX_MASK(interruptMask)


/*******************************************************************************
* Function Name: SCB3_GetTxInterruptMode
****************************************************************************//**
*
*  Returns TX interrupt mask register This register specifies which bits from 
*  TX interrupt request register will trigger an interrupt event.
*
*  \return 
*   Enabled TX interrupt sources (refer to 
*   SCB3_GetTxInterruptSource() function for bit field values).
*   
*******************************************************************************/
#define SCB3_GetTxInterruptMode()   (SCB3_INTR_TX_MASK_REG)


/*******************************************************************************
* Function Name: SCB3_GetTxInterruptSourceMasked
****************************************************************************//**
*
*  Returns TX interrupt masked request register. This register contains logical
*  AND of corresponding bits from TX interrupt request and mask registers.
*  This function is intended to be used in the interrupt service routine to identify 
*  which of enabled TX interrupt sources cause interrupt event.
*
*  \return 
*   Current status of enabled TX interrupt sources (refer to 
*   SCB3_GetTxInterruptSource() function for bit field values).
*
*******************************************************************************/
#define SCB3_GetTxInterruptSourceMasked()   (SCB3_INTR_TX_MASKED_REG)


/*******************************************************************************
* Function Name: SCB3_ClearTxInterruptSource
****************************************************************************//**
*
*  Clears TX interrupt sources in the interrupt request register.
*
*  \param interruptMask: TX interrupt sources to be cleared (refer to 
*   SCB3_GetTxInterruptSource() function for bit field values).
*
*  \sideeffects 
*   The side effects are listed in the table below for each affected interrupt 
*   source. Refer to section TX FIFO interrupt sources for detailed description.
*   - SCB3_INTR_TX_FIFO_LEVEL - Interrupt source is not cleared when 
*     transmitter FIFO has less entries than level.
*   - SCB3_INTR_TX_NOT_FULL - Interrupt source is not cleared when
*     transmitter FIFO has empty entries.
*   - SCB3_INTR_TX_EMPTY - Interrupt source is not cleared when 
*     transmitter FIFO is empty.
*   - SCB3_INTR_TX_UNDERFLOW - Interrupt source is not cleared when 
*     transmitter FIFO is empty and I2C mode with clock stretching is selected. 
*     Put data into the transmitter FIFO before clearing it. This behavior only 
*     applicable for PSoC 4100/PSoC 4200 devices.
*
*******************************************************************************/
#define SCB3_ClearTxInterruptSource(interruptMask)  SCB3_CLEAR_INTR_TX(interruptMask)


/*******************************************************************************
* Function Name: SCB3_SetTxInterrupt
****************************************************************************//**
*
*  Sets RX interrupt sources in the interrupt request register.
*
*  \param interruptMask: RX interrupt sources to set in the RX interrupt request 
*   register (refer to SCB3_GetRxInterruptSource() function for bit 
*   fields values).
*
*******************************************************************************/
#define SCB3_SetTxInterrupt(interruptMask)  SCB3_SET_INTR_TX(interruptMask)

void SCB3_SetTxFifoLevel(uint32 level);


/* APIs to service INTR_MASTER register */
/*******************************************************************************
* Function Name: SCB3_GetMasterInterruptSource
****************************************************************************//**
*
*  Returns Master interrupt request register. This register contains current 
*  status of Master interrupt sources.
*
*  \return 
*   Current status of Master interrupt sources. 
*   Each constant is a bit field value. The value returned may have multiple 
*   bits set to indicate the current status.
*   - SCB3_INTR_MASTER_SPI_DONE - SPI master transfer is complete.
*     Refer to Interrupt sources section for detailed description.
*   - SCB3_INTR_MASTER_I2C_ARB_LOST - I2C master lost arbitration.
*   - SCB3_INTR_MASTER_I2C_NACK - I2C master received negative 
*    acknowledgement (NAK).
*   - SCB3_INTR_MASTER_I2C_ACK - I2C master received acknowledgement.
*   - SCB3_INTR_MASTER_I2C_STOP - I2C master generated STOP.
*   - SCB3_INTR_MASTER_I2C_BUS_ERROR - I2C master bus error 
*     (detection of unexpected START or STOP condition).
*
*******************************************************************************/
#define SCB3_GetMasterInterruptSource() (SCB3_INTR_MASTER_REG)

/*******************************************************************************
* Function Name: SCB3_SetMasterInterruptMode
****************************************************************************//**
*
*  Writes Master interrupt mask register. This register configures which bits 
*  from Master interrupt request register will trigger an interrupt event.
*
*  \param interruptMask: Master interrupt sources to be enabled (refer to 
*   SCB3_GetMasterInterruptSource() function for bit field values).
*
*******************************************************************************/
#define SCB3_SetMasterInterruptMode(interruptMask)  SCB3_WRITE_INTR_MASTER_MASK(interruptMask)

/*******************************************************************************
* Function Name: SCB3_GetMasterInterruptMode
****************************************************************************//**
*
*  Returns Master interrupt mask register This register specifies which bits 
*  from Master interrupt request register will trigger an interrupt event.
*
*  \return 
*   Enabled Master interrupt sources (refer to 
*   SCB3_GetMasterInterruptSource() function for return values).
*
*******************************************************************************/
#define SCB3_GetMasterInterruptMode()   (SCB3_INTR_MASTER_MASK_REG)

/*******************************************************************************
* Function Name: SCB3_GetMasterInterruptSourceMasked
****************************************************************************//**
*
*  Returns Master interrupt masked request register. This register contains 
*  logical AND of corresponding bits from Master interrupt request and mask 
*  registers.
*  This function is intended to be used in the interrupt service routine to 
*  identify which of enabled Master interrupt sources cause interrupt event.
*
*  \return 
*   Current status of enabled Master interrupt sources (refer to 
*   SCB3_GetMasterInterruptSource() function for return values).
*
*******************************************************************************/
#define SCB3_GetMasterInterruptSourceMasked()   (SCB3_INTR_MASTER_MASKED_REG)

/*******************************************************************************
* Function Name: SCB3_ClearMasterInterruptSource
****************************************************************************//**
*
*  Clears Master interrupt sources in the interrupt request register.
*
*  \param interruptMask: Master interrupt sources to be cleared (refer to 
*   SCB3_GetMasterInterruptSource() function for bit field values).
*
*******************************************************************************/
#define SCB3_ClearMasterInterruptSource(interruptMask)  SCB3_CLEAR_INTR_MASTER(interruptMask)

/*******************************************************************************
* Function Name: SCB3_SetMasterInterrupt
****************************************************************************//**
*
*  Sets Master interrupt sources in the interrupt request register.
*
*  \param interruptMask: Master interrupt sources to set in the Master interrupt
*   request register (refer to SCB3_GetMasterInterruptSource() 
*   function for bit field values).
*
*******************************************************************************/
#define SCB3_SetMasterInterrupt(interruptMask)  SCB3_SET_INTR_MASTER(interruptMask)


/* APIs to service INTR_SLAVE register */
/*******************************************************************************
* Function Name: SCB3_GetSlaveInterruptSource
****************************************************************************//**
*
*  Returns Slave interrupt request register. This register contains current 
*  status of Slave interrupt sources.
*
*  \return 
*   Current status of Slave interrupt sources.
*   Each constant is a bit field value. The value returned may have multiple 
*   bits set to indicate the current status.
*   - SCB3_INTR_SLAVE_I2C_ARB_LOST - I2C slave lost arbitration: 
*     the value driven on the SDA line is not the same as the value observed 
*     on the SDA line.
*   - SCB3_INTR_SLAVE_I2C_NACK - I2C slave received negative 
*     acknowledgement (NAK).
*   - SCB3_INTR_SLAVE_I2C_ACK - I2C slave received 
*     acknowledgement (ACK).
*   - SCB3_INTR_SLAVE_I2C_WRITE_STOP - Stop or Repeated Start 
*     event for write transfer intended for this slave (address matching 
*     is performed).
*   - SCB3_INTR_SLAVE_I2C_STOP - Stop or Repeated Start event 
*     for (read or write) transfer intended for this slave (address matching 
*     is performed).
*   - SCB3_INTR_SLAVE_I2C_START - I2C slave received Start 
*     condition.
*   - SCB3_INTR_SLAVE_I2C_ADDR_MATCH - I2C slave received matching 
*     address.
*   - SCB3_INTR_SLAVE_I2C_GENERAL - I2C Slave received general 
*     call address.
*   - SCB3_INTR_SLAVE_I2C_BUS_ERROR - I2C slave bus error (detection 
*      of unexpected Start or Stop condition).
*   - SCB3_INTR_SLAVE_SPI_BUS_ERROR - SPI slave select line is 
*      deselected at an expected time while the SPI transfer.
*
*******************************************************************************/
#define SCB3_GetSlaveInterruptSource()  (SCB3_INTR_SLAVE_REG)

/*******************************************************************************
* Function Name: SCB3_SetSlaveInterruptMode
****************************************************************************//**
*
*  Writes Slave interrupt mask register. 
*  This register configures which bits from Slave interrupt request register 
*  will trigger an interrupt event.
*
*  \param interruptMask: Slave interrupt sources to be enabled (refer to 
*   SCB3_GetSlaveInterruptSource() function for bit field values).
*
*******************************************************************************/
#define SCB3_SetSlaveInterruptMode(interruptMask)   SCB3_WRITE_INTR_SLAVE_MASK(interruptMask)

/*******************************************************************************
* Function Name: SCB3_GetSlaveInterruptMode
****************************************************************************//**
*
*  Returns Slave interrupt mask register.
*  This register specifies which bits from Slave interrupt request register 
*  will trigger an interrupt event.
*
*  \return 
*   Enabled Slave interrupt sources(refer to 
*   SCB3_GetSlaveInterruptSource() function for bit field values).
*
*******************************************************************************/
#define SCB3_GetSlaveInterruptMode()    (SCB3_INTR_SLAVE_MASK_REG)

/*******************************************************************************
* Function Name: SCB3_GetSlaveInterruptSourceMasked
****************************************************************************//**
*
*  Returns Slave interrupt masked request register. This register contains 
*  logical AND of corresponding bits from Slave interrupt request and mask 
*  registers.
*  This function is intended to be used in the interrupt service routine to 
*  identify which of enabled Slave interrupt sources cause interrupt event.
*
*  \return 
*   Current status of enabled Slave interrupt sources (refer to 
*   SCB3_GetSlaveInterruptSource() function for return values).
*
*******************************************************************************/
#define SCB3_GetSlaveInterruptSourceMasked()    (SCB3_INTR_SLAVE_MASKED_REG)

/*******************************************************************************
* Function Name: SCB3_ClearSlaveInterruptSource
****************************************************************************//**
*
*  Clears Slave interrupt sources in the interrupt request register.
*
*  \param interruptMask: Slave interrupt sources to be cleared (refer to 
*   SCB3_GetSlaveInterruptSource() function for return values).
*
*******************************************************************************/
#define SCB3_ClearSlaveInterruptSource(interruptMask)   SCB3_CLEAR_INTR_SLAVE(interruptMask)

/*******************************************************************************
* Function Name: SCB3_SetSlaveInterrupt
****************************************************************************//**
*
*  Sets Slave interrupt sources in the interrupt request register.
*
*  \param interruptMask: Slave interrupt sources to set in the Slave interrupt 
*   request register (refer to SCB3_GetSlaveInterruptSource() 
*   function for return values).
*
*******************************************************************************/
#define SCB3_SetSlaveInterrupt(interruptMask)   SCB3_SET_INTR_SLAVE(interruptMask)

/** @} interrupt */ 


/***************************************
*     Vars with External Linkage
***************************************/

/**
* \addtogroup group_globals
* @{
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
extern uint8 SCB3_initVar;
/** @} globals */

/***************************************
*              Registers
***************************************/

#define SCB3_CTRL_REG               (*(reg32 *) SCB3_SCB__CTRL)
#define SCB3_CTRL_PTR               ( (reg32 *) SCB3_SCB__CTRL)

#define SCB3_STATUS_REG             (*(reg32 *) SCB3_SCB__STATUS)
#define SCB3_STATUS_PTR             ( (reg32 *) SCB3_SCB__STATUS)

#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_SPI_CTRL_REG           (*(reg32 *) SCB3_SCB__SPI_CTRL)
    #define SCB3_SPI_CTRL_PTR           ( (reg32 *) SCB3_SCB__SPI_CTRL)

    #define SCB3_SPI_STATUS_REG         (*(reg32 *) SCB3_SCB__SPI_STATUS)
    #define SCB3_SPI_STATUS_PTR         ( (reg32 *) SCB3_SCB__SPI_STATUS)

    #define SCB3_UART_CTRL_REG          (*(reg32 *) SCB3_SCB__UART_CTRL)
    #define SCB3_UART_CTRL_PTR          ( (reg32 *) SCB3_SCB__UART_CTRL)

    #define SCB3_UART_TX_CTRL_REG       (*(reg32 *) SCB3_SCB__UART_TX_CTRL)
    #define SCB3_UART_TX_CTRL_PTR       ( (reg32 *) SCB3_SCB__UART_TX_CTRL)

    #define SCB3_UART_RX_CTRL_REG       (*(reg32 *) SCB3_SCB__UART_RX_CTRL)
    #define SCB3_UART_RX_CTRL_PTR       ( (reg32 *) SCB3_SCB__UART_RX_CTRL)

    #define SCB3_UART_RX_STATUS_REG     (*(reg32 *) SCB3_SCB__UART_RX_STATUS)
    #define SCB3_UART_RX_STATUS_PTR     ( (reg32 *) SCB3_SCB__UART_RX_STATUS)
#endif /* (!SCB3_CY_SCBIP_V1) */

#if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    #define SCB3_UART_FLOW_CTRL_REG     (*(reg32 *) SCB3_SCB__UART_FLOW_CTRL)
    #define SCB3_UART_FLOW_CTRL_PTR     ( (reg32 *) SCB3_SCB__UART_FLOW_CTRL)
#endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

#define SCB3_I2C_CTRL_REG           (*(reg32 *) SCB3_SCB__I2C_CTRL)
#define SCB3_I2C_CTRL_PTR           ( (reg32 *) SCB3_SCB__I2C_CTRL)

#define SCB3_I2C_STATUS_REG         (*(reg32 *) SCB3_SCB__I2C_STATUS)
#define SCB3_I2C_STATUS_PTR         ( (reg32 *) SCB3_SCB__I2C_STATUS)

#define SCB3_I2C_MASTER_CMD_REG     (*(reg32 *) SCB3_SCB__I2C_M_CMD)
#define SCB3_I2C_MASTER_CMD_PTR     ( (reg32 *) SCB3_SCB__I2C_M_CMD)

#define SCB3_I2C_SLAVE_CMD_REG      (*(reg32 *) SCB3_SCB__I2C_S_CMD)
#define SCB3_I2C_SLAVE_CMD_PTR      ( (reg32 *) SCB3_SCB__I2C_S_CMD)

#define SCB3_I2C_CFG_REG            (*(reg32 *) SCB3_SCB__I2C_CFG)
#define SCB3_I2C_CFG_PTR            ( (reg32 *) SCB3_SCB__I2C_CFG)

#define SCB3_TX_CTRL_REG            (*(reg32 *) SCB3_SCB__TX_CTRL)
#define SCB3_TX_CTRL_PTR            ( (reg32 *) SCB3_SCB__TX_CTRL)

#define SCB3_TX_FIFO_CTRL_REG       (*(reg32 *) SCB3_SCB__TX_FIFO_CTRL)
#define SCB3_TX_FIFO_CTRL_PTR       ( (reg32 *) SCB3_SCB__TX_FIFO_CTRL)

#define SCB3_TX_FIFO_STATUS_REG     (*(reg32 *) SCB3_SCB__TX_FIFO_STATUS)
#define SCB3_TX_FIFO_STATUS_PTR     ( (reg32 *) SCB3_SCB__TX_FIFO_STATUS)

#define SCB3_TX_FIFO_WR_REG         (*(reg32 *) SCB3_SCB__TX_FIFO_WR)
#define SCB3_TX_FIFO_WR_PTR         ( (reg32 *) SCB3_SCB__TX_FIFO_WR)

#define SCB3_RX_CTRL_REG            (*(reg32 *) SCB3_SCB__RX_CTRL)
#define SCB3_RX_CTRL_PTR            ( (reg32 *) SCB3_SCB__RX_CTRL)

#define SCB3_RX_FIFO_CTRL_REG       (*(reg32 *) SCB3_SCB__RX_FIFO_CTRL)
#define SCB3_RX_FIFO_CTRL_PTR       ( (reg32 *) SCB3_SCB__RX_FIFO_CTRL)

#define SCB3_RX_FIFO_STATUS_REG     (*(reg32 *) SCB3_SCB__RX_FIFO_STATUS)
#define SCB3_RX_FIFO_STATUS_PTR     ( (reg32 *) SCB3_SCB__RX_FIFO_STATUS)

#define SCB3_RX_MATCH_REG           (*(reg32 *) SCB3_SCB__RX_MATCH)
#define SCB3_RX_MATCH_PTR           ( (reg32 *) SCB3_SCB__RX_MATCH)

#define SCB3_RX_FIFO_RD_REG         (*(reg32 *) SCB3_SCB__RX_FIFO_RD)
#define SCB3_RX_FIFO_RD_PTR         ( (reg32 *) SCB3_SCB__RX_FIFO_RD)

#define SCB3_RX_FIFO_RD_SILENT_REG  (*(reg32 *) SCB3_SCB__RX_FIFO_RD_SILENT)
#define SCB3_RX_FIFO_RD_SILENT_PTR  ( (reg32 *) SCB3_SCB__RX_FIFO_RD_SILENT)

#ifdef SCB3_SCB__EZ_DATA0
    #define SCB3_EZBUF_DATA0_REG    (*(reg32 *) SCB3_SCB__EZ_DATA0)
    #define SCB3_EZBUF_DATA0_PTR    ( (reg32 *) SCB3_SCB__EZ_DATA0)
#else
    #define SCB3_EZBUF_DATA0_REG    (*(reg32 *) SCB3_SCB__EZ_DATA00)
    #define SCB3_EZBUF_DATA0_PTR    ( (reg32 *) SCB3_SCB__EZ_DATA00)
#endif /* SCB3_SCB__EZ_DATA00 */

#define SCB3_INTR_CAUSE_REG         (*(reg32 *) SCB3_SCB__INTR_CAUSE)
#define SCB3_INTR_CAUSE_PTR         ( (reg32 *) SCB3_SCB__INTR_CAUSE)

#define SCB3_INTR_I2C_EC_REG        (*(reg32 *) SCB3_SCB__INTR_I2C_EC)
#define SCB3_INTR_I2C_EC_PTR        ( (reg32 *) SCB3_SCB__INTR_I2C_EC)

#define SCB3_INTR_I2C_EC_MASK_REG   (*(reg32 *) SCB3_SCB__INTR_I2C_EC_MASK)
#define SCB3_INTR_I2C_EC_MASK_PTR   ( (reg32 *) SCB3_SCB__INTR_I2C_EC_MASK)

#define SCB3_INTR_I2C_EC_MASKED_REG (*(reg32 *) SCB3_SCB__INTR_I2C_EC_MASKED)
#define SCB3_INTR_I2C_EC_MASKED_PTR ( (reg32 *) SCB3_SCB__INTR_I2C_EC_MASKED)

#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_INTR_SPI_EC_REG        (*(reg32 *) SCB3_SCB__INTR_SPI_EC)
    #define SCB3_INTR_SPI_EC_PTR        ( (reg32 *) SCB3_SCB__INTR_SPI_EC)

    #define SCB3_INTR_SPI_EC_MASK_REG   (*(reg32 *) SCB3_SCB__INTR_SPI_EC_MASK)
    #define SCB3_INTR_SPI_EC_MASK_PTR   ( (reg32 *) SCB3_SCB__INTR_SPI_EC_MASK)

    #define SCB3_INTR_SPI_EC_MASKED_REG (*(reg32 *) SCB3_SCB__INTR_SPI_EC_MASKED)
    #define SCB3_INTR_SPI_EC_MASKED_PTR ( (reg32 *) SCB3_SCB__INTR_SPI_EC_MASKED)
#endif /* (!SCB3_CY_SCBIP_V1) */

#define SCB3_INTR_MASTER_REG        (*(reg32 *) SCB3_SCB__INTR_M)
#define SCB3_INTR_MASTER_PTR        ( (reg32 *) SCB3_SCB__INTR_M)

#define SCB3_INTR_MASTER_SET_REG    (*(reg32 *) SCB3_SCB__INTR_M_SET)
#define SCB3_INTR_MASTER_SET_PTR    ( (reg32 *) SCB3_SCB__INTR_M_SET)

#define SCB3_INTR_MASTER_MASK_REG   (*(reg32 *) SCB3_SCB__INTR_M_MASK)
#define SCB3_INTR_MASTER_MASK_PTR   ( (reg32 *) SCB3_SCB__INTR_M_MASK)

#define SCB3_INTR_MASTER_MASKED_REG (*(reg32 *) SCB3_SCB__INTR_M_MASKED)
#define SCB3_INTR_MASTER_MASKED_PTR ( (reg32 *) SCB3_SCB__INTR_M_MASKED)

#define SCB3_INTR_SLAVE_REG         (*(reg32 *) SCB3_SCB__INTR_S)
#define SCB3_INTR_SLAVE_PTR         ( (reg32 *) SCB3_SCB__INTR_S)

#define SCB3_INTR_SLAVE_SET_REG     (*(reg32 *) SCB3_SCB__INTR_S_SET)
#define SCB3_INTR_SLAVE_SET_PTR     ( (reg32 *) SCB3_SCB__INTR_S_SET)

#define SCB3_INTR_SLAVE_MASK_REG    (*(reg32 *) SCB3_SCB__INTR_S_MASK)
#define SCB3_INTR_SLAVE_MASK_PTR    ( (reg32 *) SCB3_SCB__INTR_S_MASK)

#define SCB3_INTR_SLAVE_MASKED_REG  (*(reg32 *) SCB3_SCB__INTR_S_MASKED)
#define SCB3_INTR_SLAVE_MASKED_PTR  ( (reg32 *) SCB3_SCB__INTR_S_MASKED)

#define SCB3_INTR_TX_REG            (*(reg32 *) SCB3_SCB__INTR_TX)
#define SCB3_INTR_TX_PTR            ( (reg32 *) SCB3_SCB__INTR_TX)

#define SCB3_INTR_TX_SET_REG        (*(reg32 *) SCB3_SCB__INTR_TX_SET)
#define SCB3_INTR_TX_SET_PTR        ( (reg32 *) SCB3_SCB__INTR_TX_SET)

#define SCB3_INTR_TX_MASK_REG       (*(reg32 *) SCB3_SCB__INTR_TX_MASK)
#define SCB3_INTR_TX_MASK_PTR       ( (reg32 *) SCB3_SCB__INTR_TX_MASK)

#define SCB3_INTR_TX_MASKED_REG     (*(reg32 *) SCB3_SCB__INTR_TX_MASKED)
#define SCB3_INTR_TX_MASKED_PTR     ( (reg32 *) SCB3_SCB__INTR_TX_MASKED)

#define SCB3_INTR_RX_REG            (*(reg32 *) SCB3_SCB__INTR_RX)
#define SCB3_INTR_RX_PTR            ( (reg32 *) SCB3_SCB__INTR_RX)

#define SCB3_INTR_RX_SET_REG        (*(reg32 *) SCB3_SCB__INTR_RX_SET)
#define SCB3_INTR_RX_SET_PTR        ( (reg32 *) SCB3_SCB__INTR_RX_SET)

#define SCB3_INTR_RX_MASK_REG       (*(reg32 *) SCB3_SCB__INTR_RX_MASK)
#define SCB3_INTR_RX_MASK_PTR       ( (reg32 *) SCB3_SCB__INTR_RX_MASK)

#define SCB3_INTR_RX_MASKED_REG     (*(reg32 *) SCB3_SCB__INTR_RX_MASKED)
#define SCB3_INTR_RX_MASKED_PTR     ( (reg32 *) SCB3_SCB__INTR_RX_MASKED)

/* Defines get from SCB IP parameters. */
#define SCB3_FIFO_SIZE      (8u)  /* TX or RX FIFO size. */
#define SCB3_EZ_DATA_NR     (32u)  /* Number of words in EZ memory. */ 
#define SCB3_ONE_BYTE_WIDTH (8u)            /* Number of bits in one byte. */
#define SCB3_FF_DATA_NR_LOG2_MASK       (0x0Fu)      /* Number of bits to represent a FIFO address. */
#define SCB3_FF_DATA_NR_LOG2_PLUS1_MASK (0x1Fu) /* Number of bits to represent #bytes in FIFO. */


/***************************************
*        Registers Constants
***************************************/

#if (SCB3_SCB_IRQ_INTERNAL)
    #define SCB3_ISR_NUMBER     ((uint8) SCB3_SCB_IRQ__INTC_NUMBER)
    #define SCB3_ISR_PRIORITY   ((uint8) SCB3_SCB_IRQ__INTC_PRIOR_NUM)
#endif /* (SCB3_SCB_IRQ_INTERNAL) */

#if (SCB3_UART_RX_WAKEUP_IRQ)
    #define SCB3_RX_WAKE_ISR_NUMBER     ((uint8) SCB3_RX_WAKEUP_IRQ__INTC_NUMBER)
    #define SCB3_RX_WAKE_ISR_PRIORITY   ((uint8) SCB3_RX_WAKEUP_IRQ__INTC_PRIOR_NUM)
#endif /* (SCB3_UART_RX_WAKEUP_IRQ) */

/* SCB3_CTRL_REG */
#define SCB3_CTRL_OVS_POS           (0u)  /* [3:0]   Oversampling factor                 */
#define SCB3_CTRL_EC_AM_MODE_POS    (8u)  /* [8]     Externally clocked address match    */
#define SCB3_CTRL_EC_OP_MODE_POS    (9u)  /* [9]     Externally clocked operation mode   */
#define SCB3_CTRL_EZBUF_MODE_POS    (10u) /* [10]    EZ buffer is enabled                */
#if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    #define SCB3_CTRL_BYTE_MODE_POS (11u) /* [11]    Determines the number of bits per FIFO data element */
#endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */
#define SCB3_CTRL_ADDR_ACCEPT_POS   (16u) /* [16]    Put matched address in RX FIFO       */
#define SCB3_CTRL_BLOCK_POS         (17u) /* [17]    Ext and Int logic to resolve collide */
#define SCB3_CTRL_MODE_POS          (24u) /* [25:24] Operation mode                       */
#define SCB3_CTRL_ENABLED_POS       (31u) /* [31]    Enable SCB block                     */
#define SCB3_CTRL_OVS_MASK          ((uint32) 0x0Fu)
#define SCB3_CTRL_EC_AM_MODE        ((uint32) 0x01u << SCB3_CTRL_EC_AM_MODE_POS)
#define SCB3_CTRL_EC_OP_MODE        ((uint32) 0x01u << SCB3_CTRL_EC_OP_MODE_POS)
#define SCB3_CTRL_EZBUF_MODE        ((uint32) 0x01u << SCB3_CTRL_EZBUF_MODE_POS)
#if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    #define SCB3_CTRL_BYTE_MODE ((uint32) 0x01u << SCB3_CTRL_BYTE_MODE_POS)
#endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */
#define SCB3_CTRL_ADDR_ACCEPT       ((uint32) 0x01u << SCB3_CTRL_ADDR_ACCEPT_POS)
#define SCB3_CTRL_BLOCK             ((uint32) 0x01u << SCB3_CTRL_BLOCK_POS)
#define SCB3_CTRL_MODE_MASK         ((uint32) 0x03u << SCB3_CTRL_MODE_POS)
#define SCB3_CTRL_MODE_I2C          ((uint32) 0x00u)
#define SCB3_CTRL_MODE_SPI          ((uint32) 0x01u << SCB3_CTRL_MODE_POS)
#define SCB3_CTRL_MODE_UART         ((uint32) 0x02u << SCB3_CTRL_MODE_POS)
#define SCB3_CTRL_ENABLED           ((uint32) 0x01u << SCB3_CTRL_ENABLED_POS)

/* SCB3_STATUS_REG */
#define SCB3_STATUS_EC_BUSY_POS     (0u)  /* [0] Bus busy. Externally clocked logic access to EZ memory */
#define SCB3_STATUS_EC_BUSY         ((uint32) 0x0Fu)

/* SCB3_SPI_CTRL_REG  */
#define SCB3_SPI_CTRL_CONTINUOUS_POS        (0u)  /* [0]     Continuous or Separated SPI data transfers */
#define SCB3_SPI_CTRL_SELECT_PRECEDE_POS    (1u)  /* [1]     Precedes or coincides start of data frame  */
#define SCB3_SPI_CTRL_CPHA_POS              (2u)  /* [2]     SCLK phase                                 */
#define SCB3_SPI_CTRL_CPOL_POS              (3u)  /* [3]     SCLK polarity                              */
#define SCB3_SPI_CTRL_LATE_MISO_SAMPLE_POS  (4u)  /* [4]     Late MISO sample enabled                   */
#if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    #define SCB3_SPI_CTRL_SCLK_CONTINUOUS_POS   (5u)  /* [5]     Enable continuous SCLK generation */
    #define SCB3_SPI_CTRL_SSEL0_POLARITY_POS    (8u)  /* [8]     SS0 polarity                      */
    #define SCB3_SPI_CTRL_SSEL1_POLARITY_POS    (9u)  /* [9]     SS1 polarity                      */
    #define SCB3_SPI_CTRL_SSEL2_POLARITY_POS    (10u) /* [10]    SS2 polarity                      */
    #define SCB3_SPI_CTRL_SSEL3_POLARITY_POS    (11u) /* [11]    SS3 polarity                      */
#endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */
#define SCB3_SPI_CTRL_LOOPBACK_POS          (16u) /* [16]    Local loop-back control enabled            */
#define SCB3_SPI_CTRL_MODE_POS              (24u) /* [25:24] Submode of SPI operation                   */
#define SCB3_SPI_CTRL_SLAVE_SELECT_POS      (26u) /* [27:26] Selects SPI SS signal                      */
#define SCB3_SPI_CTRL_MASTER_MODE_POS       (31u) /* [31]    Master mode enabled                        */
#define SCB3_SPI_CTRL_CONTINUOUS            ((uint32) 0x01u)
#define SCB3_SPI_CTRL_SELECT_PRECEDE        ((uint32) 0x01u << SCB3_SPI_CTRL_SELECT_PRECEDE_POS)
#define SCB3_SPI_CTRL_SCLK_MODE_MASK        ((uint32) 0x03u << SCB3_SPI_CTRL_CPHA_POS)
#define SCB3_SPI_CTRL_CPHA                  ((uint32) 0x01u << SCB3_SPI_CTRL_CPHA_POS)
#define SCB3_SPI_CTRL_CPOL                  ((uint32) 0x01u << SCB3_SPI_CTRL_CPOL_POS)
#define SCB3_SPI_CTRL_LATE_MISO_SAMPLE      ((uint32) 0x01u << \
                                                                    SCB3_SPI_CTRL_LATE_MISO_SAMPLE_POS)
#if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    #define SCB3_SPI_CTRL_SCLK_CONTINUOUS  ((uint32) 0x01u << SCB3_SPI_CTRL_SCLK_CONTINUOUS_POS)
    #define SCB3_SPI_CTRL_SSEL0_POLARITY   ((uint32) 0x01u << SCB3_SPI_CTRL_SSEL0_POLARITY_POS)
    #define SCB3_SPI_CTRL_SSEL1_POLARITY   ((uint32) 0x01u << SCB3_SPI_CTRL_SSEL1_POLARITY_POS)
    #define SCB3_SPI_CTRL_SSEL2_POLARITY   ((uint32) 0x01u << SCB3_SPI_CTRL_SSEL2_POLARITY_POS)
    #define SCB3_SPI_CTRL_SSEL3_POLARITY   ((uint32) 0x01u << SCB3_SPI_CTRL_SSEL3_POLARITY_POS)
    #define SCB3_SPI_CTRL_SSEL_POLARITY_MASK ((uint32)0x0Fu << SCB3_SPI_CTRL_SSEL0_POLARITY_POS)
#endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

#define SCB3_SPI_CTRL_LOOPBACK              ((uint32) 0x01u << SCB3_SPI_CTRL_LOOPBACK_POS)
#define SCB3_SPI_CTRL_MODE_MASK             ((uint32) 0x03u << SCB3_SPI_CTRL_MODE_POS)
#define SCB3_SPI_CTRL_MODE_MOTOROLA         ((uint32) 0x00u)
#define SCB3_SPI_CTRL_MODE_TI               ((uint32) 0x01u << SCB3_CTRL_MODE_POS)
#define SCB3_SPI_CTRL_MODE_NS               ((uint32) 0x02u << SCB3_CTRL_MODE_POS)
#define SCB3_SPI_CTRL_SLAVE_SELECT_MASK     ((uint32) 0x03u << SCB3_SPI_CTRL_SLAVE_SELECT_POS)
#define SCB3_SPI_CTRL_SLAVE_SELECT0         ((uint32) 0x00u)
#define SCB3_SPI_CTRL_SLAVE_SELECT1         ((uint32) 0x01u << SCB3_SPI_CTRL_SLAVE_SELECT_POS)
#define SCB3_SPI_CTRL_SLAVE_SELECT2         ((uint32) 0x02u << SCB3_SPI_CTRL_SLAVE_SELECT_POS)
#define SCB3_SPI_CTRL_SLAVE_SELECT3         ((uint32) 0x03u << SCB3_SPI_CTRL_SLAVE_SELECT_POS)
#define SCB3_SPI_CTRL_MASTER                ((uint32) 0x01u << SCB3_SPI_CTRL_MASTER_MODE_POS)
#define SCB3_SPI_CTRL_SLAVE                 ((uint32) 0x00u)

/* SCB3_SPI_STATUS_REG  */
#define SCB3_SPI_STATUS_BUS_BUSY_POS    (0u)  /* [0]    Bus busy - slave selected */
#define SCB3_SPI_STATUS_EZBUF_ADDR_POS  (8u)  /* [15:8] EzAddress                 */
#define SCB3_SPI_STATUS_BUS_BUSY        ((uint32) 0x01u)
#define SCB3_SPI_STATUS_EZBUF_ADDR_MASK ((uint32) 0xFFu << SCB3_I2C_STATUS_EZBUF_ADDR_POS)

/* SCB3_UART_CTRL */
#define SCB3_UART_CTRL_LOOPBACK_POS         (16u) /* [16] Loop-back    */
#define SCB3_UART_CTRL_MODE_POS             (24u) /* [24] UART subMode */
#define SCB3_UART_CTRL_LOOPBACK             ((uint32) 0x01u << SCB3_UART_CTRL_LOOPBACK_POS)
#define SCB3_UART_CTRL_MODE_UART_STD        ((uint32) 0x00u)
#define SCB3_UART_CTRL_MODE_UART_SMARTCARD  ((uint32) 0x01u << SCB3_UART_CTRL_MODE_POS)
#define SCB3_UART_CTRL_MODE_UART_IRDA       ((uint32) 0x02u << SCB3_UART_CTRL_MODE_POS)
#define SCB3_UART_CTRL_MODE_MASK            ((uint32) 0x03u << SCB3_UART_CTRL_MODE_POS)

/* SCB3_UART_TX_CTRL */
#define SCB3_UART_TX_CTRL_STOP_BITS_POS         (0u)  /* [2:0] Stop bits: (Stop bits + 1) * 0.5 period */
#define SCB3_UART_TX_CTRL_PARITY_POS            (4u)  /* [4]   Parity bit                              */
#define SCB3_UART_TX_CTRL_PARITY_ENABLED_POS    (5u)  /* [5]   Parity enable                           */
#define SCB3_UART_TX_CTRL_RETRY_ON_NACK_POS     (8u)  /* [8]   Smart Card: re-send frame on NACK       */
#define SCB3_UART_TX_CTRL_ONE_STOP_BIT          ((uint32) 0x01u)
#define SCB3_UART_TX_CTRL_ONE_HALF_STOP_BITS    ((uint32) 0x02u)
#define SCB3_UART_TX_CTRL_TWO_STOP_BITS         ((uint32) 0x03u)
#define SCB3_UART_TX_CTRL_STOP_BITS_MASK        ((uint32) 0x07u)
#define SCB3_UART_TX_CTRL_PARITY                ((uint32) 0x01u << \
                                                                    SCB3_UART_TX_CTRL_PARITY_POS)
#define SCB3_UART_TX_CTRL_PARITY_ENABLED        ((uint32) 0x01u << \
                                                                    SCB3_UART_TX_CTRL_PARITY_ENABLED_POS)
#define SCB3_UART_TX_CTRL_RETRY_ON_NACK         ((uint32) 0x01u << \
                                                                    SCB3_UART_TX_CTRL_RETRY_ON_NACK_POS)

/* SCB3_UART_RX_CTRL */
#define SCB3_UART_RX_CTRL_STOP_BITS_POS             (0u)  /* [2:0] Stop bits: (Stop bits + 1) * 0.5 period*/
#define SCB3_UART_RX_CTRL_PARITY_POS                (4u)  /* [4]   Parity bit                             */
#define SCB3_UART_RX_CTRL_PARITY_ENABLED_POS        (5u)  /* [5]   Parity enable                          */
#define SCB3_UART_RX_CTRL_POLARITY_POS              (6u)  /* [6]   IrDA: inverts polarity of RX signal    */
#define SCB3_UART_RX_CTRL_DROP_ON_PARITY_ERR_POS    (8u)  /* [8]   Drop and lost RX FIFO on parity error  */
#define SCB3_UART_RX_CTRL_DROP_ON_FRAME_ERR_POS     (9u)  /* [9]   Drop and lost RX FIFO on frame error   */
#define SCB3_UART_RX_CTRL_MP_MODE_POS               (10u) /* [10]  Multi-processor mode                   */
#define SCB3_UART_RX_CTRL_LIN_MODE_POS              (12u) /* [12]  Lin mode: applicable for UART Standard */
#define SCB3_UART_RX_CTRL_SKIP_START_POS            (13u) /* [13]  Skip start not: only for UART Standard */
#define SCB3_UART_RX_CTRL_BREAK_WIDTH_POS           (16u) /* [19:16]  Break width: (Break width + 1)      */
#define SCB3_UART_TX_CTRL_ONE_STOP_BIT              ((uint32) 0x01u)
#define SCB3_UART_TX_CTRL_ONE_HALF_STOP_BITS        ((uint32) 0x02u)
#define SCB3_UART_TX_CTRL_TWO_STOP_BITS             ((uint32) 0x03u)
#define SCB3_UART_RX_CTRL_STOP_BITS_MASK            ((uint32) 0x07u)
#define SCB3_UART_RX_CTRL_PARITY                    ((uint32) 0x01u << \
                                                                    SCB3_UART_RX_CTRL_PARITY_POS)
#define SCB3_UART_RX_CTRL_PARITY_ENABLED            ((uint32) 0x01u << \
                                                                    SCB3_UART_RX_CTRL_PARITY_ENABLED_POS)
#define SCB3_UART_RX_CTRL_POLARITY                  ((uint32) 0x01u << \
                                                                    SCB3_UART_RX_CTRL_POLARITY_POS)
#define SCB3_UART_RX_CTRL_DROP_ON_PARITY_ERR        ((uint32) 0x01u << \
                                                                   SCB3_UART_RX_CTRL_DROP_ON_PARITY_ERR_POS)
#define SCB3_UART_RX_CTRL_DROP_ON_FRAME_ERR         ((uint32) 0x01u << \
                                                                    SCB3_UART_RX_CTRL_DROP_ON_FRAME_ERR_POS)
#define SCB3_UART_RX_CTRL_MP_MODE                   ((uint32) 0x01u << \
                                                                    SCB3_UART_RX_CTRL_MP_MODE_POS)
#define SCB3_UART_RX_CTRL_LIN_MODE                  ((uint32) 0x01u << \
                                                                    SCB3_UART_RX_CTRL_LIN_MODE_POS)
#define SCB3_UART_RX_CTRL_SKIP_START                ((uint32) 0x01u << \
                                                                    SCB3_UART_RX_CTRL_SKIP_START_POS)
#define SCB3_UART_RX_CTRL_BREAK_WIDTH_MASK          ((uint32) 0x0Fu << \
                                                                    SCB3_UART_RX_CTRL_BREAK_WIDTH_POS)
/* SCB3_UART_RX_STATUS_REG */
#define SCB3_UART_RX_STATUS_BR_COUNTER_POS     (0u)  /* [11:0] Baud Rate counter */
#define SCB3_UART_RX_STATUS_BR_COUNTER_MASK    ((uint32) 0xFFFu)

#if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    /* SCB3_UART_FLOW_CTRL_REG */
    #define SCB3_UART_FLOW_CTRL_TRIGGER_LEVEL_POS    (0u)  /* [7:0] RTS RX FIFO trigger level         */
    #define SCB3_UART_FLOW_CTRL_RTS_POLARITY_POS     (16u) /* [16]  Polarity of the RTS output signal */
    #define SCB3_UART_FLOW_CTRL_CTS_POLARITY_POS     (24u) /* [24]  Polarity of the CTS input signal  */
    #define SCB3_UART_FLOW_CTRL_CTS_ENABLED_POS      (25u) /* [25]  Enable CTS signal                 */
    #define SCB3_UART_FLOW_CTRL_TRIGGER_LEVEL_MASK   ((uint32) SCB3_FF_DATA_NR_LOG2_MASK)
    #define SCB3_UART_FLOW_CTRL_RTS_POLARITY         ((uint32) 0x01u << \
                                                                       SCB3_UART_FLOW_CTRL_RTS_POLARITY_POS)
    #define SCB3_UART_FLOW_CTRL_CTS_POLARITY         ((uint32) 0x01u << \
                                                                       SCB3_UART_FLOW_CTRL_CTS_POLARITY_POS)
    #define SCB3_UART_FLOW_CTRL_CTS_ENABLE           ((uint32) 0x01u << \
                                                                       SCB3_UART_FLOW_CTRL_CTS_ENABLED_POS)
#endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

/* SCB3_I2C_CTRL */
#define SCB3_I2C_CTRL_HIGH_PHASE_OVS_POS           (0u)   /* [3:0] Oversampling factor high: master only */
#define SCB3_I2C_CTRL_LOW_PHASE_OVS_POS            (4u)   /* [7:4] Oversampling factor low:  master only */
#define SCB3_I2C_CTRL_M_READY_DATA_ACK_POS         (8u)   /* [8]   Master ACKs data while RX FIFO != FULL*/
#define SCB3_I2C_CTRL_M_NOT_READY_DATA_NACK_POS    (9u)   /* [9]   Master NACKs data if RX FIFO ==  FULL */
#define SCB3_I2C_CTRL_S_GENERAL_IGNORE_POS         (11u)  /* [11]  Slave ignores General call            */
#define SCB3_I2C_CTRL_S_READY_ADDR_ACK_POS         (12u)  /* [12]  Slave ACKs Address if RX FIFO != FULL */
#define SCB3_I2C_CTRL_S_READY_DATA_ACK_POS         (13u)  /* [13]  Slave ACKs data while RX FIFO == FULL */
#define SCB3_I2C_CTRL_S_NOT_READY_ADDR_NACK_POS    (14u)  /* [14]  Slave NACKs address if RX FIFO == FULL*/
#define SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK_POS    (15u)  /* [15]  Slave NACKs data if RX FIFO is  FULL  */
#define SCB3_I2C_CTRL_LOOPBACK_POS                 (16u)  /* [16]  Loop-back                             */
#define SCB3_I2C_CTRL_SLAVE_MODE_POS               (30u)  /* [30]  Slave mode enabled                    */
#define SCB3_I2C_CTRL_MASTER_MODE_POS              (31u)  /* [31]  Master mode enabled                   */
#define SCB3_I2C_CTRL_HIGH_PHASE_OVS_MASK  ((uint32) 0x0Fu)
#define SCB3_I2C_CTRL_LOW_PHASE_OVS_MASK   ((uint32) 0x0Fu << \
                                                                SCB3_I2C_CTRL_LOW_PHASE_OVS_POS)
#define SCB3_I2C_CTRL_M_READY_DATA_ACK      ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_M_READY_DATA_ACK_POS)
#define SCB3_I2C_CTRL_M_NOT_READY_DATA_NACK ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_M_NOT_READY_DATA_NACK_POS)
#define SCB3_I2C_CTRL_S_GENERAL_IGNORE      ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_S_GENERAL_IGNORE_POS)
#define SCB3_I2C_CTRL_S_READY_ADDR_ACK      ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_S_READY_ADDR_ACK_POS)
#define SCB3_I2C_CTRL_S_READY_DATA_ACK      ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_S_READY_DATA_ACK_POS)
#define SCB3_I2C_CTRL_S_NOT_READY_ADDR_NACK ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_S_NOT_READY_ADDR_NACK_POS)
#define SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK_POS)
#define SCB3_I2C_CTRL_LOOPBACK              ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_LOOPBACK_POS)
#define SCB3_I2C_CTRL_SLAVE_MODE            ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_SLAVE_MODE_POS)
#define SCB3_I2C_CTRL_MASTER_MODE           ((uint32) 0x01u << \
                                                                SCB3_I2C_CTRL_MASTER_MODE_POS)
#define SCB3_I2C_CTRL_SLAVE_MASTER_MODE_MASK    ((uint32) 0x03u << \
                                                                SCB3_I2C_CTRL_SLAVE_MODE_POS)

/* SCB3_I2C_STATUS_REG  */
#define SCB3_I2C_STATUS_BUS_BUSY_POS    (0u)  /* [0]    Bus busy: internally clocked */
#define SCB3_I2C_STATUS_S_READ_POS      (4u)  /* [4]    Slave is read by master      */
#define SCB3_I2C_STATUS_M_READ_POS      (5u)  /* [5]    Master reads Slave           */
#define SCB3_I2C_STATUS_EZBUF_ADDR_POS  (8u)  /* [15:8] EZAddress                    */
#define SCB3_I2C_STATUS_BUS_BUSY        ((uint32) 0x01u)
#define SCB3_I2C_STATUS_S_READ          ((uint32) 0x01u << SCB3_I2C_STATUS_S_READ_POS)
#define SCB3_I2C_STATUS_M_READ          ((uint32) 0x01u << SCB3_I2C_STATUS_M_READ_POS)
#define SCB3_I2C_STATUS_EZBUF_ADDR_MASK ((uint32) 0xFFu << SCB3_I2C_STATUS_EZBUF_ADDR_POS)

/* SCB3_I2C_MASTER_CMD_REG */
#define SCB3_I2C_MASTER_CMD_M_START_POS             (0u)  /* [0] Master generate Start                */
#define SCB3_I2C_MASTER_CMD_M_START_ON_IDLE_POS     (1u)  /* [1] Master generate Start if bus is free */
#define SCB3_I2C_MASTER_CMD_M_ACK_POS               (2u)  /* [2] Master generate ACK                  */
#define SCB3_I2C_MASTER_CMD_M_NACK_POS              (3u)  /* [3] Master generate NACK                 */
#define SCB3_I2C_MASTER_CMD_M_STOP_POS              (4u)  /* [4] Master generate Stop                 */
#define SCB3_I2C_MASTER_CMD_M_START         ((uint32) 0x01u)
#define SCB3_I2C_MASTER_CMD_M_START_ON_IDLE ((uint32) 0x01u << \
                                                                   SCB3_I2C_MASTER_CMD_M_START_ON_IDLE_POS)
#define SCB3_I2C_MASTER_CMD_M_ACK           ((uint32) 0x01u << \
                                                                   SCB3_I2C_MASTER_CMD_M_ACK_POS)
#define SCB3_I2C_MASTER_CMD_M_NACK          ((uint32) 0x01u << \
                                                                    SCB3_I2C_MASTER_CMD_M_NACK_POS)
#define SCB3_I2C_MASTER_CMD_M_STOP          ((uint32) 0x01u << \
                                                                    SCB3_I2C_MASTER_CMD_M_STOP_POS)

/* SCB3_I2C_SLAVE_CMD_REG  */
#define SCB3_I2C_SLAVE_CMD_S_ACK_POS    (0u)  /* [0] Slave generate ACK  */
#define SCB3_I2C_SLAVE_CMD_S_NACK_POS   (1u)  /* [1] Slave generate NACK */
#define SCB3_I2C_SLAVE_CMD_S_ACK        ((uint32) 0x01u)
#define SCB3_I2C_SLAVE_CMD_S_NACK       ((uint32) 0x01u << SCB3_I2C_SLAVE_CMD_S_NACK_POS)

#define SCB3_I2C_SLAVE_CMD_S_ACK_POS    (0u)  /* [0] Slave generate ACK  */
#define SCB3_I2C_SLAVE_CMD_S_NACK_POS   (1u)  /* [1] Slave generate NACK */
#define SCB3_I2C_SLAVE_CMD_S_ACK        ((uint32) 0x01u)
#define SCB3_I2C_SLAVE_CMD_S_NACK       ((uint32) 0x01u << SCB3_I2C_SLAVE_CMD_S_NACK_POS)

/* SCB3_I2C_CFG_REG */
#if (SCB3_CY_SCBIP_V0)
#define SCB3_I2C_CFG_SDA_FILT_HYS_POS           (0u)  /* [1:0]   Trim bits for the I2C SDA filter         */
#define SCB3_I2C_CFG_SDA_FILT_TRIM_POS          (2u)  /* [3:2]   Trim bits for the I2C SDA filter         */
#define SCB3_I2C_CFG_SCL_FILT_HYS_POS           (4u)  /* [5:4]   Trim bits for the I2C SCL filter         */
#define SCB3_I2C_CFG_SCL_FILT_TRIM_POS          (6u)  /* [7:6]   Trim bits for the I2C SCL filter         */
#define SCB3_I2C_CFG_SDA_FILT_OUT_HYS_POS       (8u)  /* [9:8]   Trim bits for I2C SDA filter output path */
#define SCB3_I2C_CFG_SDA_FILT_OUT_TRIM_POS      (10u) /* [11:10] Trim bits for I2C SDA filter output path */
#define SCB3_I2C_CFG_SDA_FILT_HS_POS            (16u) /* [16]    '0': 50 ns filter, '1': 10 ns filter     */
#define SCB3_I2C_CFG_SDA_FILT_ENABLED_POS       (17u) /* [17]    I2C SDA filter enabled                   */
#define SCB3_I2C_CFG_SCL_FILT_HS_POS            (24u) /* [24]    '0': 50 ns filter, '1': 10 ns filter     */
#define SCB3_I2C_CFG_SCL_FILT_ENABLED_POS       (25u) /* [25]    I2C SCL filter enabled                   */
#define SCB3_I2C_CFG_SDA_FILT_OUT_HS_POS        (26u) /* [26]    '0': 50 ns filter, '1': 10 ns filter     */
#define SCB3_I2C_CFG_SDA_FILT_OUT_ENABLED_POS   (27u) /* [27]    I2C SDA output delay filter enabled      */
#define SCB3_I2C_CFG_SDA_FILT_HYS_MASK          ((uint32) 0x03u)
#define SCB3_I2C_CFG_SDA_FILT_TRIM_MASK         ((uint32) 0x03u << \
                                                                SCB3_I2C_CFG_SDA_FILT_TRIM_POS)
#define SCB3_I2C_CFG_SCL_FILT_HYS_MASK          ((uint32) 0x03u << \
                                                                SCB3_I2C_CFG_SCL_FILT_HYS_POS)
#define SCB3_I2C_CFG_SCL_FILT_TRIM_MASK         ((uint32) 0x03u << \
                                                                SCB3_I2C_CFG_SCL_FILT_TRIM_POS)
#define SCB3_I2C_CFG_SDA_FILT_OUT_HYS_MASK      ((uint32) 0x03u << \
                                                                SCB3_I2C_CFG_SDA_FILT_OUT_HYS_POS)
#define SCB3_I2C_CFG_SDA_FILT_OUT_TRIM_MASK     ((uint32) 0x03u << \
                                                                SCB3_I2C_CFG_SDA_FILT_OUT_TRIM_POS)
#define SCB3_I2C_CFG_SDA_FILT_HS                ((uint32) 0x01u << \
                                                                SCB3_I2C_CFG_SDA_FILT_HS_POS)
#define SCB3_I2C_CFG_SDA_FILT_ENABLED           ((uint32) 0x01u << \
                                                                SCB3_I2C_CFG_SDA_FILT_ENABLED_POS)
#define SCB3_I2C_CFG_SCL_FILT_HS                ((uint32) 0x01u << \
                                                                SCB3_I2C_CFG_SCL_FILT_HS_POS)
#define SCB3_I2C_CFG_SCL_FILT_ENABLED           ((uint32) 0x01u << \
                                                                SCB3_I2C_CFG_SCL_FILT_ENABLED_POS)
#define SCB3_I2C_CFG_SDA_FILT_OUT_HS            ((uint32) 0x01u << \
                                                                SCB3_I2C_CFG_SDA_FILT_OUT_HS_POS)
#define SCB3_I2C_CFG_SDA_FILT_OUT_ENABLED       ((uint32) 0x01u << \
                                                                SCB3_I2C_CFG_SDA_FILT_OUT_ENABLED_POS)
#else
#define SCB3_I2C_CFG_SDA_IN_FILT_TRIM_POS   (0u)  /* [1:0] Trim bits for "i2c_sda_in" 50 ns filter */
#define SCB3_I2C_CFG_SDA_IN_FILT_SEL_POS    (4u)  /* [4]   "i2c_sda_in" filter delay: 0 ns and 50 ns */
#define SCB3_I2C_CFG_SCL_IN_FILT_TRIM_POS   (8u)  /* [9:8] Trim bits for "i2c_scl_in" 50 ns filter */
#define SCB3_I2C_CFG_SCL_IN_FILT_SEL_POS    (12u) /* [12]  "i2c_scl_in" filter delay: 0 ns and 50 ns */
#define SCB3_I2C_CFG_SDA_OUT_FILT0_TRIM_POS (16u) /* [17:16] Trim bits for "i2c_sda_out" 50 ns filter 0 */
#define SCB3_I2C_CFG_SDA_OUT_FILT1_TRIM_POS (18u) /* [19:18] Trim bits for "i2c_sda_out" 50 ns filter 1 */
#define SCB3_I2C_CFG_SDA_OUT_FILT2_TRIM_POS (20u) /* [21:20] Trim bits for "i2c_sda_out" 50 ns filter 2 */
#define SCB3_I2C_CFG_SDA_OUT_FILT_SEL_POS   (28u) /* [29:28] Cumulative "i2c_sda_out" filter delay: */

#define SCB3_I2C_CFG_SDA_IN_FILT_TRIM_MASK  ((uint32) 0x03u)
#define SCB3_I2C_CFG_SDA_IN_FILT_SEL        ((uint32) 0x01u << SCB3_I2C_CFG_SDA_IN_FILT_SEL_POS)
#define SCB3_I2C_CFG_SCL_IN_FILT_TRIM_MASK  ((uint32) 0x03u << \
                                                            SCB3_I2C_CFG_SCL_IN_FILT_TRIM_POS)
#define SCB3_I2C_CFG_SCL_IN_FILT_SEL        ((uint32) 0x01u << SCB3_I2C_CFG_SCL_IN_FILT_SEL_POS)
#define SCB3_I2C_CFG_SDA_OUT_FILT0_TRIM_MASK ((uint32) 0x03u << \
                                                            SCB3_I2C_CFG_SDA_OUT_FILT0_TRIM_POS)
#define SCB3_I2C_CFG_SDA_OUT_FILT1_TRIM_MASK ((uint32) 0x03u << \
                                                            SCB3_I2C_CFG_SDA_OUT_FILT1_TRIM_POS)
#define SCB3_I2C_CFG_SDA_OUT_FILT2_TRIM_MASK ((uint32) 0x03u << \
                                                            SCB3_I2C_CFG_SDA_OUT_FILT2_TRIM_POS)
#define SCB3_I2C_CFG_SDA_OUT_FILT_SEL_MASK   ((uint32) 0x03u << \
                                                            SCB3_I2C_CFG_SDA_OUT_FILT_SEL_POS)
#endif /* (SCB3_CY_SCBIP_V0) */


/* SCB3_TX_CTRL_REG */
#define SCB3_TX_CTRL_DATA_WIDTH_POS     (0u)  /* [3:0] Data frame width: (Data width - 1) */
#define SCB3_TX_CTRL_MSB_FIRST_POS      (8u)  /* [8]   MSB first shifter-out             */
#define SCB3_TX_CTRL_ENABLED_POS        (31u) /* [31]  Transmitter enabled               */
#define SCB3_TX_CTRL_DATA_WIDTH_MASK    ((uint32) 0x0Fu)
#define SCB3_TX_CTRL_MSB_FIRST          ((uint32) 0x01u << SCB3_TX_CTRL_MSB_FIRST_POS)
#define SCB3_TX_CTRL_LSB_FIRST          ((uint32) 0x00u)
#define SCB3_TX_CTRL_ENABLED            ((uint32) 0x01u << SCB3_TX_CTRL_ENABLED_POS)

/* SCB3_TX_CTRL_FIFO_REG */
#define SCB3_TX_FIFO_CTRL_TRIGGER_LEVEL_POS     (0u)  /* [2:0] Trigger level                              */
#define SCB3_TX_FIFO_CTRL_CLEAR_POS             (16u) /* [16]  Clear TX FIFO: cleared after set           */
#define SCB3_TX_FIFO_CTRL_FREEZE_POS            (17u) /* [17]  Freeze TX FIFO: HW do not inc read pointer */
#define SCB3_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK    ((uint32) SCB3_FF_DATA_NR_LOG2_MASK)
#define SCB3_TX_FIFO_CTRL_CLEAR                 ((uint32) 0x01u << SCB3_TX_FIFO_CTRL_CLEAR_POS)
#define SCB3_TX_FIFO_CTRL_FREEZE                ((uint32) 0x01u << SCB3_TX_FIFO_CTRL_FREEZE_POS)

/* SCB3_TX_FIFO_STATUS_REG */
#define SCB3_TX_FIFO_STATUS_USED_POS    (0u)  /* [3:0]   Amount of entries in TX FIFO */
#define SCB3_TX_FIFO_SR_VALID_POS       (15u) /* [15]    Shifter status of TX FIFO    */
#define SCB3_TX_FIFO_STATUS_RD_PTR_POS  (16u) /* [18:16] TX FIFO read pointer         */
#define SCB3_TX_FIFO_STATUS_WR_PTR_POS  (24u) /* [26:24] TX FIFO write pointer        */
#define SCB3_TX_FIFO_STATUS_USED_MASK   ((uint32) SCB3_FF_DATA_NR_LOG2_PLUS1_MASK)
#define SCB3_TX_FIFO_SR_VALID           ((uint32) 0x01u << SCB3_TX_FIFO_SR_VALID_POS)
#define SCB3_TX_FIFO_STATUS_RD_PTR_MASK ((uint32) SCB3_FF_DATA_NR_LOG2_MASK << \
                                                                    SCB3_TX_FIFO_STATUS_RD_PTR_POS)
#define SCB3_TX_FIFO_STATUS_WR_PTR_MASK ((uint32) SCB3_FF_DATA_NR_LOG2_MASK << \
                                                                    SCB3_TX_FIFO_STATUS_WR_PTR_POS)

/* SCB3_TX_FIFO_WR_REG */
#define SCB3_TX_FIFO_WR_POS    (0u)  /* [15:0] Data written into TX FIFO */
#define SCB3_TX_FIFO_WR_MASK   ((uint32) 0xFFu)

/* SCB3_RX_CTRL_REG */
#define SCB3_RX_CTRL_DATA_WIDTH_POS     (0u)  /* [3:0] Data frame width: (Data width - 1) */
#define SCB3_RX_CTRL_MSB_FIRST_POS      (8u)  /* [8]   MSB first shifter-out             */
#define SCB3_RX_CTRL_MEDIAN_POS         (9u)  /* [9]   Median filter                     */
#define SCB3_RX_CTRL_ENABLED_POS        (31u) /* [31]  Receiver enabled                  */
#define SCB3_RX_CTRL_DATA_WIDTH_MASK    ((uint32) 0x0Fu)
#define SCB3_RX_CTRL_MSB_FIRST          ((uint32) 0x01u << SCB3_RX_CTRL_MSB_FIRST_POS)
#define SCB3_RX_CTRL_LSB_FIRST          ((uint32) 0x00u)
#define SCB3_RX_CTRL_MEDIAN             ((uint32) 0x01u << SCB3_RX_CTRL_MEDIAN_POS)
#define SCB3_RX_CTRL_ENABLED            ((uint32) 0x01u << SCB3_RX_CTRL_ENABLED_POS)


/* SCB3_RX_FIFO_CTRL_REG */
#define SCB3_RX_FIFO_CTRL_TRIGGER_LEVEL_POS     (0u)   /* [2:0] Trigger level                            */
#define SCB3_RX_FIFO_CTRL_CLEAR_POS             (16u)  /* [16]  Clear RX FIFO: clear after set           */
#define SCB3_RX_FIFO_CTRL_FREEZE_POS            (17u)  /* [17]  Freeze RX FIFO: HW writes has not effect */
#define SCB3_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK    ((uint32) SCB3_FF_DATA_NR_LOG2_MASK)
#define SCB3_RX_FIFO_CTRL_CLEAR                 ((uint32) 0x01u << SCB3_RX_FIFO_CTRL_CLEAR_POS)
#define SCB3_RX_FIFO_CTRL_FREEZE                ((uint32) 0x01u << SCB3_RX_FIFO_CTRL_FREEZE_POS)

/* SCB3_RX_FIFO_STATUS_REG */
#define SCB3_RX_FIFO_STATUS_USED_POS    (0u)   /* [3:0]   Amount of entries in RX FIFO */
#define SCB3_RX_FIFO_SR_VALID_POS       (15u)  /* [15]    Shifter status of RX FIFO    */
#define SCB3_RX_FIFO_STATUS_RD_PTR_POS  (16u)  /* [18:16] RX FIFO read pointer         */
#define SCB3_RX_FIFO_STATUS_WR_PTR_POS  (24u)  /* [26:24] RX FIFO write pointer        */
#define SCB3_RX_FIFO_STATUS_USED_MASK   ((uint32) SCB3_FF_DATA_NR_LOG2_PLUS1_MASK)
#define SCB3_RX_FIFO_SR_VALID           ((uint32) 0x01u << SCB3_RX_FIFO_SR_VALID_POS)
#define SCB3_RX_FIFO_STATUS_RD_PTR_MASK ((uint32) SCB3_FF_DATA_NR_LOG2_MASK << \
                                                                    SCB3_RX_FIFO_STATUS_RD_PTR_POS)
#define SCB3_RX_FIFO_STATUS_WR_PTR_MASK ((uint32) SCB3_FF_DATA_NR_LOG2_MASK << \
                                                                    SCB3_RX_FIFO_STATUS_WR_PTR_POS)

/* SCB3_RX_MATCH_REG */
#define SCB3_RX_MATCH_ADDR_POS     (0u)  /* [7:0]   Slave address                        */
#define SCB3_RX_MATCH_MASK_POS     (16u) /* [23:16] Slave address mask: 0 - doesn't care */
#define SCB3_RX_MATCH_ADDR_MASK    ((uint32) 0xFFu)
#define SCB3_RX_MATCH_MASK_MASK    ((uint32) 0xFFu << SCB3_RX_MATCH_MASK_POS)

/* SCB3_RX_FIFO_WR_REG */
#define SCB3_RX_FIFO_RD_POS    (0u)  /* [15:0] Data read from RX FIFO */
#define SCB3_RX_FIFO_RD_MASK   ((uint32) 0xFFu)

/* SCB3_RX_FIFO_RD_SILENT_REG */
#define SCB3_RX_FIFO_RD_SILENT_POS     (0u)  /* [15:0] Data read from RX FIFO: not remove data from FIFO */
#define SCB3_RX_FIFO_RD_SILENT_MASK    ((uint32) 0xFFu)

/* SCB3_RX_FIFO_RD_SILENT_REG */
#define SCB3_RX_FIFO_RD_SILENT_POS     (0u)  /* [15:0] Data read from RX FIFO: not remove data from FIFO */
#define SCB3_RX_FIFO_RD_SILENT_MASK    ((uint32) 0xFFu)

/* SCB3_EZBUF_DATA_REG */
#define SCB3_EZBUF_DATA_POS   (0u)  /* [7:0] Data from EZ Memory */
#define SCB3_EZBUF_DATA_MASK  ((uint32) 0xFFu)

/*  SCB3_INTR_CAUSE_REG */
#define SCB3_INTR_CAUSE_MASTER_POS  (0u)  /* [0] Master interrupt active                 */
#define SCB3_INTR_CAUSE_SLAVE_POS   (1u)  /* [1] Slave interrupt active                  */
#define SCB3_INTR_CAUSE_TX_POS      (2u)  /* [2] Transmitter interrupt active            */
#define SCB3_INTR_CAUSE_RX_POS      (3u)  /* [3] Receiver interrupt active               */
#define SCB3_INTR_CAUSE_I2C_EC_POS  (4u)  /* [4] Externally clock I2C interrupt active   */
#define SCB3_INTR_CAUSE_SPI_EC_POS  (5u)  /* [5] Externally clocked SPI interrupt active */
#define SCB3_INTR_CAUSE_MASTER      ((uint32) 0x01u)
#define SCB3_INTR_CAUSE_SLAVE       ((uint32) 0x01u << SCB3_INTR_CAUSE_SLAVE_POS)
#define SCB3_INTR_CAUSE_TX          ((uint32) 0x01u << SCB3_INTR_CAUSE_TX_POS)
#define SCB3_INTR_CAUSE_RX          ((uint32) 0x01u << SCB3_INTR_CAUSE_RX_POS)
#define SCB3_INTR_CAUSE_I2C_EC      ((uint32) 0x01u << SCB3_INTR_CAUSE_I2C_EC_POS)
#define SCB3_INTR_CAUSE_SPI_EC      ((uint32) 0x01u << SCB3_INTR_CAUSE_SPI_EC_POS)

/* SCB3_INTR_SPI_EC_REG, SCB3_INTR_SPI_EC_MASK_REG, SCB3_INTR_SPI_EC_MASKED_REG */
#define SCB3_INTR_SPI_EC_WAKE_UP_POS          (0u)  /* [0] Address match: triggers wakeup of chip */
#define SCB3_INTR_SPI_EC_EZBUF_STOP_POS       (1u)  /* [1] Externally clocked Stop detected       */
#define SCB3_INTR_SPI_EC_EZBUF_WRITE_STOP_POS (2u)  /* [2] Externally clocked Write Stop detected */
#define SCB3_INTR_SPI_EC_WAKE_UP              ((uint32) 0x01u)
#define SCB3_INTR_SPI_EC_EZBUF_STOP           ((uint32) 0x01u << \
                                                                    SCB3_INTR_SPI_EC_EZBUF_STOP_POS)
#define SCB3_INTR_SPI_EC_EZBUF_WRITE_STOP     ((uint32) 0x01u << \
                                                                    SCB3_INTR_SPI_EC_EZBUF_WRITE_STOP_POS)

/* SCB3_INTR_I2C_EC, SCB3_INTR_I2C_EC_MASK, SCB3_INTR_I2C_EC_MASKED */
#define SCB3_INTR_I2C_EC_WAKE_UP_POS          (0u)  /* [0] Address match: triggers wakeup of chip */
#define SCB3_INTR_I2C_EC_EZBUF_STOP_POS       (1u)  /* [1] Externally clocked Stop detected       */
#define SCB3_INTR_I2C_EC_EZBUF_WRITE_STOP_POS (2u)  /* [2] Externally clocked Write Stop detected */
#define SCB3_INTR_I2C_EC_WAKE_UP              ((uint32) 0x01u)
#define SCB3_INTR_I2C_EC_EZBUF_STOP           ((uint32) 0x01u << \
                                                                    SCB3_INTR_I2C_EC_EZBUF_STOP_POS)
#define SCB3_INTR_I2C_EC_EZBUF_WRITE_STOP     ((uint32) 0x01u << \
                                                                    SCB3_INTR_I2C_EC_EZBUF_WRITE_STOP_POS)

/* SCB3_INTR_MASTER, SCB3_INTR_MASTER_SET,
   SCB3_INTR_MASTER_MASK, SCB3_INTR_MASTER_MASKED */
#define SCB3_INTR_MASTER_I2C_ARB_LOST_POS   (0u)  /* [0] Master lost arbitration                          */
#define SCB3_INTR_MASTER_I2C_NACK_POS       (1u)  /* [1] Master receives NACK: address or write to slave  */
#define SCB3_INTR_MASTER_I2C_ACK_POS        (2u)  /* [2] Master receives NACK: address or write to slave  */
#define SCB3_INTR_MASTER_I2C_STOP_POS       (4u)  /* [4] Master detects the Stop: only self generated Stop*/
#define SCB3_INTR_MASTER_I2C_BUS_ERROR_POS  (8u)  /* [8] Master detects bus error: misplaced Start or Stop*/
#define SCB3_INTR_MASTER_SPI_DONE_POS       (9u)  /* [9] Master complete transfer: Only for SPI           */
#define SCB3_INTR_MASTER_I2C_ARB_LOST       ((uint32) 0x01u)
#define SCB3_INTR_MASTER_I2C_NACK           ((uint32) 0x01u << SCB3_INTR_MASTER_I2C_NACK_POS)
#define SCB3_INTR_MASTER_I2C_ACK            ((uint32) 0x01u << SCB3_INTR_MASTER_I2C_ACK_POS)
#define SCB3_INTR_MASTER_I2C_STOP           ((uint32) 0x01u << SCB3_INTR_MASTER_I2C_STOP_POS)
#define SCB3_INTR_MASTER_I2C_BUS_ERROR      ((uint32) 0x01u << \
                                                                    SCB3_INTR_MASTER_I2C_BUS_ERROR_POS)
#define SCB3_INTR_MASTER_SPI_DONE           ((uint32) 0x01u << SCB3_INTR_MASTER_SPI_DONE_POS)

/*
* SCB3_INTR_SLAVE, SCB3_INTR_SLAVE_SET,
* SCB3_INTR_SLAVE_MASK, SCB3_INTR_SLAVE_MASKED
*/
#define SCB3_INTR_SLAVE_I2C_ARB_LOST_POS         (0u)  /* [0]  Slave lost arbitration                   */
#define SCB3_INTR_SLAVE_I2C_NACK_POS             (1u)  /* [1]  Slave receives NACK: master reads data   */
#define SCB3_INTR_SLAVE_I2C_ACK_POS              (2u)  /* [2]  Slave receives ACK: master reads data    */
#define SCB3_INTR_SLAVE_I2C_WRITE_STOP_POS       (3u)  /* [3]  Slave detects end of write transaction   */
#define SCB3_INTR_SLAVE_I2C_STOP_POS             (4u)  /* [4]  Slave detects end of transaction intended */
#define SCB3_INTR_SLAVE_I2C_START_POS            (5u)  /* [5]  Slave detects Start                      */
#define SCB3_INTR_SLAVE_I2C_ADDR_MATCH_POS       (6u)  /* [6]  Slave address matches                    */
#define SCB3_INTR_SLAVE_I2C_GENERAL_POS          (7u)  /* [7]  General call received                    */
#define SCB3_INTR_SLAVE_I2C_BUS_ERROR_POS        (8u)  /* [8]  Slave detects bus error                  */
#define SCB3_INTR_SLAVE_SPI_EZBUF_WRITE_STOP_POS (9u)  /* [9]  Slave write complete: Only for SPI       */
#define SCB3_INTR_SLAVE_SPI_EZBUF_STOP_POS       (10u) /* [10] Slave end of transaction: Only for SPI   */
#define SCB3_INTR_SLAVE_SPI_BUS_ERROR_POS        (11u) /* [11] Slave detects bus error: Only for SPI    */
#define SCB3_INTR_SLAVE_I2C_ARB_LOST             ((uint32) 0x01u)
#define SCB3_INTR_SLAVE_I2C_NACK                 ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_I2C_NACK_POS)
#define SCB3_INTR_SLAVE_I2C_ACK                  ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_I2C_ACK_POS)
#define SCB3_INTR_SLAVE_I2C_WRITE_STOP           ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_I2C_WRITE_STOP_POS)
#define SCB3_INTR_SLAVE_I2C_STOP                 ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_I2C_STOP_POS)
#define SCB3_INTR_SLAVE_I2C_START                ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_I2C_START_POS)
#define SCB3_INTR_SLAVE_I2C_ADDR_MATCH           ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_I2C_ADDR_MATCH_POS)
#define SCB3_INTR_SLAVE_I2C_GENERAL              ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_I2C_GENERAL_POS)
#define SCB3_INTR_SLAVE_I2C_BUS_ERROR            ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_I2C_BUS_ERROR_POS)
#define SCB3_INTR_SLAVE_SPI_EZBUF_WRITE_STOP     ((uint32) 0x01u << \
                                                                   SCB3_INTR_SLAVE_SPI_EZBUF_WRITE_STOP_POS)
#define SCB3_INTR_SLAVE_SPI_EZBUF_STOP           ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_SPI_EZBUF_STOP_POS)
#define SCB3_INTR_SLAVE_SPI_BUS_ERROR           ((uint32) 0x01u << \
                                                                    SCB3_INTR_SLAVE_SPI_BUS_ERROR_POS)

/*
* SCB3_INTR_TX, SCB3_INTR_TX_SET,
* SCB3_INTR_TX_MASK, SCB3_INTR_TX_MASKED
*/
#define SCB3_INTR_TX_TRIGGER_POS        (0u)  /* [0]  Trigger on TX FIFO entires                       */
#define SCB3_INTR_TX_NOT_FULL_POS       (1u)  /* [1]  TX FIFO is not full                              */
#define SCB3_INTR_TX_EMPTY_POS          (4u)  /* [4]  TX FIFO is empty                                 */
#define SCB3_INTR_TX_OVERFLOW_POS       (5u)  /* [5]  Attempt to write to a full TX FIFO               */
#define SCB3_INTR_TX_UNDERFLOW_POS      (6u)  /* [6]  Attempt to read from an empty TX FIFO            */
#define SCB3_INTR_TX_BLOCKED_POS        (7u)  /* [7]  No access to the EZ memory                       */
#define SCB3_INTR_TX_UART_NACK_POS      (8u)  /* [8]  UART transmitter received a NACK: SmartCard mode */
#define SCB3_INTR_TX_UART_DONE_POS      (9u)  /* [9]  UART transmitter done even                       */
#define SCB3_INTR_TX_UART_ARB_LOST_POS  (10u) /* [10] UART lost arbitration: LIN or SmartCard          */
#define SCB3_INTR_TX_TRIGGER            ((uint32) 0x01u)
#define SCB3_INTR_TX_FIFO_LEVEL         (SCB3_INTR_TX_TRIGGER)
#define SCB3_INTR_TX_NOT_FULL           ((uint32) 0x01u << SCB3_INTR_TX_NOT_FULL_POS)
#define SCB3_INTR_TX_EMPTY              ((uint32) 0x01u << SCB3_INTR_TX_EMPTY_POS)
#define SCB3_INTR_TX_OVERFLOW           ((uint32) 0x01u << SCB3_INTR_TX_OVERFLOW_POS)
#define SCB3_INTR_TX_UNDERFLOW          ((uint32) 0x01u << SCB3_INTR_TX_UNDERFLOW_POS)
#define SCB3_INTR_TX_BLOCKED            ((uint32) 0x01u << SCB3_INTR_TX_BLOCKED_POS)
#define SCB3_INTR_TX_UART_NACK          ((uint32) 0x01u << SCB3_INTR_TX_UART_NACK_POS)
#define SCB3_INTR_TX_UART_DONE          ((uint32) 0x01u << SCB3_INTR_TX_UART_DONE_POS)
#define SCB3_INTR_TX_UART_ARB_LOST      ((uint32) 0x01u << SCB3_INTR_TX_UART_ARB_LOST_POS)

/*
* SCB3_INTR_RX, SCB3_INTR_RX_SET,
* SCB3_INTR_RX_MASK, SCB3_INTR_RX_MASKED
*/
#define SCB3_INTR_RX_TRIGGER_POS        (0u)   /* [0]  Trigger on RX FIFO entires            */
#define SCB3_INTR_RX_NOT_EMPTY_POS      (2u)   /* [2]  RX FIFO is not empty                  */
#define SCB3_INTR_RX_FULL_POS           (3u)   /* [3]  RX FIFO is full                       */
#define SCB3_INTR_RX_OVERFLOW_POS       (5u)   /* [5]  Attempt to write to a full RX FIFO    */
#define SCB3_INTR_RX_UNDERFLOW_POS      (6u)   /* [6]  Attempt to read from an empty RX FIFO */
#define SCB3_INTR_RX_BLOCKED_POS        (7u)   /* [7]  No access to the EZ memory            */
#define SCB3_INTR_RX_FRAME_ERROR_POS    (8u)   /* [8]  Frame error in received data frame    */
#define SCB3_INTR_RX_PARITY_ERROR_POS   (9u)   /* [9]  Parity error in received data frame   */
#define SCB3_INTR_RX_BAUD_DETECT_POS    (10u)  /* [10] LIN baud rate detection is completed   */
#define SCB3_INTR_RX_BREAK_DETECT_POS   (11u)  /* [11] Break detection is successful         */
#define SCB3_INTR_RX_TRIGGER            ((uint32) 0x01u)
#define SCB3_INTR_RX_FIFO_LEVEL         (SCB3_INTR_RX_TRIGGER)
#define SCB3_INTR_RX_NOT_EMPTY          ((uint32) 0x01u << SCB3_INTR_RX_NOT_EMPTY_POS)
#define SCB3_INTR_RX_FULL               ((uint32) 0x01u << SCB3_INTR_RX_FULL_POS)
#define SCB3_INTR_RX_OVERFLOW           ((uint32) 0x01u << SCB3_INTR_RX_OVERFLOW_POS)
#define SCB3_INTR_RX_UNDERFLOW          ((uint32) 0x01u << SCB3_INTR_RX_UNDERFLOW_POS)
#define SCB3_INTR_RX_BLOCKED            ((uint32) 0x01u << SCB3_INTR_RX_BLOCKED_POS)
#define SCB3_INTR_RX_FRAME_ERROR        ((uint32) 0x01u << SCB3_INTR_RX_FRAME_ERROR_POS)
#define SCB3_INTR_RX_PARITY_ERROR       ((uint32) 0x01u << SCB3_INTR_RX_PARITY_ERROR_POS)
#define SCB3_INTR_RX_BAUD_DETECT        ((uint32) 0x01u << SCB3_INTR_RX_BAUD_DETECT_POS)
#define SCB3_INTR_RX_BREAK_DETECT       ((uint32) 0x01u << SCB3_INTR_RX_BREAK_DETECT_POS)

/* Define all interrupt sources */
#define SCB3_INTR_I2C_EC_ALL    (SCB3_INTR_I2C_EC_WAKE_UP    | \
                                             SCB3_INTR_I2C_EC_EZBUF_STOP | \
                                             SCB3_INTR_I2C_EC_EZBUF_WRITE_STOP)

#define SCB3_INTR_SPI_EC_ALL    (SCB3_INTR_SPI_EC_WAKE_UP    | \
                                             SCB3_INTR_SPI_EC_EZBUF_STOP | \
                                             SCB3_INTR_SPI_EC_EZBUF_WRITE_STOP)

#define SCB3_INTR_MASTER_ALL    (SCB3_INTR_MASTER_I2C_ARB_LOST  | \
                                             SCB3_INTR_MASTER_I2C_NACK      | \
                                             SCB3_INTR_MASTER_I2C_ACK       | \
                                             SCB3_INTR_MASTER_I2C_STOP      | \
                                             SCB3_INTR_MASTER_I2C_BUS_ERROR | \
                                             SCB3_INTR_MASTER_SPI_DONE)

#define SCB3_INTR_SLAVE_ALL     (SCB3_INTR_SLAVE_I2C_ARB_LOST      | \
                                             SCB3_INTR_SLAVE_I2C_NACK          | \
                                             SCB3_INTR_SLAVE_I2C_ACK           | \
                                             SCB3_INTR_SLAVE_I2C_WRITE_STOP    | \
                                             SCB3_INTR_SLAVE_I2C_STOP          | \
                                             SCB3_INTR_SLAVE_I2C_START         | \
                                             SCB3_INTR_SLAVE_I2C_ADDR_MATCH    | \
                                             SCB3_INTR_SLAVE_I2C_GENERAL       | \
                                             SCB3_INTR_SLAVE_I2C_BUS_ERROR     | \
                                             SCB3_INTR_SLAVE_SPI_EZBUF_WRITE_STOP | \
                                             SCB3_INTR_SLAVE_SPI_EZBUF_STOP       | \
                                             SCB3_INTR_SLAVE_SPI_BUS_ERROR)

#define SCB3_INTR_TX_ALL        (SCB3_INTR_TX_TRIGGER   | \
                                             SCB3_INTR_TX_NOT_FULL  | \
                                             SCB3_INTR_TX_EMPTY     | \
                                             SCB3_INTR_TX_OVERFLOW  | \
                                             SCB3_INTR_TX_UNDERFLOW | \
                                             SCB3_INTR_TX_BLOCKED   | \
                                             SCB3_INTR_TX_UART_NACK | \
                                             SCB3_INTR_TX_UART_DONE | \
                                             SCB3_INTR_TX_UART_ARB_LOST)

#define SCB3_INTR_RX_ALL        (SCB3_INTR_RX_TRIGGER      | \
                                             SCB3_INTR_RX_NOT_EMPTY    | \
                                             SCB3_INTR_RX_FULL         | \
                                             SCB3_INTR_RX_OVERFLOW     | \
                                             SCB3_INTR_RX_UNDERFLOW    | \
                                             SCB3_INTR_RX_BLOCKED      | \
                                             SCB3_INTR_RX_FRAME_ERROR  | \
                                             SCB3_INTR_RX_PARITY_ERROR | \
                                             SCB3_INTR_RX_BAUD_DETECT  | \
                                             SCB3_INTR_RX_BREAK_DETECT)

/* I2C and EZI2C slave address defines */
#define SCB3_I2C_SLAVE_ADDR_POS    (0x01u)    /* 7-bit address shift */
#define SCB3_I2C_SLAVE_ADDR_MASK   (0xFEu)    /* 8-bit address mask */

/* OVS constants for IrDA Low Power operation */
#define SCB3_CTRL_OVS_IRDA_LP_OVS16     (0x00u)
#define SCB3_CTRL_OVS_IRDA_LP_OVS32     (0x01u)
#define SCB3_CTRL_OVS_IRDA_LP_OVS48     (0x02u)
#define SCB3_CTRL_OVS_IRDA_LP_OVS96     (0x03u)
#define SCB3_CTRL_OVS_IRDA_LP_OVS192    (0x04u)
#define SCB3_CTRL_OVS_IRDA_LP_OVS768    (0x05u)
#define SCB3_CTRL_OVS_IRDA_LP_OVS1536   (0x06u)

/* OVS constant for IrDA */
#define SCB3_CTRL_OVS_IRDA_OVS16        (SCB3_UART_IRDA_LP_OVS16)


/***************************************
*    Common Macro Definitions
***************************************/

/* Re-enables the SCB IP. A clear enable bit has a different effect
* on the scb IP depending on the version:
*  CY_SCBIP_V0: resets state, status, TX and RX FIFOs.
*  CY_SCBIP_V1 or later: resets state, status, TX and RX FIFOs and interrupt sources.
* Clear I2C command registers are because they are not impacted by re-enable.
*/
#define SCB3_SCB_SW_RESET   SCB3_I2CFwBlockReset()

/* TX FIFO macro */
#define SCB3_CLEAR_TX_FIFO \
                            do{        \
                                SCB3_TX_FIFO_CTRL_REG |= ((uint32)  SCB3_TX_FIFO_CTRL_CLEAR); \
                                SCB3_TX_FIFO_CTRL_REG &= ((uint32) ~SCB3_TX_FIFO_CTRL_CLEAR); \
                            }while(0)

#define SCB3_GET_TX_FIFO_ENTRIES    (SCB3_TX_FIFO_STATUS_REG & \
                                                 SCB3_TX_FIFO_STATUS_USED_MASK)

#define SCB3_GET_TX_FIFO_SR_VALID   ((0u != (SCB3_TX_FIFO_STATUS_REG & \
                                                         SCB3_TX_FIFO_SR_VALID)) ? (1u) : (0u))

/* RX FIFO macro */
#define SCB3_CLEAR_RX_FIFO \
                            do{        \
                                SCB3_RX_FIFO_CTRL_REG |= ((uint32)  SCB3_RX_FIFO_CTRL_CLEAR); \
                                SCB3_RX_FIFO_CTRL_REG &= ((uint32) ~SCB3_RX_FIFO_CTRL_CLEAR); \
                            }while(0)

#define SCB3_GET_RX_FIFO_ENTRIES    (SCB3_RX_FIFO_STATUS_REG & \
                                                    SCB3_RX_FIFO_STATUS_USED_MASK)

#define SCB3_GET_RX_FIFO_SR_VALID   ((0u != (SCB3_RX_FIFO_STATUS_REG & \
                                                         SCB3_RX_FIFO_SR_VALID)) ? (1u) : (0u))

/* Write interrupt source: set sourceMask bits in SCB3_INTR_X_MASK_REG */
#define SCB3_WRITE_INTR_I2C_EC_MASK(sourceMask) \
                                                do{         \
                                                    SCB3_INTR_I2C_EC_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_WRITE_INTR_SPI_EC_MASK(sourceMask) \
                                                do{         \
                                                    SCB3_INTR_SPI_EC_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)
#endif /* (!SCB3_CY_SCBIP_V1) */

#define SCB3_WRITE_INTR_MASTER_MASK(sourceMask) \
                                                do{         \
                                                    SCB3_INTR_MASTER_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_WRITE_INTR_SLAVE_MASK(sourceMask)  \
                                                do{         \
                                                    SCB3_INTR_SLAVE_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_WRITE_INTR_TX_MASK(sourceMask)     \
                                                do{         \
                                                    SCB3_INTR_TX_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_WRITE_INTR_RX_MASK(sourceMask)     \
                                                do{         \
                                                    SCB3_INTR_RX_MASK_REG = (uint32) (sourceMask); \
                                                }while(0)

/* Enable interrupt source: set sourceMask bits in SCB3_INTR_X_MASK_REG */
#define SCB3_ENABLE_INTR_I2C_EC(sourceMask) \
                                                do{     \
                                                    SCB3_INTR_I2C_EC_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)
#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_ENABLE_INTR_SPI_EC(sourceMask) \
                                                do{     \
                                                    SCB3_INTR_SPI_EC_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)
#endif /* (!SCB3_CY_SCBIP_V1) */

#define SCB3_ENABLE_INTR_MASTER(sourceMask) \
                                                do{     \
                                                    SCB3_INTR_MASTER_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_ENABLE_INTR_SLAVE(sourceMask)  \
                                                do{     \
                                                    SCB3_INTR_SLAVE_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_ENABLE_INTR_TX(sourceMask)     \
                                                do{     \
                                                    SCB3_INTR_TX_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_ENABLE_INTR_RX(sourceMask)     \
                                                do{     \
                                                    SCB3_INTR_RX_MASK_REG |= (uint32) (sourceMask); \
                                                }while(0)

/* Disable interrupt source: clear sourceMask bits in SCB3_INTR_X_MASK_REG */
#define SCB3_DISABLE_INTR_I2C_EC(sourceMask) \
                                do{                      \
                                    SCB3_INTR_I2C_EC_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                }while(0)

#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_DISABLE_INTR_SPI_EC(sourceMask) \
                                do{                      \
                                    SCB3_INTR_SPI_EC_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                 }while(0)
#endif /* (!SCB3_CY_SCBIP_V1) */

#define SCB3_DISABLE_INTR_MASTER(sourceMask) \
                                do{                      \
                                SCB3_INTR_MASTER_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                }while(0)

#define SCB3_DISABLE_INTR_SLAVE(sourceMask) \
                                do{                     \
                                    SCB3_INTR_SLAVE_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                }while(0)

#define SCB3_DISABLE_INTR_TX(sourceMask)    \
                                do{                     \
                                    SCB3_INTR_TX_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                 }while(0)

#define SCB3_DISABLE_INTR_RX(sourceMask)    \
                                do{                     \
                                    SCB3_INTR_RX_MASK_REG &= ((uint32) ~((uint32) (sourceMask))); \
                                }while(0)

/* Set interrupt sources: write sourceMask bits in SCB3_INTR_X_SET_REG */
#define SCB3_SET_INTR_MASTER(sourceMask)    \
                                                do{     \
                                                    SCB3_INTR_MASTER_SET_REG = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_SET_INTR_SLAVE(sourceMask) \
                                                do{ \
                                                    SCB3_INTR_SLAVE_SET_REG = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_SET_INTR_TX(sourceMask)    \
                                                do{ \
                                                    SCB3_INTR_TX_SET_REG = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_SET_INTR_RX(sourceMask)    \
                                                do{ \
                                                    SCB3_INTR_RX_SET_REG = (uint32) (sourceMask); \
                                                }while(0)

/* Clear interrupt sources: write sourceMask bits in SCB3_INTR_X_REG */
#define SCB3_CLEAR_INTR_I2C_EC(sourceMask)  \
                                                do{     \
                                                    SCB3_INTR_I2C_EC_REG = (uint32) (sourceMask); \
                                                }while(0)

#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_CLEAR_INTR_SPI_EC(sourceMask)  \
                                                do{     \
                                                    SCB3_INTR_SPI_EC_REG = (uint32) (sourceMask); \
                                                }while(0)
#endif /* (!SCB3_CY_SCBIP_V1) */

#define SCB3_CLEAR_INTR_MASTER(sourceMask)  \
                                                do{     \
                                                    SCB3_INTR_MASTER_REG = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_CLEAR_INTR_SLAVE(sourceMask)   \
                                                do{     \
                                                    SCB3_INTR_SLAVE_REG  = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_CLEAR_INTR_TX(sourceMask)      \
                                                do{     \
                                                    SCB3_INTR_TX_REG     = (uint32) (sourceMask); \
                                                }while(0)

#define SCB3_CLEAR_INTR_RX(sourceMask)      \
                                                do{     \
                                                    SCB3_INTR_RX_REG     = (uint32) (sourceMask); \
                                                }while(0)

/* Return true if sourceMask is set in SCB3_INTR_CAUSE_REG */
#define SCB3_CHECK_CAUSE_INTR(sourceMask)    (0u != (SCB3_INTR_CAUSE_REG & (sourceMask)))

/* Return true if sourceMask is set in INTR_X_MASKED_REG */
#define SCB3_CHECK_INTR_I2C_EC(sourceMask)  (0u != (SCB3_INTR_I2C_EC_REG & (sourceMask)))
#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_CHECK_INTR_SPI_EC(sourceMask)  (0u != (SCB3_INTR_SPI_EC_REG & (sourceMask)))
#endif /* (!SCB3_CY_SCBIP_V1) */
#define SCB3_CHECK_INTR_MASTER(sourceMask)  (0u != (SCB3_INTR_MASTER_REG & (sourceMask)))
#define SCB3_CHECK_INTR_SLAVE(sourceMask)   (0u != (SCB3_INTR_SLAVE_REG  & (sourceMask)))
#define SCB3_CHECK_INTR_TX(sourceMask)      (0u != (SCB3_INTR_TX_REG     & (sourceMask)))
#define SCB3_CHECK_INTR_RX(sourceMask)      (0u != (SCB3_INTR_RX_REG     & (sourceMask)))

/* Return true if sourceMask is set in SCB3_INTR_X_MASKED_REG */
#define SCB3_CHECK_INTR_I2C_EC_MASKED(sourceMask)   (0u != (SCB3_INTR_I2C_EC_MASKED_REG & \
                                                                       (sourceMask)))
#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_CHECK_INTR_SPI_EC_MASKED(sourceMask)   (0u != (SCB3_INTR_SPI_EC_MASKED_REG & \
                                                                       (sourceMask)))
#endif /* (!SCB3_CY_SCBIP_V1) */
#define SCB3_CHECK_INTR_MASTER_MASKED(sourceMask)   (0u != (SCB3_INTR_MASTER_MASKED_REG & \
                                                                       (sourceMask)))
#define SCB3_CHECK_INTR_SLAVE_MASKED(sourceMask)    (0u != (SCB3_INTR_SLAVE_MASKED_REG  & \
                                                                       (sourceMask)))
#define SCB3_CHECK_INTR_TX_MASKED(sourceMask)       (0u != (SCB3_INTR_TX_MASKED_REG     & \
                                                                       (sourceMask)))
#define SCB3_CHECK_INTR_RX_MASKED(sourceMask)       (0u != (SCB3_INTR_RX_MASKED_REG     & \
                                                                       (sourceMask)))

/* Return true if sourceMask is set in SCB3_CTRL_REG: generally is used to check enable bit */
#define SCB3_GET_CTRL_ENABLED    (0u != (SCB3_CTRL_REG & SCB3_CTRL_ENABLED))

#define SCB3_CHECK_SLAVE_AUTO_ADDR_NACK     (0u != (SCB3_I2C_CTRL_REG & \
                                                                SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK))


/***************************************
*      I2C Macro Definitions
***************************************/

/* Enable auto ACK/NACK */
#define SCB3_ENABLE_SLAVE_AUTO_ADDR_NACK \
                            do{                      \
                                SCB3_I2C_CTRL_REG |= SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK; \
                            }while(0)

#define SCB3_ENABLE_SLAVE_AUTO_DATA_ACK \
                            do{                     \
                                SCB3_I2C_CTRL_REG |= SCB3_I2C_CTRL_S_READY_DATA_ACK; \
                            }while(0)

#define SCB3_ENABLE_SLAVE_AUTO_DATA_NACK \
                            do{                      \
                                SCB3_I2C_CTRL_REG |= SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK; \
                            }while(0)

#define SCB3_ENABLE_MASTER_AUTO_DATA_ACK \
                            do{                      \
                                SCB3_I2C_CTRL_REG |= SCB3_I2C_CTRL_M_READY_DATA_ACK; \
                            }while(0)

#define SCB3_ENABLE_MASTER_AUTO_DATA_NACK \
                            do{                       \
                                SCB3_I2C_CTRL_REG |= SCB3_I2C_CTRL_M_NOT_READY_DATA_NACK; \
                            }while(0)

/* Disable auto ACK/NACK */
#define SCB3_DISABLE_SLAVE_AUTO_ADDR_NACK \
                            do{                       \
                                SCB3_I2C_CTRL_REG &= ~SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK; \
                            }while(0)

#define SCB3_DISABLE_SLAVE_AUTO_DATA_ACK \
                            do{                      \
                                SCB3_I2C_CTRL_REG &= ~SCB3_I2C_CTRL_S_READY_DATA_ACK; \
                            }while(0)

#define SCB3_DISABLE_SLAVE_AUTO_DATA_NACK \
                            do{                       \
                                SCB3_I2C_CTRL_REG &= ~SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK; \
                            }while(0)

#define SCB3_DISABLE_MASTER_AUTO_DATA_ACK \
                            do{                       \
                                SCB3_I2C_CTRL_REG &= ~SCB3_I2C_CTRL_M_READY_DATA_ACK; \
                            }while(0)

#define SCB3_DISABLE_MASTER_AUTO_DATA_NACK \
                            do{                        \
                                SCB3_I2C_CTRL_REG &= ~SCB3_I2C_CTRL_M_NOT_READY_DATA_NACK; \
                            }while(0)

/* Enable Slave autoACK/NACK Data */
#define SCB3_ENABLE_SLAVE_AUTO_DATA \
                            do{                 \
                                SCB3_I2C_CTRL_REG |= (SCB3_I2C_CTRL_S_READY_DATA_ACK |      \
                                                                  SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK); \
                            }while(0)

/* Disable Slave autoACK/NACK Data */
#define SCB3_DISABLE_SLAVE_AUTO_DATA \
                            do{                  \
                                SCB3_I2C_CTRL_REG &= ((uint32) \
                                                                  ~(SCB3_I2C_CTRL_S_READY_DATA_ACK |       \
                                                                    SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK)); \
                            }while(0)

/* Disable Master autoACK/NACK Data */
#define SCB3_DISABLE_MASTER_AUTO_DATA \
                            do{                   \
                                SCB3_I2C_CTRL_REG &= ((uint32) \
                                                                  ~(SCB3_I2C_CTRL_M_READY_DATA_ACK |       \
                                                                    SCB3_I2C_CTRL_M_NOT_READY_DATA_NACK)); \
                            }while(0)
/* Disables auto data ACK/NACK bits */
#define SCB3_DISABLE_AUTO_DATA \
                do{                        \
                    SCB3_I2C_CTRL_REG &= ((uint32) ~(SCB3_I2C_CTRL_M_READY_DATA_ACK      |  \
                                                                 SCB3_I2C_CTRL_M_NOT_READY_DATA_NACK |  \
                                                                 SCB3_I2C_CTRL_S_READY_DATA_ACK      |  \
                                                                 SCB3_I2C_CTRL_S_NOT_READY_DATA_NACK)); \
                }while(0)

/* Master commands */
#define SCB3_I2C_MASTER_GENERATE_START \
                            do{                    \
                                SCB3_I2C_MASTER_CMD_REG = SCB3_I2C_MASTER_CMD_M_START_ON_IDLE; \
                            }while(0)

#define SCB3_I2C_MASTER_CLEAR_START \
                            do{                 \
                                SCB3_I2C_MASTER_CMD_REG =  ((uint32) 0u); \
                            }while(0)

#define SCB3_I2C_MASTER_GENERATE_RESTART SCB3_I2CReStartGeneration()

#define SCB3_I2C_MASTER_GENERATE_STOP \
                            do{                   \
                                SCB3_I2C_MASTER_CMD_REG =                                            \
                                    (SCB3_I2C_MASTER_CMD_M_STOP |                                    \
                                        (SCB3_CHECK_I2C_STATUS(SCB3_I2C_STATUS_M_READ) ? \
                                            (SCB3_I2C_MASTER_CMD_M_NACK) : (0u)));                   \
                            }while(0)

#define SCB3_I2C_MASTER_GENERATE_ACK \
                            do{                  \
                                SCB3_I2C_MASTER_CMD_REG = SCB3_I2C_MASTER_CMD_M_ACK; \
                            }while(0)

#define SCB3_I2C_MASTER_GENERATE_NACK \
                            do{                   \
                                SCB3_I2C_MASTER_CMD_REG = SCB3_I2C_MASTER_CMD_M_NACK; \
                            }while(0)

/* Slave commands */
#define SCB3_I2C_SLAVE_GENERATE_ACK \
                            do{                 \
                                SCB3_I2C_SLAVE_CMD_REG = SCB3_I2C_SLAVE_CMD_S_ACK; \
                            }while(0)

#if (SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    /* Slave NACK generation for EC_AM logic on address phase. Ticket ID #183902 */
    void SCB3_I2CSlaveNackGeneration(void);
    #define SCB3_I2C_SLAVE_GENERATE_NACK SCB3_I2CSlaveNackGeneration()

#else
    #define SCB3_I2C_SLAVE_GENERATE_NACK \
                            do{                      \
                                SCB3_I2C_SLAVE_CMD_REG = SCB3_I2C_SLAVE_CMD_S_NACK; \
                            }while(0)
#endif /* (SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

#define SCB3_I2C_SLAVE_CLEAR_NACK \
                            do{               \
                                SCB3_I2C_SLAVE_CMD_REG = 0u; \
                            }while(0)

/* Return 8-bit address. The input address should be 7-bits */
#define SCB3_GET_I2C_8BIT_ADDRESS(addr) (((uint32) ((uint32) (addr) << \
                                                                    SCB3_I2C_SLAVE_ADDR_POS)) & \
                                                                        SCB3_I2C_SLAVE_ADDR_MASK)

#define SCB3_GET_I2C_7BIT_ADDRESS(addr) ((uint32) (addr) >> SCB3_I2C_SLAVE_ADDR_POS)

/* Adjust SDA filter Trim settings */
#define SCB3_DEFAULT_I2C_CFG_SDA_FILT_TRIM  (0x02u)
#define SCB3_EC_AM_I2C_CFG_SDA_FILT_TRIM    (0x03u)

#if (SCB3_CY_SCBIP_V0)
    #define SCB3_SET_I2C_CFG_SDA_FILT_TRIM(sdaTrim) \
        do{                                                 \
            SCB3_I2C_CFG_REG =                  \
                            ((SCB3_I2C_CFG_REG & (uint32) ~SCB3_I2C_CFG_SDA_FILT_TRIM_MASK) | \
                             ((uint32) ((uint32) (sdaTrim) <<SCB3_I2C_CFG_SDA_FILT_TRIM_POS)));           \
        }while(0)
#endif /* (SCB3_CY_SCBIP_V0) */

/* Enable/Disable analog and digital filter */
#define SCB3_DIGITAL_FILTER_DISABLE    (0u)
#define SCB3_DIGITAL_FILTER_ENABLE     (1u)
#define SCB3_I2C_DATA_RATE_FS_MODE_MAX (400u)
#if (SCB3_CY_SCBIP_V0)
    /* SCB3_I2C_CFG_SDA_FILT_OUT_ENABLED is disabled by default */
    #define SCB3_I2C_CFG_FILT_MASK  (SCB3_I2C_CFG_SDA_FILT_ENABLED | \
                                                 SCB3_I2C_CFG_SCL_FILT_ENABLED)
#else
    /* SCB3_I2C_CFG_SDA_OUT_FILT_SEL_MASK is disabled by default */
    #define SCB3_I2C_CFG_FILT_MASK  (SCB3_I2C_CFG_SDA_IN_FILT_SEL | \
                                                 SCB3_I2C_CFG_SCL_IN_FILT_SEL)
#endif /* (SCB3_CY_SCBIP_V0) */

#define SCB3_I2C_CFG_ANALOG_FITER_DISABLE \
        do{                                           \
            SCB3_I2C_CFG_REG &= (uint32) ~SCB3_I2C_CFG_FILT_MASK; \
        }while(0)

#define SCB3_I2C_CFG_ANALOG_FITER_ENABLE \
        do{                                          \
            SCB3_I2C_CFG_REG |= (uint32)  SCB3_I2C_CFG_FILT_MASK; \
        }while(0)

/* Return slave select number from SPI_CTRL register */
#define SCB3_GET_SPI_CTRL_SS(activeSelect) (((uint32) ((uint32) (activeSelect) << \
                                                                    SCB3_SPI_CTRL_SLAVE_SELECT_POS)) & \
                                                                        SCB3_SPI_CTRL_SLAVE_SELECT_MASK)

/* Return true if bit is set in SCB3_I2C_STATUS_REG */
#define SCB3_CHECK_I2C_STATUS(sourceMask)   (0u != (SCB3_I2C_STATUS_REG & (sourceMask)))

/* Return true if bit is set in SCB3_SPI_STATUS_REG */
#define SCB3_CHECK_SPI_STATUS(sourceMask)   (0u != (SCB3_SPI_STATUS_REG & (sourceMask)))

/* Return FIFO size depends on SCB3_CTRL_BYTE_MODE bit */
#define SCB3_GET_FIFO_SIZE(condition) ((0u != (condition)) ? \
                                                    (2u * SCB3_FIFO_SIZE) : (SCB3_FIFO_SIZE))


/***************************************
*       Get Macros Definitions
***************************************/

/* SCB3_CTRL */
#define SCB3_GET_CTRL_OVS(oversample)       (((uint32) (oversample) - 1u) & SCB3_CTRL_OVS_MASK)

#define SCB3_GET_CTRL_EC_OP_MODE(opMode)        ((0u != (opMode)) ? \
                                                                (SCB3_CTRL_EC_OP_MODE)  : (0u))

#define SCB3_GET_CTRL_EC_AM_MODE(amMode)        ((0u != (amMode)) ? \
                                                                (SCB3_CTRL_EC_AM_MODE)  : (0u))

#define SCB3_GET_CTRL_BLOCK(block)              ((0u != (block))  ? \
                                                                (SCB3_CTRL_BLOCK)       : (0u))

#define SCB3_GET_CTRL_ADDR_ACCEPT(acceptAddr)   ((0u != (acceptAddr)) ? \
                                                                (SCB3_CTRL_ADDR_ACCEPT) : (0u))

#if (SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    #define SCB3_GET_CTRL_BYTE_MODE(mode)   (0u)
#else
    #define SCB3_GET_CTRL_BYTE_MODE(mode)   ((0u != (mode)) ? \
                                                            (SCB3_CTRL_BYTE_MODE) : (0u))
#endif /* (SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

/* SCB3_I2C_CTRL */
#define SCB3_GET_I2C_CTRL_HIGH_PHASE_OVS(oversampleHigh) (((uint32) (oversampleHigh) - 1u) & \
                                                                        SCB3_I2C_CTRL_HIGH_PHASE_OVS_MASK)

#define SCB3_GET_I2C_CTRL_LOW_PHASE_OVS(oversampleLow)  ((((uint32) (oversampleLow) - 1u) << \
                                                                    SCB3_I2C_CTRL_LOW_PHASE_OVS_POS) &  \
                                                                    SCB3_I2C_CTRL_LOW_PHASE_OVS_MASK)

#define SCB3_GET_I2C_CTRL_S_NOT_READY_ADDR_NACK(wakeNack) ((0u != (wakeNack)) ? \
                                                            (SCB3_I2C_CTRL_S_NOT_READY_ADDR_NACK) : (0u))

#define SCB3_GET_I2C_CTRL_S_GENERAL_IGNORE(genCall) ((0u != (genCall)) ? \
                                                                    (SCB3_I2C_CTRL_S_GENERAL_IGNORE) : (0u))

#define SCB3_GET_I2C_CTRL_SL_MSTR_MODE(mode)    ((uint32)(mode) << SCB3_I2C_CTRL_SLAVE_MODE_POS)

/* SCB3_SPI_CTRL */
#define SCB3_GET_SPI_CTRL_CONTINUOUS(separate)  ((0u != (separate)) ? \
                                                                (SCB3_SPI_CTRL_CONTINUOUS) : (0u))

#define SCB3_GET_SPI_CTRL_SELECT_PRECEDE(mode)  ((0u != (mode)) ? \
                                                                      (SCB3_SPI_CTRL_SELECT_PRECEDE) : (0u))

#define SCB3_GET_SPI_CTRL_SCLK_MODE(mode)       (((uint32) (mode) << \
                                                                        SCB3_SPI_CTRL_CPHA_POS) & \
                                                                        SCB3_SPI_CTRL_SCLK_MODE_MASK)

#define SCB3_GET_SPI_CTRL_LATE_MISO_SAMPLE(lateMiso) ((0u != (lateMiso)) ? \
                                                                    (SCB3_SPI_CTRL_LATE_MISO_SAMPLE) : (0u))

#if (SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    #define SCB3_GET_SPI_CTRL_SCLK_CONTINUOUS(sclkType) (0u)
    #define SCB3_GET_SPI_CTRL_SSEL_POLARITY(polarity)   (0u)
#else
    #define SCB3_GET_SPI_CTRL_SCLK_CONTINUOUS(sclkType) ((0u != (sclkType)) ? \
                                                                    (SCB3_SPI_CTRL_SCLK_CONTINUOUS) : (0u))

    #define SCB3_GET_SPI_CTRL_SSEL_POLARITY(polarity)   (((uint32) (polarity) << \
                                                                     SCB3_SPI_CTRL_SSEL0_POLARITY_POS) & \
                                                                     SCB3_SPI_CTRL_SSEL_POLARITY_MASK)
#endif /* ((SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

#define SCB3_GET_SPI_CTRL_SUB_MODE(mode)        (((uint32) (mode) << SCB3_SPI_CTRL_MODE_POS) & \
                                                                                 SCB3_SPI_CTRL_MODE_MASK)

#define SCB3_GET_SPI_CTRL_SLAVE_SELECT(select)  (((uint32) (select) << \
                                                                      SCB3_SPI_CTRL_SLAVE_SELECT_POS) & \
                                                                      SCB3_SPI_CTRL_SLAVE_SELECT_MASK)

#define SCB3_GET_SPI_CTRL_MASTER_MODE(mode)     ((0u != (mode)) ? \
                                                                (SCB3_SPI_CTRL_MASTER) : (0u))

/* SCB3_UART_CTRL */
#define SCB3_GET_UART_CTRL_MODE(mode)           (((uint32) (mode) << \
                                                                            SCB3_UART_CTRL_MODE_POS) & \
                                                                            SCB3_UART_CTRL_MODE_MASK)

/* SCB3_UART_RX_CTRL */
#define SCB3_GET_UART_RX_CTRL_MODE(stopBits)    (((uint32) (stopBits) - 1u) & \
                                                                        SCB3_UART_RX_CTRL_STOP_BITS_MASK)

#define SCB3_GET_UART_RX_CTRL_PARITY(parity)    ((0u != (parity)) ? \
                                                                    (SCB3_UART_RX_CTRL_PARITY) : (0u))

#define SCB3_GET_UART_RX_CTRL_POLARITY(polarity)    ((0u != (polarity)) ? \
                                                                    (SCB3_UART_RX_CTRL_POLARITY) : (0u))

#define SCB3_GET_UART_RX_CTRL_DROP_ON_PARITY_ERR(dropErr) ((0u != (dropErr)) ? \
                                                        (SCB3_UART_RX_CTRL_DROP_ON_PARITY_ERR) : (0u))

#define SCB3_GET_UART_RX_CTRL_DROP_ON_FRAME_ERR(dropErr) ((0u != (dropErr)) ? \
                                                        (SCB3_UART_RX_CTRL_DROP_ON_FRAME_ERR) : (0u))

#define SCB3_GET_UART_RX_CTRL_MP_MODE(mpMode)   ((0u != (mpMode)) ? \
                                                        (SCB3_UART_RX_CTRL_MP_MODE) : (0u))

#define SCB3_GET_UART_RX_CTRL_BREAK_WIDTH(width)    (((uint32) ((uint32) (width) - 1u) << \
                                                                    SCB3_UART_RX_CTRL_BREAK_WIDTH_POS) & \
                                                                    SCB3_UART_RX_CTRL_BREAK_WIDTH_MASK)

/* SCB3_UART_TX_CTRL */
#define SCB3_GET_UART_TX_CTRL_MODE(stopBits)    (((uint32) (stopBits) - 1u) & \
                                                                SCB3_UART_RX_CTRL_STOP_BITS_MASK)

#define SCB3_GET_UART_TX_CTRL_PARITY(parity)    ((0u != (parity)) ? \
                                                               (SCB3_UART_TX_CTRL_PARITY) : (0u))

#define SCB3_GET_UART_TX_CTRL_RETRY_NACK(nack)  ((0u != (nack)) ? \
                                                               (SCB3_UART_TX_CTRL_RETRY_ON_NACK) : (0u))

/* SCB3_UART_FLOW_CTRL */
#if !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
    #define SCB3_GET_UART_FLOW_CTRL_TRIGGER_LEVEL(level)   ( (uint32) (level) & \
                                                                 SCB3_UART_FLOW_CTRL_TRIGGER_LEVEL_MASK)

    #define SCB3_GET_UART_FLOW_CTRL_RTS_POLARITY(polarity) ((0u != (polarity)) ? \
                                                                (SCB3_UART_FLOW_CTRL_RTS_POLARITY) : (0u))

    #define SCB3_GET_UART_FLOW_CTRL_CTS_POLARITY(polarity) ((0u != (polarity)) ? \
                                                                (SCB3_UART_FLOW_CTRL_CTS_POLARITY) : (0u))

    #define SCB3_GET_UART_FLOW_CTRL_CTS_ENABLE(ctsEn)      ((0u != (ctsEn)) ? \
                                                                (SCB3_UART_FLOW_CTRL_CTS_ENABLE) : (0u))
#endif /* !(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

/* SCB3_RX_CTRL */
#define SCB3_GET_RX_CTRL_DATA_WIDTH(dataWidth)  (((uint32) (dataWidth) - 1u) & \
                                                                SCB3_RX_CTRL_DATA_WIDTH_MASK)

#define SCB3_GET_RX_CTRL_BIT_ORDER(bitOrder)    ((0u != (bitOrder)) ? \
                                                                (SCB3_RX_CTRL_MSB_FIRST) : (0u))

#define SCB3_GET_RX_CTRL_MEDIAN(filterEn)       ((0u != (filterEn)) ? \
                                                                (SCB3_RX_CTRL_MEDIAN) : (0u))

/* SCB3_RX_MATCH */
#define SCB3_GET_RX_MATCH_ADDR(addr)    ((uint32) (addr) & SCB3_RX_MATCH_ADDR_MASK)
#define SCB3_GET_RX_MATCH_MASK(mask)    (((uint32) (mask) << \
                                                            SCB3_RX_MATCH_MASK_POS) & \
                                                            SCB3_RX_MATCH_MASK_MASK)

/* SCB3_RX_FIFO_CTRL */
#define SCB3_GET_RX_FIFO_CTRL_TRIGGER_LEVEL(level)  ((uint32) (level) & \
                                                                    SCB3_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK)

/* SCB3_TX_CTRL */
#define SCB3_GET_TX_CTRL_DATA_WIDTH(dataWidth)  (((uint32) (dataWidth) - 1u) & \
                                                                SCB3_TX_CTRL_DATA_WIDTH_MASK)

#define SCB3_GET_TX_CTRL_BIT_ORDER(bitOrder)    ((0u != (bitOrder)) ? \
                                                                (SCB3_TX_CTRL_MSB_FIRST) : (0u))

/* SCB3_TX_FIFO_CTRL */
#define SCB3_GET_TX_FIFO_CTRL_TRIGGER_LEVEL(level)  ((uint32) (level) & \
                                                                    SCB3_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK)

/* SCB3_INTR_SLAVE_I2C_GENERAL */
#define SCB3_GET_INTR_SLAVE_I2C_GENERAL(genCall)  ((0u != (genCall)) ? \
                                                                (SCB3_INTR_SLAVE_I2C_GENERAL) : (0u))

/* Return true if master mode is enabled SCB3_SPI_CTRL_REG */
#define SCB3_CHECK_SPI_MASTER   (0u != (SCB3_SPI_CTRL_REG & SCB3_SPI_CTRL_MASTER))

/* Return inactive state of SPI SCLK line depends on CPOL */
#define SCB3_GET_SPI_SCLK_INACTIVE \
            ((0u == (SCB3_SPI_CTRL_REG & SCB3_SPI_CTRL_CPOL)) ? (0u) : (1u))

/* Get output pin inactive state */
#if (SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1)
#define SCB3_GET_SPI_SS0_INACTIVE       (1u)
#define SCB3_GET_SPI_SS1_INACTIVE       (1u)
#define SCB3_GET_SPI_SS2_INACTIVE       (1u)
#define SCB3_GET_SPI_SS3_INACTIVE       (1u)
#define SCB3_GET_UART_RTS_INACTIVE      (1u)

#else
#define SCB3_GET_SPI_SS0_INACTIVE  \
        ((0u != (SCB3_SPI_CTRL_REG & SCB3_SPI_CTRL_SSEL0_POLARITY)) ? (0u) : (1u))

#define SCB3_GET_SPI_SS1_INACTIVE  \
        ((0u != (SCB3_SPI_CTRL_REG & SCB3_SPI_CTRL_SSEL1_POLARITY)) ? (0u) : (1u))

#define SCB3_GET_SPI_SS2_INACTIVE  \
        ((0u != (SCB3_SPI_CTRL_REG & SCB3_SPI_CTRL_SSEL2_POLARITY)) ? (0u) : (1u))

#define SCB3_GET_SPI_SS3_INACTIVE  \
        ((0u != (SCB3_SPI_CTRL_REG & SCB3_SPI_CTRL_SSEL3_POLARITY)) ? (0u) : (1u))

#define SCB3_GET_UART_RTS_INACTIVE \
        ((0u == (SCB3_UART_FLOW_CTRL_REG & SCB3_UART_FLOW_CTRL_RTS_POLARITY)) ? (0u) : (1u))

#endif /*(SCB3_CY_SCBIP_V0 || SCB3_CY_SCBIP_V1) */

/* Clear register constants for configuration and interrupt mask */
#define SCB3_CLEAR_REG          ((uint32) (0u))
#define SCB3_NO_INTR_SOURCES    ((uint32) (0u))
#define SCB3_DUMMY_PARAM        ((uint32) (0u))
#define SCB3_SUBMODE_SPI_SLAVE  ((uint32) (0u))

/* Return in case of I2C read error */
#define SCB3_I2C_INVALID_BYTE   ((uint32) 0xFFFFFFFFu)
#define SCB3_CHECK_VALID_BYTE   ((uint32) 0x80000000u)


/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

#define SCB3_CHECK_INTR_EC_I2C(sourceMask)  SCB3_CHECK_INTR_I2C_EC(sourceMask)
#if (!SCB3_CY_SCBIP_V1)
    #define SCB3_CHECK_INTR_EC_SPI(sourceMask)  SCB3_CHECK_INTR_SPI_EC(sourceMask)
#endif /* (!SCB3_CY_SCBIP_V1) */

#define SCB3_CY_SCBIP_V1_I2C_ONLY   (SCB3_CY_SCBIP_V1)
#define SCB3_EZBUFFER_SIZE          (SCB3_EZ_DATA_NR)

#define SCB3_EZBUF_DATA00_REG   SCB3_EZBUF_DATA0_REG
#define SCB3_EZBUF_DATA00_PTR   SCB3_EZBUF_DATA0_PTR

#endif /* (CY_SCB_SCB3_H) */


/* [] END OF FILE */
