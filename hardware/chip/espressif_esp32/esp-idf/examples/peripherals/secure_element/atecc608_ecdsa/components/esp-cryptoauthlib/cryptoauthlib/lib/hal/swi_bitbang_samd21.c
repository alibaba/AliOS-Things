/**
 * \file
 * \brief  Hardware Interface Functions - SWI bit-banged
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

#include <asf.h>
#include <stdint.h>
#include "swi_bitbang_samd21.h"
#include "atca_command.h"




SWIBuses swi_buses_default = {
    { EXT3_PIN_3, EXT3_PIN_9, EXT3_PIN_I2C_SDA, EXT3_PIN_13, EXT2_PIN_13, EXT2_PIN_5, EXT2_PIN_7, EXT2_PIN_9, EXT2_PIN_3, EXT2_PIN_15, EXT2_PIN_17, EXT1_PIN_3, EXT1_PIN_5, EXT1_PIN_7, EXT1_PIN_9, EXT1_PIN_13, EXT1_PIN_15, EXT1_PIN_17, EXT3_PIN_7, EXT3_PIN_10, EXT3_PIN_I2C_SCL, EXT3_PIN_14, EXT2_PIN_4, EXT2_PIN_6, EXT2_PIN_8, EXT2_PIN_10, EXT2_PIN_14, EXT2_PIN_16, EXT2_PIN_18, EXT1_PIN_4, EXT1_PIN_6, EXT1_PIN_8, EXT1_PIN_10, EXT1_PIN_14, EXT1_PIN_16, EXT1_PIN_18 }
};

//! declaration of the variable indicating which pin the selected device is connected to
static uint8_t device_pin;

void swi_set_pin(uint8_t id)
{
    device_pin = id;
}

void swi_enable(void)
{
    struct port_config pin_conf;

    port_get_config_defaults(&pin_conf);
    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(device_pin, &pin_conf);
}

void swi_disable(void)
{
    struct port_config pin_conf;

    port_get_config_defaults(&pin_conf);
    port_pin_set_config(device_pin, &pin_conf);
}


void swi_set_signal_pin(uint8_t is_high)
{
    if (is_high)
    {
        port_pin_set_output_level(device_pin, true);
    }
    else
    {
        port_pin_set_output_level(device_pin, false);
    }
}

void swi_send_wake_token(void)
{
    swi_set_signal_pin(0);
    delay_us(60);
    swi_set_signal_pin(1);
}

void swi_send_bytes(uint8_t count, uint8_t *buffer)
{
    uint8_t i, bit_mask;
    struct port_config pin_conf;

    port_get_config_defaults(&pin_conf);
    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(device_pin, &pin_conf);

    cpu_irq_disable();
    for (i = 0; i < count; i++)
    {
        for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1)
        {
            if (bit_mask & buffer[i])   //!< Send Logic 1 (7F)
            {
                port_pin_set_output_level(device_pin, false);
                BIT_DELAY_1L;
                port_pin_set_output_level(device_pin, true);
                BIT_DELAY_7;
            }
            else     //!< Send Logic 0 (7D)
            {
                port_pin_set_output_level(device_pin, false);
                BIT_DELAY_1L;
                port_pin_set_output_level(device_pin, true);
                BIT_DELAY_1H;
                port_pin_set_output_level(device_pin, false);
                BIT_DELAY_1L;
                port_pin_set_output_level(device_pin, true);
                BIT_DELAY_5;
            }
        }
    }
    cpu_irq_enable();
}


void swi_send_byte(uint8_t byte)
{
    swi_send_bytes(1, &byte);
}

ATCA_STATUS swi_receive_bytes(uint8_t count, uint8_t *buffer)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t i;
    uint8_t bit_mask;
    uint8_t pulse_count;
    uint16_t timeout_count;
    struct port_config pin_conf;


    port_get_config_defaults(&pin_conf);
    port_pin_set_config(device_pin, &pin_conf);

    cpu_irq_disable();
    //! Receive bits and store in buffer.
    for (i = 0; i < count; i++)
    {
        buffer[i] = 0;
        for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1)
        {
            pulse_count = 0;


            timeout_count = START_PULSE_TIME_OUT;
            //! Detect start bit.

            while (--timeout_count > 0)
            {
                //! Wait for falling edge.
                if (port_pin_get_input_level(device_pin) == 0)
                {

                    break;
                }
            }
            if (timeout_count == 0)
            {
                status = ATCA_RX_TIMEOUT;
                break;
            }

            timeout_count = START_PULSE_TIME_OUT;

            do
            {
                //! Wait for rising edge.
                if (port_pin_get_input_level(device_pin) != 0)
                {
                    pulse_count = 1;

                    break;
                }
            }
            while (--timeout_count > 0);

            if (pulse_count == 0)
            {
                status = ATCA_RX_TIMEOUT;
                break;
            }

            //!  let's just wait the maximum time for the falling edge of a zero bit
            //! to arrive after we have detected the rising edge of the start bit.
            timeout_count = ZERO_PULSE_TIME_OUT;

            //! Detect possible edge indicating zero bit.
            do
            {
                if (port_pin_get_input_level(device_pin) == 0)
                {
                    pulse_count = 2;
                    break;
                }
            }
            while (--timeout_count > 0);


            //! Wait for rising edge of zero pulse before returning. Otherwise we might interpret
            //! its rising edge as the next start pulse.
            if (pulse_count == 2)
            {
                timeout_count = ZERO_PULSE_TIME_OUT;

                do
                {
                    if (port_pin_get_input_level(device_pin) != 0)
                    {
                        break;
                    }
                }

                while (timeout_count-- > 0);

            }
            //! Update byte at current buffer index.
            else
            //! received "one" bit
            {
                buffer[i] |= bit_mask;
            }
        }

        if (status != ATCA_SUCCESS)
        {
            break;
        }

        if (i == 0)
        {
            if (buffer[0] < ATCA_RSP_SIZE_MIN)
            {
                status = ATCA_INVALID_SIZE;
                break;
            }
            else if (buffer[0] > count)
            {
                status = ATCA_SMALL_BUFFER;
                break;
            }
            else
            {
                count = buffer[0];
            }
        }
    }
    cpu_irq_enable();
    RX_TX_DELAY;        //forcing tTURNAROUND (To CryptoAuthentication)
    if (status == ATCA_RX_TIMEOUT)
    {
        if (i > 0)
        {
            //! Indicate that we timed out after having received at least one byte.
            status = ATCA_RX_FAIL;
        }
    }

    return status;
}