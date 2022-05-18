/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes CC2538 register definitions.
 *
 */

#ifndef CC2538_REG_H_
#define CC2538_REG_H_

#include <stdint.h>

// clang-format off

#define HWREG(x)                                (*((volatile uint32_t *)(x)))

/*!
 * For registers that are arrays of 32-bit integers.
 *
 * @param       reg     Register address
 * @param       idx     Register array index
 */
#define HWREG_ARR(reg, idx)                     HWREG((reg) + ((idx) << 2))

#define NVIC_ST_CTRL                            0xE000E010  // SysTick Control and Status
#define NVIC_ST_RELOAD                          0xE000E014  // SysTick Reload Value Register
#define NVIC_EN0                                0xE000E100  // Interrupt 0-31 Set Enable

#define NVIC_ST_CTRL_COUNT                      0x00010000  // Count Flag
#define NVIC_ST_CTRL_CLK_SRC                    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN                      0x00000002  // Interrupt Enable
#define NVIC_ST_CTRL_ENABLE                     0x00000001  // Enable

#define RFCORE_XREG_SRCMATCH_EN                 0x00000001  // SRCMATCH.SRC_MATCH_EN(1)
#define RFCORE_XREG_SRCMATCH_AUTOPEND           0x00000002  // SRCMATCH.AUTOPEND(1)
#define RFCORE_XREG_SRCMATCH_PEND_DATAREQ_ONLY  0x00000004  // SRCMATCH.PEND_DATAREQ_ONLY(1)

#define RFCORE_XREG_SRCMATCH_ENABLE_STATUS_SIZE 3           // Num of register for source match enable status
#define RFCORE_XREG_SRCMATCH_SHORT_ENTRIES      24          // 24 short address entries in maximum
#define RFCORE_XREG_SRCMATCH_EXT_ENTRIES        12          // 12 extended address entries in maximum
#define RFCORE_XREG_SRCMATCH_SHORT_ENTRY_OFFSET 4           // address offset for one short address entry
#define RFCORE_XREG_SRCMATCH_EXT_ENTRY_OFFSET   8           // address offset for one extended address entry

#define INT_UART0                               21          // UART0 Rx and Tx

#define IEEE_EUI64                              0x00280028  // Address of IEEE EUI-64 address

#define RFCORE_FFSM_SRCADDRESS_TABLE            0x40088400  // Source Address Table

#define RFCORE_FFSM_SRCEXTPENDEN0               0x40088590  // Enable/Disable automatic pending per extended address
#define RFCORE_FFSM_SRCSHORTPENDEN0             0x4008859C  // Enable/Disable automatic pending per short address
#define RFCORE_FFSM_EXT_ADDR0                   0x400885A8  // Local address information
#define RFCORE_FFSM_PAN_ID0                     0x400885C8  // Local address information
#define RFCORE_FFSM_PAN_ID1                     0x400885CC  // Local address information
#define RFCORE_FFSM_SHORT_ADDR0                 0x400885D0  // Local address information
#define RFCORE_FFSM_SHORT_ADDR1                 0x400885D4  // Local address information
#define RFCORE_XREG_FRMFILT0                    0x40088600  // The frame filtering function
#define RFCORE_XREG_SRCMATCH                    0x40088608  // Source address matching and pending bits
#define RFCORE_XREG_SRCSHORTEN0                 0x4008860C  // Short address matching
#define RFCORE_XREG_SRCEXTEN0                   0x40088618  // Extended address matching

