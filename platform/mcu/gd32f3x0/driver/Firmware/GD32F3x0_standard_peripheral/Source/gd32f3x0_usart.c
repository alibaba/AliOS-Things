/*!
    \file  gd32f3x0_usart.c
    \brief USART driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-06-06  V1.0.0, firmware for GD32F3x0
*/

#include "gd32f3x0_usart.h"

/*!
    \brief      reset USART
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_deinit(uint32_t usart_periph)
{
    switch(usart_periph){
    case USART0:
        rcu_periph_reset_enable(RCU_USART0RST);
        rcu_periph_reset_disable(RCU_USART0RST);
        break;
    case USART1:
        rcu_periph_reset_enable(RCU_USART1RST);
        rcu_periph_reset_disable(RCU_USART1RST);
        break;
    default:
        break;
    }
}

/*!
    \brief      configure USART baud rate value
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  baudval: baud rate value
    \param[out] none
    \retval     none
*/ 
void usart_baudrate_set(uint32_t usart_periph, uint32_t baudval)
{
    uint32_t uclk = 0U, intdiv = 0U, fradiv = 0U, udiv = 0U;
    switch(usart_periph){
    case USART0:
         uclk = rcu_clock_freq_get(CK_USART);
         break;
    case USART1:
         uclk = rcu_clock_freq_get(CK_APB1);
         break;
    default:
         break;
    }
    if(USART_CTL0(usart_periph) & USART_CTL0_OVSMOD){
        /* when oversampling by 8,configure the value of USART_BAUD */
        udiv = ((2U*uclk)+baudval/2U)/baudval;
        intdiv = udiv & 0xfff0U;
        fradiv = udiv & 0x7U;
        USART_BAUD(usart_periph) |= ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
    }else{
        /* when oversampling by 16,configure the value of USART_BAUD */
        udiv = (uclk+baudval/2U)/baudval;
        intdiv = udiv & 0xfff0U;
        fradiv = udiv & 0xfU;
        USART_BAUD(usart_periph) |= ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
    }
}

/*!
    \brief     configure USART parity
    \param[in] usart_periph: USARTx(x=0,1)
    \param[in] paritycfg: USART parity configure
      \arg       USART_PM_NONE: no parity
      \arg       USART_PM_ODD:  odd parity
      \arg       USART_PM_EVEN: even parity
    \param[out] none
    \retval     none
*/
void usart_parity_config(uint32_t usart_periph, uint32_t paritycfg)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* clear USART_CTL0 PM,PCEN bits */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_PM | USART_CTL0_PCEN); 
    /* configure USART parity mode */
    USART_CTL0(usart_periph) |= paritycfg;
}

/*!
    \brief     configure USART word length
    \param[in] usart_periph: USARTx(x=0,1)
    \param[in] wlen: USART word length configure
      \arg       USART_WL_8BIT: 8 bits
      \arg       USART_WL_9BIT: 9 bits
    \param[out] none
    \retval     none
*/
void usart_word_length_set(uint32_t usart_periph, uint32_t wlen)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* clear USART_CTL0 WL bit */
    USART_CTL0(usart_periph) &= ~USART_CTL0_WL;
    /* configure USART word length */
    USART_CTL0(usart_periph) |= wlen;
}

/*!
    \brief     configure USART stop bit length
    \param[in] usart_periph: USARTx(x=0,1)
    \param[in] stblen: USART stop bit configure
      \arg       USART_STB_1BIT:   1 bit
      \arg       USART_STB_0_5BIT: 0.5bit
      \arg       USART_STB_2BIT:   2 bits
      \arg       USART_STB_1_5BIT: 1.5bit
    \param[out] none
    \retval     none
*/
void usart_stop_bit_set(uint32_t usart_periph, uint32_t stblen)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* clear USART_CTL1 STB bits */
    USART_CTL1(usart_periph) &= ~USART_CTL1_STB;
    USART_CTL1(usart_periph) |= stblen;
}

