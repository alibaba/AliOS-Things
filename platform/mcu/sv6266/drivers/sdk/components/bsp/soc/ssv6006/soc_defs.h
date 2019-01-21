/*****************************************************************************
 *
 *            Copyright Andes Technology Corporation 2007-2008
 *                         All Rights Reserved.
 *
 *  Revision History:
 *
 *    Aug.21.2007     Created.
 ****************************************************************************/

#ifndef _SOC_DEFS_H
#define _SOC_DEFS_H

#ifndef __ASSEMBLER__
#include <inttypes.h>
#include <nds32_intrinsic.h>
#endif

/*****************************************************************************
 *  CONFIG_HEADER
 ****************************************************************************/
#ifdef XIP_MODE
	#define PARTITION_BOOT_SIZE			0x4000
	#define PARTITION_PARAM_SIZE		0x4000
	#define MAGIC_HEADER_BASE			0x30000000
#else
	#define MAGIC_HEADER_BASE			0x00000000
#endif
#if 1
	#define OFFSET_XTAL					0x04
	#define OFFSET_BUSCLK				0x08
	#define OFFSET_XIP_BIT				0x0c
	#define OFFSET_PARTITION_MAIN_SIZE	0x10
	#define OFFSET_FLASH_TOTAL_SIZE		0x14
	#define OFFSET_PSRAM_HEAP_BASE		0x18
	#define OFFSET_PSRAM_HEAP_SIZE		0x1c
	#define OFFSET_UART_FW_UPGRADE		0x20
#endif
/*****************************************************************************
 * IRQ Vector
 ****************************************************************************/
#define INTERRUPT_TYPICAL
#if defined(INTERRUPT_TYPICAL)
#define IRQ_SYS_WDT           (0)
#define IRQ_WDT           (0)
#define IRQ_SWI               (1)
#define IRQ_GROUP_02          (2)
#define IRQ_COMM              (2)
#define IRQ_SPI_M             (3)
#define IRQ_SPI_IPC           (4)
#define IRQ_BTCOEX            (5)
#define IRQ_FLASH_DMA         (6)
#define IRQ_MTX_BEACON_DONE   (7)
#define IRQ_MTX_BEACON_DTIM   (8)
#define IRQ_FBUS_DMA          (9)
#define IRQ_SBUS_DMA          (10)
#define IRQ_USB               (11)
#define IRQ_RTC               (12)
#define IRQ_SYSCTL            (13)
#define IRQ_PMU               (14)
#define IRQ_GROUP_15          (15)
#define IRQ_I2S               (16)
#define IRQ_RATE_STAT         (17)
#define IRQ_MBOX_ALT       (18)
#define IRQ_MBOX           (19)
#define IRQ_UTIMER_0          (20)
#define IRQ_UTIMER_1          (21)
#define IRQ_UTIMER_2          (22)
#define IRQ_UTIMER_3          (23)
#define IRQ_MTIMER_0          (24)
#define IRQ_MTIMER_1          (25)
#define IRQ_MTIMER_2          (26)
#define IRQ_MTIMER_3          (27)
#define IRQ_I2C_M             (28)
#define IRQ_HCI               (29)
#define IRQ_CO_DMA            (30)
#define IRQ_GROUP_31          (31)

// IRQ 02 Group
#define IRQ_GROUP_02_SPI_CLK_EN_INT               (0)
#define IRQ_GROUP_02_SDIO_WAKE                    (1)

// IRQ 15 Group
#define IRQ_GROUP_15_DMN_NOHIT                    (0)
#define IRQ_GROUP_15_ALC_ERR                      (1)
#define IRQ_GROUP_15_RLS_ERR                      (2)
#define IRQ_GROUP_15_ALC_ABT                      (3)

#define IRQ_GROUP_15_ALC_TIMEPUT                  (8)
#define IRQ_GROUP_15_REQ_LOCK                     (9)
#define IRQ_GROUP_15_TX_LIMIT                     (10)
#define IRQ_GROUP_15_ID_THOLD_RX                  (11)
#define IRQ_GROUP_15_ID_THOLD_TX                  (12)
#define IRQ_GROUP_15_DOUBLE_RLS                   (13)
#define IRQ_GROUP_15_RX_ID_LEN_THOLD              (14)
#define IRQ_GROUP_15_TX_ID_LEN_THOLD              (15)
#define IRQ_GROUP_15_ALL_ID_LEN_THOLD             (16)
#define IRQ_GROUP_15_TRASH_CAN                    (17)
#define IRQ_GROUP_15_MB_LOWTHOLD                  (18)

