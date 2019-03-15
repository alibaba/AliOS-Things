/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     zx29_timer.c
 * @brief    CSI Source File for timer Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include "zx29_timer.h"
#include <drv_intc.h>
#include <drv_timer.h>
#include <io.h>
#include <csi_core.h>

#define ERR_TIMER(errno) (CSI_DRV_ERRNO_TIMER_BASE | errno)

#define TIMER_NULL_PARAM_CHK(para)                  \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_TIMER(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

#define FALSE  0
#define TRUE   1

#define CLK_19M2        (19200000)
#define CLK_32K         (32768)
#define CLK_76M8        (76800000)
#define DEVICE_COUNT    (2)

/*******************************************************************************
 *                     Local function declarations                    *
 ******************************************************************************/
static void timer_irq_clear(uint8_t id);

static uint32_t timer_get_clk(uint8_t id);

static int timer_start(uint8_t id);

static int timer_stop(uint8_t id);

extern int32_t target_get_timer_count(void);

extern int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq);

typedef struct {
    bool                  open;        /* indicate whether have been opended or not */
    int                   flags;       /* 0: not started; 1: running; 2: calling user function */
    uint32_t              addr;        /* the base-address of timer */
    uint32_t              irq;         /* the interrupt number of timer */
    uint32_t              timeout;     /* the set time (us) */
    bool                  mode;        /* oneshot or autoreload mode */
    timer_event_cb_t      handler;     /* Current user interrupt handler */
} zx29_timer_priv_t;

/*******************************************************************************
 *                     Local variable declarations                    *
 ******************************************************************************/
static zx29_timer_priv_t timer0_info = {
    .open       =   FALSE,
    .flags      =   BSP_TCFLAGS_INACTIVE,
    .addr       =   ZX29_TIMER_0_BASE,      /* the base-address of timer0 */
    .irq        =   PIC_IRQ_TIMER0,         /* the interrupt number of timer0 */
    .timeout    =   0,                      /* the set time (us) */
    .handler    =   NULL
};

static zx29_timer_priv_t timer1_info = {
    .open       =   FALSE,
    .flags      =   BSP_TCFLAGS_INACTIVE,
    .addr       =   ZX29_TIMER_1_BASE,      /* the base-address of timer1 */
    .irq        = (32 + 17),//ICU_IRQ_TIMER1,         /* the interrupt number of timer1 */
    .timeout    =   0,                      /* the set time (us) */
    .handler    =   NULL
};

static zx29_timer_priv_t timer2_info = {
    .open       =   FALSE,
    .flags      =   BSP_TCFLAGS_INACTIVE,
    .addr       =   ZX29_TIMER_2_BASE,      /* the base-address of timer1 */
    .irq        =   PIC_IRQ_LP_TIMER,         /* the interrupt number of timer1 */
    .timeout    =   0,                      /* the set time (us) */
    .handler    =   NULL
};

static zx29_timer_priv_t timer3_info = {
    .open       =   FALSE,
    .flags      =   BSP_TCFLAGS_INACTIVE,
    .addr       =   AD_TIMER_0_BASE,      /* the base-address of timer0 */
    .irq        =   PIC_IRQ_AD_TIMER0,         /* the interrupt number of timer0 */
    .timeout    =   0,                      /* the set time (us) */
    .handler    =   NULL
};

static zx29_timer_priv_t timer4_info = {
    .open       =   FALSE,
    .flags      =   BSP_TCFLAGS_INACTIVE,
    .addr       =   AD_TIMER_1_BASE,       /* the base-address of timer1 */
    .irq        =   PIC_IRQ_AD_TIMER1,    /* the interrupt number of timer1 */
    .timeout    =   0,                     /* the set time (us) */
    .handler    =   NULL
};

