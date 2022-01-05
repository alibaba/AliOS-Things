/**
 * \file
 * \brief ATCA Hardware abstraction layer for Linux using I2C.
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

#include <linux/i2c-dev.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "atca_hal.h"
#include "hal_linux_i2c_userspace.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge.This function is not implemented.
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_UNIMPLEMENTED
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in]  bus_num  logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg     pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] found   number of devices found on this bus
 * \return ATCA_UNIMPLEMENTED
 */

ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief HAL implementation of I2C init
 *
 * this implementation assumes I2C peripheral has been enabled by user. It only initialize an
 * I2C interface using given config.
 *
 *  \param[in] hal pointer to HAL specific data that is maintained by this HAL
 *  \param[in] cfg pointer to HAL specific configuration data that is used to initialize this HAL
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_init(void* hal, ATCAIfaceCfg* cfg)
{
    ATCAHAL_t *pHal = (ATCAHAL_t*)hal;
    ATCA_STATUS ret = ATCA_BAD_PARAM;

    if (!pHal || !cfg)
    {
        return ret;
    }

    if (pHal->hal_data)
    {
        ATCAI2CMaster_t * hal_data = (ATCAI2CMaster_t*)pHal->hal_data;

        // Assume the bus had already been initialized
        hal_data->ref_ct++;

        ret = ATCA_SUCCESS;
    }
    else
    {
        ATCAI2CMaster_t * hal_data = malloc(sizeof(ATCAI2CMaster_t));
        int bus = cfg->atcai2c.bus; // 0-based logical bus number

        if (hal_data)
        {
            hal_data->ref_ct = 1;  // buses are shared, this is the first instance

            (void)snprintf(hal_data->i2c_file, sizeof(hal_data->i2c_file) - 1, "/dev/i2c-%d", bus);

            pHal->hal_data = hal_data;

            ret = ATCA_SUCCESS;
        }
        else
        {
            ret = ATCA_ALLOC_FAILURE;
        }
    }

    return ret;

}

/** \brief HAL implementation of I2C post init
 * \param[in] iface  instance
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C send
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCAI2CMaster_t * hal_data = (ATCAI2CMaster_t*)atgetifacehaldat(iface);
    int f_i2c;  // I2C file descriptor

    // for this implementation of I2C with CryptoAuth chips, txdata is assumed to have ATCAPacket format

    // other device types that don't require i/o tokens on the front end of a command need a different hal_i2c_send and wire it up instead of this one
    // this covers devices such as ATSHA204A and ATECCx08A that require a word address value pre-pended to the packet
    // txdata[0] is using _reserved byte of the ATCAPacket
    txdata[0] = 0x03; // insert the Word Address Value, Command token
    txlength++;       // account for word address value byte.

    // Initiate I2C communication
    if ( (f_i2c = open(hal_data->i2c_file, O_RDWR)) < 0)
    {
        return ATCA_COMM_FAIL;
    }

    // Set Slave Address
    if (ioctl(f_i2c, I2C_SLAVE, cfg->atcai2c.slave_address >> 1) < 0)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    // Send data
    if (write(f_i2c, txdata, txlength) != txlength)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    close(f_i2c);
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C receive function
 * \param[in]    iface     Device to interact with.
 * \param[out]   rxdata    Data received will be returned here.
 * \param[inout] rxlength  As input, the size of the rxdata buffer.
 *                         As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCAI2CMaster_t * hal_data = (ATCAI2CMaster_t*)atgetifacehaldat(iface);
    int f_i2c;  // I2C file descriptor
    uint16_t count;
    uint16_t rxdata_max_size = *rxlength;

    *rxlength = 0;
    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }

    // Initiate I2C communication
    if ( (f_i2c = open(hal_data->i2c_file, O_RDWR)) < 0)
    {
        return ATCA_COMM_FAIL;
    }

    // Set Slave Address
    if (ioctl(f_i2c, I2C_SLAVE, cfg->atcai2c.slave_address >> 1) < 0)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    // Receive count
    count = 1;
    if (read(f_i2c, rxdata, count) != count)
    {
        close(f_i2c);
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

    count = rxdata[0] - 1;
    // Receive data
    if (read(f_i2c, &rxdata[1], count) != count)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    *rxlength = rxdata[0];

    close(f_i2c);
    return ATCA_SUCCESS;
}

/** \brief method to change the bus speed of I2C.This function is not used in Linux.
 * \param[in] iface  interface on which to change bus speed
 * \param[in] speed  baud rate (typically 100000 or 400000)
 */

void change_i2c_speed(ATCAIface iface, uint32_t speed)
{

}

/** \brief wake up CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to wakeup
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCAI2CMaster_t * hal_data = (ATCAI2CMaster_t*)atgetifacehaldat(iface);
    int f_i2c;  // I2C file descriptor
    uint8_t data[4];
    uint8_t dummy_byte = 0x00;

    // Initiate I2C communication
    if ( (f_i2c = open(hal_data->i2c_file, O_RDWR)) < 0)
    {
        return ATCA_COMM_FAIL;
    }

    // Send the wake by writing to an address of 0x00
    // Create wake up pulse by sending a slave address 0f 0x00.
    // This slave address is sent to device by using a dummy write command.
    if (ioctl(f_i2c, I2C_SLAVE, 0x00) < 0)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    // Dummy Write
    if (write(f_i2c, &dummy_byte, 1) < 0)
    {
        // This command will always return NACK.
        // So, the return code is being ignored.
    }

    atca_delay_us(cfg->wake_delay); // wait tWHI + tWLO which is configured based on device type and configuration structure

    // Set Slave Address
    if (ioctl(f_i2c, I2C_SLAVE, cfg->atcai2c.slave_address >> 1) < 0)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    // Receive data
    if (read(f_i2c, data, 4) != 4)
    {
        close(f_i2c);
        return ATCA_RX_NO_RESPONSE;
    }

    close(f_i2c);
    // if necessary, revert baud rate to what came in.

    return hal_check_wake(data, 4);
}

/** \brief idle CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to idle
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCAI2CMaster_t * hal_data = (ATCAI2CMaster_t*)atgetifacehaldat(iface);
    uint8_t data = 0x02; // idle word address value
    int f_i2c;           // I2C file descriptor

    // Initiate I2C communication
    if ( (f_i2c = open(hal_data->i2c_file, O_RDWR) ) < 0)
    {
        return ATCA_COMM_FAIL;
    }

    // Set Slave Address
    if (ioctl(f_i2c, I2C_SLAVE, cfg->atcai2c.slave_address >> 1) < 0)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    // Send data
    if (write(f_i2c, &data, 1) != 1)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    close(f_i2c);
    return ATCA_SUCCESS;
}

/** \brief sleep CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to sleep
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    ATCAI2CMaster_t * hal_data = (ATCAI2CMaster_t*)atgetifacehaldat(iface);
    uint8_t data = 0x01; // sleep word address value
    int f_i2c;           // I2C file descriptor

    // Initiate I2C communication
    if ( (f_i2c = open(hal_data->i2c_file, O_RDWR)) < 0)
    {
        return ATCA_COMM_FAIL;
    }

    // Set Slave Address
    if (ioctl(f_i2c, I2C_SLAVE, cfg->atcai2c.slave_address >> 1) < 0)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    // Send data
    if (write(f_i2c, &data, 1) != 1)
    {
        close(f_i2c);
        return ATCA_COMM_FAIL;
    }

    close(f_i2c);
    return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{
    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    // if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0)
    {
        free(hal);
    }

    return ATCA_SUCCESS;
}

/** @} */