/*!
    \brief     enable USART
    \param[in] usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_enable(uint32_t usart_periph)
{
    USART_CTL0(usart_periph) |= USART_CTL0_UEN;
}

/*!
    \brief     disable USART
    \param[in] usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_disable(uint32_t usart_periph)
{
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
}

/*!
    \brief      configure USART transmitter
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  txconfig: enable or disable USART transmitter
      \arg        USART_TRANSMIT_ENABLE: enable USART transmission
      \arg        USART_TRANSMIT_DISABLE: enable USART transmission
    \param[out] none
    \retval     none
*/
void usart_transmit_config(uint32_t usart_periph, uint32_t txconfig)
{
    USART_CTL0(usart_periph) &= ~USART_CTL0_TEN;
    /* configure transfer mode */
    USART_CTL0(usart_periph) |= txconfig;
}

/*!
    \brief      configure USART receiver
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  rxconfig: enable or disable USART receiver
      \arg        USART_RECEIVE_ENABLE: enable USART reception
      \arg        USART_RECEIVE_DISABLE: disable USART reception
    \param[out] none
    \retval     none
*/
void usart_receive_config(uint32_t usart_periph, uint32_t rxconfig)
{
    USART_CTL0(usart_periph) &= ~USART_CTL0_REN;
    /* configure receiver mode */
    USART_CTL0(usart_periph) |= rxconfig;
}

/*!
    \brief      USART transmit data function
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  data: data of transmission
    \param[out] none
    \retval     none
*/
void usart_data_transmit(uint32_t usart_periph, uint32_t data)
{
    USART_TDATA(usart_periph) = (USART_TDATA_TDATA & data);
}

/*!
    \brief      USART receive data function
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     data of received
*/
uint16_t usart_data_receive(uint32_t usart_periph)
{
    return (uint16_t)(GET_BITS(USART_RDATA(usart_periph), 0U, 8U));
}

/*!
    \brief      data is transmitted/received with the LSB/MSB first
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  msbf: LSB/MSB
      \arg        USART_MSBF_LSB: LSB first
      \arg        USART_MSBF_MSB: MSB first
    \param[out] none
    \retval     none
*/
void usart_data_first_config(uint32_t usart_periph, uint32_t msbf)
{
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* configure LSB or MSB first */
    USART_CTL1(usart_periph) &= ~(USART_CTL1_MSBF);
    USART_CTL1(usart_periph) |= (USART_CTL1_MSBF & msbf);
}

/*!
    \brief      USART inverted configure
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  invertpara: refer to usart_invert_enum
    \param[out] none
    \retval     none
*/
void usart_invert_config(uint32_t usart_periph, usart_invert_enum invertpara)
{
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* inverted or not the specified signal */
    switch(invertpara){
    case USART_DINV_ENABLE:
        USART_CTL1(usart_periph) |= USART_CTL1_DINV;
        break;
    case USART_DINV_DISABLE:
        USART_CTL1(usart_periph) &= ~(USART_CTL1_DINV);
        break;
    case USART_TXPIN_DISABLE:
        USART_CTL1(usart_periph) &= ~(USART_CTL1_TINV);
        break;
    case USART_RXPIN_DISABLE:
        USART_CTL1(usart_periph) &= ~(USART_CTL1_RINV);
        break;
    case USART_SWAP_DISABLE:
        USART_CTL1(usart_periph) &= ~(USART_CTL1_STRP);
        break;
    case USART_TXPIN_ENABLE:
        USART_CTL1(usart_periph) |= USART_CTL1_TINV;
        break;
    case USART_RXPIN_ENABLE:
        USART_CTL1(usart_periph) |= USART_CTL1_RINV;
        break;
    case USART_SWAP_ENABLE:
        USART_CTL1(usart_periph) |= USART_CTL1_STRP;
        break;
    default:
        break;
    }
}

/*!
    \brief      overrun function is enabled
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_overrun_enable(uint32_t usart_periph)
{
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* enable overrun function */
    USART_CTL2(usart_periph) &= ~(USART_CTL2_OVRD);
}

/*!
    \brief      overrun function is disabled
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_overrun_disable(uint32_t usart_periph)
{
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* disable overrun function */
    USART_CTL2(usart_periph) |= USART_CTL2_OVRD;
}

/*!
    \brief      configure the USART oversample mode
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  oversamp: oversample value
      \arg        USART_OVSMOD_8: oversampling by 8
      \arg        USART_OVSMOD_16: oversampling by 16
    \param[out] none
    \retval     none
*/
void usart_oversample_config(uint32_t usart_periph, uint32_t oversamp)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* clear OVSMOD bit */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_OVSMOD);
    USART_CTL0(usart_periph) |= oversamp;
}

