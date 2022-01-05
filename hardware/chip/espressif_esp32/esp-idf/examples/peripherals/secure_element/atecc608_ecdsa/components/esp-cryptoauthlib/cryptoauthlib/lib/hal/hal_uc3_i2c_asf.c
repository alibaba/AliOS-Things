/**
 * \file
 * \brief ATCA Hardware abstraction layer for SAMV71 I2C over ASF drivers.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the ASF I2C primitives to set up the interface.
 *
 * Prerequisite: add SERCOM I2C Master Polled support to application in Atmel Studio
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
#include <string.h>
#include <stdio.h>

#include "atca_hal.h"
#include "atca_device.h"
#include "atca_execution.h"
#include "hal_uc3_i2c_asf.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using I2C driver of ASF.
 *
   @{ */

/** \brief logical to physical bus mapping structure */
static ATCAI2CMaster_t i2c_hal_data[MAX_I2C_BUSES];   // map logical, 0-based bus number to index

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in]  bus_num  Logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg      Pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] found    Number of devices found on this bus
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief
    - this HAL implementation assumes you've included the ASF Twi libraries in your project, otherwise,
    the HAL layer will not compile because the ASF TWI drivers are a dependency *
 */

/** \brief hal_i2c_init manages requests to initialize a physical interface.  it manages use counts so when an interface
 * has released the physical layer, it will disable the interface for some other use.
 * You can have multiple ATCAIFace instances using the same bus, and you can have multiple ATCAIFace instances on
 * multiple i2c buses, so hal_i2c_init manages these things and ATCAIFace is abstracted from the physical details.
 */