static zx29_timer_priv_t *const timer_instance[] = {
//#ifdef CONFIG_SANECHIPS_TIMER0
    &timer0_info,
//#endif
//#ifdef CONFIG_SANECHIPS_TIMER1
    &timer1_info,
    &timer2_info,
    &timer3_info,
    &timer4_info,
//#endif
};

static int32_t timer_get_id(timer_handle_t handle)
{
    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;
    volatile zx_timer_reg_t *addr = (volatile zx_timer_reg_t *)timer_priv->addr;

    if ((int)addr == ZX29_TIMER_0_BASE) {
        return 0;
    }

    if ((int)addr == ZX29_TIMER_1_BASE) {
        return 1;
    }

    if ((int)addr == ZX29_TIMER_2_BASE) {
        return 2;
    }

    if ((int)addr == AD_TIMER_0_BASE) {
        return 3;
    }

    if ((int)addr == AD_TIMER_1_BASE) {
        return 4;
    }

    return -1;
}

/**
 * This function is used for get current timer count value
 * @param[in]  id       id of timer
 * @return       the operation status, 0 is OK, others is error
 */
static void timer_irq_clear(uint8_t id)
{
    zx29_timer_priv_t *timer_priv = timer_instance[id];

    switch (id) {
        case TIMER0: {
            volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
            /*clear int of ap crm*/
            __set_bit(1, (uint32_t *)IRQ_CLEAR_BASE);
            __clear_bit(1, (uint32_t *)IRQ_CLEAR_BASE);
            timer_reg->status = timer_reg->status;
            break;
        }
        case TIMER1: {
            volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
            timer_reg->status = timer_reg->status;
            break;
        }
        case TIMER2: {
            volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
            timer_reg->status = timer_reg->status;
            break;
        }
        case TIMER3: {
            volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
            timer_reg->status = timer_reg->status;
            break;
        }
        case TIMER4: {
            volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
            timer_reg->status = timer_reg->status;
            break;
        }
        default:
            break;
    }
}

/**
 * This function is used for get timer clock
 *@param[in]  id        id of timer
 *@return       the operation status, 0 is OK, others is error
 */
static uint32_t timer_get_clk(uint8_t id)
{
    uint32_t clk_source = 0;
    uint32_t clk_div = 0;
    uint32_t wclk_div = 0;

    zx29_timer_priv_t *timer_priv = timer_instance[id];

    switch (id) {
        case TIMER0: {
            volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
            if ((getreg32((uint32_t *)TIMER_CLKSEL_BASE) & TIMER0_CLKSEL) == 0) {
                clk_source = CLK_32K;
            }

            else {
                clk_source = CLK_19M2;
            }

            clk_div = ((getreg32((uint32_t *)TIMER_DIVSEL_BASE) & TIMER_CLKDIV_MASK) + 1);
            wclk_div = (((timer_reg->config >> 16) & TIMER_WCLKDIV_MASK) + 1);
            break;
        }
        case TIMER1: {
            volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
            if ((getreg32((uint32_t *)TIMER_CLKSEL_BASE) & TIMER1_CLKSEL) == 0) {
                clk_source = CLK_32K;
            } else {
                clk_source = CLK_19M2;
            }

            clk_div = (((getreg32((uint32_t *)TIMER_DIVSEL_BASE) >> 4) & TIMER_CLKDIV_MASK) + 1);
            wclk_div = (((timer_reg->config >> 16) & TIMER_WCLKDIV_MASK) + 1);
            break;
        }
        case TIMER3: {
            volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
            if ((getreg32((uint32_t *)AD_TIMER0_CLKSEL_BASE) & 0x30) == 0) {
                clk_source = CLK_32K;
            } else if ((getreg32((uint32_t *)AD_TIMER0_CLKSEL_BASE) & 0x30) == 0x10){
                clk_source = CLK_76M8;
            } else {
                clk_source = CLK_19M2;
            }

            clk_div = (((getreg32((uint32_t *)AD_TIMER0_CLKSEL_BASE) & 0x1f000) >> 12) + 1);
            wclk_div = (((timer_reg->config >> 16) & TIMER_WCLKDIV_MASK) + 1);
            break;
        }
        case TIMER4: {
            volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
            if ((getreg32((uint32_t *)AD_TIMER1_CLKSEL_BASE) & 0x30) == 0) {
                clk_source = CLK_32K;
            } else if ((getreg32((uint32_t *)AD_TIMER1_CLKSEL_BASE) & 0x30) == 0x10){
                clk_source = CLK_76M8;
            } else {
                clk_source = CLK_19M2;
            }

            clk_div = (((getreg32((uint32_t *)AD_TIMER1_CLKSEL_BASE) & 0x1f000) >> 12) + 1);
            wclk_div = (((timer_reg->config >> 16) & TIMER_WCLKDIV_MASK) + 1);
            break;
        }
        default:
            return 0;
    }

    return (clk_source / (clk_div * wclk_div));
}

