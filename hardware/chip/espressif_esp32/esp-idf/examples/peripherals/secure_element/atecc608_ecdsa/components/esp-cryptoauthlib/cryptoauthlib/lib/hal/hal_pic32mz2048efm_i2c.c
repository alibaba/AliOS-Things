/**
 * \file
 * \brief ATCA Hardware abstraction layer for PIC32MZ2048
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "atca_hal.h"
#include "atca_device.h"
#include "hal/hal_pic32mz2048efm_i2c.h"
#include "driver/i2c/drv_i2c.h"
#include "system_definitions.h"
#include "driver/i2c/src/drv_i2c_local.h"

DRV_HANDLE drvI2CMasterHandle;         //Returned from DRV_I2C_Open for I2C Master
DRV_HANDLE drvI2CMasterHandle1;        //Returned from DRV_I2C_Open for I2C Master
DRV_I2C_BUFFER_HANDLE write_bufHandle; //Returned from calling a Data Transfer function
DRV_I2C_BUFFER_HANDLE read_bufHandle;  //Returned from calling a Data Transfer function

/**
 * \brief
 * Logical to physical bus mapping structure
 */
static ATCAI2CMaster_t i2c_hal_data[MAX_I2C_BUSES];   // map logical, 0-based bus number to index

/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in]  bus_num  logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg     pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] found   number of devices found on this bus
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses  an array of logical bus numbers
 * \param[in] max_buses  maximum number of buses the app wants to attempt to discover
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    i2c_buses[0] = 0;
    return ATCA_SUCCESS;
}

