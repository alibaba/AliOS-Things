/**
 * \file
 * \brief ATCA Hardware abstraction layer for PIC32MX695F512H I2C over plib drivers.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the xxx I2C primitives to set up the interface.
 *
 * Prerequisite:
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

#include <plib.h>
#include <stdio.h>
#include <string.h>

#include "hal/atca_hal.h"
#include "hal/hal_pic32mx695f512h_i2c.h"


/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

/**
 * \brief
 * Logical to physical bus mapping structure
 */
static ATCAI2CMaster_t i2c_hal_data[MAX_I2C_BUSES];   // map logical, 0-based bus number to index


/****** I2C Driver implementation *******/
static bool StartTransfer(I2C_MODULE i2c_id, bool restart)
{
    I2C_STATUS status;

    // Send the Start (or Restart) signal
    if (restart)
    {
        I2CRepeatStart(i2c_id);
    }
    else
    {
        // Wait for the bus to be idle, then start the transfer
        while (!I2CBusIsIdle(i2c_id))
        {
            ;
        }

        if (I2CStart(i2c_id) != I2C_SUCCESS)
        {
            //DBPRINTF("Error: Bus collision during transfer Start\n");
            return FALSE;
        }
    }

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(i2c_id);
    }

    while (!(status & I2C_START));

    return TRUE;
}

static bool Check_slave_ACK(I2C_MODULE i2c_id)
{
    bool status = false;

    switch (i2c_id)
    {
    case I2C1:
        if ((I2C1STATbits.ACKSTAT) != 1)
        {
            status = TRUE;
        }
        break;
    case I2C3:
        if ((I2C3STATbits.ACKSTAT) != 1)
        {
            status  = TRUE;
        }
        break;
    default:
        status = false;
    }


    return status;
}



static bool TransmitOneByte(I2C_MODULE i2c_id, uint8_t data)
{
    // Wait for the transmitter to be ready
    while (!I2CTransmitterIsReady(i2c_id))
    {
        ;
    }

    // Transmit the byte
    if (I2CSendByte(i2c_id, data) == I2C_MASTER_BUS_COLLISION)
    {
        //DBPRINTF("Error: I2C Master Bus Collision\n");
        return FALSE;
    }

    // Wait for the transmission to finish
    while (!I2CTransmissionHasCompleted(i2c_id))
    {
        ;
    }

    return Check_slave_ACK(i2c_id);
}

static uint8_t ReceiveOneByte(I2C_MODULE i2c_id, bool ack)
{
    uint8_t data;

    // Enable I2C receive
    I2CReceiverEnable(i2c_id, TRUE);

    // Wait until 1-byte is fully received
    while (!I2CReceivedDataIsAvailable(i2c_id))
    {
        ;
    }

    // Save the byte received
    data = I2CGetByte(i2c_id);

    // Perform acknowledgement sequence
    I2CAcknowledgeByte(i2c_id, ack);

    // Wait until acknowledgement is successfully sent
    while (!I2CAcknowledgeHasCompleted(i2c_id))
    {
        ;
    }

    return data;
}

static void StopTransfer(I2C_MODULE i2c_id)
{
    I2C_STATUS status;

    // Send the Stop signal
    I2CStop(i2c_id);

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(i2c_id);
    }

    while (!(status & I2C_STOP));
}

void i2c_write(I2C_MODULE i2c_id, uint8_t address, uint8_t *data, int len)
{
    uint8_t i2cBuffer[len + 1];
    int i;

    i2cBuffer[0] = address | 0x00;
    memcpy(&i2cBuffer[1], data, len);

    if (!StartTransfer(i2c_id, FALSE))
    {
        return;
    }
    for (i = 0; i < len + 1; i++)
    {
        if (!TransmitOneByte(i2c_id, i2cBuffer[i]))
        {
            break;
        }
    }

    StopTransfer(i2c_id);
}

ATCA_STATUS i2c_read(I2C_MODULE i2c_id, uint8_t address, uint8_t *data, uint16_t len)
{
    uint16_t i;
    ATCA_STATUS status = !ATCA_SUCCESS;

    if (!StartTransfer(i2c_id, FALSE))
    {
        return ATCA_COMM_FAIL;
    }

    if (!TransmitOneByte(i2c_id, (address | 0x01)))
    {
        StopTransfer(i2c_id);
        return ATCA_COMM_FAIL;
    }

    for (i = 0; i < len; i++)
    {
        if (i < len - 1) // send ACK
        {
            data[i] = ReceiveOneByte(i2c_id, TRUE);
        }
        else             // send NACK
        {
            data[i] = ReceiveOneByte(i2c_id, FALSE);
        }
    }

    StopTransfer(i2c_id);

    return ATCA_SUCCESS;

}
/****************************************/


/**
 * \brief
 * This HAL implementation assumes you've included the Plib libraries in your project, otherwise,
 * the HAL layer will not compile because the Plib drivers are a dependency
 */

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge.This function is currently not implemented.
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number.This function is currently not implemented.
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/**
 * \brief
 * hal_i2c_init manages requests to initialize a physical interface. It manages use counts so when an interface
 * has released the physical layer, it will disable the interface for some other use.
 * You can have multiple ATCAIFace instances using the same bus, and you can have multiple ATCAIFace instances on
 * multiple i2c buses, so hal_i2c_init manages these things and ATCAIFace is abstracted from the physical details.
 */