/**
 * This function is used for wait timer count overflow
 * @param[in]  id       id of timer
 * @return      the operation status, 0 is OK, others is error
 */
#if 0
static void timer_wait_for_ovf(uint8_t id)
{
    zx29_timer_priv_t *timer_priv = timer_instance[id];
    if (id < 2) {
        volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
        while (!(timer_reg->status & TIMER_STATUS_OVF));
    } else {
        volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
        while (!(timer_reg->status & TIMER_STATUS_OVF));
    }

}
#endif

/**
 * This function is used for start timer count
 * @param[in]  id       id of timer
 * @return       the operation status, 0 is OK, others is error
 */
static int timer_start(uint8_t id)
{
    zx29_timer_priv_t *timer_priv = timer_instance[id];

    if (id < 2) {
        volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
        timer_reg->cnt = 0;/*当前计数值清零*/
        if (timer_priv->timeout == 0) {
            timer_reg->load = 0xffffffff;
        } else {
            timer_reg->load = (timer_priv->timeout) * timer_get_clk(id) / USEC_PER_SEC;/*计数值load到load寄>存器中*/
        }
        timer_reg->config = timer_priv->mode;/*oneshot or autoreload*/
        if (timer_priv->handler != NULL) {
            timer_reg->irq_en = 1;/*overflow int enable*/
        }

        timer_reg->sync_en = TIMER_SYNC_MASK;

        while (timer_reg->sync_en != 0);

        timer_reg->en = 1;

    } else {
        volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
        timer_reg->cnt = 0;/*当前计数值清零*/
        if (timer_priv->timeout == 0) {
            timer_reg->load = 0xffffffff;
        } else {
//            timer_reg->load = ((timer_priv->timeout)/1000) * timer_get_clk(id) / (USEC_PER_SEC/1000);/*计数值load到load寄>存器中*/
            timer_reg->load = ((timer_priv->timeout)/10) * ((timer_get_clk(id)*10) / (USEC_PER_SEC));/*计数值load到load寄>存器中*/
        }
        timer_reg->config = timer_priv->mode;/*oneshot or autoreload*/
        if (timer_priv->handler != NULL) {
            timer_reg->irq_en = 1;/*overflow int enable*/
        }

        timer_reg->sync_en = TIMER_SYNC_MASK;

        while (timer_reg->sync_en != 0);

        timer_reg->en = 1;

    }

    return 0;
}

/**
 * This function is used for stop timer count
 * @param[in]  id       id of timer
 * @return       the operation status, 0 is OK, others is error
 */
static int timer_stop(uint8_t id)
{
    zx29_timer_priv_t *timer_priv = timer_instance[id];
    if (id < 2) {
        volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
        timer_reg->en = 0;/*关闭定时器*/
    } else {
        volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
        timer_reg->en = 0;/*关闭定时器*/
    }

    return 0;
}