#define RFCORE_XREG_FRMCTRL0                    0x40088624  // Frame handling
#define RFCORE_XREG_FRMCTRL1                    0x40088628  // Frame handling
#define RFCORE_XREG_RXENABLE                    0x4008862C  // RX enabling
#define RFCORE_XREG_FREQCTRL                    0x4008863C  // Controls the RF frequency
#define RFCORE_XREG_TXPOWER                     0x40088640  // Controls the output power
#define RFCORE_XREG_FSMSTAT0                    0x40088648  // Radio finite state machine status
#define RFCORE_XREG_FSMSTAT1                    0x4008864C  // Radio status register
#define RFCORE_XREG_FIFOPCTRL                   0x40088650  // FIFOP threshold
#define RFCORE_XREG_CCACTRL0                    0x40088658  // CCA threshold
#define RFCORE_XREG_RSSI                        0x40088660  // RSSI status register
#define RFCORE_XREG_RSSISTAT                    0x40088664  // RSSI valid status register
#define RFCORE_XREG_AGCCTRL1                    0x400886C8  // AGC reference level
#define RFCORE_XREG_RFC_OBS_CTRL                0x400887AC  // RF Core observable output
#define RFCORE_XREG_TXFILTCFG                   0x400887E8  // TX filter configuration
#define RFCORE_XREG_RFRND                       0x4008869C  // Random data
#define RFCORE_SFR_RFDATA                       0x40088828  // The TX FIFO and RX FIFO
#define RFCORE_SFR_RFERRF                       0x4008882C  // RF error interrupt flags
#define RFCORE_SFR_RFIRQF0                      0x40088834  // RF interrupt flags
#define RFCORE_SFR_RFST                         0x40088838  // RF CSMA-CA/strobe processor
#define CCTEST_OBSSEL                           0x44010014  // CCTEST observable output route

#define RFCORE_XREG_FRMFILT0_FRAME_FILTER_EN    0x00000001  // Enables frame filtering

#define RFCORE_XREG_FRMCTRL0_AUTOACK            0x00000020
#define RFCORE_XREG_FRMCTRL0_ENERGY_SCAN        0x00000010
#define RFCORE_XREG_FRMCTRL0_AUTOCRC            0x00000040
#define RFCORE_XREG_FRMCTRL0_INFINITY_RX        0x00000008

#define RFCORE_XREG_FRMCTRL1_PENDING_OR         0x00000004

#define RFCORE_XREG_RFRND_IRND                  0x00000001

#define RFCORE_XREG_FSMSTAT0_STATE_MASK         0x0000003F
#define RFCORE_XREG_FSMSTAT0_CAL_DONE           0x00000080
#define RFCORE_XREG_FSMSTAT0_CAL_RUN            0x00000040

#define RFCORE_XREG_FSMSTAT0_STATE_IDLE         0x00000000
#define RFCORE_XREG_FSMSTAT0_STATE_RX_CAL       0x00000002
#define RFCORE_XREG_FSMSTAT0_STATE_SFD_WAIT0    0x00000003
#define RFCORE_XREG_FSMSTAT0_STATE_SFD_WAIT1    0x00000004
#define RFCORE_XREG_FSMSTAT0_STATE_SFD_WAIT2    0x00000005
#define RFCORE_XREG_FSMSTAT0_STATE_SFD_WAIT3    0x00000006
#define RFCORE_XREG_FSMSTAT0_STATE_RX0          0x00000007
#define RFCORE_XREG_FSMSTAT0_STATE_RX1          0x00000008
#define RFCORE_XREG_FSMSTAT0_STATE_RX2          0x00000009
#define RFCORE_XREG_FSMSTAT0_STATE_RX3          0x0000000A
#define RFCORE_XREG_FSMSTAT0_STATE_RX4          0x0000000B
#define RFCORE_XREG_FSMSTAT0_STATE_RX5          0x0000000C
#define RFCORE_XREG_FSMSTAT0_STATE_RX6          0x0000000D
#define RFCORE_XREG_FSMSTAT0_STATE_RX_WAIT      0x0000000E
#define RFCORE_XREG_FSMSTAT0_STATE_RX_FRST      0x00000010
#define RFCORE_XREG_FSMSTAT0_STATE_RX_OVER      0x00000011
#define RFCORE_XREG_FSMSTAT0_STATE_TX_CAL       0x00000020
#define RFCORE_XREG_FSMSTAT0_STATE_TX0          0x00000022
#define RFCORE_XREG_FSMSTAT0_STATE_TX1          0x00000023
#define RFCORE_XREG_FSMSTAT0_STATE_TX2          0x00000024
#define RFCORE_XREG_FSMSTAT0_STATE_TX3          0x00000025
#define RFCORE_XREG_FSMSTAT0_STATE_TX4          0x00000026
#define RFCORE_XREG_FSMSTAT0_STATE_TX_FINAL     0x00000027
#define RFCORE_XREG_FSMSTAT0_STATE_RXTX_TRANS   0x00000028
#define RFCORE_XREG_FSMSTAT0_STATE_ACK_CAL      0x00000030
#define RFCORE_XREG_FSMSTAT0_STATE_ACK0         0x00000031
#define RFCORE_XREG_FSMSTAT0_STATE_ACK1         0x00000032
#define RFCORE_XREG_FSMSTAT0_STATE_ACK2         0x00000033
#define RFCORE_XREG_FSMSTAT0_STATE_ACK3         0x00000034
#define RFCORE_XREG_FSMSTAT0_STATE_ACK4         0x00000035
#define RFCORE_XREG_FSMSTAT0_STATE_ACK5         0x00000036
#define RFCORE_XREG_FSMSTAT0_STATE_ACK_DELAY    0x00000037
#define RFCORE_XREG_FSMSTAT0_STATE_TX_UNDER     0x00000038
#define RFCORE_XREG_FSMSTAT0_STATE_TX_DOWN0     0x0000001A
#define RFCORE_XREG_FSMSTAT0_STATE_TX_DOWN1     0x0000003A

