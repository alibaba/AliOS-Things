/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _HAL_SIGNAL_SPY_H_
#define _HAL_SIGNAL_SPY_H_

#include "cs_types.h"
#include "chip_id.h"



/// @defgroup signalSpy HAL Debug Signal Spy
/// @par signal spy Operation
/// This chapter contains a brief description of the Signal Spy possibilities.
/// The signal spy pins are meant to be used with a logic analyser to follow
/// their evolution over time while the system is running some code.
///
/// Please also note that the signal spy pins are on with the debug port .
/// #hal_DbgPortSetup must be called with #HAL_DBG_DEBUG_BUS,
/// which disables the EXL and PXTS feature.
/// @par Some internal signal monitoring
/// The different pins of the signal spy output those signals:
/// - DBG_STB : The Frame Interrupt
/// - DBG_DATA_0 : Internal TCO RX_ON or GMSK_ON
/// - DBG_DATA_1 : Internal TCO TX_ON or RX_DEC_ON
/// - DBG_DATA_2 to DBG_DATA_6 : 4 different set of signals: System signal,
/// Baseband Signals, Tx PA Ramp signals, Rx Gain signals
/// - DBG_DATA_7 : Selection on interrupt source and a few other useful signals
/// - DBG_CLK : The debug clock, which can be configured to output any clock
/// in the chip.
///
/// @par The four modes for signal spy
///  <TABLE BORDER >
/// <TR>
///     <TH> Mode       </TH>
///     <TH> System     </TH>
///     <TH> BB modules </TH>
///     <TH> Tx PA      </TH>
///     <TH> Rx gain    </TH>
/// </TR>
/// <TR>
///     <TD> DGB_DATA_2 </TD>
///     <TD> IRQ to XCPU </TD>
///     <TD> Busy Apc </TD>
///     <TD> PA_RAMP_0 </TD>
///     <TD> SPI_SEND_CMD </TD>
/// </TR>
/// <TR>
///     <TD> DGB_DATA_3      </TD>
///     <TD> IRQ to BCPU    </TD>
///     <TD> Busy vitac     </TD>
///     <TD> PA_RAMP_1      </TD>
///     <TD> NEXT_FC        </TD>
/// </TR>
/// <TR>
///     <TD> DGB_DATA_4      </TD>
///     <TD> IRQ_Select     </TD>
///     <TD> IRQ_Select     </TD>
///     <TD> PA_RAMP_2      </TD>
///     <TD> FIRST_GAIN     </TD>
/// </TR>
/// <TR>
///     <TD> DGB_DATA_5      </TD>
///     <TD> sys_irq_sc_L   </TD>
///     <TD> Busy ITLV      </TD>
///     <TD> PA_RAMP_3      </TD>
///     <TD> NEXT_GAIN      </TD>
/// </TR>
/// <TR>
///     <TD> DGB_DATA_6      </TD>
///     <TD> Gain_masked    </TD>
///     <TD> Busy VOC       </TD>
///     <TD> PA_RAMP_4      </TD>
///     <TD> Gain_masked    </TD>
/// </TR>
/// </TABLE>
///
/// @par Interrupt Selection
/// DGB_DATA_4 and DGB_DATA_7 can each select a signal within all interrupt source
/// an some other signals. These signals are summed-up in the #HAL_DBG_SIGNAL_SPY_IRQ_SEL_T
/// enum.
///
/// @par Remainder
/// The pins of the debug port have 4 modes, they can be used to output EXL (@ref debugExl)
/// and/or, PXTS (@ref debugPxts) data, or they can be used to spy signal (@ref signalSpy).
/// Except for PXTS and EXL, those mode cannot be used simultaneously.
/// @{
///

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// HAL_DBG_SIGNAL_SPY_MODE_T
// -----------------------------------------------------------------------------
/// The 4 signal spy Mode
// =============================================================================
typedef enum
{
    /// System Signals
    HAL_DBG_SIGNAL_SPY_MODE_SYS = 0,
    /// IPs SPI
    HAL_DBG_SIGNAL_SPY_MODE_IPSPI,
    /// IPs Signals
    HAL_DBG_SIGNAL_SPY_MODE_IPSIG,
    /// Clock control signals
    HAL_DBG_SIGNAL_SPY_MODE_RF
} HAL_DBG_SIGNAL_SPY_MODE_T;