/**
 * \brief initialize an I2C interface using given config
 *
 * \param[in] hal - opaque ptr to HAL data
 * \param[in] cfg - interface configuration
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    if (cfg->atcai2c.bus >= MAX_I2C_BUSES)
    {
        return ATCA_COMM_FAIL;
    }
    ATCAI2CMaster_t* data = &i2c_hal_data[cfg->atcai2c.bus];

    if (data->ref_ct <= 0)
    {
        // Bus isn't being used, enable it

        switch (cfg->atcai2c.bus)
        {
//            case 0:
//                data->id = I2C0;
//                break;
        case 1:
            data->id = I2C1;
            break;
//            case 2:
//                data->id = I2C2;
//                break;
        case 3:
            data->id = I2C3;
            break;
        default:
            return ATCA_COMM_FAIL;
        }

        // Set the I2C baudrate
        I2CSetFrequency(data->id, GetPeripheralClock(), cfg->atcai2c.baud);
        // Enable the I2C bus
        I2CEnable(data->id, TRUE);
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

/**
 * \brief HAL implementation of I2C post init
 *
 * \param[in] iface  instance
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of I2C send over ASF
 *
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;

    txdata[0] = 0x03;   // insert the Word Address Value, Command token
    txlength++;         // account for word address value byte.

    i2c_write(i2c_hal_data[bus].id, cfg->atcai2c.slave_address, txdata, txlength);

    return ATCA_SUCCESS;
}

/**
 * \brief HAL implementation of I2C receive function for ASF I2C
 * \param[in]    iface     Device to interact with.
 * \param[out]   rxdata    Data received will be returned here.
 * \param[inout] rxlength  As input, the size of the rxdata buffer.
 *                         As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;
    int count;
    int retries = 10;
    int status = !ATCA_SUCCESS;
    uint16_t rxdata_max_size = *rxlength;

    *rxlength = 0;
    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }

    while (retries-- > 0 && status != ATCA_SUCCESS)
    {
        status = i2c_read(i2c_hal_data[bus].id, cfg->atcai2c.slave_address, rxdata, 1);
    }

    if (status != ATCA_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }
    if (rxdata[0] < ATCA_RSP_SIZE_MIN)
    {
        return ATCA_INVALID_SIZE;
    }
    if (rxdata[0] > rxdata_max_size)
    {
        return ATCA_SMALL_BUFFER;
    }

    count  = rxdata[0] - 1;
    status = i2c_read(i2c_hal_data[bus].id, cfg->atcai2c.slave_address, &rxdata[1], count);
    if (status != ATCA_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    *rxlength = rxdata[0];

    return ATCA_SUCCESS;
}

/**
 * \brief method to change the bus speed of I2C
 *
 * \param[in] iface  interface on which to change bus speed
 * \param[in] speed  baud rate (typically 100000 or 400000)
 */
void change_i2c_speed(ATCAIface iface, uint32_t speed)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;

    // Disable the I2C bus
    I2CEnable(i2c_hal_data[bus].id, FALSE);

    // Set the I2C baudrate
    I2CSetFrequency(i2c_hal_data[bus].id, GetPeripheralClock(), speed);

    // Enable the I2C bus
    I2CEnable(i2c_hal_data[bus].id, TRUE);
}

/**
 * \brief wake up CryptoAuth device using I2C bus
 *
 * \param[in] iface  interface to logical device to wakeup
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;
    uint32_t bdrt = cfg->atcai2c.baud;

    uint8_t data[4];

    if (bdrt != 100000)     // if not already at 100KHz, change it
    {
        change_i2c_speed(iface, 100000);
    }

    // Send 0x00 as wake pulse
    i2c_write(i2c_hal_data[bus].id, 0x00, NULL, NULL);

    atca_delay_ms(3);   // wait tWHI + tWLO which is configured based on device type and configuration structure
    //atca_delay_us(cfg->wake_delay);

    // if necessary, revert baud rate to what came in.
    if (bdrt != 100000)
    {
        change_i2c_speed(iface, cfg->atcai2c.baud);
    }

    i2c_read(i2c_hal_data[bus].id, cfg->atcai2c.slave_address, data, 4);

    return hal_check_wake(data, 4);
}

/**
 * \brief idle CryptoAuth device using I2C bus
 *
 * \param[in] iface  interface to logical device to idle
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;
    uint8_t data[4];

    data[0] = 0x02; // idle word address value

    i2c_write(i2c_hal_data[bus].id, cfg->atcai2c.slave_address, data, 1);

    return ATCA_SUCCESS;
}

/**
 * \brief sleep CryptoAuth device using I2C bus
 *
 * \param[in] iface  interface to logical device to sleep
 *
 * \return ATCA_SUCESS
 */
ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int bus = cfg->atcai2c.bus;
    uint8_t data[4];

    data[0] = 0x01; // idle word address value

    i2c_write(i2c_hal_data[bus].id, cfg->atcai2c.slave_address, data, 1);

    return ATCA_SUCCESS;
}

/**
 * \brief manages reference count on given bus and releases resource if no more refences exist
 *
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 *
 * \return ATCA_SUCESS
 */
ATCA_STATUS hal_i2c_release(void *hal_data)
{
    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    // if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0)
    {
        I2CEnable(hal->id, FALSE);
        hal->ref_ct = 0;
    }

    return ATCA_SUCCESS;
}

/** @} */