#define RFCORE_XREG_FSMSTAT1_RX_ACTIVE          0x00000001
#define RFCORE_XREG_FSMSTAT1_TX_ACTIVE          0x00000002
#define RFCORE_XREG_FSMSTAT1_LOCK_STATUS        0x00000004
#define RFCORE_XREG_FSMSTAT1_SAMPLED_CCA        0x00000008
#define RFCORE_XREG_FSMSTAT1_CCA                0x00000010  // Clear channel assessment
#define RFCORE_XREG_FSMSTAT1_SFD                0x00000020
#define RFCORE_XREG_FSMSTAT1_FIFOP              0x00000040
#define RFCORE_XREG_FSMSTAT1_FIFO               0x00000080

#define RFCORE_XREG_RSSISTAT_RSSI_VALID         0x00000001  // RSSI value is valid.

#define RFCORE_XREG_RFC_OBS_POL_INV             0x00000040  // Invert polarity of OBS signal
#define RFCORE_XREG_RFC_OBS_MUX_ZERO            0x00000000  // Observable = constant zero
#define RFCORE_XREG_RFC_OBS_MUX_ONE             0x00000001  // Observable = constant one
#define RFCORE_XREG_RFC_OBS_MUX_SNIFF_DATA      0x00000008  // RFC sniff data
#define RFCORE_XREG_RFC_OBS_MUX_SNIFF_CLK       0x00000009  // RFC sniff clock
#define RFCORE_XREG_RFC_OBS_MUX_RSSI_VALID      0x0000000c  // RSSI valid
#define RFCORE_XREG_RFC_OBS_MUX_DEMOD_CCA       0x0000000d  // Clear channel assessment
#define RFCORE_XREG_RFC_OBS_MUX_SAMPLED_CCA     0x0000000e  // Sampled CCA signal
#define RFCORE_XREG_RFC_OBS_MUX_SFD_SYNC        0x0000000f  // SFD received or transmitted
#define RFCORE_XREG_RFC_OBS_MUX_TX_ACTIVE       0x00000010  // Transmitter is active
#define RFCORE_XREG_RFC_OBS_MUX_RX_ACTIVE       0x00000011  // Receiver is active
#define RFCORE_XREG_RFC_OBS_MUX_FFCTRL_FIFO     0x00000012  // One or more bytes in FIFO
#define RFCORE_XREG_RFC_OBS_MUX_FFCTRL_FIFOP    0x00000013  // One or more frames in FIFO
#define RFCORE_XREG_RFC_OBS_MUX_PACKET_DONE     0x00000014  // Packet received
#define RFCORE_XREG_RFC_OBS_MUX_RFC_XOR_RAND_IQ 0x00000016  // RAND I ^ RAND Q
#define RFCORE_XREG_RFC_OBS_MUX_RFC_RAND_Q      0x00000017  // Random data from Q channel
#define RFCORE_XREG_RFC_OBS_MUX_RFC_RAND_I      0x00000018  // Random data from I channel
#define RFCORE_XREG_RFC_OBS_MUX_LOCK_STATUS     0x00000019  // PLL is in lock
#define RFCORE_XREG_RFC_OBS_MUX_PA_PD           0x00000028  // Power amp power down
#define RFCORE_XREG_RFC_OBS_MUX_LNA_PD          0x0000002a  // LNA power down