/*!
    \brief      sample bit method configure
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  osb: sample bit
      \arg        USART_OSB_1BIT: 1 bit
      \arg        USART_OSB_3BIT: 3 bits
    \param[out] none
    \retval     none
*/
void usart_sample_bit_config(uint32_t usart_periph, uint32_t osb)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    
    USART_CTL2(usart_periph) &= ~(USART_CTL2_OSB);
    USART_CTL2(usart_periph) |= osb;
}

/*!
    \brief      auto baud rate detection enable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_autobaud_detection_enable(uint32_t usart_periph)
{
    USART_CTL1(usart_periph) |= USART_CTL1_ABDEN;
}

/*!
    \brief      auto baud rate detection disable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_autobaud_detection_disable(uint32_t usart_periph)
{
    USART_CTL1(usart_periph) &= ~(USART_CTL1_ABDEN);
}

/*!
    \brief      auto baud rate detection mode configure
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  abdmod: auto baud rate detection mode
      \arg        USART_ABDM_FTOR: falling edge to rising edge measurement
      \arg        USART_ABDM_FTOF: falling edge to falling edge measurement
    \param[out] none
    \retval     none
*/
void usart_autobaud_detection_mode_config(uint32_t usart_periph, uint32_t abdmod)
{
    /* reset ABDM bits */
    USART_CTL1(usart_periph) &= ~(USART_CTL1_ABDM);
    USART_CTL1(usart_periph) |= abdmod;
}

/*!
    \brief      mute mode enable
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_mute_mode_enable(uint32_t usart_periph)
{
    USART_CTL0(usart_periph) |= USART_CTL0_MEN;
}

/*!
    \brief      mute mode disable
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_mute_mode_disable(uint32_t usart_periph)
{
    USART_CTL0(usart_periph) &= ~(USART_CTL0_MEN);
}

/*!
    \brief      wakeup method in mute mode configure
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  wmethod: two methods be used to enter or exit the mute mode
      \arg        USART_WM_IDLE: idle line
      \arg        USART_WM_ADDR: address mark
    \param[out] none
    \retval     none
*/
void usart_mute_mode_wakeup_config(uint32_t usart_periph, uint32_t wmethod)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL0(usart_periph) &= ~(USART_CTL0_WM);
    USART_CTL0(usart_periph) |= wmethod;
}

/*!
    \brief      address detection mode configure
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  addmod: address detection mode
      \arg        USART_ADDM_4BIT: 4 bits
      \arg        USART_ADDM_FULLBIT: full bits
    \param[out] none
    \retval     none
*/
void usart_address_detection_mode_config(uint32_t usart_periph, uint32_t addmod)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL1(usart_periph) &= ~(USART_CTL1_ADDM);
    USART_CTL1(usart_periph) |= USART_CTL1_ADDM & (addmod);
}

/*!
    \brief      address of the USART terminal
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  addr: 0x00-0xFF, address of USART terminal
    \param[out] none
    \retval     none
*/
void usart_address_config(uint32_t usart_periph, uint8_t addr)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL1(usart_periph) &= ~(USART_CTL1_ADDR);
    USART_CTL1(usart_periph) |= (USART_CTL1_ADDR & (((uint32_t)addr) << 24));
}

/*!
    \brief      receiver timeout enable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_receiver_timeout_enable(uint32_t usart_periph)
{
    USART_CTL1(usart_periph) |= USART_CTL1_RTEN;
}

/*!
    \brief      receiver timeout disable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_receiver_timeout_disable(uint32_t usart_periph)
{
    USART_CTL1(usart_periph) &= ~(USART_CTL1_RTEN);
}

/*!
    \brief      receiver timeout threshold configure
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  rtimeout: 0x000000-0xFFFFFF, receiver timeout value in terms of number of baud clocks
    \param[out] none
    \retval     none
*/
void usart_receiver_timeout_config(uint32_t usart_periph, uint32_t rtimeout)
{
    USART_RT(usart_periph) &= ~(USART_RT_RT);
    USART_RT(usart_periph) |= rtimeout;
}