// =============================================================================
// HAL_DBG_SIGNAL_SPY_IRQ_SEL_T
// -----------------------------------------------------------------------------
/// The interrupt sources and signals which can be displayed on the
/// signal spy
// =============================================================================
// FIXME:this list is not up to date for Gallite
typedef enum
{
    /// System irq 0 form the TCU
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_TCU0 = 0,
    /// System irq 1 from the TCU
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_TCU1,
    /// System Frame interrupt
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_FINT,
    /// Comregs Port 0
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_COM0,
    // Comregs Port 1
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_COM1,
    /// VOC IRQ
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_VOC,
    /// System interrupt from the DMA
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_DMA,
    /// System interrupt from the GPIO
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_GPIO,
    /// System interrupt from the Keypad
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_KEYPAD,
    /// System interrupt from the timer module
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_TIMERS,
    /// System interrupt from the OS timer
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_OS_TIMER,
    /// System interrupt from the calendar/RTC clock
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_CALENDAR,
    /// System interrupt from the SPI
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_SPI,
    /// System interrupt from the trace uart
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_TRACE_UART,
    /// System interrupt from the UART
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_UART,
    /// System interrupt from the I2C
    HAL_DBG_SIGNAL_SPY_IRQ_I2C,
    /// System interrupt from the SCI module
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_SCI,
    /// System interrupt from the RF-SPI
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_RTF_SPI,
    /// System interrupt from the LPS
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_LPS,
    /// System interrupt from the Spy module (write attempt on a protected zone)
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_PAGE_SPY,
    /// System uses EBC (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_EBC_SYS_BUSY,
    /// Rx Start of calibration
    HAL_DBG_SIGNAL_SPY_IRQ_RX_SOC,
    /// GPIO 24 out internal value (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_GPIO_24,
    /// GPIO 25 out internal value (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_GPIO_25,
    /// GPIO 26 out internal value (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_GPIO_26,
    /// GPIO 27 out internal value (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_GPIO_27,
    /// The System AHB bus is ready (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_SYS_HREADY,
    /// XCPU instruction fetch (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_XCPU_I_FETCH,
    /// XCPU instruction cache miss (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_XCPU_I_MISS,
    /// XCPU data fetch (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_XCPU_D_FETCH,
    /// XCPU data cache miss  (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_XCPU_D_MISS,
    /// XCPU irq (Active low) : if an interrupt reaches the XCPU through the
    /// different masks and the interrupt controller
    HAL_DBG_SIGNAL_SPY_IRQ_XCPU = 31,

    /// Baseband software interrupt 0
    HAL_DBG_SIGNAL_SPY_IRQ_BB_S0 = 64,
    /// Baseband software interrupt 1
    HAL_DBG_SIGNAL_SPY_IRQ_BB_S1,
    /// Baseband irq 0 from the TCU
    HAL_DBG_SIGNAL_SPY_IRQ_BB_TCU0,
    /// Baseband irq 1 from the TCU
    HAL_DBG_SIGNAL_SPY_IRQ_BB_TCU1,
    /// Baseband Frame Interrupt
    HAL_DBG_SIGNAL_SPY_IRQ_BB_FINT,
    /// Baseband interrupt from the IFC about the channel 0
    HAL_DBG_SIGNAL_SPY_IRQ_BB_IFC_CH0,
    /// Baseband interrupt from the IFC about the channel 1
    HAL_DBG_SIGNAL_SPY_IRQ_BB_IFC_CH1,
    /// Baseband interrupt from the IFC about the channel 2
    HAL_DBG_SIGNAL_SPY_IRQ_BB_IFC_CH2,
    /// Baseband interrupt from the interleaver
    HAL_DBG_SIGNAL_SPY_IRQ_BB_ITLV,
    /// Baseband interrupt from the Viterbi Accelerator
    HAL_DBG_SIGNAL_SPY_IRQ_BB_VITAC,
    /// Baseband interrupt from the RF Interface
    HAL_DBG_SIGNAL_SPY_IRQ_BB_RFIF,
    /// Baseband interrupt from the Vocoder
    HAL_DBG_SIGNAL_SPY_IRQ_BB_VOC,
    /// Baseband interrupt from the Vocoder DMA channel
    HAL_DBG_SIGNAL_SPY_IRQ_BB_VOCDMA,
    /// Baseband interrupt from the mailbox
    HAL_DBG_SIGNAL_SPY_IRQ_BB_MAILBOX,
    /// The PLL is locked (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_PLL_LOCKED,
    /// The XCOR is busy  (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_BUSY_XCOR,
    /// bb_irq_sc_L Critical Section Status (Low =  in critical section )
    /// (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_RESERVED_50,
    /// Reserved
    HAL_DBG_SIGNAL_SPY_IRQ_RESERVED_51,
    ///
    HAL_DBG_SIGNAL_SPY_IRQ_EBC_BB_BUSY,
    /// GPIO 28 out internal value (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_GPIO_28,
    /// GPIO 29 out internal value (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_GPIO_29,
    /// GPIO 30 out internal value (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_GPIO_30,
    /// GPIO 31 out internal value (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_GPIO_31,
    /// The Baseband AHB bus is busy
    HAL_DBG_SIGNAL_SPY_IRQ_BB_HREADY,
    /// BCPU instruction fetch (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_BCPU_I_FETCH,
    /// BCPU instruction cache miss (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_BCPU_I_MISS,
    /// BCPU data fetch  (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_BCPU_D_FETCH,
    /// BCPU data cache miss  (not an interrupt)
    HAL_DBG_SIGNAL_SPY_IRQ_BCPU_D_MISS,
    /// BCPU irq (Active low) : if an interrupt reaches the XCPU through the
    /// different masks and the interrupt controller
    HAL_DBG_SIGNAL_SPY_IRQ_BCPU = 95
} HAL_DBG_SIGNAL_SPY_IRQ_SEL_T;


