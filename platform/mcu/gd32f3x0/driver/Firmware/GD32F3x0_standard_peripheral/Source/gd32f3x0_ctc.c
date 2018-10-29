/*!
    \file  gd32f3x0_ctc.c
    \brief CTC driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0_ctc.h"

#define CTC_FLAG_MASK            ((uint32_t)0x00000700U)

/*!
    \brief      reset CTC clock trim controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_deinit(void)
{
    /* reset CTC */
    rcu_periph_reset_enable(RCU_CTCRST);
    rcu_periph_reset_disable(RCU_CTCRST);
}

/*!
    \brief      configure the IRC48M trim value
    \param[in]  trim_value: 8-bit IRC48M trim value
    \param[out] none
    \retval     none
*/
void ctc_irc48m_trim_value_config(uint8_t trim_value)
{
    /* clear TRIMVALUE bits */
    CTC_CTL0 &= (~(uint32_t)CTC_CTL0_TRIMVALUE);
    /* set TRIMVALUE bits */
    CTC_CTL0 |= CTL0_TRIMVALUE(trim_value);
}

/*!
    \brief      generate software reference source sync pulse
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_software_refsource_pulse_generate(void)
{
    CTC_CTL0 |= (uint32_t)CTC_CTL0_SWREFPUL;
}

/*!
    \brief      configure hardware automatically trim mode
    \param[in]  hardmode:
      \arg        CTC_HARDWARE_TRIM_MODE_ENABLE: hardware automatically trim mode enable
      \arg        CTC_HARDWARE_TRIM_MODE_DISABLE: hardware automatically trim mode disable
    \param[out] none
    \retval     none
*/
void ctc_hardware_trim_mode_config(uint32_t hardmode)
{
    CTC_CTL0 &= (uint32_t)(~CTC_CTL0_AUTOTRIM);
    CTC_CTL0 |= (uint32_t)hardmode;
}

/*!
    \brief      enable CTC trim counter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_counter_enable(void)
{
    CTC_CTL0 |= (uint32_t)CTC_CTL0_CNTEN;
}

/*!
    \brief      disable CTC trim counter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void ctc_counter_disable(void)
{
    CTC_CTL0 &= (uint32_t)(~CTC_CTL0_CNTEN);
}

/*!
    \brief      configure reference signal source polarity
    \param[in]  polarity:
      \arg        CTC_REFSOURCE_POLARITY_FALLING: reference signal source polarity is falling edge
      \arg        CTC_REFSOURCE_POLARITY_RISING: reference signal source polarity is rising edge
    \param[out] none
    \retval     none
*/
void ctc_refsource_polarity_config(uint32_t polarity)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_REFPOL);
    CTC_CTL1 |= (uint32_t)polarity;
}

/*!
    \brief      select reference signal source
    \param[in]  refs:
      \arg        CTC_REFSOURCE_GPIO: GPIO is selected
      \arg        CTC_REFSOURCE_LXTAL: LXTAL is clock selected
      \arg        CTC_REFSOURCE_USBSOF: USBSOF is selected
    \param[out] none
    \retval     none
*/
void ctc_refsource_signal_select(uint32_t refs)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_REFSEL);
    CTC_CTL1 |= (uint32_t)refs;
}

/*!
    \brief      configure reference signal source prescaler
    \param[in]  prescaler:
      \arg        CTC_REFSOURCE_PSC_OFF: reference signal not divided
      \arg        CTC_REFSOURCE_PSC_DIV2: reference signal divided by 2
      \arg        CTC_REFSOURCE_PSC_DIV4: reference signal divided by 4
      \arg        CTC_REFSOURCE_PSC_DIV8: reference signal divided by 8
      \arg        CTC_REFSOURCE_PSC_DIV16: reference signal divided by 16
      \arg        CTC_REFSOURCE_PSC_DIV32: reference signal divided by 32
      \arg        CTC_REFSOURCE_PSC_DIV64: reference signal divided by 64
      \arg        CTC_REFSOURCE_PSC_DIV128: reference signal divided by 128
    \param[out] none
    \retval     none
*/
void ctc_refsource_prescaler_config(uint32_t prescaler)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_REFPSC);
    CTC_CTL1 |= (uint32_t)prescaler;
}

