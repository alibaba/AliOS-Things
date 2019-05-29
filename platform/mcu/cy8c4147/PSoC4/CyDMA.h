/*******************************************************************************
* File Name: CyDMA.h
* Version 1.10
*
* Description:
*  This file provides global DMA defines and API function definitions.
*
* Note:
*  All the API function in this file are documented in the DMA Channel
*  component datasheet.
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef CY_DMA_GLOBAL_P4_H
#define CY_DMA_GLOBAL_P4_H

#include "cytypes.h"


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define CYDMA_CH_NR         (CYDEV_DMA_CHANNELS_AVAILABLE)
#define CYDMA_DESCR_NR      (2)


/***************************************
*     Data Struct Definitions
***************************************/

/* Channel control registers. Each channel has a dedicated control register. */
typedef struct
{
    uint32 ctl[CYDMA_CH_NR];
} cydma_channel_ctl_struct;

/* Descriptor structure specifies transfer settings. */
typedef struct
{
    void * src;     /* Source address */
    void * dst;     /* Destination address */
    uint32 ctl;     /* Control word */
    uint32 status;  /* Status word */
} cydma_descriptor_struct;

/* Each channel has two descriptor structures for double buffering purposes.
* The two descriptor structures are identical. All descriptors are placed
* sequentially in the DMAC MMIO register space.
*/
typedef struct
{
    cydma_descriptor_struct descriptor[CYDMA_CH_NR][CYDMA_DESCR_NR];
} cydma_descriptor_ram_struct;

/* Configuration structure used by CyDmaSetConfiguration() API. */
typedef struct
{
    /* Specifies the size of the data element. The DMA transfer engine transfers
    * one data element at a time. How these transfers occur is controlled by the
    * transferMode parameter.
    */
    uint32  dataElementSize;

    /* Specifies the total number of data elements this descriptor transfers.
    * Valid ranges are 1 to 65536.
    */
    int32   numDataElements;

    /* Specifies the source and destination widths. */
    uint32  srcDstTransferWidth;

    /* Specifies whether the source and/or destination address will be
    * incremented after the transfer of each single data element.
    */
    uint32  addressIncrement;

    /* Specifies the type of DMA trigger. */
    uint32  triggerType;

    /* Specifies how the DMA reacts to each trigger event. */
    uint32  transferMode;

    /* Specifies whether the descriptor is preemptable. */ 
    uint32  preemptable;

    /* Specifies what occurs after a descriptor completes. */
    uint32  actions;

} cydma_init_struct;


/***************************************
*        Function Prototypes
***************************************/

/* Callback function pointer type */
typedef void (*cydma_callback_t)(void);

/* Default interrupt service routine */
CY_ISR_PROTO(CyDmaInterrupt);

/* DMA controller specific functions */
void   CyDmaEnable(void);
void   CyDmaDisable(void);

uint32 CyDmaGetStatus(void);
uint32 CyDmaGetActiveChannels(void);
void * CyDmaGetActiveSrcAddress(void);
void * CyDmaGetActiveDstAddress(void);

void   CyDmaSetInterruptVector(cyisraddress interruptVector);
uint32 CyDmaGetInterruptSource(void);
void   CyDmaClearInterruptSource(uint32 interruptMask);
uint32 CyDmaGetInterruptSourceMasked(void);
void   CyDmaSetInterruptSourceMask(uint32 interruptMask);
uint32 CyDmaGetInterruptSourceMask(void);
cydma_callback_t CyDmaSetInterruptCallback(int32 channel, cydma_callback_t callback);
cydma_callback_t CyDmaGetInterruptCallback(int32 channel);

/* Channel specific functions */
int32  CyDmaChAlloc(void);
cystatus CyDmaChFree(int32 channel);

void   CyDmaChEnable(int32 channel);
void   CyDmaChDisable(int32 channel);
void   CyDmaTrigger(int32 channel);