#define IRQ_GROUP_15_EDCA0_LOW_THRESHOLD          (20)
#define IRQ_GROUP_15_EDCA1_LOW_THRESHOLD          (21)
#define IRQ_GROUP_15_EDCA2_LOW_THRESHOLD          (22)
#define IRQ_GROUP_15_EDCA3_LOW_THRESHOLD          (23)
#define IRQ_GROUP_15_EDCA_BK                      (24)
#define IRQ_GROUP_15_EDCA_BE                      (25)
#define IRQ_GROUP_15_EDCA_VI                      (26)
#define IRQ_GROUP_15_EDCA_VO                      (27)
#define IRQ_GROUP_15_EDCA_MNT                     (28)
#define IRQ_GROUP_15_EDCA_BCN                     (29)

// IRQ 31 Group
#define IRQ_GROUP_31_GPI0                         (0)
#define IRQ_GROUP_31_GPI1                         (1)
#define IRQ_GROUP_31_GPI2                         (2)
#define IRQ_GROUP_31_GPI3                         (3)
#define IRQ_GROUP_31_GPI4                         (4)
#define IRQ_GROUP_31_GPI5                         (5)
#define IRQ_GROUP_31_GPI6                         (6)
#define IRQ_GROUP_31_GPI7                         (7)
#define IRQ_GROUP_31_GPI8                         (8)
#define IRQ_GROUP_31_GPI9                         (9)
#define IRQ_GROUP_31_GPI10                        (10)
#define IRQ_GROUP_31_GPI11                        (11)
#define IRQ_GROUP_31_GPI12                        (12)
#define IRQ_GROUP_31_GPI13                        (13)
#define IRQ_GROUP_31_GPI14                        (14)
#define IRQ_GROUP_31_GPI15                        (15)
#define IRQ_GROUP_31_GPI16                        (16)
#define IRQ_GROUP_31_GPI17                        (17)
#define IRQ_GROUP_31_GPI18                        (18)
#define IRQ_GROUP_31_GPI19                        (19)
#define IRQ_GROUP_31_GPI20                        (20)
#define IRQ_GROUP_31_GPI21                        (21)
#define IRQ_GROUP_31_GPI22                        (22)
#define IRQ_GROUP_31_WIFI_PHY                     (23)
#define IRQ_GROUP_31_UART_DBG_TX                  (24)
#define IRQ_GROUP_31_UART_DBG_RX                  (25)
#define IRQ_GROUP_31_UART_RX_TOUT                 (26)
#define IRQ_GROUP_31_UART_DBG_MULTI               (27)
#define IRQ_GROUP_31_UART_DATA_TX                 (28)
#define IRQ_GROUP_31_UART_DATA_RX                 (29)
#define IRQ_GROUP_31_UART_DATA_RX_TOUT            (30)
#define IRQ_GROUP_31_UART_DATA_MULTI              (31)

#define M_INT_WDT               (0)
#define M_INT_MCU_WDT           (0)
#define M_INT_SYS_WDT           (0)
#define M_INT_SWI               (1)
#define M_INT_FENCE             (1)
#define M_INT_COMM              (2)
#define M_INT_SDIO              (2)
#define M_INT_SPI               (2)
#define M_INT_SPI_M             (3)
#define M_INT_SPI_IPC           (4)
#define M_INT_BTCOEX            (5)
#define M_INT_FLASH_DMA         (6)
#define M_INT_MTX_BEACON_DONE   (7)
#define M_INT_MTX_BEACON_DTIM   (8)
#define M_INT_FBUS_DMA          (9)
#define M_INT_SBUS_DMA          (10)
#define M_INT_USB               (11)
#define M_INT_RTC               (12)
#define M_INT_SYSCTL            (13)
#define M_INT_PMU               (14)
#define M_INT_GROUP_15          (15)
#define M_INT_I2S               (16)
#define M_INT_RATE_STAT         (17)
#define M_INT_MAILBOX_ALT       (18)
#define M_INT_MAILBOX           (19)
#define M_INT_UTIMER_0          (20)
#define M_INT_UTIMER_1          (21)
#define M_INT_UTIMER_2          (22)
#define M_INT_UTIMER_3          (23)
#define M_INT_MTIMER_0          (24)
#define M_INT_MTIMER_1          (25)
#define M_INT_MTIMER_2          (26)
#define M_INT_MTIMER_3          (27)
#define M_INT_I2C_M             (28)
#define M_INT_HCI               (29)
#define M_INT_CO_DMA            (30)
#define M_INT_IRQ31             (31)

