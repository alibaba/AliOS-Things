/**
 * \file
 * \brief ATCA Hardware abstraction layer for SAM4S I2C over ASF drivers.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the ASF I2C primitives to set up the interface.
 *
 * Prerequisite: add "TWI - Two-Wire Interface (Common API) (service)" module to application in Atmel Studio
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
#include "hal_sam4s_i2c_asf.h"
#include "atca_device.h"
#include "atca_execution.h"



//!< Uncomment when debugging
/*#define DEBUG_HAL*/

/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using I2C driver of ASF.
 *
   @{ */

/**
 * \brief
 * Logical to physical bus mapping structure
 */
static ATCAI2CMaster_t i2c_hal_data[MAX_I2C_BUSES];   // map logical, 0-based bus number to index
static twi_master_options_t opt_twi_master;


#ifdef DEBUG_HAL
static void print_array(uint8_t *data, uint32_t data_size)
{
    uint32_t n;

    for (n = 0; n < data_size; n++)
    {
        printf("%.2x ", data[n]);
        if (((n + 1) % 16) == 0)
        {
            printf("\r\n");
            if ((n + 1) != data_size)
            {
                printf("         ");
            }
        }
    }
    if (data_size % 16 != 0)
    {
        printf("\r\n");
    }
}
#endif

/**
 * \brief
 * This HAL implementation assumes you've included the ASF TWI libraries in your project, otherwise,
 * the HAL layer will not compile because the ASF TWI drivers are a dependency
 */

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    /* logical bus numbers 0-2 map to the SAM4S i2c buses:
       ID_TWI0
       ID_TWI1

        TWS0 bus will respond on the SAM4S Xplained Pro board

     */

    i2c_buses[0] = 0;
#if MAX_I2C_BUSES == 2
    i2c_buses[1] = -1;
#endif
    return ATCA_SUCCESS;

}

/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 * \return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    ATCAIfaceCfg *head = cfg;
    uint8_t slaveAddress = 0x01;
    ATCADevice device;

#ifdef ATCA_NO_HEAP
    struct atca_device disc_device;
    struct atca_command disc_command;
    struct atca_iface disc_iface;
#endif
    ATCAPacket packet;
    ATCA_STATUS status;
    uint8_t revs608[][4] = { { 0x00, 0x00, 0x60, 0x01 }, { 0x00, 0x00, 0x60, 0x02 } };
    uint8_t revs508[][4] = { { 0x00, 0x00, 0x50, 0x00 } };
    uint8_t revs108[][4] = { { 0x80, 0x00, 0x10, 0x01 } };
    uint8_t revs204[][4] = { { 0x00, 0x02, 0x00, 0x08 }, { 0x00, 0x02, 0x00, 0x09 }, { 0x00, 0x04, 0x05, 0x00 } };
    int i;

    /** \brief default configuration, to be reused during discovery process */
    ATCAIfaceCfg discoverCfg = {
        .iface_type             = ATCA_I2C_IFACE,
        .devtype                = ATECC508A,
        .atcai2c.slave_address  = 0x07,
        .atcai2c.bus            = bus_num,
        .atcai2c.baud           = 400000,
        .wake_delay             = 800,
        .rx_retries             = 3
    };

    if (bus_num < 0)
    {
        return ATCA_COMM_FAIL;
    }

#ifdef ATCA_NO_HEAP
    disc_device.mCommands = &disc_command;
    disc_device.mIface    = &disc_iface;
    status = initATCADevice(&discoverCfg, &disc_device);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }
    device = &disc_device;
#else
    device = newATCADevice(&discoverCfg);
    if (device == NULL)
    {
        return ATCA_COMM_FAIL;
    }
#endif

    // iterate through all addresses on given i2c bus
    // all valid 7-bit addresses go from 0x07 to 0x78
    for (slaveAddress = 0x07; slaveAddress <= 0x78; slaveAddress++)
    {
        discoverCfg.atcai2c.slave_address = slaveAddress << 1;  // turn it into an 8-bit address which is what the rest of the i2c HAL is expecting when a packet is sent

        memset(packet.data, 0x00, sizeof(packet.data));
        // build an info command
        packet.param1 = INFO_MODE_REVISION;
        packet.param2 = 0;
        // get devrev info and set device type accordingly
        atInfo(device->mCommands, &packet);
        if ((status = atca_execute_command(&packet, device)) != ATCA_SUCCESS)
        {
            continue;
        }

        // determine device type from common info and dev rev response byte strings... start with unknown
        discoverCfg.devtype = ATCA_DEV_UNKNOWN;
        for (i = 0; i < (int)sizeof(revs608) / 4; i++)
        {
            if (memcmp(&packet.data[1], &revs608[i], 4) == 0)
            {
                discoverCfg.devtype = ATECC608A;
                break;
            }
        }

        for (i = 0; i < (int)sizeof(revs508) / 4; i++)
        {
            if (memcmp(&packet.data[1], &revs508[i], 4) == 0)
            {
                discoverCfg.devtype = ATECC508A;
                break;
            }
        }

        for (i = 0; i < (int)sizeof(revs204) / 4; i++)
        {
            if (memcmp(&packet.data[1], &revs204[i], 4) == 0)
            {
                discoverCfg.devtype = ATSHA204A;
                break;
            }
        }

        for (i = 0; i < (int)sizeof(revs108) / 4; i++)
        {
            if (memcmp(&packet.data[1], &revs108[i], 4) == 0)
            {
                discoverCfg.devtype = ATECC108A;
                break;
            }
        }

        if (discoverCfg.devtype != ATCA_DEV_UNKNOWN)
        {
            // now the device type is known, so update the caller's cfg array element with it
            (*found)++;
            memcpy( (uint8_t*)head, (uint8_t*)&discoverCfg, sizeof(ATCAIfaceCfg));
            head->devtype = discoverCfg.devtype;
            head++;
        }

        atca_delay_ms(15);
    }