// =============================================================================
// HAL_DBG_SIGNAL_SPY_CLK_SEL_T
// -----------------------------------------------------------------------------
/// The clocks which can be displayed on the signal spy clock pin.
// =============================================================================
// FIXME:this list is not up to date for Gallite)
typedef enum
{
    HAL_DBG_SIGNAL_SPY_CLK_SYS_NOGATE                          = 0x00000000,
    HAL_DBG_SIGNAL_SPY_CLK_SYSD_SCI                            = 0x00000001,
    HAL_DBG_SIGNAL_SPY_CLK_SYSD_SPI                            = 0x00000002,
    HAL_DBG_SIGNAL_SPY_CLK_SYSD_OSC                            = 0x00000003,
    HAL_DBG_SIGNAL_SPY_CLK_SYSD_PWM                            = 0x00000004,
    HAL_DBG_SIGNAL_SPY_CLK_BB_NOGATE                           = 0x00000005,
    HAL_DBG_SIGNAL_SPY_CLK_32K                                 = 0x00000006,
/// this clock is taken before clock root
    HAL_DBG_SIGNAL_SPY_CLK_OSC                                 = 0x00000007,
/// this clock is taken before clock root
    HAL_DBG_SIGNAL_SPY_CLK_RF                                  = 0x00000008,
/// this clock is taken before clock root
    HAL_DBG_SIGNAL_SPY_CLK_SLOW                                = 0x00000009,
    HAL_DBG_SIGNAL_SPY_CLK_SYS_FAST                            = 0x0000000A,
    HAL_DBG_SIGNAL_SPY_CLK_BB_FAST                             = 0x0000000B,
    HAL_DBG_SIGNAL_SPY_CLK_RF_RX                               = 0x0000000C,
    HAL_DBG_SIGNAL_SPY_CLK_RF_TX                               = 0x0000000D,
    HAL_DBG_SIGNAL_SPY_CLK_TCU                                 = 0x0000000E,
    HAL_DBG_SIGNAL_SPY_CLK_104M                                = 0x0000000F,
    HAL_DBG_SIGNAL_SPY_CLK_RF_SPI                              = 0x00000010,
    HAL_DBG_SIGNAL_SPY_CLK_HOST                                = 0x00000011,
    HAL_DBG_SIGNAL_SPY_CLK_AUDIO_BCK                           = 0x00000012,
    HAL_DBG_SIGNAL_SPY_CLK_UART                                = 0x00000013,
    HAL_DBG_SIGNAL_SPY_CLK_MEM_BRIDGE                          = 0x00000014,
    HAL_DBG_SIGNAL_SPY_CLK_VOC                                 = 0x00000015,
    HAL_DBG_SIGNAL_SPY_CLK_USBPHY                              = 0x00000016
} HAL_DBG_SIGNAL_SPY_CLK_SEL_T;



// =============================================================================
//  FUNCTIONS
// =============================================================================



// =============================================================================
// hal_DbgSignalSpyCfg
// -----------------------------------------------------------------------------
/// This function configures the signal spy for the chosen mode.
/// (The device on the bus is the signal spy).
///
/// @param rxOn If \c TRUE outputs RX_ON \n
///             If \c FALSE outputs GMSK_ON
/// @param txOn If \c TRUE outputs TX_ON \n
///             If \c FALSE outputs RX_DEC_ON
/// @param busMode Select the mode within: - #HAL_DBG_SIGNAL_SPY_MODE_SYS
///                                        - #HAL_DBG_SIGNAL_SPY_MODE_BB
///                                        - #HAL_DBG_SIGNAL_SPY_MODE_TX
///                                        - #HAL_DBG_SIGNAL_SPY_MODE_RX
/// @param irqSel1 Select the signal for DBG_DATA_4 when the mode allows it
/// @param irqSel2 Select the signal for DBG_DATA_7
/// @param clkSel Select the clock to output on the DBG_CLK pin.
// =============================================================================
PUBLIC VOID hal_DbgSignalSpyCfg(
    BOOL rxOn,
    BOOL txOn,
    HAL_DBG_SIGNAL_SPY_MODE_T busMode,
    HAL_DBG_SIGNAL_SPY_IRQ_SEL_T irqSel1,
    HAL_DBG_SIGNAL_SPY_IRQ_SEL_T irqSel2,
    HAL_DBG_SIGNAL_SPY_CLK_SEL_T clkSel);



///  @} <- End of the debug_bus group



#endif // _HAL_SIGNAL_SPY_H_
