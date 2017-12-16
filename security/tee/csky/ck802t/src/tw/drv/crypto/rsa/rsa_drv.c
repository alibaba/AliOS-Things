/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 **/

#include "tee_types.h"
#include "tee_dbg.h"
#include "rsa_drv.h"
#include "device.h"
#include "crypto_drv.h"

#undef  INTERRUPT_SUPPORT

#ifdef INTERRUPT_SUPPORT
void _rsa_intr(uint32_t irq);
static irq_handler_t _rsa_irq_handler = {
    "RSA_INTR",
    INTC_RSA,
    INTC_RSA%4,
    _rsa_intr,
    FALSE
};

 /* interrupt callback function point */
static void (*_rsa_intr_callback)(void) = NULL;
void _rsa_intr(uint32_t irq)
{
    if(NULL != _rsa_intr_callback)
        _rsa_intr_callback();
}
#endif

/*
 * write one to corresponding bit clear the interrupt
 * Parameters: void
 */
static uint32_t _rsa_clear_intr(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_ISR = 0xffff;
    ptr->RSA_IMR = 0x0000;
    return SUCCESS;
}

/*
 * write modulus M to MWID register
 * Parameters:
 *   width: width of modulus M
 */
static uint32_t _rsa_set_mwidth(uint32_t width)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_MWID = width;
    return SUCCESS;
}

/*
 * write key D to DWID register
 * Parameters:
 *   width: width of key D
 */
static uint32_t _rsa_set_dwidth(uint32_t width)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_DWID = width;
    return SUCCESS;
}

/*
 * write B to BWID register
 * Parameters:
 *   width: width of B
 */
static uint32_t _rsa_set_bwidth(uint32_t width)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_BWID = width;
    return SUCCESS;
}

/*
 * set the CAL_Q and RSA_EN before encryption or decryption
 * parameters: void
 */
static uint32_t _rsa_cal_q(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_CTRL |= 0x6;
    return SUCCESS;
}

/*
 * set the RSA start
 * parameters: void
 */
static uint32_t _rsa_start(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_CTRL |= 0x3;
    return SUCCESS;
}

/*
 * software reset for RSA
 * parameters: void
 */
static uint32_t _rsa_reset(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_RST |= 0x1;
    return SUCCESS;
}

/*
 * the counter of loop Modular Multiplication
 * parameters: void
 */
static uint32_t _rsa_lp_cnt(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    uint32_t temp;
    return temp = ptr->RSA_LP_CNT;
}

#if 0
/*
 * calculate the Q value: Q0
 * parameters: void
 */
static uint32_t _rsa_q0(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    uint32_t temp;
    return temp = ptr->RSA_Q0;
}

/*
 * calculate the Q value: Q1
 * parameters: void
 */
static uint32_t _rsa_q1(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    uint32_t temp;
    return temp = ptr->RSA_Q1;
}
#endif

/*
 * wait cal_q calculation done
 * parameters: void
 */
static uint32_t _rsa_cal_q_done(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    uint32_t temp = (ptr->RSA_ISR>>5) & 0x1;
    return temp;
}

#ifdef INTERRUPT_SUPPORT
/*
 * interrupt
 * parameters: void
 */
static uint32_t _rsa_abnormal_1(void)
{
    uint32_t temp;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    return temp = (ptr->RSA_ISR >> 1) & 0x1;
}

/*
 * interrupt
 * parameters: void
 */
static uint32_t _rsa_abnormal_2(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    uint32_t temp;
    return temp = (ptr->RSA_ISR >> 2) & 0x1;
}

/*
 * interrupt
 * parameters: void
 */
static uint32_t _rsa_abnormal_3(void)
{
    uint32_t temp;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    return temp = (ptr->RSA_ISR >> 3) & 0x1;
}

/*
 * interrupt
 * parameters: void
 */
static uint32_t _rsa_abnormal_4(void)
{
    uint32_t temp;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    return temp = (ptr->RSA_ISR >> 4) & 0x1;
}

#endif

/*
 * RSA_DONE
 * parameters: void
 */
static uint32_t _rsa_done(void)
{
    uint32_t temp;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    return temp = (ptr->RSA_ISR) & 0x1;
}

#ifdef INTERRUPT_SUPPORT
/*
 * mask cal_q calculation
 * parameters: void
 */
static uint32_t _rsa_unmask_cal_q_done(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_IMR |= 0x20;
    return SUCCESS;
}

/*
 * mask interrupt
 * parameters: void
 */
static uint32_t _rsa_unmask_abnormal_1(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_IMR |= 0x2;
    return SUCCESS;
}

/*
 * mask interrupt
 * parameters: void
 */
static uint32_t _rsa_unmask_abnormal_2(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_IMR |= 0x4;
    return SUCCESS;
}

/*
 * mask interrupt
 * parameters: void
 */
