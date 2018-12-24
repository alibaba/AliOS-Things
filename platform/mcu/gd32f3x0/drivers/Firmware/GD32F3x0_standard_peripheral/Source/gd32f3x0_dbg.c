/*!
    \file  gd32f3x0_dbg.c
    \brief DBG driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06, V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0_dbg.h"

#define DBG_RESET_VAL       0x00000000U

/*!
    \brief      deinitialize the DBG
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dbg_deinit(void)
{
    DBG_CTL0 = DBG_RESET_VAL;
    DBG_CTL1 = DBG_RESET_VAL;
}

/*!
    \brief      read DBG_ID code register
    \param[in]  none
    \param[out] none
    \retval     DBG_ID code
*/
uint32_t dbg_id_get(void)
{
    return DBG_ID;
}

/*!
    \brief      enable low power behavior when the mcu is in debug mode
    \param[in]  dbg_low_power:
                this parameter can be any combination of the following values:
      \arg        DBG_LOW_POWER_SLEEP: keep debugger connection during sleep mode
      \arg        DBG_LOW_POWER_DEEPSLEEP: keep debugger connection during deepsleep mode
      \arg        DBG_LOW_POWER_STANDBY: keep debugger connection during standby mode
    \param[out] none
    \retval     none
*/
void dbg_low_power_enable(uint32_t dbg_low_power)
{
    DBG_CTL0 |= dbg_low_power;
}

/*!
    \brief      disable low power behavior when the mcu is in debug mode
    \param[in]  dbg_low_power:
                this parameter can be any combination of the following values:
      \arg        DBG_LOW_POWER_SLEEP: donot keep debugger connection during sleep mode
      \arg        DBG_LOW_POWER_DEEPSLEEP: donot keep debugger connection during deepsleep mode
      \arg        DBG_LOW_POWER_STANDBY: donot keep debugger connection during standby mode
    \param[out] none
    \retval     none
*/
void dbg_low_power_disable(uint32_t dbg_low_power)
{
    DBG_CTL0 &= ~dbg_low_power;
}

/*!
    \brief      enable peripheral behavior when the mcu is in debug mode
    \param[in]  dbg_periph: DBG peripheral 
                only one parameter can be selected which is shown as below:
      \arg        DBG_SLEEP_HOLD: keep debugger connection during sleep mode
      \arg        DBG_DEEPSLEEP_HOLD: keep debugger connection during deepsleep mode
      \arg        DBG_STANDBY_HOLD: keep debugger connection during standby mode
      \arg        DBG_FWDGT_HOLD: debug FWDGT kept when core is halted
      \arg        DBG_WWDGT_HOLD: debug WWDGT kept when core is halted
      \arg        DBG_TIMER0_HOLD: TIMER0 counter kept when core is halted
      \arg        DBG_TIMER1_HOLD: TIMER1 counter kept when core is halted
      \arg        DBG_TIMER2_HOLD: TIMER2 counter kept when core is halted
      \arg        DBG_TIMER5_HOLD: hold TIMER5 counter when core is halted
      \arg        DBG_TIMER13_HOLD: hold TIMER13 counter when core is halted
      \arg        DBG_TIMER14_HOLD: hold TIMER14 counter when core is halted
      \arg        DBG_TIMER15_HOLD: hold TIMER15 counter when core is halted
      \arg        DBG_TIMER16_HOLD: hold TIMER16 counter when core is halted
      \arg        DBG_I2C0_HOLD: hold I2C0 SMBUS when core is halted
      \arg        DBG_I2C1_HOLD: hold I2C1 SMBUS when core is halted
      \arg        DBG_I2C2_HOLD: hold I2C2 SMBUS when core is halted
      \arg        DBG_RTC_HOLD: hold RTC calendar and wakeup counter when core is halted
    \param[out] none
    \retval     none
*/
void dbg_periph_enable(dbg_periph_enum dbg_periph)
{
    DBG_REG_VAL(dbg_periph) |= BIT(DBG_BIT_POS(dbg_periph));
}

/*!
    \brief      disable peripheral behavior when the mcu is in debug mode
    \param[in]  dbg_periph: DBG peripheral 
                only one parameter can be selected which is shown as below:
      \arg        DBG_SLEEP_HOLD: keep debugger connection during sleep mode
      \arg        DBG_DEEPSLEEP_HOLD: keep debugger connection during deepsleep mode
      \arg        DBG_STANDBY_HOLD: keep debugger connection during standby mode
      \arg        DBG_FWDGT_HOLD: debug FWDGT kept when core is halted
      \arg        DBG_WWDGT_HOLD: debug WWDGT kept when core is halted
      \arg        DBG_TIMER0_HOLD: TIMER0 counter kept when core is halted
      \arg        DBG_TIMER1_HOLD: TIMER1 counter kept when core is halted
      \arg        DBG_TIMER2_HOLD: TIMER2 counter kept when core is halted
      \arg        DBG_TIMER5_HOLD: hold TIMER5 counter when core is halted
      \arg        DBG_TIMER13_HOLD: hold TIMER13 counter when core is halted
      \arg        DBG_TIMER14_HOLD: hold TIMER14 counter when core is halted
      \arg        DBG_TIMER15_HOLD: hold TIMER15 counter when core is halted
      \arg        DBG_TIMER16_HOLD: hold TIMER16 counter when core is halted
      \arg        DBG_I2C0_HOLD: hold I2C0 SMBUS when core is halted
      \arg        DBG_I2C1_HOLD: hold I2C1 SMBUS when core is halted
      \arg        DBG_I2C2_HOLD: hold I2C2 SMBUS when core is halted
      \arg        DBG_RTC_HOLD: hold RTC calendar and wakeup counter when core is halted
    \param[out] none
    \retval     none
*/
void dbg_periph_disable(dbg_periph_enum dbg_periph)
{
    DBG_REG_VAL(dbg_periph) &= ~BIT(DBG_BIT_POS(dbg_periph));
}
