/**
 * \file
 * \brief ATCA Hardware abstraction layer for Linux using kit protocol over a USB HID device.
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

#include <libudev.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "atca_hal.h"
#include "hal_linux_kit_hid.h"
#include "hal/kit_protocol.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

// File scope globals
atcahid_t _gHid;

/** \brief discover hid buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge.This function is currently not implemented.
 * \param[in] cdc_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_UNIMPLEMENTED
 */

ATCA_STATUS hal_kit_hid_discover_buses(int hid_buses[], int max_buses)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number.This function is currently not implemented.
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_kit_hid_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief HAL implementation of Kit USB HID init
 *  \param[in] hal pointer to HAL specific data that is maintained by this HAL
 *  \param[in] cfg pointer to HAL specific configuration data that is used to initialize this HAL
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_init(void* hal, ATCAIfaceCfg* cfg)
{
    ATCAHAL_t *phal = (ATCAHAL_t*)hal;
    int status = 0;
    struct udev *udev = NULL;
    struct udev_enumerate *enumerate = NULL;
    struct udev_list_entry *list = NULL;
    struct udev_list_entry *list_entry = NULL;
    struct udev_device *syspath_device = NULL;
    struct udev_device *device = NULL;
    int i = 0;
    char hid_filter[20];
    char device_hid[20];
    FILE *file_descriptor = NULL;
    int index = 0;

    // Check the input variables
    if ((cfg == NULL) || (phal == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    // Initialize the _gHid structure
    memset(&_gHid, 0, sizeof(_gHid));
    for (i = 0; i < HID_DEVICES_MAX; i++)
    {
        _gHid.kits[i].read_handle = NULL;
        _gHid.kits[i].write_handle = NULL;
    }

    _gHid.num_kits_found = 0;

    // Create the udev object
    udev = udev_new();
    if (udev == NULL)
    {
        return ATCA_COMM_FAIL;
    }

    // Create the enumerate object
    enumerate = udev_enumerate_new(udev);
    if (enumerate == NULL)
    {
        // Free the udev object
        udev_unref(udev);

        return ATCA_COMM_FAIL;
    }

    // Create the list of available USB devices
    status = udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    if (status >= 0)
    {
        status = udev_enumerate_scan_devices(enumerate);
        if (status >= 0)
        {
            list = udev_enumerate_get_list_entry(enumerate);
        }
    }

    // Create the HID filter string
    memset(hid_filter, 0, sizeof(hid_filter));
    sprintf(hid_filter, "vid_%04x&pid_%04x", cfg->atcahid.vid, cfg->atcahid.pid);

    // Find the available kit USB devices
    udev_list_entry_foreach(list_entry, list)
    {
        // Get the udev device
        syspath_device = udev_device_new_from_syspath(udev, udev_list_entry_get_name(list_entry));
        device = udev_device_get_parent_with_subsystem_devtype(syspath_device, "usb", "usb_device");

        // Create the device HID string
        memset(device_hid, 0, sizeof(device_hid));
        sprintf(device_hid, "vid_%s&pid_%s",
                udev_device_get_sysattr_value(device, "idVendor"),
                udev_device_get_sysattr_value(device, "idProduct"));

        // Determine if this is the correct kit USB device
        if (strcasecmp(device_hid, hid_filter) == 0)
        {
            // Open the kit USB device for reading and writing
            if (_gHid.kits[index].read_handle != NULL)
            {
                fclose(_gHid.kits[index].read_handle);
            }
            if (_gHid.kits[index].write_handle != NULL)
            {
                fclose(_gHid.kits[index].write_handle);
            }

            file_descriptor = fopen(udev_device_get_devnode(syspath_device), "rb+");
            if (file_descriptor != NULL)
            {
                _gHid.kits[index].read_handle = file_descriptor;
                _gHid.kits[index].write_handle = file_descriptor;

                // Increment the opened kit USB device index
                index++;

#ifdef KIT_DEBUG
                printf("Kit USB Device Node: %s\n", udev_device_get_devnode(syspath_device));
                printf("  Manufacturer %s (%s)\n",
                       udev_device_get_sysattr_value(device, "manufacturer"),
                       udev_device_get_sysattr_value(device, "product"));
                printf("  VID/PID:     %s %s\n",
                       udev_device_get_sysattr_value(device, "idVendor"),
                       udev_device_get_sysattr_value(device, "idProduct"));
#endif          // KIT_DEBUG
            }
            else
            {
#ifdef KIT_DEBUG
                printf("fopen(\"%s\") failed with errno=%d\n",
                       udev_device_get_devnode(syspath_device),
                       errno);
#endif          // KIT_DEBUG
            }
        }

        // Free the udev device object
        udev_device_unref(syspath_device);
    }

    // Save the results of this discovery of HID
    if (index > 0)
    {
        _gHid.num_kits_found = index;
        phal->hal_data = &_gHid;
    }

    // Free the enumerator object
    udev_enumerate_unref(enumerate);

    // Free the udev object
    udev_unref(udev);

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of Kit HID post init
 *  \param[in] iface  instance
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_post_init(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    atcahid_t* pHid = atgetifacehaldat(iface);
    ATCAIfaceCfg *pCfg = atgetifacecfg(iface);
    int i = 0;

    if ((pHid == NULL) || (pCfg == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    // Perform the kit protocol init
    for (i = 0; i < pHid->num_kits_found; i++)
    {
        status = kit_init(iface);
        if (status != ATCA_SUCCESS)
        {
            BREAK(status, "kit_init() Failed");
        }
    }

    return status;
}

/** \brief HAL implementation of send over Kit protocol.This function is called by the top layer.
 *  \param[in] iface     instance
 *  \param[in] txdata    pointer to bytes to send
 *  \param[in] txlength  number of bytes to send
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_phy_send(ATCAIface iface, uint8_t* txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atcahid_t* pHid = (atcahid_t*)atgetifacehaldat(iface);
    size_t bytes_written = 0;

    if ((txdata == NULL) || (cfg == NULL) || (pHid == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    if (pHid->kits[cfg->atcahid.idx].write_handle == NULL)
    {
        return ATCA_COMM_FAIL;
    }

    // Send the data to the kit USB device
    if (txlength > 0)
    {
        bytes_written = fwrite(txdata, sizeof(uint8_t), txlength,
                               pHid->kits[cfg->atcahid.idx].write_handle);
        if (bytes_written != txlength)
        {
            clearerr(pHid->kits[cfg->atcahid.idx].write_handle);
            return ATCA_TX_FAIL;
        }
    }

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of kit protocol receive.This function is called by the top layer.
 * \param[in]    iface   instance
 * \param[out]   rxdata  pointer to space to receive the data
 * \param[inout] rxsize  ptr to expected number of receive bytes to request
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_phy_receive(ATCAIface iface, uint8_t* rxdata, int* rxsize)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atcahid_t* pHid = (atcahid_t*)atgetifacehaldat(iface);
    bool continue_read = true;
    size_t bytes_read = 0;
    size_t total_bytes_read = 0;

    if ((rxdata == NULL) || (rxsize == NULL) || (cfg == NULL) || (pHid == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    if (pHid->kits[cfg->atcahid.idx].read_handle == NULL)
    {
        return ATCA_COMM_FAIL;
    }

    // Receive the data from the kit USB device
    do
    {
        bytes_read = fread(&rxdata[total_bytes_read], sizeof(uint8_t), 1,
                           pHid->kits[cfg->atcahid.idx].read_handle);
        if (ferror(pHid->kits[cfg->atcahid.idx].read_handle) != 0)
        {
            clearerr(pHid->kits[cfg->atcahid.idx].read_handle);
            return ATCA_RX_FAIL;
        }

        total_bytes_read += bytes_read;

        // Check if the kit protocol message has been received
        if (strstr((char*)rxdata, "\n") != NULL)
        {
            continue_read = false;
        }
    }
    while (continue_read == true);

    // Save the total bytes read
    *rxsize = total_bytes_read;

    return ATCA_SUCCESS;
}

/** \brief Number of USB HID devices found
 *  \param[out] num_found
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_phy_num_found(int8_t* num_found)
{
    *num_found = _gHid.num_kits_found;

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of kit protocol send over USB HID
 *  \param[in] iface     instance
 *  \param[in] txdata    pointer to bytes to send
 *  \param[in] txlength  number of bytes to send
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_send(ATCAIface iface, uint8_t* txdata, int txlength)
{
    // Call the kit_send() function that will call phy_send() implemented below
    return kit_send(iface, txdata, txlength);
}

/** \brief HAL implementation of send over USB HID
 * \param[in]    iface   instance
 * \param[in]    rxdata  pointer to space to receive the data
 * \param[inout] rxsize  ptr to expected number of receive bytes to request
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_receive(ATCAIface iface, uint8_t* rxdata, uint16_t* rxsize)
{
    // Call the kit_receive() function that will call phy_receive() implemented below
    return kit_receive(iface, rxdata, rxsize);
}

/** \brief Call the wake for kit protocol over USB HID
 * \param[in] iface  ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_wake(ATCAIface iface)
{
    // Call the kit_wake() function that will call phy_send() and phy_receive()
    return kit_wake(iface);
}

/** \brief Call the idle for kit protocol over USB HID
 * \param[in] iface  ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_idle(ATCAIface iface)
{
    // Call the kit_idle() function that will call phy_send() and phy_receive()
    return kit_idle(iface);
}

/** \brief Call the sleep for kit protocol over USB HID
 * \param[in] iface  ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_sleep(ATCAIface iface)
{
    // Call the kit_sleep() function that will call phy_send() and phy_receive()
    return kit_sleep(iface);
}

/** \brief Close the physical port for HID
 * \param[in] hal_data  The hardware abstraction data specific to this HAL
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_release(void* hal_data)
{
    atcahid_t* phaldat = (atcahid_t*)hal_data;
    int i = 0;

    if (phaldat == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Close all kit USB devices
    for (i = 0; i < phaldat->num_kits_found; i++)
    {
        if (_gHid.kits[i].read_handle != NULL)
        {
            fclose(_gHid.kits[i].read_handle);
            _gHid.kits[i].read_handle = NULL;
            _gHid.kits[i].write_handle = NULL;
        }
    }

    return ATCA_SUCCESS;
}

/** @} */