void zx29_timer_irqhandler(int idx)
{
    zx29_timer_priv_t *timer_priv = timer_instance[idx];
    uint32_t timeout = timer_priv->timeout;

    timer_priv->flags |= BSP_TCFLAGS_HANDLER;

    timer_irq_clear(idx);/*清除上溢状态*/

    csi_intc_ack_irq(timer_priv->irq);

    if (timer_priv->handler != NULL) {
        //timer_priv->handler(TIMER_EVENT_TIMEOUT, &timeout);
        timer_priv->handler(idx, TIMER_EVENT_TIMEOUT);
    }

    if (1 == timer_priv->mode) {
        timer_priv->flags &= ~BSP_TCFLAGS_HANDLER;
        return;
    } else {
        if (timeout == 0) {
            timer_irq_clear(idx);
            timer_stop(idx);
            timer_priv->flags = BSP_TCFLAGS_INACTIVE;
            return;
        } else {
            timer_stop(idx);/*去使能定时器*/
            timer_priv->timeout = timeout;
            timer_start(idx);/*使能定时器*/
        }
    }

    timer_priv->flags &= ~BSP_TCFLAGS_HANDLER;

    return;
}

/**
  \brief       Initialize TIMER Interface. 1. Initializes the resources needed for the TIMER interface 2.registers event callback function
  \param[in]   idx  instance timer index
  \param[in]   cb_event  Pointer to \ref timer_event_cb_t
  \return      pointer to timer instance
*/
timer_handle_t csi_timer_initialize(int32_t idx, timer_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_TIMER_NUM) {
        return NULL;
    }

    uint32_t base = 0u;
    uint32_t irq = 0u;

    int32_t real_idx = target_get_timer(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }

    zx29_timer_priv_t *timer_priv = timer_instance[idx];
    timer_priv->addr = base;
    timer_priv->irq  = irq;
    timer_priv->timeout = DW_TIMER_INIT_DEFAULT_VALUE;

    timer_priv->handler = cb_event;

    if (cb_event != NULL) {
        csi_intc_set_attribute(irq, 1 , INT_MODE_HIGH_LEVEL);
        csi_intc_ack_irq(irq);
        csi_intc_enable_irq(irq);
    }

//    timer_attach_irq(idx);

    if (timer_priv->open == false) {
        timer_priv->open = true;
        return (timer_handle_t)timer_priv;;
    } else {
        return NULL;
    }
}

/**
  \brief       De-initialize TIMER Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_uninitialize(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;
    timer_priv->handler = NULL;

    if (timer_priv->open == true) {
        if (timer_priv->flags | BSP_TCFLAGS_ACTIVE) {
            csi_timer_stop(handle);
        }

        timer_priv->open = false;
    }

    return 0;
}

/**
  \brief       config timer mode.
  \param[in]   handle timer handle to operate.
  \param[in]   mode      \ref timer_mode_e
  \return      error code
*/
int32_t csi_timer_config(timer_handle_t handle, timer_mode_e mode)
{
    TIMER_NULL_PARAM_CHK(handle);

    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;

    uint32_t temp;
    int32_t  id = timer_get_id(handle);

    if (id == TIMER0) {
        temp = getreg32((uint32_t *)TIMER_CLKEN_BASE) | (TIMER_CLKEN);
        putreg32(temp, (uint32_t *)TIMER_CLKEN_BASE);
    }

    if (id == TIMER1) {
        temp = getreg32((uint32_t *)TIMER_CLKEN_BASE) | (TIMER_CLKEN << 4);
        putreg32(temp, (uint32_t *)TIMER_CLKEN_BASE);
    }

    if (id == TIMER3) {
        temp = getreg32((uint32_t *)AD_TIMER0_CLKEN_BASE) | (AD_TIMER_CLKEN);
        temp &= ~(0x1f << 12);
        temp |= (0x2 << 4);
        putreg32(temp, (uint32_t *)AD_TIMER0_CLKEN_BASE);
    }

    if (id == TIMER4) {
        temp = getreg32((uint32_t *)AD_TIMER1_CLKEN_BASE) | (AD_TIMER_CLKEN);
        temp &= ~(0x1f << 12);
        temp |= (0x2 << 4);
        putreg32(temp, (uint32_t *)AD_TIMER1_CLKEN_BASE);
    }

    timer_priv->mode = mode;

    return 0;
}