#define M_INT_WDT_ON                (0)
#define M_INT_SWI_ON                (0)
#define M_INT_COMM_ON               (0)
#define M_INT_SPI_M_ON              (0)
#define M_INT_SPI_IPC_ON            (0)
#define M_INT_BTCOEX_ON             (0)
#define M_INT_FLASH_DMA_ON          (0)
#define M_INT_MTX_BEACON_DONE_ON    (0)
#define M_INT_MTX_BEACON_DTIM_ON    (0)
#define M_INT_FBUS_DMA_ON           (0)
#define M_INT_SBUS_DMA_ON           (0)
#define M_INT_USB_ON                (0)
#define M_INT_RTC_ON                (0)
#define M_INT_SYSCTL_ON             (0)
#define M_INT_PMU_ON                (0)
#define M_INT_GROUP_15_ON           (0)
#define M_INT_I2S_ON                (0)
#define M_INT_RATE_STAT_ON          (0)
#define M_INT_MAILBOX_ALT_ON        (0)
#define M_INT_MAILBOX_ON            (0)
#define M_INT_UTIMER_0_ON           (0)
#define M_INT_UTIMER_1_ON           (0)
#define M_INT_UTIMER_2_ON           (0)
#define M_INT_UTIMER_3_ON           (0)
#define M_INT_MTIMER_0_ON           (0)
#define M_INT_MTIMER_1_ON           (0)
#define M_INT_MTIMER_2_ON           (0)
#define M_INT_MTIMER_3_ON           (0)
#define M_INT_I2C_M_ON              (0)
#define M_INT_HCI_ON                (0)
#define M_INT_CO_DMA_ON             (0)
#define M_INT_IRQ31_ON              (0)
#elif   defined(INTERRUPT_IOT)
#define M_INT_WDT               (0)
#define M_INT_MCU_WDT           (0)
#define M_INT_SYS_WDT           (0)
#define M_INT_SWI               (1)
#define M_INT_FENCE             (1)
#define M_INT_GPI_0             (2)
#define M_INT_GPI_1             (3)
#define M_INT_GPI_2             (4)
#define M_INT_BTCOEX            (5)
#define M_INT_FLASH_DMA         (6)
#define M_INT_MTX_BEACON_DONE   (7)
#define M_INT_MTX_BEACON_DTIM   (8)
#define M_INT_FBUS_DMA          (9)
#define M_INT_SBUS_DMA          (10)
#define M_INT_USB               (11)
#define M_INT_RTC               (12)
#define M_INT_SYSCTL            (13)
#define M_INT_PMU               (14)
#define M_INT_GROUP_15          (15)
#define M_INT_I2S               (16)
#define M_INT_RATE_STAT         (17)
#define M_INT_MAILBOX_ALT       (18)
#define M_INT_MAILBOX           (19)
#define M_INT_UTIMER_0          (20)
#define M_INT_UTIMER_1          (21)
#define M_INT_UTIMER_2          (22)
#define M_INT_UTIMER_3          (23)
#define M_INT_MTIMER_0          (24)
#define M_INT_MTIMER_1          (25)
#define M_INT_MTIMER_2          (26)
#define M_INT_MTIMER_3          (27)
#define M_INT_UART_RX           (28)
#define M_INT_HSUART_RX         (29)
#define M_INT_CO_DMA            (30)
#define M_INT_IRQ31             (31)

#define M_INT_DEFINE(NAME, id, on_off, priority) \
    const unsigned char __INT_##NAME            = (id); \
    const unsigned char __INT_##NAME##_on_off   = (on_off);

