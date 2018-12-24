/* Auto-generated config file hpl_tcc_config.h */
#ifndef HPL_TCC_CONFIG_H
#define HPL_TCC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#include <peripheral_clk_config.h>
#ifndef CONF_TCC0_ENABLE
#define CONF_TCC0_ENABLE 1
#endif

#ifndef CONF_TCC0_PWM_ENABLE
#define CONF_TCC0_PWM_ENABLE 1
#endif

// <h> Basic settings
// <y> TCC0 Prescaler
// <TCC_CTRLA_PRESCALER_DIV1_Val"> No division
// <TCC_CTRLA_PRESCALER_DIV2_Val"> Divide by 2
// <TCC_CTRLA_PRESCALER_DIV4_Val"> Divide by 4
// <TCC_CTRLA_PRESCALER_DIV8_Val"> Divide by 8
// <TCC_CTRLA_PRESCALER_DIV16_Val"> Divide by 16
// <TCC_CTRLA_PRESCALER_DIV64_Val"> Divide by 64
// <TCC_CTRLA_PRESCALER_DIV256_Val"> Divide by 256
// <TCC_CTRLA_PRESCALER_DIV1024_Val"> Divide by 1024
// <i> This defines the TCC0 prescaler value
// <id> tcc_prescaler
#ifndef CONF_TCC0_PRESCALER
#define CONF_TCC0_PRESCALER TCC_CTRLA_PRESCALER_DIV8_Val
#endif

//<o> TCC0 Period Value <0x000000-0xFFFFFF>
// <id> tcc_per
#ifndef CONF_TCC0_PER
#define CONF_TCC0_PER 0x2710
#endif

// </h>

// <h> PWM Waveform Output settings
// <o> TCC0 Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tcc_arch_wave_per_val
#ifndef CONF_TCC0_WAVE_PER_VAL
#define CONF_TCC0_WAVE_PER_VAL 0x3e8
#endif

// <o> TCC0 Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tcc_arch_wave_duty_val
#ifndef CONF_TCC0_WAVE_DUTY_VAL
#define CONF_TCC0_WAVE_DUTY_VAL 0x1f4
#endif

// <o> TCC0 Waveform Channel Select <0x00-0x03>
// <id> tcc_arch_sel_ch
#ifndef CONF_TCC0_SEL_CH
#define CONF_TCC0_SEL_CH 0x0
#endif

/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TCC0_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TCC0_PER_REG                                                                                              \
	((uint32_t)(((double)CONF_TCC0_WAVE_PER_VAL * CONF_GCLK_TCC0_FREQUENCY) / 1000000 / (1 << CONF_TCC0_PRESCALER) - 1))
#define CONF_TCC0_CCX_REG ((uint32_t)(((double)(double)CONF_TCC0_PER_REG * CONF_TCC0_WAVE_DUTY_VAL) / 1000))

#elif CONF_TCC0_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TCC0_PER_REG ((uint32_t)(((double)CONF_TCC0_WAVE_PER_VAL * CONF_GCLK_TCC0_FREQUENCY) / 64000000 - 1))
#define CONF_TCC0_CCX_REG ((uint32_t)(((double)CONF_TCC0_PER_REG * CONF_TCC0_WAVE_DUTY_VAL) / 1000))

#elif CONF_TCC0_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TCC0_PER_REG ((uint32_t)(((double)CONF_TCC0_WAVE_PER_VAL * CONF_GCLK_TCC0_FREQUENCY) / 256000000 - 1))
#define CONF_TCC0_CCX_REG ((uint32_t)(((double)CONF_TCC0_PER_REG * CONF_TCC0_WAVE_DUTY_VAL) / 1000))

#elif CONF_TCC0_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TCC0_PER_REG ((uint32_t)(((double)CONF_TCC0_WAVE_PER_VAL * CONF_GCLK_TCC0_FREQUENCY) / 1024000000 - 1))
#define CONF_TCC0_CCX_REG ((uint32_t)(((double)CONF_TCC0_PER_REG * CONF_TCC0_WAVE_DUTY_VAL) / 1000))
#endif
// </h>

// <h> Advanced settings
/* Commented intentionally. Timer uses fixed value of the following bit(s)/bitfield(s) of CTRL A register.
 * May be used by other abstractions based on TC. */
//#define CONF_TCC0_RESOLUTION TCC_CTRLA_RESOLUTION_NONE_Val
// <q> Run in standby
// <i> Indicates whether the TCC0 will continue running in standby sleep mode or not
// <id> tcc_arch_runstdby
#ifndef CONF_TCC0_RUNSTDBY
#define CONF_TCC0_RUNSTDBY 0
#endif

// <y> TCC0 Prescaler and Counter Synchronization Selection
// <TCC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TCC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TCC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCCx clock or on the next prescaled GCLK_TCCx clock.
// <id> tcc_arch_prescsync
#ifndef CONF_TCC0_PRESCSYNC
#define CONF_TCC0_PRESCSYNC TCC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <y> TCC0 Waveform Generation Selection
// <TCC_WAVE_WAVEGEN_NPWM_Val"> Single-slope PWM
// <TCC_WAVE_WAVEGEN_DSCRITICAL_Val"> Dual-slope, critical interrupt/event at ZERO (DSCRITICAL)
// <TCC_WAVE_WAVEGEN_DSBOTTOM_Val"> Dual-slope, interrupt/event at ZERO (DSBOTTOM)
// <TCC_WAVE_WAVEGEN_DSBOTH_Val"> Dual-slope, interrupt/event at Top and ZERO (DSBOTH)
// <TCC_WAVE_WAVEGEN_DSTOP_Val"> Dual-slope, interrupt/event at Top (DSTOP)
// <id> tcc_arch_wavegen
#ifndef CONF_TCC0_WAVEGEN
#define CONF_TCC0_WAVEGEN TCC_WAVE_WAVEGEN_NPWM_Val
#endif
// <q> TCC0 Auto Lock
// <i> Indicates whether the TCC0 Auto Lock is enabled or not
// <id> tcc_arch_alock
#ifndef CONF_TCC0_ALOCK
#define CONF_TCC0_ALOCK 0
#endif