#ifdef ATCA_NO_HEAP
    releaseATCADevice(device);
#else
    deleteATCADevice(&device);
#endif

    return ATCA_SUCCESS;
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
        case 0:
            data->twi_id = ID_TWI0;
            data->twi_master_instance = TWI0;
            break;
        case 1:
            data->twi_id = ID_TWI1;
            data->twi_master_instance = TWI1;
            // configure TWI1 pins
            gpio_configure_pin(PIO_PB4_IDX, (PIO_PERIPH_A | PIO_PULLUP));
            gpio_configure_pin(PIO_PB5_IDX, (PIO_PERIPH_A | PIO_PULLUP));
            // disable JTAG
            MATRIX->CCFG_SYSIO |= (1 << 4) | (1 << 5);
            break;
        default:
            return ATCA_COMM_FAIL;
        }

        pmc_enable_periph_clk(data->twi_id);
        opt_twi_master.master_clk = sysclk_get_cpu_hz();
        opt_twi_master.speed = cfg->atcai2c.baud;
        opt_twi_master.smbus = 0;
        twi_master_init(data->twi_master_instance, &opt_twi_master);

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
#ifdef DEBUG_HAL
    printf("hal_i2c_send()\r\n");

    printf("\r\nCommand Packet (size:0x%.8x)\r\n", (uint32_t)txlength);
    printf("Count  : %.2x\r\n", txdata[1]);
    printf("Opcode : %.2x\r\n", txdata[2]);
    printf("Param1 : %.2x\r\n", txdata[3]);
    printf("Param2 : "); print_array(&txdata[4], 2);
    if (txdata[1] > 7)
    {
        printf("Data   : "); print_array(&txdata[6], txdata[1] - 7);
    }
    printf("CRC    : "); print_array(&txdata[txdata[1] - 1], 2);
    printf("\r\n");
#endif

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);

    txdata[0] = 0x03;   // insert the Word Address Value, Command token
    txlength++;         // account for word address value byte.

    twi_package_t packet = {
        .chip           = cfg->atcai2c.slave_address >> 1,
        .addr           = { 0 },
        .addr_length    = 0,
        .buffer         = (void*)txdata,
        .length         = (uint32_t)txlength //(uint32_t)txdata[1]
    };

    // for this implementation of I2C with CryptoAuth chips, txdata is assumed to have ATCAPacket format

    // other device types that don't require i/o tokens on the front end of a command need a different
    // hal_i2c_send and wire it up instead of this one
    //this covers devices such as ATSHA204A and ATECCx08A that require a word address value pre-pended to the packet
    if (twi_master_write(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &packet) != TWI_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

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
#ifdef DEBUG_HAL
    printf("hal_i2c_receive()\r\n");
#endif

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int retries = cfg->rx_retries;
    uint32_t status = !ATCA_SUCCESS;
    uint16_t rxdata_max_size = *rxlength;

    twi_package_t packet = {
        .chip           = cfg->atcai2c.slave_address >> 1,
        .addr           = { 0 },
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

#ifdef DEBUG_HAL
    printf("\r\nResponse Packet (size:0x%.4x)\r\n", rxlength);
    printf("Count  : %.2x\r\n", rxdata[0]);
    if (rxdata[0] > 3)
    {
        printf("Data   : "); print_array(&rxdata[1], rxdata[0] - 3);
        printf("CRC    : "); print_array(&rxdata[rxdata[0] - 2], 2);
    }
    printf("\r\n");
#endif

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

    pmc_enable_periph_clk(i2c_hal_data[cfg->atcai2c.bus].twi_id);
    opt_twi_master.master_clk = sysclk_get_cpu_hz();
    opt_twi_master.speed      = speed;
    opt_twi_master.smbus      = 0;
    twi_master_init(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, &opt_twi_master);
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
    int retries = cfg->rx_retries;
    uint32_t bdrt = cfg->atcai2c.baud;
    int status = !TWI_SUCCESS;
    uint8_t data[4];

    // if not already at 100kHz, change it
    if (bdrt != 100000)
    {
        change_i2c_speed(iface, 100000);
    }

    // Send 0x00 as wake pulse
    twi_write_byte(i2c_hal_data[cfg->atcai2c.bus].twi_master_instance, 0x00);

    // rounded up to the nearest ms
    atca_delay_ms(((uint32_t)cfg->wake_delay + (1000 - 1)) / 1000);   // wait tWHI + tWLO which is configured based on device type and configuration structure

    twi_package_t packet = {
        .chip           = cfg->atcai2c.slave_address >> 1,
        .addr           = { 0 },
        .addr_length    = 0,
        .buffer         = (void*)data,
        .length         = 4
    };

    // if necessary, revert baud rate to what came in.
    if (bdrt != 100000)
    {
        change_i2c_speed(iface, bdrt);
    }

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

/**
 * \brief idle CryptoAuth device using I2C bus
 *
 * \param[in] iface  interface to logical device to idle
 *
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

/**
 * \brief sleep CryptoAuth device using I2C bus
 *
 * \param[in] iface  interface to logical device to sleep
 *
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

/**
 * \brief manages reference count on given bus and releases resource if no more refences exist
 *
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 *
 * \return ATCA_SUCCESS
 */
ATCA_STATUS hal_i2c_release(void *hal_data)
{
    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    // if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0)
    {
        twi_reset(hal->twi_master_instance);
        hal->ref_ct = 0;
    }

    return ATCA_SUCCESS;
}

/** @} */