void   CyDmaSetPriority(int32 channel, int32 priority);
int32  CyDmaGetPriority(int32 channel);

void   CyDmaSetNextDescriptor(int32 channel, int32 descriptor);
int32  CyDmaGetNextDescriptor(int32 channel);

/* Descriptor specific functions */
void   CyDmaSetConfiguration(int32 channel, int32 descriptor, const cydma_init_struct * config);

void   CyDmaValidateDescriptor(int32 channel, int32 descriptor);

uint32 CyDmaGetDescriptorStatus(int32 channel, int32 descriptor);

void   CyDmaSetSrcAddress(int32 channel, int32 descriptor, void * srcAddress);
void * CyDmaGetSrcAddress(int32 channel, int32 descriptor);

void   CyDmaSetDstAddress(int32 channel, int32 descriptor, void * dstAddress);
void * CyDmaGetDstAddress(int32 channel, int32 descriptor);

void   CyDmaSetDataElementSize(int32 channel, int32 descriptor, uint32 dataElementSize);
uint32 CyDmaGetDataElementSize(int32 channel, int32 descriptor);

void   CyDmaSetNumDataElements(int32 channel, int32 descriptor, int32 numDataElements);
int32  CyDmaGetNumDataElements(int32 channel, int32 descriptor);

void   CyDmaSetSrcDstTransferWidth(int32 channel, int32 descriptor, uint32 transferWidth);
uint32 CyDmaGetSrcDstTransferWidth(int32 channel, int32 descriptor);

void   CyDmaSetAddressIncrement(int32 channel, int32 descriptor, uint32 addressIncrement);
uint32 CyDmaGetAddressIncrement(int32 channel, int32 descriptor);

void   CyDmaSetTriggerType(int32 channel, int32 descriptor, uint32 triggerType);
uint32 CyDmaGetTriggerType(int32 channel, int32 descriptor);

void   CyDmaSetTransferMode(int32 channel, int32 descriptor, uint32 transferMode);
uint32 CyDmaGetTransferMode(int32 channel, int32 descriptor);

void   CyDmaSetPreemptable(int32 channel, int32 descriptor, uint32 preemptable);
uint32 CyDmaGetPreemptable(int32 channel, int32 descriptor);

void   CyDmaSetPostCompletionActions(int32 channel, int32 descriptor, uint32 actions);
uint32 CyDmaGetPostCompletionActions(int32 channel, int32 descriptor);


/***************************************
*           API Constants
***************************************/

#define CYDMA_INTR_NUMBER               (CYDEV_INTR_NUMBER_DMA)
#define CYDMA_INTR_PRIO                 (3u)

#define CYDMA_INVALID_CHANNEL           (-1)

#define CYDMA_TRIGGER_MASK              (0xC0020000U)
#define CYDMA_TR_SEL_MASK               (0x00000007U)
#define CYDMA_TR_GROUP_SHR              (3)

#define CYDMA_MAX_DATA_NR               (65536)

#define CY_DMA_MAX_PRIO                 (3)


/*******************************************************************************
* Bit fields for CyDmaSetDataElementSize()/CyDmaGetDataElementSize().
*******************************************************************************/

/* Specifies the size of the data element that is transferred at a time. */
#define CYDMA_BYTE                      (0x00000000U)    /* 1 byte */
#define CYDMA_HALFWORD                  (0x00010000U)    /* 2 bytes */
#define CYDMA_WORD                      (0x00020000U)    /* 4 bytes */


/*******************************************************************************
* Bit fields for CyDmaSetSrcDstTransferWidth()/CyDmaGetSrcDstTransferWidth().
*******************************************************************************/

/* Source and Destination widths are set by the data element size. */
#define CYDMA_ELEMENT_ELEMENT           (0x00000000U)