/*!
    \brief      configure clock trim base limit value
    \param[in]  limit_value: 8-bit clock trim base limit value
    \param[out] none
    \retval     none
*/
void ctc_clock_limit_value_config(uint8_t limit_value)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_CKLIM);
    CTC_CTL1 |= CTL1_CKLIM(limit_value);
}

/*!
    \brief      configure CTC counter reload value
    \param[in]  reload_value: 16-bit CTC counter reload value
    \param[out] none
    \retval     none
*/
void ctc_counter_reload_value_config(uint16_t reload_value)
{
    CTC_CTL1 &= (uint32_t)(~CTC_CTL1_RLVALUE);
    CTC_CTL1 |= (uint32_t)reload_value;
}

/*!
    \brief      read CTC counter capture value when reference sync pulse occurred
    \param[in]  none
    \param[out] none
    \retval     the 16-bit CTC counter capture value
*/
uint16_t ctc_counter_capture_value_read(void)
{
    uint16_t capture_value = 0U;
    capture_value = (uint16_t)GET_STAT_REFCAP(CTC_STAT);
    return (capture_value);
}

/*!
    \brief      read CTC trim counter direction when reference sync pulse occurred
    \param[in]  none
    \param[out] none
    \retval     FlagStatus: SET or RESET
      \arg        SET: CTC trim counter direction is down-counting
      \arg        RESET: CTC trim counter direction is up-counting
*/
FlagStatus ctc_counter_direction_read(void)
{
    FlagStatus ret_status = RESET;
    if(RESET != (CTC_STAT & CTC_STAT_REFDIR)){
        ret_status = SET;
    }
    return ret_status;
}

/*!
    \brief      read CTC counter reload value
    \param[in]  none
    \param[out] none
    \retval     the 16-bit CTC counter reload value
*/
uint16_t ctc_counter_reload_value_read(void)
{
    uint16_t reload_value = 0U;
    reload_value = (uint16_t)(CTC_CTL1 & CTC_CTL1_RLVALUE);
    return (reload_value);
}

/*!
    \brief      read the IRC48M trim value
    \param[in]  none
    \param[out] none
    \retval     the 8-bit IRC48M trim value
*/
uint8_t ctc_irc48m_trim_value_read(void)
{
    uint8_t trim_value = 0U;
    trim_value = (uint8_t)GET_CTL0_TRIMVALUE(CTC_CTL0);
    return (trim_value);
}

/*!
    \brief      enable the CTC interrupt
    \param[in]  interrupt: CTC interrupt enable
      \arg        CTC_INT_CKOK: clock trim OK interrupt enable
      \arg        CTC_INT_CKWARN: clock trim warning interrupt enable
      \arg        CTC_INT_ERR: error interrupt enable
      \arg        CTC_INT_EREF: expect reference interrupt enable
    \param[out] none
    \retval     none
*/
void ctc_interrupt_enable(uint32_t interrupt)
{
    CTC_CTL0 |= (uint32_t)interrupt; 
}

/*!
    \brief      disable the CTC interrupt
    \param[in]  interrupt: CTC interrupt enable source
      \arg        CTC_INT_CKOK: clock trim OK interrupt enable
      \arg        CTC_INT_CKWARN: clock trim warning interrupt enable
      \arg        CTC_INT_ERR: error interrupt enable
      \arg        CTC_INT_EREF: expect reference interrupt enable
    \param[out] none
    \retval     none
*/
void ctc_interrupt_disable(uint32_t interrupt)
{
    CTC_CTL0 &= (uint32_t)(~interrupt); 
}