#define M_INT_WDT_ON                (0)
#define M_INT_SWI_ON                (0)
#define M_INT_GPI_0_ON              (0)
#define M_INT_GPI_1_ON              (0)
#define M_INT_GPI_2_ON              (0)
#define M_INT_BTCOEX_ON             (0)
#define M_INT_FLASH_DMA_ON          (0)
#define M_INT_MTX_BEACON_DONE_ON    (0)
#define M_INT_MTX_BEACON_DTIM_ON    (0)
#define M_INT_FBUS_DMA_ON           (0)
#define M_INT_SBUS_DMA_ON           (0)
#define M_INT_USB_ON                (0)
#define M_INT_RTC_ON                (0)
#define M_INT_SYSCTL_ON             (0)
#define M_INT_PMU_ON                (0)
#define M_INT_IRQ15_ON              (0)
#define M_INT_I2S_ON                (0)
#define M_INT_RATE_STAT_ON          (0)
#define M_INT_MAILBOX_ALT_ON        (0)
#define M_INT_MAILBOX_ON            (0)
#define M_INT_UTIMER_0_ON           (0)
#define M_INT_UTIMER_1_ON           (0)
#define M_INT_UTIMER_2_ON           (0)
#define M_INT_UTIMER_3_ON           (0)
#define M_INT_MTIMER_0_ON           (0)
#define M_INT_MTIMER_1_ON           (0)
#define M_INT_MTIMER_2_ON           (0)
#define M_INT_MTIMER_3_ON           (0)
#define M_INT_UART_RX_ON            (0)
#define M_INT_HSUART_RX_ON          (0)
#define M_INT_CO_DMA_ON             (0)
#define M_INT_IRQ31_ON              (0)


#define M_INT_WDT_PRI                (0)
#define M_INT_SWI_PRI                (0)
#define M_INT_GPI_0_PRI              (0)
#define M_INT_GPI_1_PRI              (0)
#define M_INT_GPI_2_PRI              (0)
#define M_INT_BTCOEX_PRI             (0)
#define M_INT_FLASH_DMA_PRI          (0)
#define M_INT_MTX_BEACON_DONE_PRI    (0)
#define M_INT_MTX_BEACON_DTIM_PRI    (0)
#define M_INT_FBUS_DMA_PRI           (0)
#define M_INT_SBUS_DMA_PRI           (0)
#define M_INT_USB_PRI                (0)
#define M_INT_RTC_PRI                (0)
#define M_INT_SYSCTL_PRI             (0)
#define M_INT_PMU_PRI                (0)
#define M_INT_IRQ15_PRI              (0)
#define M_INT_I2S_PRI                (0)
#define M_INT_RATE_STAT_PRI          (0)
#define M_INT_MAILBOX_ALT_PRI        (0)
#define M_INT_MAILBOX_PRI            (0)
#define M_INT_UTIMER_0_PRI           (0)
#define M_INT_UTIMER_1_PRI           (0)
#define M_INT_UTIMER_2_PRI           (0)
#define M_INT_UTIMER_3_PRI           (0)
#define M_INT_MTIMER_0_PRI           (0)
#define M_INT_MTIMER_1_PRI           (0)
#define M_INT_MTIMER_2_PRI           (0)
#define M_INT_MTIMER_3_PRI           (0)
#define M_INT_UART_RX_PRI            (0)
#define M_INT_HSUART_RX_PRI          (0)
#define M_INT_CO_DMA_PRI             (0)
#define M_INT_IRQ31_PRI              (0)
#else
#error please define interrupt type.
#endif