/*!
    \brief      LIN mode enable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_lin_mode_enable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL1(usart_periph) |= USART_CTL1_LMEN;
}

/*!
    \brief      LIN mode disable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_lin_mode_disable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL1(usart_periph) &= ~(USART_CTL1_LMEN);
}

/*!
    \brief      LIN break detection length
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  lblen: LIN break detection length
      \arg        USART_LBLEN_10B: 10 bits break detection
      \arg        USART_LBLEN_11B: 11 bits break detection
    \param[out] none
    \retval     none
*/
void usart_lin_break_dection_length_config(uint32_t usart_periph, uint32_t lblen)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL1(usart_periph) |= USART_CTL1_LBLEN & (lblen);
}

/*!
    \brief      half-duplex enable
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_halfduplex_enable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) |= USART_CTL2_HDEN;
}

/*!
    \brief      half-duplex disable
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_halfduplex_disable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) &= ~(USART_CTL2_HDEN);
}

/*!
    \brief      clock enable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_clock_enable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL1(usart_periph) |= USART_CTL1_CKEN;
}

/*!
    \brief      clock disable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_clock_disable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL1(usart_periph) &= ~(USART_CTL1_CKEN);
}

/*!
    \brief      configure USART synchronous mode parameters
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  clen: last bit clock pulse
      \arg        USART_CLEN_NONE: clock pulse of the last data bit (MSB) is not output to the CK pin
      \arg        USART_CLEN_EN:   clock pulse of the last data bit (MSB) is output to the CK pin
    \param[in]  cph: clock phase
      \arg        USART_CPH_1CK: first clock transition is the first data capture edge
      \arg        USART_CPH_2CK: second clock transition is the first data capture edge
    \param[in]  cpl: clock polarity
      \arg        USART_CPL_LOW:  steady low value on CK pin
      \arg        USART_CPL_HIGH: steady high value on CK pin
    \param[out] none
    \retval     none
*/
void usart_synchronous_clock_config(uint32_t usart_periph, uint32_t clen, uint32_t cph, uint32_t cpl)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* reset USART_CTL1 CLEN,CPH,CPL bits */
    USART_CTL1(usart_periph) &= ~(USART_CTL1_CLEN | USART_CTL1_CPH | USART_CTL1_CPL);

    USART_CTL1(usart_periph) |= (USART_CTL1_CLEN & clen);
    USART_CTL1(usart_periph) |= (USART_CTL1_CPH & cph);
    USART_CTL1(usart_periph) |= (USART_CTL1_CPL & cpl);
}

/*!
    \brief      smartcard mode enable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_enable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) |= USART_CTL2_SCEN;
}

/*!
    \brief      smartcard mode disable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_disable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) &= ~(USART_CTL2_SCEN);
}

/*!
    \brief      NACK enable in smartcard mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_nack_enable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) |= USART_CTL2_NKEN;
}

/*!
    \brief      NACK disable in smartcard mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_nack_disable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) &= ~(USART_CTL2_NKEN);
}

/*!
    \brief      guard time value configure in smartcard mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  guat: 0x00-0xFF
    \param[out] none
    \retval     none
*/
void usart_guard_time_config(uint32_t usart_periph, uint32_t guat)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_GP(usart_periph) &= ~(USART_GP_GUAT);
    USART_GP(usart_periph) |= (USART_GP_GUAT & ((guat) << 8));
}

/*!
    \brief      block length configure
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  bl: 0x00-0xFF
    \param[out] none
    \retval     none
*/
void usart_block_length_config(uint32_t usart_periph, uint32_t bl)
{
    USART_RT(usart_periph) &= ~(USART_RT_BL);
    USART_RT(usart_periph) |= (USART_RT_BL & ((bl) << 24));
}

/*!
    \brief      smartcard auto-retry number configure
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  scrtnum: 0x0-0x7, smartcard auto-retry number
    \param[out] none
    \retval     none
*/
void usart_smartcard_autoretry_config(uint32_t usart_periph, uint32_t scrtnum)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    USART_CTL2(usart_periph) &= ~(USART_CTL2_SCRTNUM);
    USART_CTL2(usart_periph) |= (USART_CTL2_SCRTNUM & (scrtnum << 17));
}

