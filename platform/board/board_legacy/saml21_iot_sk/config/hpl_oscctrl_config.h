/* Auto-generated config file hpl_oscctrl_config.h */
#ifndef HPL_OSCCTRL_CONFIG_H
#define HPL_OSCCTRL_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <e> External Multipurpose Crystal Oscillator Configuration
// <i> Indicates whether configuration for XOSC is enabled or not
// <id> enable_xosc
#ifndef CONF_XOSC_CONFIG
#define CONF_XOSC_CONFIG 0
#endif

// <o> Frequency <400000-32000000>
// <i> Oscillation frequency of the resonator connected to the External Multipurpose Crystal Oscillator.
// <id> xosc_frequency
#ifndef CONF_XOSC_FREQUENCY
#define CONF_XOSC_FREQUENCY 400000
#endif

// <h> External Multipurpose Crystal Oscillator Control
// <q> Oscillator enable
// <i> Indicates whether External Multipurpose Crystal Oscillator is enabled or not
// <id> xosc_arch_enable
#ifndef CONF_XOSC_ENABLE
#define CONF_XOSC_ENABLE 0
#endif

// <o> Start-Up Time
// <0x0=>31us
// <0x1=>61us
// <0x2=>122us
// <0x3=>244us
// <0x4=>488us
// <0x5=>977us
// <0x6=>1953us
// <0x7=>3906us
// <0x8=>7813us
// <0x9=>15625us
// <0xA=>31250us
// <0xB=>62500us
// <0xC=>125000us
// <0xD=>250000us
// <0xE=>500000us
// <0xF=>1000000us
// <id> xosc_arch_startup
#ifndef CONF_XOSC_STARTUP
#define CONF_XOSC_STARTUP 0x0
#endif

// <q> Automatic Amplitude Gain Control
// <i> Indicates whether Automatic Amplitude Gain Control is enabled or not
// <id> xosc_arch_ampgc
#ifndef CONF_XOSC_AMPGC
#define CONF_XOSC_AMPGC 0
#endif

// <o> External Multipurpose Crystal Oscillator Gain
// <0x0=>2MHz
// <0x1=>4MHz
// <0x2=>8MHz
// <0x3=>16MHz
// <0x4=>30MHz
// <id> xosc_arch_gain
#ifndef CONF_XOSC_GAIN
#define CONF_XOSC_GAIN 0x0
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> xosc_arch_ondemand
#ifndef CONF_XOSC_ONDEMAND
#define CONF_XOSC_ONDEMAND 1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> xosc_arch_runstdby
#ifndef CONF_XOSC_RUNSTDBY
#define CONF_XOSC_RUNSTDBY 0
#endif

// <q> Crystal connected to XIN/XOUT Enable
// <i> Indicates whether the connections between the I/O pads and the external clock or crystal oscillator is enabled or not
// <id> xosc_arch_xtalen
#ifndef CONF_XOSC_XTALEN
#define CONF_XOSC_XTALEN 0
#endif
//</h>
//</e>

// <e> 16MHz Internal Oscillator Configuration
// <i> Indicates whether configuration for OSC8M is enabled or not
// <id> enable_osc16m
#ifndef CONF_OSC16M_CONFIG
#define CONF_OSC16M_CONFIG 1
#endif

// <h> 16MHz Internal Oscillator Control
// <q> Enable
// <i> Indicates whether 16MHz Internal Oscillator is enabled or not
// <id> osc16m_arch_enable
#ifndef CONF_OSC16M_ENABLE
#define CONF_OSC16M_ENABLE 1
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> osc16m_arch_ondemand
#ifndef CONF_OSC16M_ONDEMAND
#define CONF_OSC16M_ONDEMAND 1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> osc16m_arch_runstdby
#ifndef CONF_OSC16M_RUNSTDBY
#define CONF_OSC16M_RUNSTDBY 0
#endif

// <y> Oscillator Frequency Selection(Mhz)
// <OSCCTRL_OSC16MCTRL_FSEL_4_Val"> 4
// <OSCCTRL_OSC16MCTRL_FSEL_8_Val"> 8
// <OSCCTRL_OSC16MCTRL_FSEL_12_Val"> 12
// <OSCCTRL_OSC16MCTRL_FSEL_16_Val"> 16
// <i> This defines the oscillator frequency (Mhz)
// <id> osc16m_freq
#ifndef CONF_OSC16M_FSEL
#define CONF_OSC16M_FSEL OSCCTRL_OSC16MCTRL_FSEL_16_Val
#endif