/* Source width is set by data element size. Destination width is a 
* word (4bytes). If the source width is smaller than the destination
* width, the upper bytes of the destination are written with zeros.
*/
#define CYDMA_ELEMENT_WORD              (0x00100000U)

/* Source width is a word (4 bytes). Destination width is set by data
* element size. If the source width is larger than the destination width,
* the upper bytes of the source are ignored during the transaction.
*/
#define CYDMA_WORD_ELEMENT              (0x00400000U)

/* Both source and destination widths are words. However, the data element
* size still has an effect in this mode. For example, if the data element
* size is set to a byte, then the upper three bytes of destination is
* padded with zeros, and the upper three bytes of the source is ignored
* during the transaction.
*/
#define CYDMA_WORD_WORD                 (0x00500000U)


/*******************************************************************************
* Bit fields for CyDmaSetAddressIncrement()/CyDmaGetAddressIncrement().
*******************************************************************************/

/* Specifies whether the source and/or destination address is incremented
* after the transfer of each single data element. The defines can be OR'd
* together.
*/
#define CYDMA_INC_SRC_ADDR              (0x00800000U)
#define CYDMA_INC_DST_ADDR              (0x00200000U)
#define CYDMA_INC_NONE                  (0x00000000U)


/*******************************************************************************
* Bit fields for CyDmaSetTriggerType()/CyDmaGetTriggerType().
*******************************************************************************/

/* Specifies the type of DMA trigger */
#define CYDMA_PULSE                     (0x00000000U)
#define CYDMA_LEVEL_FOUR                (0x01000000U)
#define CYDMA_LEVEL_EIGHT               (0x02000000U)
#define CYDMA_PULSE_UNKNOWN             (0x03000000U)


/*******************************************************************************
* Bit fields for CyDmaSetTransferMode()/CyDmaGetTransferMode().
*******************************************************************************/

/* Each trigger causes the DMA to transfer a single data element. */
#define CYDMA_SINGLE_DATA_ELEMENT       (0x00000000U)
    
/* Each trigger automatically transfers all data elements associated with
* the current descriptor, one data element at a time.
*/
#define CYDMA_ENTIRE_DESCRIPTOR         (0x40000000U)

/* Each trigger automatically transfers all data elements associated with
* the current descriptor, one data element at a time. Upon completion the
* next descriptor is automatically triggered.
*/
#define CYDMA_ENTIRE_DESCRIPTOR_CHAIN   (0x80000000U)


/*******************************************************************************
* Bit fields for CyDmaSetPreemptable()/CyDmaGetPreemptable().
*******************************************************************************/

/* Specifies whether the descriptor is preemptable. */
#define CYDMA_PREEMPTABLE               (0x10000000U)
#define CYDMA_NON_PREEMPTABLE           (0x00000000U)


/*******************************************************************************
* Bit fields for CyDmaSetPostCompletionActions()/CyDmaGetPostCompletionActions()
*******************************************************************************/

/* Specifies what occurs after a descriptor completes. The defines that can be
* OR'd together.
*
* On completion of descriptor chain to the next descriptor. */
#define CYDMA_CHAIN                     (0x20000000U)

/* Invalidate the descriptor when it completes. */
#define CYDMA_INVALIDATE                (0x04000000U)
    
/* On completion of descriptor generate an interrupt request. */
#define CYDMA_GENERATE_IRQ              (0x08000000U)
    
/* No actions after the descriptor completes. */
#define CYDMA_NONE                      (0x00000000U)


/*******************************************************************************
* Bit fields for CyDmaGetStatus().
*******************************************************************************/

/* Bit fields to access the status register bits of the DMA transfer engine. */
#define CYDMA_TRANSFER_INDEX            (0x0000FFFFU)
#define CYDMA_CH_NUM                    (0x001F0000U)
#define CYDMA_STATE                     (0x07000000U)
#define CYDMA_PRIO                      (0x30000000U)
#define CYDMA_DESCRIPTOR                (0x40000000U)
#define CYDMA_ACTIVE                    (0x80000000U)