// <q> TCC0 Capture Channel 0 Enable
// <i> Indicates whether the TCC0 Capture Channel 0 is enabled or not
// <id> tcc_arch_cpten0
#ifndef CONF_TCC0_CPTEN0
#define CONF_TCC0_CPTEN0 0
#endif

// <q> TCC0 Capture Channel 1 Enable
// <i> Indicates whether the TCC0 Capture Channel 1 is enabled or not
// <id> tcc_arch_cpten1
#ifndef CONF_TCC0_CPTEN1
#define CONF_TCC0_CPTEN1 0
#endif

// <q> TCC0 Capture Channel 2 Enable
// <i> Indicates whether the TCC0 Capture Channel 2 is enabled or not
// <id> tcc_arch_cpten2
#ifndef CONF_TCC0_CPTEN2
#define CONF_TCC0_CPTEN2 0
#endif

// <q> TCC0 Capture Channel 3 Enable
// <i> Indicates whether the TCC0 Capture Channel 3 is enabled or not
// <id> tcc_arch_cpten3
#ifndef CONF_TCC0_CPTEN3
#define CONF_TCC0_CPTEN3 0
#endif

// <q> TCC0 Capture Channel 4 Enable
// <i> Indicates whether the TCC0 Capture Channel 4 is enabled or not
// <id> tcc_arch_cpten4
#ifndef CONF_TCC0_CPTEN4
#define CONF_TCC0_CPTEN4 1
#endif

// <q> TCC0 Capture Channel 5 Enable
// <i> Indicates whether the TCC0 Capture Channel 5 is enabled or not
// <id> tcc_arch_cpten5
#ifndef CONF_TCC0_CPTEN5
#define CONF_TCC0_CPTEN5 0
#endif

// <q> TCC0 Capture Channel 6 Enable
// <i> Indicates whether the TCC0 Capture Channel 6 is enabled or not
// <id> tcc_arch_cpten6
#ifndef CONF_TCC0_CPTEN6
#define CONF_TCC0_CPTEN6 0
#endif

// <q> TCC0 Capture Channel 7 Enable
// <i> Indicates whether the TCC0 Capture Channel 7 is enabled or not
// <id> tcc_arch_cpten7
#ifndef CONF_TCC0_CPTEN7
#define CONF_TCC0_CPTEN7 0
#endif

// <q> TCC0 Lock update
// <i> Indicates whether the TCC0 Lock update is enabled or not
// <id> tcc_arch_lupd
#ifndef CONF_TCC0_LUPD
#define CONF_TCC0_LUPD 1
#endif

/* Commented intentionally. Timer uses fixed value of the following bit(s)/bitfield(s) of CTRL B register.
 * May be used by other abstractions based on TC. */
//#define CONF_TCC0_DIR     0
//#define CONF_TCC0_ONESHOT 0

/* Commented intentionally. No fault control for timers. */
/*#define CONF_TCC0_FAULT_A_SRC       TCC_FCTRLA_SRC_DISABLE_Val
#define CONF_TCC0_FAULT_A_KEEP        0
#define CONF_TCC0_FAULT_A_QUAL        0
#define CONF_TCC0_FAULT_A_BLANK       TCC_FCTRLA_BLANK_DISABLE_Val
#define CONF_TCC0_FAULT_A_RESTART     0
#define CONF_TCC0_FAULT_A_HALT        TCC_FCTRLA_HALT_DISABLE_Val
#define CONF_TCC0_FAULT_A_CHSEL       TCC_FCTRLA_CHSEL_CC0_Val
#define CONF_TCC0_FAULT_A_CAPTURE     TCC_FCTRLA_CAPTURE_DISABLE_Val
#define CONF_TCC0_FAULT_A_BLACNKPRESC 0
#define CONF_TCC0_FAULT_A_BLANKVAL    0
#define CONF_TCC0_FAULT_A_FILTERVAL   0

#define CONF_TCC0_FAULT_B_SRC         TCC_FCTRLB_SRC_DISABLE_Val
#define CONF_TCC0_FAULT_B_KEEP        0
#define CONF_TCC0_FAULT_B_QUAL        0
#define CONF_TCC0_FAULT_B_BLANK       TCC_FCTRLB_BLANK_DISABLE_Val
#define CONF_TCC0_FAULT_B_RESTART     0
#define CONF_TCC0_FAULT_B_HALT        TCC_FCTRLB_HALT_DISABLE_Val
#define CONF_TCC0_FAULT_B_CHSEL       TCC_FCTRLB_CHSEL_CC0_Val
#define CONF_TCC0_FAULT_B_CAPTURE     TCC_FCTRLB_CAPTURE_DISABLE_Val
#define CONF_TCC0_FAULT_B_BLACNKPRESC 0
#define CONF_TCC0_FAULT_B_BLANKVAL    0
#define CONF_TCC0_FAULT_B_FILTERVAL   0*/

/* Commented intentionally. No dead-time control for timers. */
/*#define CONF_TCC0_OTMX   0
#define CONF_TCC0_DTIEN0 0
#define CONF_TCC0_DTIEN1 0
#define CONF_TCC0_DTIEN2 0
#define CONF_TCC0_DTIEN3 0
#define CONF_TCC0_DTHS   0*/

/* Commented intentionally. No driver control for timers. */
/*#define CONF_TCC0_NRE0       0
#define CONF_TCC0_NRE1       0
#define CONF_TCC0_NRE2       0
#define CONF_TCC0_NRE3       0
#define CONF_TCC0_NRE4       0
#define CONF_TCC0_NRE5       0
#define CONF_TCC0_NRE6       0
#define CONF_TCC0_NRE7       0
#define CONF_TCC0_NVR0       0
#define CONF_TCC0_NVR1       0
#define CONF_TCC0_NVR2       0
#define CONF_TCC0_NVR3       0
#define CONF_TCC0_NVR4       0
#define CONF_TCC0_NVR5       0
#define CONF_TCC0_NVR6       0
#define CONF_TCC0_NVR7       0
#define CONF_TCC0_INVEN0     0
#define CONF_TCC0_INVEN1     0
#define CONF_TCC0_INVEN2     0
#define CONF_TCC0_INVEN3     0
#define CONF_TCC0_INVEN4     0
#define CONF_TCC0_INVEN5     0
#define CONF_TCC0_INVEN6     0
#define CONF_TCC0_INVEN7     0
#define CONF_TCC0_FILTERVAL0 0
#define CONF_TCC0_FILTERVAL1 0*/