// <q> Oscillator Calibration Control
// <i> Indicates whether Oscillator Calibration is enabled or not
// <id> osc16m_arch_calib_enable
#ifndef CONF_OSC16M_CALIB_ENABLE
#define CONF_OSC16M_CALIB_ENABLE 0
#endif

// <o> 4MHz Frequency Calibration <0x0-0x3F>
// <id> osc16m_arch_4m_fcal
#ifndef CONF_OSC16M_FCAL
#define CONF_OSC16M_4M_FCAL 0
#endif

// <o> 4MHz Temperature Calibration <0x0-0x3F>
// <id> osc16m_arch_4m_tcal
#ifndef CONF_OSC16M_TCAL
#define CONF_OSC16M_4M_TCAL 0
#endif

// <o> 8MHz Frequency Calibration <0x0-0x3F>
// <id> osc16m_arch_8m_fcal
#ifndef CONF_OSC16M_FCAL
#define CONF_OSC16M_8M_FCAL 0
#endif

// <o> 8MHz Temperature Calibration <0x0-0x3F>
// <id> osc16m_arch_8m_tcal
#ifndef CONF_OSC16M_TCAL
#define CONF_OSC16M_8M_TCAL 0
#endif

// <o> 12MHz Frequency Calibration <0x0-0x3F>
// <id> osc16m_arch_12m_fcal
#ifndef CONF_OSC16M_FCAL
#define CONF_OSC16M_12M_FCAL 0
#endif

// <o> 12MHz Temperature Calibration <0x0-0x3F>
// <id> osc16m_arch_12m_tcal
#ifndef CONF_OSC16M_TCAL
#define CONF_OSC16M_12M_TCAL 0
#endif

// <o> 16MHz Frequency Calibration <0x0-0x3F>
// <id> osc16m_arch_fcal
#ifndef CONF_OSC16M_FCAL
#define CONF_OSC16M_16M_FCAL 0
#endif

// <o> 16MHz Temperature Calibration <0x0-0x3F>
// <id> osc16m_arch_16m_tcal
#ifndef CONF_OSC16M_TCAL
#define CONF_OSC16M_16M_TCAL 0
#endif
//</h>
//</e>

// <e> DFLL Configuration
// <i> Indicates whether configuration for DFLL is enabled or not
// <id> enable_dfll48m
#ifndef CONF_DFLL_CONFIG
#define CONF_DFLL_CONFIG 0
#endif

// <y> Reference Clock Source
// <GCLK_PCHCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <GCLK_PCHCTRL_GEN_GCLK1_Val"> Generic clock generator 1
// <GCLK_PCHCTRL_GEN_GCLK2_Val"> Generic clock generator 2
// <GCLK_PCHCTRL_GEN_GCLK3_Val"> Generic clock generator 3
// <GCLK_PCHCTRL_GEN_GCLK4_Val"> Generic clock generator 4
// <GCLK_PCHCTRL_GEN_GCLK5_Val"> Generic clock generator 5
// <GCLK_PCHCTRL_GEN_GCLK6_Val"> Generic clock generator 6
// <GCLK_PCHCTRL_GEN_GCLK7_Val"> Generic clock generator 7
// <i> Select the clock source.
// <id> dfll48m_ref_clock
#ifndef CONF_DFLL_GCLK
#define CONF_DFLL_GCLK GCLK_PCHCTRL_GEN_GCLK3_Val
#endif

// <h> Digital Frequency Locked Loop Control
// <q> DFLL Enable
// <i> Indicates whether DFLL is enabled or not
// <id> dfll48m_arch_enable
#ifndef CONF_DFLL_ENABLE
#define CONF_DFLL_ENABLE 0
#endif

// <q> Wait Lock
// <i> Indicates whether Wait Lock is enabled or not
// <id> dfll_arch_waitlock
#ifndef CONF_DFLL_WAITLOCK
#define CONF_DFLL_WAITLOCK 0
#endif

// <q> Bypass Coarse Lock
// <i> Indicates whether Bypass Coarse Lock is enabled or not
// <id> dfll_arch_bplckc
#ifndef CONF_DFLL_BPLCKC
#define CONF_DFLL_BPLCKC 0
#endif