/*!
    \brief      get CTC interrupt flag
    \param[in]  interrupt: the CTC interrupt flag
      \arg        CTC_INT_FLAG_CKOK: clock trim OK interrupt
      \arg        CTC_INT_FLAG_CKWARN: clock trim warning interrupt 
      \arg        CTC_INT_FLAG_ERR: error interrupt 
      \arg        CTC_INT_FLAG_EREF: expect reference interrupt
      \arg        CTC_INT_FLAG_CKERR: clock trim error bit interrupt
      \arg        CTC_INT_FLAG_REFMISS: reference sync pulse miss interrupt 
      \arg        CTC_INT_FLAG_TRIMERR: trim value error interrupt
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus ctc_interrupt_flag_get(uint32_t interrupt)
{
    uint32_t ctc_int = 0U, intenable = 0U;
    FlagStatus ret_status = RESET;

    if(interrupt & CTC_FLAG_MASK){
        intenable = CTC_CTL0 & CTC_INT_ERR;
    }else{
        intenable = CTC_CTL0 & interrupt;
    }
    ctc_int = CTC_STAT & interrupt;

    if(ctc_int && intenable){
        ret_status = SET;
    }
    return ret_status;
}

/*!
    \brief      clear CTC interrupt flag
    \param[in]  interrupt: the CTC interrupt flag
      \arg        CTC_INT_FLAG_CKOK: clock trim OK interrupt
      \arg        CTC_INT_FLAG_CKWARN: clock trim warning interrupt 
      \arg        CTC_INT_FLAG_ERR: error interrupt 
      \arg        CTC_INT_FLAG_EREF: expect reference interrupt 
      \arg        CTC_INT_FLAG_CKERR: clock trim error bit interrupt
      \arg        CTC_INT_FLAG_REFMISS: reference sync pulse miss interrupt 
      \arg        CTC_INT_FLAG_TRIMERR: trim value error interrupt
    \param[out] none
    \retval     none
*/ 
void ctc_interrupt_flag_clear(uint32_t interrupt)
{
    if(interrupt & CTC_FLAG_MASK){
        CTC_INTC |= CTC_INTC_ERRIC;
    }else{
        CTC_INTC |= interrupt;
    }
}

/*!
    \brief      get CTC flag
    \param[in]  flag: the CTC flag
      \arg        CTC_FLAG_CKOK: clock trim OK flag
      \arg        CTC_FLAG_CKWARN: clock trim warning flag 
      \arg        CTC_FLAG_ERR: error flag 
      \arg        CTC_FLAG_EREF: expect reference flag
      \arg        CTC_FLAG_CKERR: clock trim error bit
      \arg        CTC_FLAG_REFMISS: reference sync pulse miss
      \arg        CTC_FLAG_TRIMERR: trim value error bit
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus ctc_flag_get(uint32_t flag)
{
    FlagStatus ret_status = RESET;
    
    if(RESET != (CTC_STAT & flag)){
        ret_status = SET;
    }
    return ret_status;
}

/*!
    \brief      clear CTC flag
    \param[in]  flag: the CTC flag
      \arg        CTC_FLAG_CKOK: clock trim OK flag
      \arg        CTC_FLAG_CKWARN: clock trim warning flag 
      \arg        CTC_FLAG_ERR: error flag 
      \arg        CTC_FLAG_EREF: expect reference flag
      \arg        CTC_FLAG_CKERR: clock trim error bit
      \arg        CTC_FLAG_REFMISS: reference sync pulse miss
      \arg        CTC_FLAG_TRIMERR: trim value error bit
    \param[out] none
    \retval     none
*/
void ctc_flag_clear(uint32_t flag)
{
    if(flag & CTC_FLAG_MASK){
        CTC_INTC |= CTC_INTC_ERRIC;
    }else{
        CTC_INTC |= flag;
    }
}