/* Defines for STATE: Only one define can be active at a time. */
#define CYDMA_IDLE                      (0x00000000U)
#define CYDMA_LOAD_DESCR                (0x01000000U)
#define CYDMA_LOAD_SRC                  (0x02000000U)
#define CYDMA_STORE_DST                 (0x03000000U)
#define CYDMA_STORE_DESCR               (0x04000000U)
#define CYDMA_WAIT_TRIG_DEACT           (0x05000000U)
#define CYDMA_STORE_ERROR               (0x06000000U)


/*******************************************************************************
* Bit fields for CyDmaGetDescriptorStatus().
*******************************************************************************/

/* Bit fields to access the status register bits of the descriptor. */
#define CYDMA_TRANSFER_INDEX            (0x0000FFFFU)
#define CYDMA_RESPONSE                  (0x00070000U)
#define CYDMA_VALID                     (0x80000000U)

/* Response code meaning */
#define CYDMA_NO_ERROR                  (0x00000000U)
#define CYDMA_DONE                      (0x00010000U)
#define CYDMA_SRC_BUS_ERROR             (0x00020000U)
#define CYDMA_DST_BUS_ERROR             (0x00030000U)
#define CYDMA_SRC_MISAL                 (0x00040000U)
#define CYDMA_DST_MISAL                 (0x00050000U)
#define CYDMA_INVALID_DESCR             (0x00060000U)


/***************************************
*             Registers
***************************************/

#define CYDMA_CTL_REG                   (*(reg32 *) CYREG_DMAC_CTL)
#define CYDMA_STATUS_REG                (*(reg32 *) CYREG_DMAC_STATUS)
#define CYDMA_STATUS_SRC_ADDR_REG       (*(reg32 *) CYREG_DMAC_STATUS_SRC_ADDR)
#define CYDMA_STATUS_DST_ADDR_REG       (*(reg32 *) CYREG_DMAC_STATUS_DST_ADDR)
#define CYDMA_STATUS_CH_ACT_REG         (*(reg32 *) CYREG_DMAC_STATUS_CH_ACT)
#define CYDMA_INTR_REG                  (*(reg32 *) CYREG_DMAC_INTR)
#define CYDMA_INTR_SET_REG              (*(reg32 *) CYREG_DMAC_INTR_SET)
#define CYDMA_INTR_MASK_REG             (*(reg32 *) CYREG_DMAC_INTR_MASK)
#define CYDMA_INTR_MASKED_REG           (*(reg32 *) CYREG_DMAC_INTR_MASKED)
#define CYDMA_CH_CTL_BASE               (*(volatile cydma_channel_ctl_struct *) CYREG_DMAC_CH_CTL0)
#define CYDMA_DESCR_BASE                (*(volatile cydma_descriptor_ram_struct *) CYDEV_DMAC_DESCR0_BASE)

#define CYDMA_TR_CTL_REG                (*(reg32 *) CYREG_PERI_TR_CTL)


/***************************************
*       Register Constants
***************************************/

#define CYDMA_ENABLED                   (0x80000000U)
#define CYDMA_PRIO_POS                  (28)
#define CYDMA_DESCRIPTOR_POS            (30)

/* Descriptor control register */
#define CYDMA_DATA_SIZE                 (0x00030000U)
#define CYDMA_DATA_NR                   (0x0000FFFFUL)
#define CYDMA_TRANSFER_WIDTH            (0x00500000U)
#define CYDMA_ADDR_INCR                 (0x00A00000U)
#define CYDMA_TRANSFER_MODE             (0xC0000000U)
#define CYDMA_TRIGGER_TYPE              (0x03000000U)
#define CYDMA_POST_COMPLETE_ACTIONS     (0x2C000000U)

#endif /* CY_DMA_GLOBAL_P4_H */


/* [] END OF FILE */