/*!
    \brief      early NACK enable in smartcard mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_early_nack_enable(uint32_t usart_periph)
{
    USART_RFCS(usart_periph) |= USART_RFCS_ELNACK;
}

/*!
    \brief      early NACK disable in smartcard mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_early_nack_disable(uint32_t usart_periph)
{
    USART_RFCS(usart_periph) &= ~USART_RFCS_ELNACK;
}

/*!
    \brief      IrDA mode enable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_irda_mode_enable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) |= USART_CTL2_IREN;
}

/*!
    \brief      IrDA mode disable
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_irda_mode_disable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) &= ~(USART_CTL2_IREN);
}

/*!
    \brief      IrDA low-power configure
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  irlp: IrDA low-power or normal
      \arg        USART_IRLP_LOW:    low-power
      \arg        USART_IRLP_NORMAL: normal
    \param[out] none
    \retval     none
*/
void usart_irda_lowpower_config(uint32_t usart_periph, uint32_t irlp)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) |= (USART_CTL2_IRLP & irlp);
}

/*!
    \brief      configure the peripheral clock prescaler in USART IrDA low-power mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  psc: 0x00-0xFF
    \param[out] none
    \retval     none
*/
void usart_prescaler_config(uint32_t usart_periph, uint32_t psc)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    USART_GP(usart_periph) &= ~(USART_GP_PSC);
    USART_GP(usart_periph) |= psc;
}

/*!
    \brief      configure hardware flow control RTS
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  rtsconfig: enable or disable RTS
      \arg        USART_RTS_ENABLE:  enable RTS
      \arg        USART_RTS_DISABLE: disable RTS
    \param[out] none
    \retval     none
*/
void usart_hardware_flow_rts_config(uint32_t usart_periph, uint32_t rtsconfig)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) &= ~(USART_CTL2_RTSEN);
    USART_CTL2(usart_periph) |= rtsconfig;
}

/*!
    \brief      configure hardware flow control CTS
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  ctsconfig:  enable or disable CTS
      \arg        USART_CTS_ENABLE:  enable CTS
      \arg        USART_CTS_DISABLE: disable CTS
    \param[out] none
    \retval     none
*/
void usart_hardware_flow_cts_config(uint32_t usart_periph, uint32_t ctsconfig)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) &= ~USART_CTL2_CTSEN;
    USART_CTL2(usart_periph) |= ctsconfig;
}

/*!
    \brief      RS485 driver enable
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_rs485_driver_enable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) |= USART_CTL2_DEM;
}

/*!
    \brief      RS485 driver disable
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_rs485_driver_disable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) &= ~(USART_CTL2_DEM);
}

/*!
    \brief      driver enable assertion time configure
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  deatime: 0x00-0x1F
    \param[out] none
    \retval     none
*/
void usart_driver_assertime_config(uint32_t usart_periph, uint32_t deatime)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL0(usart_periph) &= ~(USART_CTL0_DEA);
    USART_CTL0(usart_periph) |= (USART_CTL0_DEA & ((deatime) << 21));
}

/*!
    \brief      driver enable de-assertion time configure
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  dedtime: 0x00-0x1F
    \param[out] none
    \retval     none
*/
void usart_driver_deassertime_config(uint32_t usart_periph, uint32_t dedtime)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL0(usart_periph) &= ~(USART_CTL0_DED);
    USART_CTL0(usart_periph) |= (USART_CTL0_DED & ((dedtime) << 16));
}

/*!
    \brief      configure driver enable polarity mode
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  dep: DE signal
      \arg        USART_DEP_HIGH: DE signal is active high
      \arg        USART_DEP_LOW:  DE signal is active low
    \param[out] none
    \retval     none
*/
void usart_depolarity_config(uint32_t usart_periph, uint32_t dep)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* reset DEP bit */
    USART_CTL2(usart_periph) &= ~(USART_CTL2_DEP);
    USART_CTL2(usart_periph) |= (USART_CTL2_DEP & dep);
}

