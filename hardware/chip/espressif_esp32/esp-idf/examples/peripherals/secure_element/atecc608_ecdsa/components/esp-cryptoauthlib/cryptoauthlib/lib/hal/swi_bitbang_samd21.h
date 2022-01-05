/**
 * \file
 * \brief Hardware Interface Functions - SWI bit-banged
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef SWI_BITBANG_SAMD21_H_
#define SWI_BITBANG_SAMD21_H_

#include "atca_status.h"
#include <delay.h>


#define MAX_SWI_BUSES   36      //!< SAMD21 xplainned pro has 36 free GPIO pins available


typedef struct
{
    uint32_t pin_sda[MAX_SWI_BUSES];
} SWIBuses;

extern SWIBuses swi_buses_default;


/**
 * \name Macros for Bit-Banged SWI Timing
 *
 * Times to drive bits at 230.4 kbps.
   @{ */

//! delay macro for width of one pulse (start pulse or zero pulse)
//! should be 4.34 us, is 4.05 us

#define BIT_DELAY_1L        delay_us(3)
//! should be 4.34 us, is 4.05us
#define BIT_DELAY_1H        delay_us(3)

//! time to keep pin high for five pulses plus stop bit (used to bit-bang CryptoAuth 'zero' bit)
//! should be 26.04 us, is 26.92 us
#define BIT_DELAY_5        delay_us(26)    // considering pin set delay

//! time to keep pin high for seven bits plus stop bit (used to bit-bang CryptoAuth 'one' bit)
//! should be 34.72 us, is 35.13 us
#define BIT_DELAY_7        delay_us(34)    // considering pin set delay

//! turn around time when switching from receive to transmit
//! should be 93 us (Setting little less value as there would be other process before these steps)
#define RX_TX_DELAY         delay_us(65)


//! Lets set the timeout value for start pulse detection to the uint8_t maximum.
//! This value is decremented while waiting for the falling edge of a start pulse.
#define START_PULSE_TIME_OUT    (600)

//! Maximum time between rising edge of start pulse
//! and falling edge of zero pulse is 8.6 us. Therefore, a value of 40 (around 15 us)
//! gives ample time to detect a zero pulse and also leaves enough time to detect
//! the following start pulse.
//! This value is decremented while waiting for the falling edge of a zero pulse.
#define ZERO_PULSE_TIME_OUT     (40)

/** @} */


/**
 * \brief Set SWI signal pin.
 *        Other functions will use this pin.
 *
 * \param[in] id  definition of GPIO pin to be used
 */
void swi_set_pin(uint8_t id);



/**
 * \brief Configure GPIO pin for SWI signal as output.
 */
void swi_enable(void);

/**
 * \brief Configure GPIO pin for SWI signal as input.
 */
void swi_disable(void);

/**
 * \brief Set signal pin Low or High.
 *
 * \param[in] is_high  0: Low, else: High.
 */
void swi_set_signal_pin(uint8_t is_high);

/**
 * \brief Send a Wake Token.
 */
void swi_send_wake_token(void);

/**
 * \brief Send a number of bytes.This function should not be called directly ,instead should use hal_swi_send() which call this function.
 *
 * \param[in] count   number of bytes to send.
 * \param[in] buffer  pointer to buffer containing bytes to send
 */
void swi_send_bytes(uint8_t count, uint8_t *buffer);

/**
 * \brief Send one byte.
 *
 * \param[in] byte  byte to send
 */
void swi_send_byte(uint8_t byte);

/**
 * \brief Receive a number of bytes.This function should not be called directly ,instead should use hal_swi_receive() which call this function.
 *
 * \param[in]  count   number of bytes to receive
 * \param[out] buffer  pointer to receive buffer
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS swi_receive_bytes(uint8_t count, uint8_t *buffer);


#endif /* SWI_BITBANG_SAMD21_H_ */