// <q> TCC0 Debug Running Mode
// <i> Indicates whether the TCC0 Debug Running Mode is enabled or not
// <id> tcc_arch_dbgrun
#ifndef CONF_TCC0_DBGRUN
#define CONF_TCC0_DBGRUN 0
#endif

/* Commented intentionally. Timer uses fixed value of the following bit(s)/bitfield(s) of Debug Control register.
 * May be used by other abstractions based on TC. */
//#define CONF_TCC0_FDDBD  0

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TCC0_EVENT_CONTROL_ENABLE
#define CONF_TCC0_EVENT_CONTROL_ENABLE 0
#endif

// <q> Match or Capture Channel 0 Event Output
// <i> This bit indicates whether match/capture event on channel 0 is enabled and will be generated
// <id> tcc_arch_mceo0
#ifndef CONF_TCC0_MCEO0
#define CONF_TCC0_MCEO0 0
#endif

// <q> Match or Capture Channel 0 Event Input
// <i> This bit indicates whether match/capture 0 incoming event is enabled
// <id> tcc_arch_mcei0
#ifndef CONF_TCC0_MCEI0
#define CONF_TCC0_MCEI0 0
#endif
// <q> Match or Capture Channel 1 Event Output
// <i> This bit indicates whether match/capture event on channel 1 is enabled and will be generated
// <id> tcc_arch_mceo1
#ifndef CONF_TCC0_MCEO1
#define CONF_TCC0_MCEO1 0
#endif

// <q> Match or Capture Channel 1 Event Input
// <i> This bit indicates whether match/capture 1 incoming event is enabled
// <id> tcc_arch_mcei1
#ifndef CONF_TCC0_MCEI1
#define CONF_TCC0_MCEI1 0
#endif
// <q> Match or Capture Channel 2 Event Output
// <i> This bit indicates whether match/capture event on channel 2 is enabled and will be generated
// <id> tcc_arch_mceo2
#ifndef CONF_TCC0_MCEO2
#define CONF_TCC0_MCEO2 0
#endif

// <q> Match or Capture Channel 2 Event Input
// <i> This bit indicates whether match/capture 2 incoming event is enabled
// <id> tcc_arch_mcei2
#ifndef CONF_TCC0_MCEI2
#define CONF_TCC0_MCEI2 0
#endif
// <q> Match or Capture Channel 3 Event Output
// <i> This bit indicates whether match/capture event on channel 3 is enabled and will be generated
// <id> tcc_arch_mceo3
#ifndef CONF_TCC0_MCEO3
#define CONF_TCC0_MCEO3 0
#endif

// <q> Match or Capture Channel 3 Event Input
// <i> This bit indicates whether match/capture 3 incoming event is enabled
// <id> tcc_arch_mcei3
#ifndef CONF_TCC0_MCEI3
#define CONF_TCC0_MCEI3 0
#endif

// <q> Timer/Counter Event Input 0
// <i> This bit is used to enable input event 0 to the TCC
// <id> tcc_arch_tcei0
#ifndef CONF_TCC0_TCEI0
#define CONF_TCC0_TCEI0 0
#endif

// <q> Timer/Counter Event Input 0 Invert
// <i> This bit inverts the event 0 input
// <id> tcc_arch_tceinv0
#ifndef CONF_TCC0_TCINV0
#define CONF_TCC0_TCINV0 0
#endif
// <q> Timer/Counter Event Input 1
// <i> This bit is used to enable input event 1 to the TCC
// <id> tcc_arch_tcei1
#ifndef CONF_TCC0_TCEI1
#define CONF_TCC0_TCEI1 0
#endif

// <q> Timer/Counter Event Input 1 Invert
// <i> This bit inverts the event 1 input
// <id> tcc_arch_tceinv1
#ifndef CONF_TCC0_TCINV1
#define CONF_TCC0_TCINV1 0
#endif

// <q> Timer/Counter Event Output
// <i> This bit is used to enable the counter cycle event.
//<id> tcc_arch_cnteo
#ifndef CONF_TCC0_CNTEO
#define CONF_TCC0_CNTEO 0
#endif

// <q> Re-trigger Event Output
// <i> This bit is used to enable the counter re-trigger event.
//<id> tcc_arch_trgeo
#ifndef CONF_TCC0_TRGEO
#define CONF_TCC0_TRGEO 0
#endif

// <q> Overflow/Underflow Event Output
// <i> This bit is used to enable enable event on overflow/underflow.
//<id> tcc_arch_ovfeo
#ifndef CONF_TCC0_OVFEO
#define CONF_TCC0_OVFEO 0
#endif

// <o> Timer/Counter Interrupt and Event Output Selection
// <0=> An interrupt/event is generated when a new counter cycle starts
// <1=> An interrupt/event is generated when a counter cycle ends
// <2=> An interrupt/event is generated when a counter cycle ends, except for the first and last cycles
// <3=> An interrupt/event is generated when a new counter cycle starts or a counter cycle ends
// <i> These bits define on which part of the counter cycle the counter event output is generated
// <id> tcc_arch_cntsel
#ifndef CONF_TCC0_CNTSEL
#define CONF_TCC0_CNTSEL 0
#endif

// <o> Timer/Counter Event Input 0 Action
// <0=>Event action disabled
// <1=>Start restart or re-trigger on event
// <2=>Count on event
// <3=>Start on event
// <4=>Increment on event
// <5=>Count on active state of asynchronous event
// <6=>Capture overflow times (Max value)
// <7=>Non-recoverable fault
// <i> These bits define the action the TCC performs on TCE0 event input 0
// <id> tcc_arch_evact0
#ifndef CONF_TCC0_EVACT0
#define CONF_TCC0_EVACT0 0
#endif

