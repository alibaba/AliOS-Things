#include "rtc-board.h"
#include "em_cmu.h"
#include "em_rtc.h"
#include "system/timer.h"
#include "debug.h"


// Assume 32kHz RTC/RTCC clock, cmuClkDiv_2 prescaler, 16 ticks per millisecond
#define RTC_DIVIDER                     ( cmuClkDiv_1 )
#define RTC_CLOCK                       ( 32768U )
#define MSEC_TO_TICKS_DIVIDER           ( 1000U * RTC_DIVIDER )
#define MSEC_TO_TICKS_ROUNDING_FACTOR   ( MSEC_TO_TICKS_DIVIDER / 2 )

#define TICKS_TO_MSEC_ROUNDING_FACTOR   ( RTC_CLOCK / 2 )

#define TIMEDIFF( a, b )              ((( (a)<<8) - ((b)<<8) ) >> 8 )
#define RTC_MAX_VALUE                 (_RTC_CNT_MASK)
#define RTC_CLOSE_TO_MAX_VALUE        (RTC_MAX_VALUE-100UL)

#define RTC_COMP_INT                  RTC_IF_COMP0
#define RTC_INTCLEAR(x)               RTC_IntClear(x)
#define RTC_INTENABLE(x)              RTC_IntEnable(x)
#define RTC_COMPARESET( x )           RTC_CompareSet(0, (x) & _RTC_COMP0_MASK)

#define RTC_MIN_TIMEOUT               3

static uint32_t g_rtc_ticks;

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

static void delay_ticks(uint32_t ticks)
{
    uint32_t start_time;
    volatile uint32_t now;

    if (ticks) {
        start_time = RTC_CounterGet();
        do {
            now = RTC_CounterGet();
        } while (TIMEDIFF(now, start_time) < ticks);
    }
}

TimerTime_t RtcTempCompensation( TimerTime_t period, float temperature )
{
    return period;
}

uint32_t RtcMs2Tick( uint32_t milliseconds )
{
    return (uint32_t)((((uint64_t)(milliseconds) * RTC_CLOCK ) + MSEC_TO_TICKS_ROUNDING_FACTOR ) / MSEC_TO_TICKS_DIVIDER );
}

uint32_t RtcTick2Ms( uint32_t tick )
{
    return (uint32_t)((((uint64_t)(tick) * RTC_DIVIDER * 1000U ) + TICKS_TO_MSEC_ROUNDING_FACTOR ) / RTC_CLOCK );
}

void RtcDelayMs(uint32_t delay)
{
    uint32_t total_ticks;

    total_ticks = RtcMs2Tick(delay);

    while (total_ticks > RTC_CLOSE_TO_MAX_VALUE) {
        delay_ticks(RTC_CLOSE_TO_MAX_VALUE);
        total_ticks -= RTC_CLOSE_TO_MAX_VALUE;
    }

    delay_ticks(total_ticks);
}


uint32_t RtcGetTimerValue( void )
{
    return RTC_CounterGet();
}

uint32_t RtcSetTimerContext( void )
{
    g_rtc_ticks = RTC_CounterGet();
    return g_rtc_ticks;
}

uint32_t RtcGetTimerContext( void )
{
    return g_rtc_ticks;
}

uint32_t RtcGetMinimumTimeout( void )
{
    return (RTC_MIN_TIMEOUT);
}

uint32_t RtcGetTimerElapsedTime( void )
{
    uint32_t ticks = RTC_CounterGet();
    uint32_t elapsed_time;

    if (ticks >= g_rtc_ticks) {
        elapsed_time = ticks - g_rtc_ticks;
    } else {
        elapsed_time = (ticks + 0x1000000 - g_rtc_ticks);
    }

    return elapsed_time;
}

void RtcSetAlarm( uint32_t timeout )
{
    uint32_t comp_val;
    uint32_t cnt;
    uint32_t ticks = timeout;

    RTC_INTCLEAR(RTC_COMP_INT);

    cnt = RtcGetTimerContext();

    cnt += ticks;

    if (cnt > RTC_MAX_VALUE) {
        cnt -= (RTC_MAX_VALUE + 1);
    }

    RTC_COMPARESET(cnt);

    RTC_INTENABLE(RTC_COMP_INT);
}

void RtcStopAlarm( void )
{
    RTC_IntDisable(RTC_COMP_INT);
}

uint32_t RtcGetCalendarTime( uint16_t *milliseconds )
{
    uint32_t seconds = RtcGetTimerContext();

    *milliseconds = seconds % 1000;

    seconds = seconds / 1000;

    return seconds;
}

static uint32_t g_rtc_bkup_data0;
static uint32_t g_rtc_bkup_data1;
void RtcBkupWrite( uint32_t data0, uint32_t data1 )
{
    g_rtc_bkup_data0 = data0;
    g_rtc_bkup_data1 = data1;
}

void RtcBkupRead( uint32_t *data0, uint32_t *data1 )
{
    *data0 = g_rtc_bkup_data0;
    *data1 = g_rtc_bkup_data1;
}

void RtcProcess( void )
{
    // Not used on this platform.
    // }
}



