#include <stdio.h>
#include "lega_cm4.h"
#include "lega_common.h"
#include "lega_peripheral_reg.h"
#include "lega_timer.h"

lega_timer_cb_t g_lega_timer_handler[LEGA_TIMER_NUM];

void TIMER_IRQHandler(void)
{
    lega_intrpt_enter();
    if(TIMER1->MIS)
    {
        TIMER1->INTCLR = 1; //clear irq
        if(g_lega_timer_handler[LEGA_TIMER1_INDEX].cb)
        {
            g_lega_timer_handler[LEGA_TIMER1_INDEX].cb(g_lega_timer_handler[LEGA_TIMER1_INDEX].arg);
        }
    }
    if(TIMER2->MIS)
    {
        TIMER2->INTCLR = 1; //clear irq
        if(g_lega_timer_handler[LEGA_TIMER2_INDEX].cb)
        {
            g_lega_timer_handler[LEGA_TIMER2_INDEX].cb(g_lega_timer_handler[LEGA_TIMER2_INDEX].arg);
        }
    }
    lega_intrpt_exit();
}

/**
 * init a hardware timer
 *
 * @param[in]  tmr         timer struct
 * @param[in]  period      micro seconds for repeat timer trigger
 * @param[in]  auto_reoad  set to 0, if you just need oneshot timer
 * @param[in]  cb          callback to be triggered after useconds
 * @param[in]  ch          timer channel
 * @param[in]  arg         passed to cb
 */
int32_t lega_timer_init(lega_timer_dev_t *tim)
{
    uint8_t timer_mode;
    uint32_t reg_value;
    if(NULL == tim)
    {
        return -1;
    }
    if(tim->port >= LEGA_TIMER_NUM)
    {
        return -1;
    }
    // Set Timer Clock Enable
    reg_value = REG_RD(PERI_CLK_CFG);
    REG_WR(PERI_CLK_CFG, (reg_value|TIMER_SCLK_EN));
    if(TIMER_RELOAD_AUTO == tim->config.reload_mode)
    {
        timer_mode = PERIODIC_MODE;
    }
    else if(TIMER_RELOAD_MANU == tim->config.reload_mode)
    {
        timer_mode = ONE_SHOT_MODE;
    }
    else
    {
        return -1;
    }
    g_lega_timer_handler[tim->port].cb = tim->config.cb;
    g_lega_timer_handler[tim->port].arg = tim->config.arg;
    if(LEGA_TIMER1_INDEX == tim->port)
    {
        TIMER1->CONTROL = TIMER_DISABLE; //disable timer first
        TIMER1->LOAD = tim->config.period * (SYSTEM_CLOCK / 1000000); //1000000 for us
        TIMER1->CONTROL |= (TIMER_PRESCALE | TIMER_SIZE | timer_mode); //timer control
    }
    else //if(LEGA_TIMER2_INDEX == tim->port)
    {
        TIMER2->CONTROL = TIMER_DISABLE; //disable timer first
        TIMER2->LOAD = tim->config.period * (SYSTEM_CLOCK / 1000000); //1000000 for us
        TIMER2->CONTROL |= (TIMER_PRESCALE | TIMER_SIZE | timer_mode); //timer control
    }
    return 0;
}

/**
 * start a hardware timer
 *
 * @return  0 == success, EIO == failure
 */
int32_t lega_timer_start(lega_timer_dev_t *tim)
{
    if(NULL == tim)
    {
        return -1;
    }
    if(LEGA_TIMER1_INDEX == tim->port)
    {
        TIMER1->CONTROL |= (TIMER_ENABLE | INTERRUPT_EN); //timer control
    }
    else if(LEGA_TIMER2_INDEX == tim->port)
    {
        TIMER2->CONTROL |= (TIMER_ENABLE | INTERRUPT_EN); //timer control
    }
    else
    {
        return -1;
    }
    NVIC_EnableIRQ(TIMER_IRQn); //0x20
    return 0;
}

/**
 * stop a hardware timer
 *
 * @param[in]  tmr  timer struct
 * @param[in]  cb   callback to be triggered after useconds
 * @param[in]  arg  passed to cb
 */
void lega_timer_stop(lega_timer_dev_t *tim)
{
    if(NULL == tim)
    {
        return;
    }

    if(LEGA_TIMER1_INDEX == tim->port)
    {
        TIMER1->CONTROL = TIMER_DISABLE; //timer control
    }
    else if(LEGA_TIMER2_INDEX == tim->port)
    {
        TIMER2->CONTROL = TIMER_DISABLE; //timer control
    }
    else
    {
        return;
    }

    //two timers use same IRQ
    //NVIC_DisableIRQ(TIMER_IRQn);
    g_lega_timer_handler[tim->port].cb = NULL;
    g_lega_timer_handler[tim->port].arg = NULL;
}

/**
 * De-initialises an TIMER interface, Turns off an TIMER hardware interface
 *
 * @param[in]  timer  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t lega_timer_finalize(lega_timer_dev_t *tim)
{
    //uint32_t reg_value;
    lega_timer_stop(tim);
    // Set Timer Clock Disable
    //one clk enable for 2 timer
    //reg_value = REG_RD(PERI_CLK_CFG);
    //REG_WR(PERI_CLK_CFG, (reg_value&(~TIMER_SCLK_EN)));
    return 0;
}