#if 1
//#include "irq.h"

    /* IRQ priority */
    #define LOWEST              (3)
    #define LOW                 (2)
    #define HIGH                (1)
    #define HIGHEST             (0)

    #define IRQ_PRIO(irq)            (irq##_PRIO << (2 * (((irq) >= 16) ? ((irq) - 16): (irq))))

    #define IRQ_WDT_PRIO                        LOWEST
    #define IRQ_SWI_PRIO                        LOWEST
    #define IRQ_COMM_PRIO                  LOWEST
    #define IRQ_SPI_M_PRIO                   LOWEST
    #define IRQ_SPI_IPC_PRIO                        LOWEST
    #define IRQ_BTCOEX_PRIO                       LOWEST
    #define IRQ_FLASH_DMA_PRIO             LOWEST
    #define IRQ_MTX_BEACON_DONE_PRIO                LOWEST
    #define IRQ_MTX_BEACON_DTIM_PRIO                LOWEST
    #define IRQ_FBUS_DMA_PRIO                    LOWEST
    #define IRQ_SBUS_DMA_PRIO                        LOWEST
    #define IRQ_USB_PRIO                        HIGH
    #define IRQ_RTC_PRIO                        LOWEST
    #define IRQ_SYSCTL_PRIO                     LOWEST
    #define IRQ_PMU_PRIO                        LOWEST
    #define IRQ_GROUP_15_PRIO                   LOW

    #define IRQ_I2S_PRIO                        LOWEST
    #define IRQ_RATE_STAT_PRIO               LOWEST
    #define IRQ_MBOX_ALT_PRIO                LOWEST
    #define IRQ_MBOX_PRIO                       HIGHEST
    #define IRQ_UTIMER_0_PRIO                   HIGH
    #define IRQ_UTIMER_1_PRIO                   LOW
    #define IRQ_UTIMER_2_PRIO                   LOW
    #define IRQ_UTIMER_3_PRIO                   LOW
    #define IRQ_MTIMER_0_PRIO                   LOW
    #define IRQ_MTIMER_1_PRIO                   LOW
    #define IRQ_MTIMER_2_PRIO                   LOW
    #define IRQ_MTIMER_3_PRIO                   LOW
    #define IRQ_I2C_M_PRIO                        LOWEST
    #define IRQ_HCI_PRIO                        LOWEST
    #define IRQ_CO_DMA_PRIO                     LOWEST
    #define IRQ_GROUP_31_PRIO                   LOW


    #define PRI1_DEFAULT            (   IRQ_PRIO(IRQ_WDT) \
                                      | IRQ_PRIO(IRQ_SWI) \
                                      | IRQ_PRIO(IRQ_COMM) \
                                      | IRQ_PRIO(IRQ_SPI_M) \
                                      | IRQ_PRIO(IRQ_SPI_IPC) \
                                      | IRQ_PRIO(IRQ_BTCOEX) \
                                      | IRQ_PRIO(IRQ_FLASH_DMA) \
                                      | IRQ_PRIO(IRQ_MTX_BEACON_DONE) \
                                      | IRQ_PRIO(IRQ_MTX_BEACON_DTIM) \
                                      | IRQ_PRIO(IRQ_FBUS_DMA) \
                                      | IRQ_PRIO(IRQ_SBUS_DMA) \
                                      | IRQ_PRIO(IRQ_USB) \
                                      | IRQ_PRIO(IRQ_RTC) \
                                      | IRQ_PRIO(IRQ_SYSCTL) \
                                      | IRQ_PRIO(IRQ_PMU) \
                                      | IRQ_PRIO(IRQ_GROUP_15))

    #define PRI2_DEFAULT            (   IRQ_PRIO(IRQ_I2S) \
                                      | IRQ_PRIO(IRQ_RATE_STAT) \
                                      | IRQ_PRIO(IRQ_MBOX_ALT) \
                                      | IRQ_PRIO(IRQ_MBOX) \
                                      | IRQ_PRIO(IRQ_UTIMER_0) \
                                      | IRQ_PRIO(IRQ_UTIMER_1) \
                                      | IRQ_PRIO(IRQ_UTIMER_2) \
                                      | IRQ_PRIO(IRQ_UTIMER_3) \
                                      | IRQ_PRIO(IRQ_MTIMER_0) \
                                      | IRQ_PRIO(IRQ_MTIMER_1) \
                                      | IRQ_PRIO(IRQ_MTIMER_2) \
                                      | IRQ_PRIO(IRQ_MTIMER_3) \
                                      | IRQ_PRIO(IRQ_I2C_M) \
                                      | IRQ_PRIO(IRQ_HCI) \
                                      | IRQ_PRIO(IRQ_CO_DMA) \
                                      | IRQ_PRIO(IRQ_GROUP_31))

#endif

/*****************************************************************************
 * Macros for Register Access
 ****************************************************************************/
#define REG32(reg)		(  *( (volatile uint32_t *) (reg) ) )
#define REG32_R(_REG_) (*(volatile uint32_t *) (_REG_))
#define REG32_W(_REG_,_val_) (*(volatile uint32_t *) (_REG_) = (_val_))

#define REG32_SET_BITS(_reg, _set, _clr)    \
({                                                  \
    u32 _regval;                                    \
    _regval = REG32_R(_reg);                          \
    _regval = REG32_R(_reg);                          \
    _regval &= ~(_clr);                             \
    _regval |= (_set);                              \
    REG32_W(_reg,_regval);                            \
})

#define SR_CLRB32(reg, bit)             \
{                                       \
        int mask = __nds32__mfsr(reg)& ~(1<<bit);\
        __nds32__mtsr(mask, reg);        \
        __nds32__dsb();                          \
}

#define SR_SETB32(reg,bit)\
{\
        int mask = __nds32__mfsr(reg)|(1<<bit);\
        __nds32__mtsr(mask, reg);                       \
        __nds32__dsb();                                         \
}


#endif /* end of include guard: SOC_REGS_H */