// <q> Quick Lock Disable
// <i> Indicates whether Quick Lock Disable is enabled or not
// <id> dfll_arch_qldis
#ifndef CONF_DFLL_QLDIS
#define CONF_DFLL_QLDIS 0
#endif

// <q> Chill Cycle Disable
// <i> Indicates whether Chill Cycle Disable is enabled or not
// <id> dfll_arch_ccdis
#ifndef CONF_DFLL_CCDIS
#define CONF_DFLL_CCDIS 0
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> dfll_arch_ondemand
#ifndef CONF_DFLL_ONDEMAND
#define CONF_DFLL_ONDEMAND 1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> dfll_arch_runstdby
#ifndef CONF_DFLL_RUNSTDBY
#define CONF_DFLL_RUNSTDBY 0
#endif

// <q> USB Clock Recovery Mode
// <i> Indicates whether USB Clock Recovery Mode is enabled or not
// <id> dfll_arch_usbcrm
#ifndef CONF_DFLL_USBCRM
#define CONF_DFLL_USBCRM 0
#endif

// <q> Lose Lock After Wake
// <i> Indicates whether Lose Lock After Wake is enabled or not
// <id> dfll_arch_llaw
#ifndef CONF_DFLL_LLAW
#define CONF_DFLL_LLAW 0
#endif

// <q> Stable DFLL Frequency
// <i> Indicates whether Stable DFLL Frequency is enabled or not
// <id> dfll_arch_stable
#ifndef CONF_DFLL_STABLE
#define CONF_DFLL_STABLE 0
#endif

// <o> Operating Mode Selection
// <0=>Open Loop Mode
// <1=>Closed Loop Mode
// <id> dfll48m_mode
#ifndef CONF_DFLL_MODE
#define CONF_DFLL_MODE 0
#endif

// <o> Coarse Maximum Step <0x0-0x1F>
// <id> dfll_arch_cstep
#ifndef CONF_DFLL_CSTEP
#define CONF_DFLL_CSTEP 1
#endif

// <o> Fine Maximum Step <0x0-0x3FF>
// <id> dfll_arch_fstep
#ifndef CONF_DFLL_FSTEP
#define CONF_DFLL_FSTEP 1
#endif

// <o> DFLL Multiply Factor <0x0-0xFFFF>
//  <id> dfll48m_mul
#ifndef CONF_DFLL_MUL
#define CONF_DFLL_MUL 0
#endif

// <e> DFLL Calibration Overwrite
// <i> Indicates whether Overwrite Calibration value of DFLL
// <id> dfll_arch_calibration
#ifndef CONF_DFLL_OVERWRITE_CALIBRATION
#define CONF_DFLL_OVERWRITE_CALIBRATION 0
#endif

// <o> Coarse Value <0x0-0x3F>
// <id> dfll_arch_coarse
#ifndef CONF_DFLL_COARSE
#define CONF_DFLL_COARSE (0x1f / 4)
#endif

// <o> Fine Value <0x0-0x3FF>
// <id> dfll_arch_fine
#ifndef CONF_DFLL_FINE
#define CONF_DFLL_FINE (0x200)
#endif

//</e>

//</h>

//</e>

// <e> DPLL Configuration
// <i> Indicates whether configuration for DPLL is enabled or not
// <id> enable_fdpll96m
#ifndef CONF_DPLL_CONFIG
#define CONF_DPLL_CONFIG 0
#endif

// <y> Reference Clock Source
// <GCLK_GENCTRL_SRC_XOSC32K"> 32kHz External Crystal Oscillator (XOSC32K)
// <GCLK_GENCTRL_SRC_XOSC"> External Crystal Oscillator 0.4-32MHz (XOSC)
// <GCLK_PCHCTRL_GEN_GCLK0_Val"> Generic clock generator 0
// <GCLK_PCHCTRL_GEN_GCLK1_Val"> Generic clock generator 1
// <GCLK_PCHCTRL_GEN_GCLK2_Val"> Generic clock generator 2
// <GCLK_PCHCTRL_GEN_GCLK3_Val"> Generic clock generator 3
// <GCLK_PCHCTRL_GEN_GCLK4_Val"> Generic clock generator 4
// <GCLK_PCHCTRL_GEN_GCLK5_Val"> Generic clock generator 5
// <GCLK_PCHCTRL_GEN_GCLK6_Val"> Generic clock generator 6
// <GCLK_PCHCTRL_GEN_GCLK7_Val"> Generic clock generator 7
// <i> Select the clock source.
// <id> fdpll96m_ref_clock
#ifndef CONF_DPLL_GCLK
#define CONF_DPLL_GCLK GCLK_GENCTRL_SRC_XOSC32K