// <o> Timer/Counter Event Input 1 Action
// <0=>Event action disabled
// <1=>Re-trigger counter on event
// <2=>Direction control
// <3=>Stop counter on event
// <4=>Decrement counter on event
// <5=>Period capture value in CC0 register, pulse width capture value in CC1 register
// <6=>Period capture value in CC1 register, pulse width capture value in CC0 register
// <7=>Non-recoverable fault
// <i> These bits define the action the TCC performs on TCE0 event input 0
// <id> tcc_arch_evact1
#ifndef CONF_TCC0_EVACT1
#define CONF_TCC0_EVACT1 0
#endif
// </e>

/* Commented intentionally. No pattern control for timers. */
/*#define CONF_TCC0_PGE0 0
#define CONF_TCC0_PGE1 0
#define CONF_TCC0_PGE2 0
#define CONF_TCC0_PGE3 0
#define CONF_TCC0_PGE4 0
#define CONF_TCC0_PGE5 0
#define CONF_TCC0_PGE6 0
#define CONF_TCC0_PGE7 0
#define CONF_TCC0_PGV0 0
#define CONF_TCC0_PGV1 0
#define CONF_TCC0_PGV2 0
#define CONF_TCC0_PGV3 0
#define CONF_TCC0_PGV4 0
#define CONF_TCC0_PGV5 0
#define CONF_TCC0_PGV6 0
#define CONF_TCC0_PGV7 0*/

/* Commented intentionally. No pattern waveform control for timers. */
/*#define CONF_TCC0_WAVEGEN TCC_WAVE_WAVEGEN_MFRQ_Val
#define CONF_TCC0_RAMP    TCC_WAVE_RAMP_RAMP1_Val
#define CONF_TCC0_CIPEREN 0
#define CONF_TCC0_CICCEN0 0
#define CONF_TCC0_CICCEN1 0
#define CONF_TCC0_CICCEN2 0
#define CONF_TCC0_CICCEN3 0
#define CONF_TCC0_POL0    0
#define CONF_TCC0_POL1    0
#define CONF_TCC0_POL2    0
#define CONF_TCC0_POL3    0
#define CONF_TCC0_POL4    0
#define CONF_TCC0_POL5    0
#define CONF_TCC0_POL6    0
#define CONF_TCC0_POL7    0
#define CONF_TCC0_SWAP0   0
#define CONF_TCC0_SWAP1   0
#define CONF_TCC0_SWAP2   0
#define CONF_TCC0_SWAP3   0*/

//<o> TCC0 Compare and Capture value 0 <0x000000-0xFFFFFF>
// <id> tcc_arch_cc0
#ifndef CONF_TCC0_CC0
#define CONF_TCC0_CC0 0x0
#endif

//<o> TCC0 Compare and Capture value 1 <0x000000-0xFFFFFF>
// <id> tcc_arch_cc1
#ifndef CONF_TCC0_CC1
#define CONF_TCC0_CC1 0x0
#endif

//<o> TCC0 Compare and Capture value 2 <0x000000-0xFFFFFF>
// <id> tcc_arch_cc2
#ifndef CONF_TCC0_CC2
#define CONF_TCC0_CC2 0x0
#endif

//<o> TCC0 Compare and Capture value 3 <0x000000-0xFFFFFF>
// <id> tcc_arch_cc3
#ifndef CONF_TCC0_CC3
#define CONF_TCC0_CC3 0x0
#endif

/* Commented intentionally. No pattern control for timers. */
/*#define CONF_TCC0_PATTB_PGEB0 0
#define CONF_TCC0_PATTB_PGEB1 0
#define CONF_TCC0_PATTB_PGEB2 0
#define CONF_TCC0_PATTB_PGEB3 0
#define CONF_TCC0_PATTB_PGEB4 0
#define CONF_TCC0_PATTB_PGEB5 0
#define CONF_TCC0_PATTB_PGEB6 0
#define CONF_TCC0_PATTB_PGEB7 0
#define CONF_TCC0_PATTB_PGVB0 0
#define CONF_TCC0_PATTB_PGVB1 0
#define CONF_TCC0_PATTB_PGVB2 0
#define CONF_TCC0_PATTB_PGVB3 0
#define CONF_TCC0_PATTB_PGVB4 0
#define CONF_TCC0_PATTB_PGVB5 0
#define CONF_TCC0_PATTB_PGVB6 0
#define CONF_TCC0_PATTB_PGVB7 0*/

/* Commented intentionally. No waveform control for timers. */
/*#define CONF_TCC0_WAVEGENB TCC_WAVEB_WAVEGENB_MFRQ_Val
#define CONF_TCC0_RAMPB    TCC_WAVE_RAMP_RAMP1_Val
#define CONF_TCC0_CIPERENB 0
#define CONF_TCC0_CICCEN0B 0
#define CONF_TCC0_CICCEN1B 0
#define CONF_TCC0_CICCEN2B 0
#define CONF_TCC0_CICCEN3B 0
#define CONF_TCC0_POL0B    0
#define CONF_TCC0_POL1B    0
#define CONF_TCC0_POL2B    0
#define CONF_TCC0_POL3B    0
#define CONF_TCC0_POL4B    0
#define CONF_TCC0_POL5B    0
#define CONF_TCC0_POL6B    0
#define CONF_TCC0_POL7B    0
#define CONF_TCC0_SWAP0B   0
#define CONF_TCC0_SWAP1B   0
#define CONF_TCC0_SWAP2B   0
#define CONF_TCC0_SWAP3B   0*/

/* Commented intentionally. No buffering for timers. */
/*#define CONF_TCC0_PERB 0
#define CONF_TCC0_CCB0 0
#define CONF_TCC0_CCB1 0
#define CONF_TCC0_CCB2 0
#define CONF_TCC0_CCB3 0*/
// </h>

