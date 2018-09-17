#include "rtc.h"
#include "em_cmu.h"
#include "em_rtc.h"
#include "timeServer.h"
#include "debug.h"

static const RTC_Init_TypeDef init_rtc = {
  true,  // Start counting when init completed.
  false, // Disable updating RTC during debug halt.
  false  // Count until max. to wrap around.q
};

void startLfrcoForRtc(void)
{
    /* Starting LFRCO and waiting until it is stable */
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

    /* Enabling clock to the interface of the low energy modules */
    CMU_ClockEnable(cmuClock_CORELE, true);

    CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_LFRCO);
    CMU_ClockEnable(cmuClock_RTC, true);
}

void startLfxoForRtc(void)
{
    CMU_ClockEnable(cmuClock_CORELE, true);

    CMU_OscillatorEnable(cmuOsc_LFXO,true, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO); 
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);  

    CMU_ClockEnable(cmuClock_RTC, true);
}

void rtc_setup(void)
{
    /* Configuring clocks in the Clock Management Unit (CMU) */
    startLfxoForRtc();

    /* Enabling Interrupt from RTC */
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_EnableIRQ(RTC_IRQn);

    /* Initialize the RTC */
    RTC_Init(&init_rtc);

    RTC_IntClear(RTC_IFC_COMP0);

    RTC_IntEnable(RTC_IEN_COMP0);

    RTC_Enable(true);
}

void RTC_IRQHandler(void)
{
    if (RTC_IntGet() & RTC_IF_COMP0) {
        /* Clear interrupt source */
        RTC_IntClear(RTC_IFC_COMP0);

        TimerIrqHandler();
    }

    if (RTC_IntGet() & RTC_IF_COMP1) {
        /* Clear interrupt source */
        RTC_IntClear(RTC_IFC_COMP1);
    }
}