/*!
    \brief      configure USART DMA reception
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  dmacmd: enable or disable DMA for reception
      \arg        USART_DENR_ENABLE:  DMA enable for reception
      \arg        USART_DENR_DISABLE: DMA disable for reception
    \param[out] none
    \retval     none
*/
void usart_dma_receive_config(uint32_t usart_periph, uint32_t dmacmd)
{
    USART_CTL2(usart_periph) &= ~USART_CTL2_DENR;
    /* configure DMA reception */
    USART_CTL2(usart_periph) |= dmacmd;
}

/*!
    \brief      configure USART DMA transmission
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  dmacmd: enable or disable DMA for transmission
      \arg        USART_DENT_ENABLE:  DMA enable for transmission
      \arg        USART_DENT_DISABLE: DMA disable for transmission
    \param[out] none
    \retval     none
*/
void usart_dma_transmit_config(uint32_t usart_periph, uint32_t dmacmd)
{
    USART_CTL2(usart_periph) &= ~USART_CTL2_DENT;
    /* configure DMA transmission */
    USART_CTL2(usart_periph) |= dmacmd;
}

/*!
    \brief      disable DMA on reception error
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_reception_error_dma_disable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) |= USART_CTL2_DDRE;
}

/*!
    \brief      enable DMA on reception error 
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_reception_error_dma_enable(uint32_t usart_periph)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);

    USART_CTL2(usart_periph) &= ~(USART_CTL2_DDRE);
}

/*!
    \brief      USART be able to wake up the MCU from deep-sleep mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_wakeup_enable(uint32_t usart_periph)
{
    USART_CTL0(usart_periph) |= USART_CTL0_UESM;
}

/*!
    \brief      USART be not able to wake up the MCU from deep-sleep mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[out] none
    \retval     none
*/
void usart_wakeup_disable(uint32_t usart_periph)
{
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UESM);
}

/*!
    \brief      wakeup mode from deep-sleep mode
    \param[in]  usart_periph: USARTx(x=0)
    \param[in]  wum: wakeup mode 
      \arg        USART_WUM_ADDR:   WUF active on address match
      \arg        USART_WUM_STARTB: WUF active on start bit
      \arg        USART_WUM_RBNE:   WUF active on RBNE
    \param[out] none
    \retval     none
*/
void usart_wakeup_mode_config(uint32_t usart_periph, uint32_t wum)
{
    /* disable USART */
    USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN);
    /* reset WUM bit */
    USART_CTL2(usart_periph) &= ~(USART_CTL2_WUM);
    USART_CTL2(usart_periph) |= USART_CTL2_WUM & (wum);
}

/*!
    \brief      receive FIFO enable
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_receive_fifo_enable(uint32_t usart_periph)
{
    USART_RFCS(usart_periph) |= USART_RFCS_RFEN;
}

/*!
    \brief      receive FIFO disable
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     none
*/
void usart_receive_fifo_disable(uint32_t usart_periph)
{
    USART_RFCS(usart_periph) &= ~(USART_RFCS_RFEN);
}

/*!
    \brief      read receive FIFO counter number
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[out] none
    \retval     receive FIFO counter number
*/
uint8_t usart_receive_fifo_counter_number(uint32_t usart_periph)
{
    return (uint8_t)(GET_BITS(USART_RFCS(usart_periph), 12U, 14U));
}