#include <peripheral_clk_config.h>
#ifndef CONF_TCC2_ENABLE
#define CONF_TCC2_ENABLE 1
#endif

#ifndef CONF_TCC2_PWM_ENABLE
#define CONF_TCC2_PWM_ENABLE 1
#endif

// <h> Basic settings
// <y> TCC2 Prescaler
// <TCC_CTRLA_PRESCALER_DIV1_Val"> No division
// <TCC_CTRLA_PRESCALER_DIV2_Val"> Divide by 2
// <TCC_CTRLA_PRESCALER_DIV4_Val"> Divide by 4
// <TCC_CTRLA_PRESCALER_DIV8_Val"> Divide by 8
// <TCC_CTRLA_PRESCALER_DIV16_Val"> Divide by 16
// <TCC_CTRLA_PRESCALER_DIV64_Val"> Divide by 64
// <TCC_CTRLA_PRESCALER_DIV256_Val"> Divide by 256
// <TCC_CTRLA_PRESCALER_DIV1024_Val"> Divide by 1024
// <i> This defines the TCC2 prescaler value
// <id> tcc_prescaler
#ifndef CONF_TCC2_PRESCALER
#define CONF_TCC2_PRESCALER TCC_CTRLA_PRESCALER_DIV8_Val
#endif

//<o> TCC2 Period Value <0x000000-0xFFFFFF>
// <id> tcc_per
#ifndef CONF_TCC2_PER
#define CONF_TCC2_PER 0x2710
#endif

// </h>

// <h> PWM Waveform Output settings
// <o> TCC2 Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tcc_arch_wave_per_val
#ifndef CONF_TCC2_WAVE_PER_VAL
#define CONF_TCC2_WAVE_PER_VAL 0x3e8
#endif

// <o> TCC2 Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tcc_arch_wave_duty_val
#ifndef CONF_TCC2_WAVE_DUTY_VAL
#define CONF_TCC2_WAVE_DUTY_VAL 0x1f4
#endif

// <o> TCC2 Waveform Channel Select <0x00-0x03>
// <id> tcc_arch_sel_ch
#ifndef CONF_TCC2_SEL_CH
#define CONF_TCC2_SEL_CH 0x0
#endif

/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TCC2_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TCC2_PER_REG                                                                                              \
	((uint32_t)(((double)CONF_TCC2_WAVE_PER_VAL * CONF_GCLK_TCC2_FREQUENCY) / 1000000 / (1 << CONF_TCC2_PRESCALER) - 1))
#define CONF_TCC2_CCX_REG ((uint32_t)(((double)(double)CONF_TCC2_PER_REG * CONF_TCC2_WAVE_DUTY_VAL) / 1000))

#elif CONF_TCC2_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TCC2_PER_REG ((uint32_t)(((double)CONF_TCC2_WAVE_PER_VAL * CONF_GCLK_TCC2_FREQUENCY) / 64000000 - 1))
#define CONF_TCC2_CCX_REG ((uint32_t)(((double)CONF_TCC2_PER_REG * CONF_TCC2_WAVE_DUTY_VAL) / 1000))

#elif CONF_TCC2_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TCC2_PER_REG ((uint32_t)(((double)CONF_TCC2_WAVE_PER_VAL * CONF_GCLK_TCC2_FREQUENCY) / 256000000 - 1))
#define CONF_TCC2_CCX_REG ((uint32_t)(((double)CONF_TCC2_PER_REG * CONF_TCC2_WAVE_DUTY_VAL) / 1000))

#elif CONF_TCC2_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TCC2_PER_REG ((uint32_t)(((double)CONF_TCC2_WAVE_PER_VAL * CONF_GCLK_TCC2_FREQUENCY) / 1024000000 - 1))
#define CONF_TCC2_CCX_REG ((uint32_t)(((double)CONF_TCC2_PER_REG * CONF_TCC2_WAVE_DUTY_VAL) / 1000))
#endif
// </h>

// <h> Advanced settings
/* Commented intentionally. Timer uses fixed value of the following bit(s)/bitfield(s) of CTRL A register.
 * May be used by other abstractions based on TC. */
//#define CONF_TCC2_RESOLUTION TCC_CTRLA_RESOLUTION_NONE_Val
// <q> Run in standby
// <i> Indicates whether the TCC2 will continue running in standby sleep mode or not
// <id> tcc_arch_runstdby
#ifndef CONF_TCC2_RUNSTDBY
#define CONF_TCC2_RUNSTDBY 0
#endif

// <y> TCC2 Prescaler and Counter Synchronization Selection
// <TCC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TCC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TCC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCCx clock or on the next prescaled GCLK_TCCx clock.
// <id> tcc_arch_prescsync
#ifndef CONF_TCC2_PRESCSYNC
#define CONF_TCC2_PRESCSYNC TCC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <y> TCC2 Waveform Generation Selection
// <TCC_WAVE_WAVEGEN_NPWM_Val"> Single-slope PWM
// <TCC_WAVE_WAVEGEN_DSCRITICAL_Val"> Dual-slope, critical interrupt/event at ZERO (DSCRITICAL)
// <TCC_WAVE_WAVEGEN_DSBOTTOM_Val"> Dual-slope, interrupt/event at ZERO (DSBOTTOM)
// <TCC_WAVE_WAVEGEN_DSBOTH_Val"> Dual-slope, interrupt/event at Top and ZERO (DSBOTH)
// <TCC_WAVE_WAVEGEN_DSTOP_Val"> Dual-slope, interrupt/event at Top (DSTOP)
// <id> tcc_arch_wavegen
#ifndef CONF_TCC2_WAVEGEN
#define CONF_TCC2_WAVEGEN TCC_WAVE_WAVEGEN_NPWM_Val
#endif
// <q> TCC2 Auto Lock
// <i> Indicates whether the TCC2 Auto Lock is enabled or not
// <id> tcc_arch_alock
#ifndef CONF_TCC2_ALOCK
#define CONF_TCC2_ALOCK 0
#endif

