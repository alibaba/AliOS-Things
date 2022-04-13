#include <k_api.h>
#include <freertos_to_aos.h>
#include <typedef.h>
#include <sysregs.h>
#include <arch_timer.h>
#include <ptimer.h>

#define BEATS_PER_SEC RHINO_CONFIG_TICKS_PER_SECOND

void RTOS_Tick_Handler( void );

static int timer_value_for_period;
static unsigned timer_frq;

static void vtimer_on(void)
{
    arm_write_sysreg(CNTV_CTL_EL0, 1);
}

void p_timer_off(void)
{
    //arm_write_sysreg(CNTV_CTL_EL0, 0);
    arm_write_sysreg(CNTP_CTL_EL0, 0); //lk, 29
}

static void v_timer_set(int val)
{
    arm_write_sysreg(CNTV_TVAL_EL0, val);
}

void timer_set_next_event(void)
{
    int time_drift;
    /* The timer indicates an overtime with a negative value inside this register */
    arm_read_sysreg(CNTV_TVAL_EL0, time_drift);
    /* Correct next period by this time drift. The drift is caused by the software */
    v_timer_set(timer_value_for_period + time_drift);
}

int timer_init(unsigned beats_per_second)
{
    arm_read_sysreg(CNTFRQ, timer_frq);
    timer_frq = MT_ARCH_TIMER_FREQUENCY;
    if(!timer_frq) {
        printf("[arch-timer] frequency is zero!\n");
        return -1;
    }
    timer_value_for_period = timer_frq / beats_per_second;
    v_timer_set(timer_value_for_period);
    vtimer_on();

    return 0;
}

/* Function called by FreeRTOS_Tick_Handler as last action */
void vClearTickInterrupt(void)
{
    timer_set_next_event();
}

void vConfigureTickInterrupt( void )
{
    #if 0
    unsigned int irq = GIC_PPI_TIMER_VI_INT;

    /* Register the standard FreeRTOS Cortex-A tick handler as the timer's
       interrupt handler.  The handler clears the interrupt using the
       configCLEAR_TICK_INTERRUPT() macro, which is defined in FreeRTOSConfig.h. */
    printf("configure arch timer, HZ is %d\n", BEATS_PER_SEC);
    request_irq(irq, RTOS_Tick_Handler, NULL, "timer_irq", NULL);
    timer_init(BEATS_PER_SEC);
    #else
    printf("configure arch timer, HZ is %d\n", BEATS_PER_SEC);
    //gic_enable_interrupt(PTIMER_INTERRUPT);
    if (rt_hw_timer_init(BEATS_PER_SEC) != 0) {
           printf("Failed to initialize CPU timer!\n");
    }
    #endif
}

void RTOS_Tick_Handler( void )
{
	krhino_intrpt_enter();
	krhino_tick_proc();
	krhino_intrpt_exit();

	//vClearTickInterrupt();
}

static int check_timeout_tick(u64 start_tick,
                              unsigned long timeout_tick)
{
    u64 cur_tick;
    u64 elapse_tick;

    // get current tick, 1us per tick
    cur_tick = hw_timer3_count();

    // check elapse time, down counter
    elapse_tick = start_tick - cur_tick;

    // check if timeout
    if ((u64)timeout_tick <= elapse_tick) {
        // timeout
        return 1;
    }

    return 0;
}

static void busy_wait_us(unsigned int timeout_us)
{
    unsigned long timeout_tick;
    u64 start_tick;

    // get timeout tick
    timeout_tick = timeout_us;
    start_tick = hw_timer3_count();

    // wait for timeout
    while (!check_timeout_tick(start_tick, timeout_tick));
}

static void busy_wait_ms(unsigned int timeout_ms)
{
    unsigned long timeout_tick;
    u64 start_tick;

    // get timeout tick
    timeout_tick = timeout_ms * 1000;
    start_tick = hw_timer3_count();;
    // wait for timeout
    while (!check_timeout_tick(start_tick, timeout_tick));
}

/* delay usec useconds */
void udelay(unsigned long usec)
{
    busy_wait_us(usec);
}

/* delay msec mseconds */
void mdelay(unsigned long msec)
{
    busy_wait_ms(msec);
}

/* delay nsec nseconds, round up >= 1us */
void ndelay(unsigned long nsec)
{
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
    busy_wait_us(DIV_ROUND_UP(nsec, 1000));
}