/*!
    \brief      get flag in STAT/RFCS register
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  flag: flag type
      \arg        USART_FLAG_PERR:  parity error flag
      \arg        USART_FLAG_FERR:  frame error flag
      \arg        USART_FLAG_NERR:  noise error flag
      \arg        USART_FLAG_ORERR: overrun error
      \arg        USART_FLAG_IDLE:  idle line detected flag
      \arg        USART_FLAG_RBNE:  read data buffer not empty
      \arg        USART_FLAG_TC:    transmission completed
      \arg        USART_FLAG_TBE:   transmit data register empty
      \arg        USART_FLAG_LBD:   LIN break detected flag
      \arg        USART_FLAG_CTSF:   CTS change flag
      \arg        USART_FLAG_CTS:   CTS level
      \arg        USART_FLAG_RT:    receiver timeout flag
      \arg        USART_FLAG_EB:    end of block flag
      \arg        USART_FLAG_ABDE:  auto baudrate detection error
      \arg        USART_FLAG_ABD:   auto baudrate detection flag
      \arg        USART_FLAG_BSY:   busy flag
      \arg        USART_FLAG_AM:    address match flag
      \arg        USART_FLAG_SB:    send break flag
      \arg        USART_FLAG_RWU:   receiver wakeup from mute mode.
      \arg        USART_FLAG_WU:    wakeup from deep-sleep mode flag
      \arg        USART_FLAG_TEA:   transmit enable acknowledge flag
      \arg        USART_FLAG_REA:   receive enable acknowledge flag 
      \arg        USART_FLAG_RFFINT:receive FIFO full interrupt flag
      \arg        USART_FLAG_RFE:   receive FIFO empty flag
      \arg        USART_FLAG_RFF:   receive FIFO full flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus usart_flag_get(uint32_t usart_periph, usart_flag_enum flag)
{
    if(RESET != (USART_REG_VAL(usart_periph, flag) & BIT(USART_BIT_POS(flag)))){
        return SET;
    }else{
        return RESET;
    }
}

/*!
    \brief      clear USART status
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  flag: flag type
      \arg        USART_FLAG_PERR: parity error flag
      \arg        USART_FLAG_FERR: frame error flag
      \arg        USART_FLAG_NERR: noise detected flag
      \arg        USART_FLAG_ORERR:overrun error flag
      \arg        USART_FLAG_IDLE: idle line detected flag
      \arg        USART_FLAG_TC:   transmission complete flag
      \arg        USART_FLAG_LBD:  LIN break detected flag
      \arg        USART_FLAG_CTSF: CTS change flag
      \arg        USART_FLAG_RT:   receiver timeout flag
      \arg        USART_FLAG_EB:   end of block flag
      \arg        USART_FLAG_AM:   address match flag
      \arg        USART_FLAG_WU:   wakeup from deep-sleep mode flag
    \param[out] none
    \retval     none
*/
void usart_flag_clear(uint32_t usart_periph, usart_flag_enum flag)
{
    USART_INTC(usart_periph) |= BIT(USART_BIT_POS(flag));
}

/*!
    \brief      enable USART interrupt
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  inttype: interrupt type
      \arg        USART_INT_IDLE: idle interrupt
      \arg        USART_INT_RBNE: read data buffer not empty interrupt and
                                  overrun error interrupt enable interrupt
      \arg        USART_INT_TC:   transmission complete interrupt
      \arg        USART_INT_TBE:  transmit data register empty interrupt
      \arg        USART_INT_PERR: parity error interrupt
      \arg        USART_INT_AM:   address match interrupt
      \arg        USART_INT_RT:   receiver timeout interrupt
      \arg        USART_INT_EB:   end of block interrupt
      \arg        USART_INT_LBD:  LIN break detection interrupt
      \arg        USART_INT_ERR:  error interrupt enable in multibuffer communication
      \arg        USART_INT_CTS:  CTS interrupt
      \arg        USART_INT_WU:   wakeup from deep-sleep mode interrupt
      \arg        USART_INT_RFF:  receive FIFO full interrupt enable
    \param[out] none
    \retval     none
*/
void usart_interrupt_enable(uint32_t usart_periph, uint32_t inttype)
{
    USART_REG_VAL(usart_periph, inttype) |= BIT(USART_BIT_POS(inttype));
}

/*!
    \brief      disable USART interrupt
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  inttype: interrupt type
      \arg        USART_INT_IDLE: idle interrupt
      \arg        USART_INT_RBNE: read data buffer not empty interrupt and
                                  overrun error interrupt
      \arg        USART_INT_TC:   transmission complete interrupt
      \arg        USART_INT_TBE:  transmit data register empty interrupt
      \arg        USART_INT_PERR: parity error interrupt
      \arg        USART_INT_AM:   address match interrupt
      \arg        USART_INT_RT:   receiver timeout interrupt
      \arg        USART_INT_EB:   end of block interrupt
      \arg        USART_INT_LBD:  LIN break detection interrupt
      \arg        USART_INT_ERR:  error interrupt enable in multibuffer communication
      \arg        USART_INT_CTS:  CTS interrupt
      \arg        USART_INT_WU:   wakeup from deep-sleep mode interrupt
      \arg        USART_INT_RFF:  receive FIFO full interrupt enable
    \param[out] none
    \retval     none
*/
void usart_interrupt_disable(uint32_t usart_periph, uint32_t inttype)
{
    USART_REG_VAL(usart_periph, inttype) &= ~BIT(USART_BIT_POS(inttype));
}