/** \brief initialize an I2C interface using given config
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    twi_options_t opt;
    ATCA_STATUS status = !ATCA_SUCCESS;
    ATCAI2CMaster_t* data = &i2c_hal_data[cfg->atcai2c.bus];


    if (data->ref_ct <= 0)
    {
        // Bus isn't being used, enable it
        switch (cfg->atcai2c.bus)
        {

        case 2:
            data->twi_master_instance = &AVR32_TWI;
            static const gpio_map_t TWI_GPIO_MAP =
            {
                { AVR32_TWI_SDA_0_0_PIN, AVR32_TWI_SDA_0_0_FUNCTION },
                { AVR32_TWI_SCL_0_0_PIN, AVR32_TWI_SCL_0_0_FUNCTION }
            };

            gpio_enable_module(TWI_GPIO_MAP, sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));
            // options settings
            opt.pba_hz = 48000000;
            opt.speed = 100000;
            opt.chip = 0;

            // initialize TWI driver with options
            twi_master_init(&AVR32_TWI, &opt);
            break;
        default:
            return ATCA_COMM_FAIL;
        }

        // store this for use during the release phase
        data->bus_index = cfg->atcai2c.bus;
        // buses are shared, this is the first instance
        data->ref_ct = 1;
    }
    else
    {
        // Bus is already is use, increment reference counter
        data->ref_ct++;
    }

    ((ATCAHAL_t*)hal)->hal_data = data;

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C post init
 * \param[in] iface  instance
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}


/** \brief HAL implementation of I2C send over ASF
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{
    twi_package_t packet;

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);

    txdata[0] = 0x03;     // insert the Word Address Value, Command token
    txlength++;

    // TWI chip address to communicate with
    packet.chip = cfg->atcai2c.slave_address >> 1;
    // TWI address/commands to issue to the other chip (node)
    packet.addr[0] = 0;
    packet.addr[1] = 0;
    packet.addr[2] = 0;
    // Length of the TWI data address segment (1-3 bytes)
    packet.addr_length = 0;
    // Where to find the data to be written
    packet.buffer = (void*)txdata;
    // How many bytes do we want to write
    packet.length = (uint32_t)txlength;

    if (twi_master_write(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &packet) != TWI_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }


    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C receive function for ASF I2C
 * \param[in]    iface     Device to interact with.
 * \param[out]   rxdata    Data received will be returned here.
 * \param[inout] rxlength  As input, the size of the rxdata buffer.
 *                         As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int retries = cfg->rx_retries;
    uint32_t status = !ATCA_SUCCESS;
    uint16_t rxdata_max_size = *rxlength;

    twi_package_t packet = {
        .chip           = cfg->atcai2c.slave_address >> 1,
        .addr[0]        = { 0 },
        .addr[1]        = { 0 },
        .addr[2]        = { 0 },
        .addr_length    = 0,
        .buffer         = (void*)rxdata,
        .length         = (uint32_t)1
    };

    *rxlength = 0;
    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }

    while (retries-- > 0 && status != ATCA_SUCCESS)
    {
        if (twi_master_read(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &packet) != TWI_SUCCESS)
        {
            status = ATCA_COMM_FAIL;
        }
        else
        {
            status = ATCA_SUCCESS;
        }
    }

    if (status != ATCA_SUCCESS)
    {
        return status;
    }
    if (rxdata[0] < ATCA_RSP_SIZE_MIN)
    {
        return ATCA_INVALID_SIZE;
    }
    if (rxdata[0] > rxdata_max_size)
    {
        return ATCA_SMALL_BUFFER;
    }

    atca_delay_ms(1);

    //Update receive length with first byte received and set to read rest of the data
    packet.length = rxdata[0] - 1;
    packet.buffer = &rxdata[1];

    if (twi_master_read(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &packet) != TWI_SUCCESS)
    {
        status = ATCA_COMM_FAIL;
    }
    else
    {
        status = ATCA_SUCCESS;
    }
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    *rxlength = rxdata[0];

    return ATCA_SUCCESS;
}

/** \brief method to change the bus speed of I2C
 * \param[in] iface  interface on which to change bus speed
 * \param[in] speed  baud rate (typically 100000 or 400000)
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS change_i2c_speed(ATCAIface iface, uint32_t speed)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief wake up CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to wakeup
 * \return ATCA_SUCCESS on success, otherwise an error code.

 */

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int retries = cfg->rx_retries;
    uint32_t bdrt = cfg->atcai2c.baud;
    int status = !TWI_SUCCESS;
    uint8_t data[4];
    twi_package_t packet;

    // TWI chip address to communicate with
    packet.chip = 0x00;
    // TWI address/commands to issue to the other chip (node)
    packet.addr[0] = 0;
    packet.addr[1] = 0;
    packet.addr[2] = 0;
    // Length of the TWI data address segment (1-3 bytes)
    packet.addr_length = 0;
    // Where to find the data to be written
    packet.buffer = (void*)&data[0];
    // How many bytes do we want to write
    packet.length = 1;

    // if not already at 100kHz, change it
    if (bdrt != 100000)
    {
        change_i2c_speed(iface, 100000);
    }

    // Send 0x00 as wake pulse
    twi_master_write(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &packet);

    // rounded up to the nearest ms
    atca_delay_ms(((uint32_t)cfg->wake_delay + (1000 - 1)) / 1000);   // wait tWHI + tWLO which is configured based on device type and configuration structure



    // if necessary, revert baud rate to what came in.
    if (bdrt != 100000)
    {
        change_i2c_speed(iface, bdrt);
    }

    packet.chip = cfg->atcai2c.slave_address >> 1;
    packet.buffer         = (void*)data;
    packet.length         = (uint32_t)4;
    while (retries-- > 0 && status != TWI_SUCCESS)
    {
        status = twi_master_read(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &packet);
    }
    if (status != TWI_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return hal_check_wake(data, 4);
}


/** \brief idle CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to idle
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data[4];

    data[0] = 0x02; // idle word address value
    twi_package_t packet = {
        .chip           = cfg->atcai2c.slave_address >> 1,
        .addr           = { 0 },
        .addr_length    = 0,
        .buffer         = (void*)data,
        .length         = 1
    };

    if (twi_master_write(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &packet) != TWI_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief sleep CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to sleep
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data[4];

    data[0] = 0x01; // sleep word address value
    twi_package_t packet = {
        .chip           = cfg->atcai2c.slave_address >> 1,
        .addr           = { 0 },
        .addr_length    = 0,
        .buffer         = (void*)data,
        .length         = 1
    };

    if (twi_master_write(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &packet) != TWI_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{
    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    // if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0)
    {
        twi_master_disable(hal->twi_master_instance);
        hal->ref_ct = 0;
    }

    return ATCA_SUCCESS;
}

/** @} */