#endif

// <h> Digital Phase Locked Loop Control
// <q> Enable
// <i> Indicates whether Digital Phase Locked Loop is enabled or not
// <id> fdpll96m_arch_enable
#ifndef CONF_DPLL_ENABLE
#define CONF_DPLL_ENABLE 0
#endif

// <q> On Demand Control
// <i> Indicates whether On Demand Control is enabled or not
// <id> fdpll96m_arch_ondemand
#ifndef CONF_DPLL_ONDEMAND
#define CONF_DPLL_ONDEMAND 1
#endif

// <q> Run in Standby
// <i> Indicates whether Run in Standby is enabled or not
// <id> fdpll96m_arch_runstdby
#ifndef CONF_DPLL_RUNSTDBY
#define CONF_DPLL_RUNSTDBY 0
#endif

// <o> Loop Divider Ratio Fractional Part <0x0-0xF>
// <id> fdpll96m_ldrfrac
#ifndef CONF_DPLL_LDRFRAC
#define CONF_DPLL_LDRFRAC 0xd
#endif

// <o> Loop Divider Ratio Integer Part <0x0-0xFFF>
// <id> fdpll96m_ldr
#ifndef CONF_DPLL_LDR
#define CONF_DPLL_LDR 0x5b7
#endif

// <o> Clock Divider <0x0-0x3FF>
// <id> fdpll96m_clock_div
#ifndef CONF_DPLL_DIV
#define CONF_DPLL_DIV 0
#endif

// <q> Lock Bypass
// <i> Indicates whether Lock Bypass is enabled or not
// <id> fdpll96m_arch_lbypass
#ifndef CONF_DPLL_LBYPASS
#define CONF_DPLL_LBYPASS 0
#endif

// <o> Lock Time
// <0=>No time-out, automatic lock
// <4=>The Time-out if no lock within 8 ms
// <5=>The Time-out if no lock within 9 ms
// <6=>The Time-out if no lock within 10 ms
// <7=>The Time-out if no lock within 11 ms
// <id> fdpll96m_arch_ltime
#ifndef CONF_DPLL_LTIME
#define CONF_DPLL_LTIME 0
#endif

// <o> Reference Clock Selection
// <0=>XOSC32K clock reference
// <1=>XOSC clock reference
// <2=>GCLK clock reference
// <id> fdpll96m_arch_refclk
#ifndef CONF_DPLL_REFCLK
#define CONF_DPLL_REFCLK 0
#endif

// <q> Wake Up Fast
// <i> Indicates whether Wake Up Fast is enabled or not
// <id> fdpll96m_arch_wuf
#ifndef CONF_DPLL_WUF
#define CONF_DPLL_WUF 0
#endif

// <q> Low-Power Enable
// <i> Indicates whether Low-Power Enable is enabled or not
// <id> fdpll96m_arch_lpen
#ifndef CONF_DPLL_LPEN
#define CONF_DPLL_LPEN 0
#endif

// <o> Reference Clock Selection
// <0=>Default filter mode
// <1=>Low bandwidth filter
// <2=>High bandwidth filter
// <3=>High damping filter
// <id> fdpll96m_arch_filter
#ifndef CONF_DPLL_FILTER
#define CONF_DPLL_FILTER 0
#endif

// <y> Output Clock Prescaler
// <OSCCTRL_DPLLPRESC_PRESC_DIV1_Val"> 1
// <OSCCTRL_DPLLPRESC_PRESC_DIV2_Val"> 2
// <OSCCTRL_DPLLPRESC_PRESC_DIV4_Val"> 4
// <id> fdpll96m_presc
#ifndef CONF_DPLL_PRESC
#define CONF_DPLL_PRESC OSCCTRL_DPLLPRESC_PRESC_DIV1_Val
#endif
//</h>
//</e>

// <<< end of configuration section >>>

#endif // HPL_OSCCTRL_CONFIG_H