/*!
    \brief      enable USART command
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  cmdtype: command type
      \arg        USART_CMD_ABDCMD: auto baudrate detection command
      \arg        USART_CMD_SBKCMD: send break command
      \arg        USART_CMD_MMCMD:  mute mode command
      \arg        USART_CMD_RXFCMD: receive data flush command
      \arg        USART_CMD_TXFCMD: transmit data flush request
    \param[out] none
    \retval     none
*/
void usart_command_enable(uint32_t usart_periph, uint32_t cmdtype)
{
    USART_CMD(usart_periph) |= (cmdtype);   
}

/*!
    \brief      get USART interrupt and flag status
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  int_flag: interrupt and flag type, refer to usart_interrupt_flag_enum
      \arg        USART_INT_FLAG_EB:   end of block interrupt and flag
      \arg        USART_INT_FLAG_RT:   receiver timeout interrupt and flag
      \arg        USART_INT_FLAG_AM:   address match interrupt and flag 
      \arg        USART_INT_FLAG_PERR: parity error interrupt and flag 
      \arg        USART_INT_FLAG_TBE:  transmitter buffer empty interrupt and flag 
      \arg        USART_INT_FLAG_TC:   transmission complete interrupt and flag
      \arg        USART_INT_FLAG_RBNE: read data buffer not empty interrupt and flag
      \arg        USART_INT_FLAG_RBNE_ORERR: read data buffer not empty interrupt and overrun error flag
      \arg        USART_INT_FLAG_IDLE: IDLE line detected interrupt and flag
      \arg        USART_INT_FLAG_LBD:  LIN break detected interrupt and flag 
      \arg        USART_INT_FLAG_WU:   wakeup from deep-sleep mode interrupt and flag
      \arg        USART_INT_FLAG_CTS:  CTS interrupt and flag
      \arg        USART_INT_FLAG_ERR_NERR:  error interrupt and noise error flag
      \arg        USART_INT_FLAG_ERR_ORERR: error interrupt and overrun error
      \arg        USART_INT_FLAG_ERR_FERR:  error interrupt and frame error flag
      \arg        USART_INT_FLAG_RFF: receive FIFO full interrupt and flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus usart_interrupt_flag_get(uint32_t usart_periph, uint32_t int_flag)
{
    uint32_t intenable = 0U, flagstatus = 0U;
    /* get the interrupt enable bit status */
    intenable = (USART_REG_VAL(usart_periph, int_flag) & BIT(USART_BIT_POS(int_flag)));
    /* get the corresponding flag bit status */
    flagstatus = (USART_REG_VAL2(usart_periph, int_flag) & BIT(USART_BIT_POS2(int_flag)));

    if(flagstatus && intenable){
        return SET;
    }else{
        return RESET; 
    }
}

/*!
    \brief      clear USART interrupt flag
    \param[in]  usart_periph: USARTx(x=0,1)
    \param[in]  flag: USART interrupt flag
      \arg        USART_INT_FLAG_PERR: parity error flag
      \arg        USART_INT_FLAG_FERR: frame error flag
      \arg        USART_INT_FLAG_NERR: noise detected flag
      \arg        USART_INT_FLAG_ORERR:overrun error flag
      \arg        USART_INT_FLAG_IDLE: idle line detected flag
      \arg        USART_INT_FLAG_TC:   transmission complete flag
      \arg        USART_INT_FLAG_LBD:  LIN break detected flag
      \arg        USART_INT_FLAG_CTS:  CTS change flag
      \arg        USART_INT_FLAG_RT:   receiver timeout flag
      \arg        USART_INT_FLAG_EB:   end of block flag
      \arg        USART_INT_FLAG_AM:   address match flag
      \arg        USART_INT_FLAG_WU:   wakeup from deep-sleep mode flag
    \param[out] none
    \retval     none
*/
void usart_interrupt_flag_clear(uint32_t usart_periph, uint32_t flag)
{
    USART_INTC(usart_periph) |= BIT(USART_BIT_POS2(flag));
}