#define RFCORE_SFR_RFERRF_NLOCK                 0x00000001  // Failed to achieve PLL lock.
#define RFCORE_SFR_RFERRF_RXABO                 0x00000002  // RX Aborted.
#define RFCORE_SFR_RFERRF_RXOVERF               0x00000004  // RX FIFO overflowed.
#define RFCORE_SFR_RFERRF_RXUNDERF              0x00000008  // RX FIFO underflowed.
#define RFCORE_SFR_RFERRF_TXOVERF               0x00000010  // TX FIFO overflowed.
#define RFCORE_SFR_RFERRF_TXUNDERF              0x00000020  // TX FIFO underflowed.
#define RFCORE_SFR_RFERRF_STROBEERR             0x00000040  // Command Strobe Error.

#define RFCORE_SFR_RFST_INSTR_RXON              0xE3        // Instruction set RX on
#define RFCORE_SFR_RFST_INSTR_TXON              0xE9        // Instruction set TX on
#define RFCORE_SFR_RFST_INSTR_RFOFF             0xEF        // Instruction set RF off
#define RFCORE_SFR_RFST_INSTR_FLUSHRX           0xED        // Instruction set flush rx buffer
#define RFCORE_SFR_RFST_INSTR_FLUSHTX           0xEE        // Instruction set flush tx buffer

#define CCTEST_OBSSEL_EN                        0x00000080  // Enable the OBS output on this pin
#define CCTEST_OBSSEL_SEL_OBS0                  0x00000000  // Route OBS0 to pin
#define CCTEST_OBSSEL_SEL_OBS1                  0x00000001  // Route OBS1 to pin
#define CCTEST_OBSSEL_SEL_OBS2                  0x00000002  // Route OBS2 to pin

#define ANA_REGS_BASE                           0x400D6000  // ANA_REGS
#define ANA_REGS_O_IVCTRL                       0x00000004  // Analog control register

#define SYS_CTRL_CLOCK_CTRL                     0x400D2000  // The clock control register
#define SYS_CTRL_SYSDIV_32MHZ                   0x00000000  // Sys_div for sysclk 32MHz
#define SYS_CTRL_CLOCK_CTRL_AMP_DET             0x00200000

#define SYS_CTRL_PWRDBG                         0x400D2074
#define SYS_CTRL_PWRDBG_FORCE_WARM_RESET        0x00000008

#define SYS_CTRL_RCGCUART                       0x400D2028
#define SYS_CTRL_SCGCUART                       0x400D202C
#define SYS_CTRL_DCGCUART                       0x400D2030
#define SYS_CTRL_I_MAP                          0x400D2098
#define SYS_CTRL_RCGCRFC                        0x400D20A8
#define SYS_CTRL_SCGCRFC                        0x400D20AC
#define SYS_CTRL_DCGCRFC                        0x400D20B0
#define SYS_CTRL_EMUOVR                         0x400D20B4

#define SYS_CTRL_RCGCRFC_RFC0                   0x00000001
#define SYS_CTRL_SCGCRFC_RFC0                   0x00000001
#define SYS_CTRL_DCGCRFC_RFC0                   0x00000001

#define SYS_CTRL_I_MAP_ALTMAP                   0x00000001

#define SYS_CTRL_RCGCUART_UART0                 0x00000001
#define SYS_CTRL_SCGCUART_UART0                 0x00000001
#define SYS_CTRL_DCGCUART_UART0                 0x00000001

#define SYS_CTRL_RCGCUART_UART1                 0x00000002
#define SYS_CTRL_SCGCUART_UART1                 0x00000002
#define SYS_CTRL_DCGCUART_UART1                 0x00000002

#define IOC_PA0_SEL                             0x400D4000  // Peripheral select control
#define IOC_PA1_SEL                             0x400D4004  // Peripheral select control
#define IOC_PA2_SEL                             0x400D4008
#define IOC_PA3_SEL                             0x400D400C
#define IOC_UARTRXD_UART0                       0x400D4100
#define IOC_UARTRXD_UART1                       0x400D4108