/**
  \brief       Set timer.
  \param[in]   instance  timer instance to operate.
  \param[in]   timeout the timeout value in microseconds(us).
  \return      error code
*/
int32_t csi_timer_set_timeout(timer_handle_t handle, uint32_t timeout)
{
    TIMER_NULL_PARAM_CHK(handle);

    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;
    timer_priv->timeout = timeout;
    return 0;
}

/**
  \brief       Start timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_start(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;
    int32_t id = timer_get_id(handle);

    if (id < 2) {
        if ((timer_priv->timeout) * timer_get_clk(id) / USEC_PER_SEC > 0xffff) {
            return -1;
        }
    }

    timer_irq_clear(id);

    timer_start(id);

    /* And enable the timer interrupt */
    timer_priv->flags |= BSP_TCFLAGS_ACTIVE;

    return 0;
}

/**
  \brief       Stop timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_stop(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;
    int32_t id = timer_get_id(handle);

    timer_irq_clear(id);
    timer_stop(id);

    timer_priv->flags &= ~BSP_TCFLAGS_ACTIVE;

    return 0;
}

/**
  \brief       suspend timer.
  \param[in]   instance  timer instance to operate.
  \return      error code
*/
int32_t csi_timer_suspend(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    return ERR_TIMER(DRV_ERROR_UNSUPPORTED);
}

/**
  \brief       resume timer.
  \param[in]   handle timer handle to operate.
  \return      error code
*/
int32_t csi_timer_resume(timer_handle_t handle)
{
    TIMER_NULL_PARAM_CHK(handle);

    int32_t id = timer_get_id(handle);

    timer_stop(id);
    timer_start(id);
    return 0;
}

/**
  \brief       get timer current value
  \param[in]   handle timer handle to operate.
  \param[out]   value     timer current value
  \return      error code
*/
int32_t csi_timer_get_current_value(timer_handle_t handle, uint32_t *value)
{
    TIMER_NULL_PARAM_CHK(handle);

    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;
    int32_t id = timer_get_id(handle);
    if (id < 2) {
        volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
        *value = timer_reg->cnt;
    } else {
        volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
        *value = timer_reg->cnt;
    }

    return 0;
}

/**
  \brief       get timer reload value
  \param[in]   handle timer handle to operate.
  \param[out]   value    timer reload value
  \return      error code
*/
int32_t csi_timer_get_load_value(timer_handle_t handle, uint32_t *value)
{
    TIMER_NULL_PARAM_CHK(handle);

    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;
    int32_t id = timer_get_id(handle);
    if (id < 2) {
        volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;
        *value = timer_reg->load;
    } else {
        volatile ad_timer_reg_t *timer_reg = (ad_timer_reg_t *)timer_priv->addr;
        *value = timer_reg->load;
    }

    return 0;
}

/**
  \brief       Get TIMER status.
  \param[in]   handle timer handle to operate.
  \return      TIMER status \ref timer_status_t
*/
timer_status_t csi_timer_get_status(timer_handle_t handle)
{
    timer_status_t timer_status = {0};

    if (handle == NULL) {
        return timer_status;
    }

    zx29_timer_priv_t *timer_priv = (zx29_timer_priv_t *)handle;
//    volatile zx_timer_reg_t *timer_reg = (zx_timer_reg_t *)timer_priv->addr;

    timer_status.active = timer_priv->flags;

//    int32_t timeleft = timer_reg->cnt;

//    if (timeleft < 0) {
//        timer_status.timeout = 1;
//    }

    return timer_status;
}