// <q> TCC2 Capture Channel 0 Enable
// <i> Indicates whether the TCC2 Capture Channel 0 is enabled or not
// <id> tcc_arch_cpten0
#ifndef CONF_TCC2_CPTEN0
#define CONF_TCC2_CPTEN0 1
#endif

// <q> TCC2 Capture Channel 1 Enable
// <i> Indicates whether the TCC2 Capture Channel 1 is enabled or not
// <id> tcc_arch_cpten1
#ifndef CONF_TCC2_CPTEN1
#define CONF_TCC2_CPTEN1 0
#endif

// <q> TCC2 Capture Channel 2 Enable
// <i> Indicates whether the TCC2 Capture Channel 2 is enabled or not
// <id> tcc_arch_cpten2
#ifndef CONF_TCC2_CPTEN2
#define CONF_TCC2_CPTEN2 0
#endif

// <q> TCC2 Capture Channel 3 Enable
// <i> Indicates whether the TCC2 Capture Channel 3 is enabled or not
// <id> tcc_arch_cpten3
#ifndef CONF_TCC2_CPTEN3
#define CONF_TCC2_CPTEN3 0
#endif

// <q> TCC2 Capture Channel 4 Enable
// <i> Indicates whether the TCC2 Capture Channel 4 is enabled or not
// <id> tcc_arch_cpten4
#ifndef CONF_TCC2_CPTEN4
#define CONF_TCC2_CPTEN4 0
#endif

// <q> TCC2 Capture Channel 5 Enable
// <i> Indicates whether the TCC2 Capture Channel 5 is enabled or not
// <id> tcc_arch_cpten5
#ifndef CONF_TCC2_CPTEN5
#define CONF_TCC2_CPTEN5 0
#endif

// <q> TCC2 Capture Channel 6 Enable
// <i> Indicates whether the TCC2 Capture Channel 6 is enabled or not
// <id> tcc_arch_cpten6
#ifndef CONF_TCC2_CPTEN6
#define CONF_TCC2_CPTEN6 0
#endif

// <q> TCC2 Capture Channel 7 Enable
// <i> Indicates whether the TCC2 Capture Channel 7 is enabled or not
// <id> tcc_arch_cpten7
#ifndef CONF_TCC2_CPTEN7
#define CONF_TCC2_CPTEN7 0
#endif

// <q> TCC2 Lock update
// <i> Indicates whether the TCC2 Lock update is enabled or not
// <id> tcc_arch_lupd
#ifndef CONF_TCC2_LUPD
#define CONF_TCC2_LUPD 1
#endif

/* Commented intentionally. Timer uses fixed value of the following bit(s)/bitfield(s) of CTRL B register.
 * May be used by other abstractions based on TC. */
//#define CONF_TCC2_DIR     0
//#define CONF_TCC2_ONESHOT 0

/* Commented intentionally. No fault control for timers. */
/*#define CONF_TCC2_FAULT_A_SRC       TCC_FCTRLA_SRC_DISABLE_Val
#define CONF_TCC2_FAULT_A_KEEP        0
#define CONF_TCC2_FAULT_A_QUAL        0
#define CONF_TCC2_FAULT_A_BLANK       TCC_FCTRLA_BLANK_DISABLE_Val
#define CONF_TCC2_FAULT_A_RESTART     0
#define CONF_TCC2_FAULT_A_HALT        TCC_FCTRLA_HALT_DISABLE_Val
#define CONF_TCC2_FAULT_A_CHSEL       TCC_FCTRLA_CHSEL_CC0_Val
#define CONF_TCC2_FAULT_A_CAPTURE     TCC_FCTRLA_CAPTURE_DISABLE_Val
#define CONF_TCC2_FAULT_A_BLACNKPRESC 0
#define CONF_TCC2_FAULT_A_BLANKVAL    0
#define CONF_TCC2_FAULT_A_FILTERVAL   0

#define CONF_TCC2_FAULT_B_SRC         TCC_FCTRLB_SRC_DISABLE_Val
#define CONF_TCC2_FAULT_B_KEEP        0
#define CONF_TCC2_FAULT_B_QUAL        0
#define CONF_TCC2_FAULT_B_BLANK       TCC_FCTRLB_BLANK_DISABLE_Val
#define CONF_TCC2_FAULT_B_RESTART     0
#define CONF_TCC2_FAULT_B_HALT        TCC_FCTRLB_HALT_DISABLE_Val
#define CONF_TCC2_FAULT_B_CHSEL       TCC_FCTRLB_CHSEL_CC0_Val
#define CONF_TCC2_FAULT_B_CAPTURE     TCC_FCTRLB_CAPTURE_DISABLE_Val
#define CONF_TCC2_FAULT_B_BLACNKPRESC 0
#define CONF_TCC2_FAULT_B_BLANKVAL    0
#define CONF_TCC2_FAULT_B_FILTERVAL   0*/

/* Commented intentionally. No dead-time control for timers. */
/*#define CONF_TCC2_OTMX   0
#define CONF_TCC2_DTIEN0 0
#define CONF_TCC2_DTIEN1 0
#define CONF_TCC2_DTIEN2 0
#define CONF_TCC2_DTIEN3 0
#define CONF_TCC2_DTHS   0*/

/* Commented intentionally. No driver control for timers. */
/*#define CONF_TCC2_NRE0       0
#define CONF_TCC2_NRE1       0
#define CONF_TCC2_NRE2       0
#define CONF_TCC2_NRE3       0
#define CONF_TCC2_NRE4       0
#define CONF_TCC2_NRE5       0
#define CONF_TCC2_NRE6       0
#define CONF_TCC2_NRE7       0
#define CONF_TCC2_NVR0       0
#define CONF_TCC2_NVR1       0
#define CONF_TCC2_NVR2       0
#define CONF_TCC2_NVR3       0
#define CONF_TCC2_NVR4       0
#define CONF_TCC2_NVR5       0
#define CONF_TCC2_NVR6       0
#define CONF_TCC2_NVR7       0
#define CONF_TCC2_INVEN0     0
#define CONF_TCC2_INVEN1     0
#define CONF_TCC2_INVEN2     0
#define CONF_TCC2_INVEN3     0
#define CONF_TCC2_INVEN4     0
#define CONF_TCC2_INVEN5     0
#define CONF_TCC2_INVEN6     0
#define CONF_TCC2_INVEN7     0
#define CONF_TCC2_FILTERVAL0 0
#define CONF_TCC2_FILTERVAL1 0*/