#define IOC_PA0_OVER                            0x400D4080
#define IOC_PA1_OVER                            0x400D4084
#define IOC_PA2_OVER                            0x400D4088
#define IOC_PA3_OVER                            0x400D408C

#define IOC_MUX_OUT_SEL_UART0_TXD               0x00000000
#define IOC_MUX_OUT_SEL_UART1_TXD               0x00000002

#define IOC_OVERRIDE_OE                         0x00000008  // PAD Config Override Output Enable
#define IOC_OVERRIDE_DIS                        0x00000000  // PAD Config Override Disabled

#define IOC_PAD_IN_SEL_PA0                      0x00000000  // PA0
#define IOC_PAD_IN_SEL_PA1                      0x00000001  // PA1
#define IOC_PAD_IN_SEL_PA2                      0x00000002  // PA2
#define IOC_PAD_IN_SEL_PA3                      0x00000003  // PA3

#define UART0_BASE                              0x4000C000
#define UART1_BASE                              0x4000D000
#define GPIO_A_BASE                             0x400D9000  // GPIO A
#define GPIO_B_BASE                             0x400DA000  // GPIO B
#define GPIO_C_BASE                             0x400DB000  // GPIO C
#define GPIO_D_BASE                             0x400DC000  // GPIO D

#define GPIO_O_DIR                              0x00000400
#define GPIO_O_AFSEL                            0x00000420

#define GPIO_PIN(x)                             (1UL << x)  // Arbitrary GPIO pin
#define GPIO_PIN_0                              0x00000001  // GPIO pin 0
#define GPIO_PIN_1                              0x00000002  // GPIO pin 1
#define GPIO_PIN_2                              0x00000004  // GPIO pin 2
#define GPIO_PIN_3                              0x00000008  // GPIO pin 3
#define GPIO_PIN_4                              0x00000010  // GPIO pin 4
#define GPIO_PIN_5                              0x00000020  // GPIO pin 5
#define GPIO_PIN_6                              0x00000040  // GPIO pin 6
#define GPIO_PIN_7                              0x00000080  // GPIO pin 7

#define UART_O_DR                               0x00000000  // UART data
#define UART_O_FR                               0x00000018  // UART flag
#define UART_O_IBRD                             0x00000024
#define UART_O_FBRD                             0x00000028
#define UART_O_LCRH                             0x0000002C
#define UART_O_CTL                              0x00000030  // UART control
#define UART_O_IM                               0x00000038  // UART interrupt mask
#define UART_O_MIS                              0x00000040  // UART masked interrupt status
#define UART_O_ICR                              0x00000044  // UART interrupt clear
#define UART_O_CC                               0x00000FC8  // UART clock configuration

#define UART_FR_RXFE                            0x00000010  // UART receive FIFO empty
#define UART_FR_TXFF                            0x00000020  // UART transmit FIFO full
#define UART_FR_RXFF                            0x00000040  // UART receive FIFO full

#define UART_CONFIG_WLEN_8                      0x00000060  // 8 bit data
#define UART_CONFIG_STOP_ONE                    0x00000000  // One stop bit
#define UART_CONFIG_PAR_NONE                    0x00000000  // No parity

#define UART_CTL_UARTEN                         0x00000001  // UART enable
#define UART_CTL_TXE                            0x00000100  // UART transmit enable
#define UART_CTL_RXE                            0x00000200  // UART receive enable

#define UART_IM_RXIM                            0x00000010  // UART receive interrupt mask
#define UART_IM_RTIM                            0x00000040  // UART receive time-out interrupt

#define SOC_ADC_ADCCON1                         0x400D7000  // ADC Control
#define SOC_ADC_RNDL                            0x400D7014  // RNG low data
#define SOC_ADC_RNDH                            0x400D7018  // RNG high data

#define SOC_ADC_ADCCON1_RCTRL0                  0x00000004  // ADCCON1 RCTRL bit 0
#define SOC_ADC_ADCCON1_RCTRL1                  0x00000008  // ADCCON1 RCTRL bit 1

#define FLASH_CTRL_FCTL                         0x400D3008  // Flash control
#define FLASH_CTRL_DIECFG0                      0x400D3014  // Flash information

// clang-format on

#endif
