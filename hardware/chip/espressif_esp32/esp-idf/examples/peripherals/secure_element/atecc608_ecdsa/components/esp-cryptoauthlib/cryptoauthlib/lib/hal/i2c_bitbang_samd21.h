/**
 * \file
 * \brief  definitions for bit-banged I2C
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

#ifndef I2C_BITBANG_SAMD21_H_
#define I2C_BITBANG_SAMD21_H_

#include "atca_status.h"
#include <delay.h>


#define MAX_I2C_BUSES   18       //The MAX_I2C_BUSES is the number of free pins in samd21 xplained pro


typedef struct
{
    uint8_t pin_sda[MAX_I2C_BUSES];
    uint8_t pin_scl[MAX_I2C_BUSES];
} I2CBuses;

extern I2CBuses i2c_buses_default;

extern uint8_t pin_sda;
extern uint8_t pin_scl;

#   define I2C_ENABLE()         {  struct port_config pin_conf; \
                                   port_get_config_defaults(&pin_conf); \
                                   pin_conf.direction  = PORT_PIN_DIR_OUTPUT_WTH_READBACK; \
                                   port_pin_set_config(pin_sda, &pin_conf); \
                                   pin_conf.direction  = PORT_PIN_DIR_OUTPUT; \
                                   port_pin_set_config(pin_scl, &pin_conf); }
#   define I2C_DISABLE()         {  struct port_config pin_conf; \
                                    port_get_config_defaults(&pin_conf); \
                                    pin_conf.direction  = PORT_PIN_DIR_INPUT; \
                                    pin_conf.input_pull = PORT_PIN_PULL_UP; \
                                    port_pin_set_config(pin_sda, &pin_conf); \
                                    port_pin_set_config(pin_scl, &pin_conf); }
#   define I2C_CLOCK_LOW()       port_pin_set_output_level(pin_scl, false)
#   define I2C_CLOCK_HIGH()      port_pin_set_output_level(pin_scl, true)
#   define I2C_DATA_LOW()        port_pin_set_output_level(pin_sda, false)
#   define I2C_DATA_HIGH()       port_pin_set_output_level(pin_sda, true)
#   define I2C_DATA_IN()         port_pin_get_input_level(pin_sda)
#   define I2C_SET_OUTPUT()      {  struct port_config pin_conf; \
                                    port_get_config_defaults(&pin_conf); \
                                    pin_conf.direction  = PORT_PIN_DIR_OUTPUT_WTH_READBACK; \
                                    port_pin_set_config(pin_sda, &pin_conf); }
#   define I2C_SET_OUTPUT_HIGH() { I2C_SET_OUTPUT(); I2C_DATA_HIGH(); }
#   define I2C_SET_OUTPUT_LOW()  { I2C_SET_OUTPUT(); I2C_DATA_LOW(); }
#   define I2C_SET_INPUT()       {  struct port_config pin_conf; \
                                    port_get_config_defaults(&pin_conf); \
                                    pin_conf.direction  = PORT_PIN_DIR_INPUT; \
                                    port_pin_set_config(pin_sda, &pin_conf); }
#   define DISABLE_INTERRUPT()   cpu_irq_disable()
#   define ENABLE_INTERRUPT()    cpu_irq_enable()


#define I2C_CLOCK_DELAY_WRITE_LOW()  delay_us(1)
#define I2C_CLOCK_DELAY_WRITE_HIGH() delay_us(1)
#define I2C_CLOCK_DELAY_READ_LOW()   delay_us(1)
#define I2C_CLOCK_DELAY_READ_HIGH()  delay_us(1)
#define I2C_CLOCK_DELAY_SEND_ACK()   delay_us(1)
//! This delay is inserted to make the Start and Stop hold time at least 250 ns.
#define I2C_HOLD_DELAY()    delay_us(1)




//! loop count when waiting for an acknowledgment
#define I2C_ACK_TIMEOUT                 (4)


/**
 * \brief Set I2C data and clock pin.
 *        Other functions will use these pins.
 *
 * \param[in] sda  definition of GPIO pin to be used as data pin
 * \param[in] scl  definition of GPIO pin to be used as clock pin
 */
void i2c_set_pin(uint8_t sda, uint8_t scl);


/**
 * \brief  Assigns the logical bus number for discovering the devices
 *
 *
 * \param[in]  i2c_bitbang_buses         The logical bus numbers are assigned to the variables.
 * \param[in]  max_buses                 Maximum number of bus used for discovering.
 */

void i2c_discover_buses(int i2c_bitbang_buses[], int max_buses);

/**
 * \brief Configure GPIO pins for I2C clock and data as output.
 */
void i2c_enable(void);

/**
 * \brief Configure GPIO pins for I2C clock and data as input.
 */
void i2c_disable(void);


/**
 * \brief Send a START condition.
 */
void i2c_send_start(void);

/**
 * \brief Send an ACK or NACK (after receive).
 *
 * \param[in] ack  0: NACK, else: ACK
 */
void i2c_send_ack(uint8_t ack);

/**
 * \brief Send a STOP condition.
 */
void i2c_send_stop(void);

/**
 * \brief Send a Wake Token.
 */
void i2c_send_wake_token(void);

/**
 * \brief Send one byte.
 *
 * \param[in] i2c_byte  byte to write
 *
 * \return ATCA_STATUS
 */
ATCA_STATUS i2c_send_byte(uint8_t i2c_byte);

/**
 * \brief Send a number of bytes.
 *
 * \param[in] count  number of bytes to send
 * \param[in] data   pointer to buffer containing bytes to send
 *
 * \return ATCA_STATUS
 */
ATCA_STATUS i2c_send_bytes(uint8_t count, uint8_t *data);

/**
 * \brief Receive one byte (MSB first).
 *
 * \param[in] ack  0:NACK, else:ACK
 *
 * \return Number of bytes received
 */
uint8_t i2c_receive_one_byte(uint8_t ack);

/**
 * \brief Receive one byte and send ACK.
 *
 * \param[out] data  pointer to received byte
 */
void i2c_receive_byte(uint8_t *data);

/**
 * \brief Receive a number of bytes.
 *
 * \param[out] data   pointer to receive buffer
 * \param[in]  count  number of bytes to receive
 */
void i2c_receive_bytes(uint8_t count, uint8_t *data);

#endif /* I2C_BITBANG_SAMD21_H_ */