// <q> TCC2 Debug Running Mode
// <i> Indicates whether the TCC2 Debug Running Mode is enabled or not
// <id> tcc_arch_dbgrun
#ifndef CONF_TCC2_DBGRUN
#define CONF_TCC2_DBGRUN 0
#endif

/* Commented intentionally. Timer uses fixed value of the following bit(s)/bitfield(s) of Debug Control register.
 * May be used by other abstractions based on TC. */
//#define CONF_TCC2_FDDBD  0

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TCC2_EVENT_CONTROL_ENABLE
#define CONF_TCC2_EVENT_CONTROL_ENABLE 0
#endif

// <q> Match or Capture Channel 0 Event Output
// <i> This bit indicates whether match/capture event on channel 0 is enabled and will be generated
// <id> tcc_arch_mceo0
#ifndef CONF_TCC2_MCEO0
#define CONF_TCC2_MCEO0 0
#endif

// <q> Match or Capture Channel 0 Event Input
// <i> This bit indicates whether match/capture 0 incoming event is enabled
// <id> tcc_arch_mcei0
#ifndef CONF_TCC2_MCEI0
#define CONF_TCC2_MCEI0 0
#endif
// <q> Match or Capture Channel 1 Event Output
// <i> This bit indicates whether match/capture event on channel 1 is enabled and will be generated
// <id> tcc_arch_mceo1
#ifndef CONF_TCC2_MCEO1
#define CONF_TCC2_MCEO1 0
#endif

// <q> Match or Capture Channel 1 Event Input
// <i> This bit indicates whether match/capture 1 incoming event is enabled
// <id> tcc_arch_mcei1
#ifndef CONF_TCC2_MCEI1
#define CONF_TCC2_MCEI1 0
#endif
// <q> Match or Capture Channel 2 Event Output
// <i> This bit indicates whether match/capture event on channel 2 is enabled and will be generated
// <id> tcc_arch_mceo2
#ifndef CONF_TCC2_MCEO2
#define CONF_TCC2_MCEO2 0
#endif

// <q> Match or Capture Channel 2 Event Input
// <i> This bit indicates whether match/capture 2 incoming event is enabled
// <id> tcc_arch_mcei2
#ifndef CONF_TCC2_MCEI2
#define CONF_TCC2_MCEI2 0
#endif
// <q> Match or Capture Channel 3 Event Output
// <i> This bit indicates whether match/capture event on channel 3 is enabled and will be generated
// <id> tcc_arch_mceo3
#ifndef CONF_TCC2_MCEO3
#define CONF_TCC2_MCEO3 0
#endif

// <q> Match or Capture Channel 3 Event Input
// <i> This bit indicates whether match/capture 3 incoming event is enabled
// <id> tcc_arch_mcei3
#ifndef CONF_TCC2_MCEI3
#define CONF_TCC2_MCEI3 0
#endif

// <q> Timer/Counter Event Input 0
// <i> This bit is used to enable input event 0 to the TCC
// <id> tcc_arch_tcei0
#ifndef CONF_TCC2_TCEI0
#define CONF_TCC2_TCEI0 0
#endif

// <q> Timer/Counter Event Input 0 Invert
// <i> This bit inverts the event 0 input
// <id> tcc_arch_tceinv0
#ifndef CONF_TCC2_TCINV0
#define CONF_TCC2_TCINV0 0
#endif
// <q> Timer/Counter Event Input 1
// <i> This bit is used to enable input event 1 to the TCC
// <id> tcc_arch_tcei1
#ifndef CONF_TCC2_TCEI1
#define CONF_TCC2_TCEI1 0
#endif

// <q> Timer/Counter Event Input 1 Invert
// <i> This bit inverts the event 1 input
// <id> tcc_arch_tceinv1
#ifndef CONF_TCC2_TCINV1
#define CONF_TCC2_TCINV1 0
#endif

// <q> Timer/Counter Event Output
// <i> This bit is used to enable the counter cycle event.
//<id> tcc_arch_cnteo
#ifndef CONF_TCC2_CNTEO
#define CONF_TCC2_CNTEO 0
#endif

// <q> Re-trigger Event Output
// <i> This bit is used to enable the counter re-trigger event.
//<id> tcc_arch_trgeo
#ifndef CONF_TCC2_TRGEO
#define CONF_TCC2_TRGEO 0
#endif

// <q> Overflow/Underflow Event Output
// <i> This bit is used to enable enable event on overflow/underflow.
//<id> tcc_arch_ovfeo
#ifndef CONF_TCC2_OVFEO
#define CONF_TCC2_OVFEO 0
#endif

// <o> Timer/Counter Interrupt and Event Output Selection
// <0=> An interrupt/event is generated when a new counter cycle starts
// <1=> An interrupt/event is generated when a counter cycle ends
// <2=> An interrupt/event is generated when a counter cycle ends, except for the first and last cycles
// <3=> An interrupt/event is generated when a new counter cycle starts or a counter cycle ends
// <i> These bits define on which part of the counter cycle the counter event output is generated
// <id> tcc_arch_cntsel
#ifndef CONF_TCC2_CNTSEL
#define CONF_TCC2_CNTSEL 0
#endif

// <o> Timer/Counter Event Input 0 Action
// <0=>Event action disabled
// <1=>Start restart or re-trigger on event
// <2=>Count on event
// <3=>Start on event
// <4=>Increment on event
// <5=>Count on active state of asynchronous event
// <6=>Capture overflow times (Max value)
// <7=>Non-recoverable fault
// <i> These bits define the action the TCC performs on TCE0 event input 0
// <id> tcc_arch_evact0
#ifndef CONF_TCC2_EVACT0
#define CONF_TCC2_EVACT0 0
#endif