/** \brief initialize an I2C interface using given config
 * \param[in] hal  opaque ptr to HAL data
 * \param[in] cfg  interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    DRV_I2C_Object *tempI2cObj;

    if (cfg->atcai2c.bus >= MAX_I2C_BUSES)
    {
        return ATCA_COMM_FAIL;
    }
    ATCAI2CMaster_t* data = &i2c_hal_data[cfg->atcai2c.bus];

    if (data->ref_ct <= 0)
    {
        // Bus isn't being used, enable it
        tempI2cObj = (DRV_I2C_Object*)(cfg->cfg_data);
        drvI2CMasterHandle = DRV_I2C_Open(tempI2cObj->i2cDriverInstanceIndex, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE);
        if (drvI2CMasterHandle == DRV_HANDLE_INVALID)
        {
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
 * \return ATCA_SUCCESS on success, otherwise an error code.
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
uint32_t Debug_count = 0;
uint32_t bytes_transferred = 0;
ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    DRV_I2C_BUFFER_EVENT Transaction;

    // for this implementation of I2C with CryptoAuth chips, txdata is assumed to have ATCAPacket format

    // other device types that don't require i/o tokens on the front end of a command need a different hal_i2c_send and wire it up instead of this one
    // this covers devices such as ATSHA204A and ATECCx08A that require a word address value pre-pended to the packet
    // txdata[0] is using _reserved byte of the ATCAPacket

    txdata[0] = 0x03;   // insert the Word Address Value, Command token
    txlength++;         // account for word address value byte.
    uint8_t tx = cfg->atcai2c.slave_address;
    write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle, cfg->atcai2c.slave_address, txdata, txlength, NULL);
    if ( ((DRV_I2C_BUFFER_HANDLE)(-1)) == write_bufHandle)
    {
        while (1)
        {
            ;
        }
    }
    do
    {
        Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle, write_bufHandle);

        if (Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE)
        {
            Debug_count++;
            bytes_transferred = DRV_I2C_BytesTransferred(drvI2CMasterHandle, write_bufHandle);
        }

        if (Transaction == DRV_I2C_BUFFER_EVENT_ERROR)
        {
            return ATCA_COMM_FAIL;
        }
    }
    while (Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE);

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
    DRV_I2C_BUFFER_EVENT Transaction = DRV_I2C_BUFFER_EVENT_ERROR;
    uint16_t count;
    uint16_t rxdata_max_size = *rxlength;

    *rxlength = 0;
    if (rxdata_max_size < 1)
    {
        return ATCA_SMALL_BUFFER;
    }

    read_bufHandle = DRV_I2C_Receive(drvI2CMasterHandle, cfg->atcai2c.slave_address, rxdata, 1, NULL);
    do
    {
        Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle, read_bufHandle);
        if (Transaction == DRV_I2C_BUFFER_EVENT_ERROR)
        {
            return ATCA_COMM_FAIL;
        }
    }
    while (Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE && Transaction != DRV_I2C_BUFFER_EVENT_ERROR);

    if (rxdata[0] < ATCA_RSP_SIZE_MIN)
    {
        return ATCA_INVALID_SIZE;
    }
    if (rxdata[0] > rxdata_max_size)
    {
        return ATCA_SMALL_BUFFER;
    }

    count = rxdata[0] - 1;
    read_bufHandle = DRV_I2C_Receive(drvI2CMasterHandle, cfg->atcai2c.slave_address, &rxdata[1], count, NULL);
    do
    {
        Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle, read_bufHandle);
        if (Transaction == DRV_I2C_BUFFER_EVENT_ERROR)
        {
            return ATCA_COMM_FAIL;
        }
    }
    while (Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE && Transaction != DRV_I2C_BUFFER_EVENT_ERROR);

    *rxlength = rxdata[0];

    return ATCA_SUCCESS;
}

/** \brief wake up CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to wakeup
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    DRV_I2C_Object *tempI2cObj;
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    //int bus = cfg->atcai2c.bus;
    //int retries = cfg->rx_retries;
    uint32_t bdrt = cfg->atcai2c.baud;
    uint8_t data[4];
    DRV_I2C_BUFFER_EVENT Rx_Transaction = DRV_I2C_BUFFER_EVENT_ERROR;

    data[0] = 0x00;

    if (bdrt != 100000)   // if not already at 100KHz, change it
    {
        uint32_t backupBaudRate;
        DRV_I2C_INIT *tempi2cDriverInit;

        DRV_I2C_Close(drvI2CMasterHandle);

        /* DeInitialize Drivers */
        tempI2cObj = (DRV_I2C_Object*)(cfg->cfg_data);
        DRV_I2C_Deinitialize(tempI2cObj->i2cDriverInstance);

        /* Re-Initialize Drivers for ECC508A sleep configuration - I2C speed at 100KHZ */
        tempi2cDriverInit = (DRV_I2C_INIT*)tempI2cObj->i2cDriverInit;
        backupBaudRate = tempi2cDriverInit->baudRate;
        tempi2cDriverInit->baudRate = 100000;

        tempI2cObj->i2cDriverInstance = DRV_I2C_Initialize(tempI2cObj->i2cDriverInstanceIndex, (SYS_MODULE_INIT*)tempi2cDriverInit);
        drvI2CMasterHandle1 = DRV_I2C_Open(tempI2cObj->i2cDriverInstanceIndex, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE);
        write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle1, cfg->atcai2c.slave_address, &data, 1, NULL);
        if ( ((DRV_I2C_BUFFER_HANDLE)(-1)) == write_bufHandle)
        {
            while (1)
            {
                ;
            }
        }

        atca_delay_us(cfg->wake_delay); // wait tWHI + tWLO which is configured based on device type and configuration structure
        read_bufHandle = DRV_I2C_Receive(drvI2CMasterHandle1, cfg->atcai2c.slave_address, &data, 4, NULL);
        do
        {
            Rx_Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle1, read_bufHandle);
        }
        while (Rx_Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE && Rx_Transaction != DRV_I2C_BUFFER_EVENT_ERROR);

        DRV_I2C_Close(drvI2CMasterHandle1);

        /* DeInitialize Drivers */
        DRV_I2C_Deinitialize(tempI2cObj->i2cDriverInstance);

        /* Re-Initialize Drivers back for ECC508A default configuration*/
        tempi2cDriverInit->baudRate = backupBaudRate;
        tempI2cObj->i2cDriverInstance = DRV_I2C_Initialize(tempI2cObj->i2cDriverInstanceIndex, (SYS_MODULE_INIT*)tempi2cDriverInit);
        drvI2CMasterHandle = DRV_I2C_Open(tempI2cObj->i2cDriverInstanceIndex, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE);
    }
    else
    {
        DRV_I2C_BUFFER_EVENT statusevent;
        // Send the wake by writing to an address of 0x00
        write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle, cfg->atcai2c.slave_address, &data, 1, NULL);
        if ( ((DRV_I2C_BUFFER_HANDLE)(-1)) == write_bufHandle)
        {
            while (1)
            {
                ;
            }
        }
        atca_delay_us(cfg->wake_delay); // wait tWHI + tWLO which is configured based on device type and configuration structure
        read_bufHandle = DRV_I2C_Receive(drvI2CMasterHandle, cfg->atcai2c.slave_address, &data, 4, NULL);
        do
        {
            Rx_Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle, read_bufHandle);
        }
        while (Rx_Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE && Rx_Transaction != DRV_I2C_BUFFER_EVENT_ERROR);
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
    uint8_t data[1];

    data[0] = 0x02;  // idle word address value
    write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle, cfg->atcai2c.slave_address, &data, 1, NULL);

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

    data[0] = 0x01;  // sleep word address value

    write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle, cfg->atcai2c.slave_address, &data, 1, NULL);

    return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data  opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{

    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    // if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0)
    {
        DRV_I2C_Close(drvI2CMasterHandle);
        hal->ref_ct = 0;
    }

    return ATCA_SUCCESS;

}