static uint32_t _rsa_unmask_abnormal_3(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_IMR |= 0x8;
    return SUCCESS;
}

/*
 * mask interrupt
 * parameters: void
 */
static uint32_t _rsa_unmask_abnormal_4(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_IMR |= 0x10;
    return SUCCESS;
}

/*
 * mask RSA_DONE
 * parameters: void
 */
static uint32_t _rsa_unmask_done(void)
{
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    ptr->RSA_IMR |= 0x1;
    return SUCCESS;
}
#endif

/*
 * write RFM
 * Parameters:
 *   data :    store the modulus M
 *   length:    the data length
 */
static uint32_t _rsa_load_m(uint32_t data[], uint32_t length)
{
    int32_t i;
    uint32_t baseaddr;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    baseaddr = (uint32_t)&ptr->RSA_RFM;
    for (i = 1; i < length + 1; i++) {
        *(volatile uint32_t *)baseaddr = data[i - 1];
        baseaddr = baseaddr+4;
    }
    return SUCCESS;
}

/*
 * write RFD
 * Parameters:
 *   data :     store the exponent D
 *   length:    the data length
 */
static uint32_t _rsa_load_d(uint32_t data[], uint32_t length)
{
    int32_t i;
    uint32_t baseaddr;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    baseaddr = (uint32_t)&ptr->RSA_RFD;
    for (i = 1; i < length + 1; i++) {
        *(volatile uint32_t *)baseaddr = data[i - 1];
        baseaddr = baseaddr + 4;
    }
    return SUCCESS;
}

/*
 * write RFC
 * Parameters:
 *   data :     store the auxiliary key C
 *   length:    the data length
 */
static uint32_t _rsa_load_c(uint32_t data[], uint32_t length)
{
    int32_t i;
    uint32_t baseaddr;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    baseaddr = (uint32_t)&ptr->RSA_RFC;
    for (i = 1; i < length + 1; i++) {
        *(volatile uint32_t *)baseaddr = data[i - 1];
        baseaddr = baseaddr + 4;
    }
    return SUCCESS;
}

/*
 * write RFB
 * Parameters:
 *   data :     store the data B
 *   length:    the data length
 */
static uint32_t _rsa_load_b(uint32_t data[], uint32_t length)
{
    int32_t i;
    uint32_t baseaddr;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    baseaddr = (uint32_t)&ptr->RSA_RFB;
    for (i = 1; i < length + 1; i++) {
        *(volatile uint32_t *)baseaddr = data[i-1];
        baseaddr = baseaddr + 4;
    }
    return SUCCESS;
}

/*
 * write RFR
 * Parameters:
 *   data :     read the result data
 *   length:    the data length
 */
static uint32_t _rsa_read_r(uint32_t data[], uint32_t length)
{
    int32_t i;
    uint32_t baseaddr;
    volatile prsa_reg_t ptr = (prsa_reg_t)PLATFORM_RSA_ADDRBASE;
    baseaddr = (uint32_t)&ptr->RSA_RFR;
    for (i = 1; i < length + 1; i++) {
        data[i-1] = *(volatile uint32_t *)baseaddr;
        baseaddr = baseaddr + 4;
    }
    return SUCCESS;
}

int32_t _rsa_clk_on(void)
{
    return 0;
}

int32_t _rsa_clk_off(void)
{
    return 0;
}

int32_t rsa_drv_mode_exp(void *arg)
{
    rsa_drv_ioctl_t *p = (rsa_drv_ioctl_t *)arg;
    int32_t ret = SUCCESS;

    if (!VALID_MAGIC(p->magic)) {
        return EARG;
    }

    if ((p->words) & 1) {
        return EARG;
    }

    /* reset for safe */
    _rsa_reset();
    /* clear and disable intr */
    _rsa_clear_intr();

    /* set m */
    _rsa_set_mwidth((p->words) >> 1);
    _rsa_load_m(p->m, p->words);

    /* set d */
    _rsa_set_dwidth((p->d_valid_bits) - 1);
    _rsa_load_d(p->d, p->words);

    /* set b */
    _rsa_set_bwidth((p->words) >> 1);
    _rsa_load_b(p->b, p->words);

    /* set c */
    _rsa_load_c(p->c, p->words);

    _rsa_cal_q();
    while(!_rsa_cal_q_done());
    _rsa_start();
    while((!_rsa_done()) && (_rsa_lp_cnt() < _MAX_RSA_LP_CNT));

    _rsa_read_r(p->r, p->words);

    if (_rsa_lp_cnt() >= _MAX_RSA_LP_CNT) {
        ret = FAIL;
        goto __out;
    }

__out:
    _rsa_reset();

    return ret;
}

int32_t rsa_drv_suspend(dev_t *dev, int32_t state)
{
    return SUCCESS;
}

int32_t rsa_drv_resume(dev_t *dev)
{
    return SUCCESS;
}