// <o> Timer/Counter Event Input 1 Action
// <0=>Event action disabled
// <1=>Re-trigger counter on event
// <2=>Direction control
// <3=>Stop counter on event
// <4=>Decrement counter on event
// <5=>Period capture value in CC0 register, pulse width capture value in CC1 register
// <6=>Period capture value in CC1 register, pulse width capture value in CC0 register
// <7=>Non-recoverable fault
// <i> These bits define the action the TCC performs on TCE0 event input 0
// <id> tcc_arch_evact1
#ifndef CONF_TCC2_EVACT1
#define CONF_TCC2_EVACT1 0
#endif
// </e>

/* Commented intentionally. No pattern control for timers. */
/*#define CONF_TCC2_PGE0 0
#define CONF_TCC2_PGE1 0
#define CONF_TCC2_PGE2 0
#define CONF_TCC2_PGE3 0
#define CONF_TCC2_PGE4 0
#define CONF_TCC2_PGE5 0
#define CONF_TCC2_PGE6 0
#define CONF_TCC2_PGE7 0
#define CONF_TCC2_PGV0 0
#define CONF_TCC2_PGV1 0
#define CONF_TCC2_PGV2 0
#define CONF_TCC2_PGV3 0
#define CONF_TCC2_PGV4 0
#define CONF_TCC2_PGV5 0
#define CONF_TCC2_PGV6 0
#define CONF_TCC2_PGV7 0*/

/* Commented intentionally. No pattern waveform control for timers. */
/*#define CONF_TCC2_WAVEGEN TCC_WAVE_WAVEGEN_MFRQ_Val
#define CONF_TCC2_RAMP    TCC_WAVE_RAMP_RAMP1_Val
#define CONF_TCC2_CIPEREN 0
#define CONF_TCC2_CICCEN0 0
#define CONF_TCC2_CICCEN1 0
#define CONF_TCC2_CICCEN2 0
#define CONF_TCC2_CICCEN3 0
#define CONF_TCC2_POL0    0
#define CONF_TCC2_POL1    0
#define CONF_TCC2_POL2    0
#define CONF_TCC2_POL3    0
#define CONF_TCC2_POL4    0
#define CONF_TCC2_POL5    0
#define CONF_TCC2_POL6    0
#define CONF_TCC2_POL7    0
#define CONF_TCC2_SWAP0   0
#define CONF_TCC2_SWAP1   0
#define CONF_TCC2_SWAP2   0
#define CONF_TCC2_SWAP3   0*/

//<o> TCC2 Compare and Capture value 0 <0x000000-0xFFFFFF>
// <id> tcc_arch_cc0
#ifndef CONF_TCC2_CC0
#define CONF_TCC2_CC0 0x0
#endif

//<o> TCC2 Compare and Capture value 1 <0x000000-0xFFFFFF>
// <id> tcc_arch_cc1
#ifndef CONF_TCC2_CC1
#define CONF_TCC2_CC1 0x0
#endif

//<o> TCC2 Compare and Capture value 2 <0x000000-0xFFFFFF>
// <id> tcc_arch_cc2
#ifndef CONF_TCC2_CC2
#define CONF_TCC2_CC2 0x0
#endif

//<o> TCC2 Compare and Capture value 3 <0x000000-0xFFFFFF>
// <id> tcc_arch_cc3
#ifndef CONF_TCC2_CC3
#define CONF_TCC2_CC3 0x0
#endif

/* Commented intentionally. No pattern control for timers. */
/*#define CONF_TCC2_PATTB_PGEB0 0
#define CONF_TCC2_PATTB_PGEB1 0
#define CONF_TCC2_PATTB_PGEB2 0
#define CONF_TCC2_PATTB_PGEB3 0
#define CONF_TCC2_PATTB_PGEB4 0
#define CONF_TCC2_PATTB_PGEB5 0
#define CONF_TCC2_PATTB_PGEB6 0
#define CONF_TCC2_PATTB_PGEB7 0
#define CONF_TCC2_PATTB_PGVB0 0
#define CONF_TCC2_PATTB_PGVB1 0
#define CONF_TCC2_PATTB_PGVB2 0
#define CONF_TCC2_PATTB_PGVB3 0
#define CONF_TCC2_PATTB_PGVB4 0
#define CONF_TCC2_PATTB_PGVB5 0
#define CONF_TCC2_PATTB_PGVB6 0
#define CONF_TCC2_PATTB_PGVB7 0*/

/* Commented intentionally. No waveform control for timers. */
/*#define CONF_TCC2_WAVEGENB TCC_WAVEB_WAVEGENB_MFRQ_Val
#define CONF_TCC2_RAMPB    TCC_WAVE_RAMP_RAMP1_Val
#define CONF_TCC2_CIPERENB 0
#define CONF_TCC2_CICCEN0B 0
#define CONF_TCC2_CICCEN1B 0
#define CONF_TCC2_CICCEN2B 0
#define CONF_TCC2_CICCEN3B 0
#define CONF_TCC2_POL0B    0
#define CONF_TCC2_POL1B    0
#define CONF_TCC2_POL2B    0
#define CONF_TCC2_POL3B    0
#define CONF_TCC2_POL4B    0
#define CONF_TCC2_POL5B    0
#define CONF_TCC2_POL6B    0
#define CONF_TCC2_POL7B    0
#define CONF_TCC2_SWAP0B   0
#define CONF_TCC2_SWAP1B   0
#define CONF_TCC2_SWAP2B   0
#define CONF_TCC2_SWAP3B   0*/

/* Commented intentionally. No buffering for timers. */
/*#define CONF_TCC2_PERB 0
#define CONF_TCC2_CCB0 0
#define CONF_TCC2_CCB1 0
#define CONF_TCC2_CCB2 0
#define CONF_TCC2_CCB3 0*/
// </h>

// <<< end of configuration section >>>

#endif // HPL_TCC_CONFIG_H
