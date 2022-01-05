/**
 * \file
 * \brief ATCA Hardware abstraction layer for Windows using kit protocol over a USB CDC device.
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

#include "atca_hal.h"
#include "kit_phy.h"
#include "hal_win_kit_cdc.h"
#include "kit_protocol.h"

#include <SetupAPI.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdio.h>


// File scope globals
atcacdc_t _gCdc;



/** \brief HAL implementation of Kit USB CDC init
 *  \param[in] hal pointer to HAL specific data that is maintained by this HAL
 *  \param[in] cfg pointer to HAL specific configuration data that is used to initialize this HAL
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_cdc_init(void* hal, ATCAIfaceCfg* cfg)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    ATCAHAL_t *phal = NULL;
    GUID *pcdc_guid = NULL;
    HDEVINFO dev_info;
    SP_DEVINFO_DATA dev_data;
    DWORD device_index = 0;
    DWORD required_size = 0;
    HKEY reg_key;
    DWORD reg_type = 0;
    TCHAR reg_data[256];
    TCHAR cdc_data[256];
    BOOL result = FALSE;
    LONG reg_result = 0;
    DCB dcb_settings;
    COMMTIMEOUTS comm_timeouts;
    int i = 0;
    int index = 0;

    // Check the input variables
    if ((hal == NULL) || (cfg == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    // Cast the hal to the ATCAHAL_t strucure
    phal = (ATCAHAL_t*)hal;

    // Initialize the _gCdc structure
    memset(&_gCdc, 0, sizeof(_gCdc));
    for (i = 0; i < CDC_DEVICES_MAX; i++)
    {
        _gCdc.kits[i].read_handle = INVALID_HANDLE_VALUE;
        _gCdc.kits[i].write_handle = INVALID_HANDLE_VALUE;
    }

    _gCdc.num_kits_found = 0;

    // Get the number of available GUIDs
    result = SetupDiClassGuidsFromName(_T("Ports"), NULL, 0, &required_size);
    if (required_size == 0)
    {
        return ATCA_COMM_FAIL;
    }

    // Allocated the memory the GUID list
    pcdc_guid = malloc(required_size * sizeof(GUID));

    // Get the number of available GUIDs
    result = SetupDiClassGuidsFromName(_T("Ports"), pcdc_guid, required_size, &required_size);
    if (result == TRUE)
    {
        //Query the devices
        dev_info = SetupDiGetClassDevs(pcdc_guid, NULL, NULL, DIGCF_PRESENT);
        if (dev_info != INVALID_HANDLE_VALUE)
        {
            // Initialize the dev_data object
            memset(&dev_data, 0, sizeof(SP_DEVINFO_DATA));
            dev_data.cbSize = sizeof(SP_DEVINFO_DATA);

            while (SetupDiEnumDeviceInfo(dev_info, device_index, &dev_data))
            {
                // Get the USB CDC friendly name
                required_size = sizeof(reg_data);
                result = SetupDiGetDeviceRegistryProperty(dev_info, &dev_data, SPDRP_FRIENDLYNAME, &reg_type,
                                                          (PBYTE)reg_data, required_size, &required_size);
                if (result == TRUE)
                {
                    // Determine if this is the correct kit USB CDC device
                    if ((_tcsstr(reg_data, _T("AT90USB"))) ||
                        (_tcsstr(reg_data, _T("XPLAINED"))) ||
                        (_tcsstr(reg_data, _T("Class ASF"))))
                    {
                        // Get the registry key for the kit USB CDC device
                        reg_key = SetupDiOpenDevRegKey(dev_info, &dev_data, DICS_FLAG_GLOBAL,
                                                       0, DIREG_DEV, KEY_QUERY_VALUE);
                        if (reg_key != INVALID_HANDLE_VALUE)
                        {
                            // Get the port used with this kit USB CDC device
                            required_size = sizeof(reg_data);
                            reg_result = RegQueryValueEx(reg_key, _T("PortName"), NULL, NULL,
                                                         (LPBYTE)reg_data, &required_size);
                            if (reg_result == ERROR_SUCCESS)
                            {
                                _sntprintf_s(cdc_data, sizeof(cdc_data) / sizeof(cdc_data[0]), _TRUNCATE, _T("\\\\.\\%s"), reg_data);

                                // Open the kit USB device for reading and writing
                                if (_gCdc.kits[index].read_handle != INVALID_HANDLE_VALUE)
                                {
                                    CloseHandle(_gCdc.kits[index].read_handle);
                                }
                                _gCdc.kits[index].read_handle  = INVALID_HANDLE_VALUE;
                                _gCdc.kits[index].write_handle = INVALID_HANDLE_VALUE;

                                _gCdc.kits[index].read_handle = CreateFile(cdc_data,
                                                                           GENERIC_READ | GENERIC_WRITE,
                                                                           0,
                                                                           NULL,
                                                                           OPEN_EXISTING,
                                                                           FILE_ATTRIBUTE_NORMAL,
                                                                           NULL);
                                _gCdc.kits[index].write_handle = _gCdc.kits[index].read_handle;

                                if (_gCdc.kits[index].read_handle != INVALID_HANDLE_VALUE)
                                {
                                    // Get the comm settings
                                    dcb_settings.DCBlength = sizeof(DCB);
                                    result = GetCommState(_gCdc.kits[index].read_handle, &dcb_settings);
                                    if (result == FALSE)
                                    {
                                        continue;
                                    }

                                    // Set the comm settings
                                    dcb_settings.BaudRate = cfg->atcauart.baud;
                                    dcb_settings.ByteSize = cfg->atcauart.wordsize;
                                    dcb_settings.StopBits = cfg->atcauart.stopbits;

                                    switch (cfg->atcauart.parity)
                                    {
                                    case 0:  dcb_settings.Parity = EVENPARITY;
                                    case 1:  dcb_settings.Parity = ODDPARITY;
                                    case 2:  dcb_settings.Parity = NOPARITY;
                                    default: dcb_settings.Parity = NOPARITY;
                                    }

                                    result = SetCommState(_gCdc.kits[index].read_handle, &dcb_settings);
                                    if (result == FALSE)
                                    {
                                        continue;
                                    }

                                    // Set the comm timeout settings
                                    comm_timeouts.ReadIntervalTimeout = 3;
                                    comm_timeouts.ReadTotalTimeoutMultiplier = 3;
                                    comm_timeouts.ReadTotalTimeoutConstant = 2;
                                    comm_timeouts.WriteTotalTimeoutMultiplier = 3;
                                    comm_timeouts.WriteTotalTimeoutConstant = 2;

                                    SetCommTimeouts(_gCdc.kits[index].read_handle, &comm_timeouts);
                                }

                                // Increment the opened kit USB device index
                                index++;
                            }

                            // Close the registry key
                            RegCloseKey(reg_key);
                        }
                    }
                }

                // Break the while loop, if the maximum number of supported
                // kit USB devices have been found
                if (index == CDC_DEVICES_MAX)
                {
                    break;
                }

                // Increment the device member index
                device_index++;
            }
        }

        // Delete device info now that we're done
        SetupDiDestroyDeviceInfoList(dev_info);
    }

    // Free allocated memory
    free(pcdc_guid);

    // Save the results of this discovery of HID
    if (index > 0)
    {
        _gCdc.num_kits_found = index;
        phal->hal_data = &_gCdc;
        return status;
    }
    return ATCA_NO_DEVICES;
}

/** \brief discover all CDC kits available.This function is currently not implemented.
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_UNIMPLEMENTED
 */

ATCA_STATUS hal_cdc_discover_buses(int i2c_buses[], int max_buses)
{
    // TODO: This should be set to the com port index(s)
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover any CryptoAuth devices on a given logical bus number.This function is currently not implemented.
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_cdc_discover_devices(int bus_num, ATCAIfaceCfg cfg[], int *found)
{
    // TODO: Add kitg protocol calls to discover all devices
    return ATCA_UNIMPLEMENTED;
}


/** \brief HAL implementation of Kit USB CDC post init
 *  \param[in] iface instance
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_cdc_post_init(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    atcacdc_t* pHalDat = atgetifacehaldat(iface);
    ATCAIfaceCfg *pCfg = atgetifacecfg(iface);
    int i = 0;

    do
    {
        // Check the pointers
        if (pHalDat == NULL || pCfg == NULL)
        {
            status = ATCA_BAD_PARAM;
            BREAK(status, "NULL pointers in hal_kit_cdc_post_init");
        }
        // Init all kit USB devices
        for (i = 0; i < pHalDat->num_kits_found; i++)
        {
            // Set the port
            pCfg->atcauart.port = i;
            // Perform the kit protocol init
            status = kit_init(iface);
            if (status != ATCA_SUCCESS)
            {
                BREAK(status, "kit_init() Failed");
            }
        }
    }
    while (0);
    return status;
}

/** \brief HAL implementation of kit protocol send .It is called by the top layer.
 *  \param[in] iface     instance
 *  \param[in] txdata    pointer to bytes to send
 *  \param[in] txlength  number of bytes to send
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_phy_send(ATCAIface iface, const char* txdata, int txlength)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int cdcid = cfg->atcauart.port;
    atcacdc_t* pCdc = (atcacdc_t*)atgetifacehaldat(iface);
    uint8_t buffer[CDC_BUFFER_MAX];
    DWORD bytes_to_send = 0;
    DWORD bytes_left = 0;
    DWORD bytes_sent = 0;
    BOOL result = FALSE;

    if ((txdata == NULL) || (pCdc == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    if (pCdc->kits[cdcid].write_handle == INVALID_HANDLE_VALUE)
    {
        return ATCA_COMM_FAIL;
    }

    bytes_left = txlength;

    while (bytes_left > 0)
    {
        memset(buffer, 0, CDC_BUFFER_MAX);

        if (bytes_left >= CDC_BUFFER_MAX)
        {
            bytes_to_send = CDC_BUFFER_MAX;
        }
        else
        {
            bytes_to_send = bytes_left;
        }

        memcpy(&buffer[0], &txdata[(txlength - bytes_left)], bytes_to_send);

        result = WriteFile(pCdc->kits[cdcid].write_handle, buffer, bytes_to_send, &bytes_sent, NULL);
        if (result == FALSE)
        {
            return ATCA_TX_FAIL;
        }

        bytes_left -= bytes_sent;
    }
    return status;
}


/** \brief HAL implementation of kit protocol receive data.It is called by the top layer.
 * \param[in]    iface   instance
 * \param[out]   rxdata  pointer to space to receive the data
 * \param[inout] rxsize  ptr to expected number of receive bytes to request
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_phy_receive(ATCAIface iface, char* rxdata, int* rxsize)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    int cdcid = cfg->atcauart.port;
    atcacdc_t* pCdc = (atcacdc_t*)atgetifacehaldat(iface);
    uint8_t buffer[CDC_BUFFER_MAX] = { 0 };
    BOOL continue_read = TRUE;
    DWORD bytes_read = 0;
    DWORD total_bytes = 0;
    BOOL result = true;
    uint8_t* location = NULL;
    int bytes_remain = 0;
    int bytes_to_copy = 0;
    int size_adjust = 0;

    // Verify the input variables
    if ((rxdata == NULL) || (rxsize == NULL) || (pCdc == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    if (pCdc->kits[cdcid].read_handle == INVALID_HANDLE_VALUE)
    {
        return ATCA_COMM_FAIL;
    }

    DWORD start_time_ms = GetTickCount();
    while (continue_read == true)
    {
        // If the CDC port disappears while trying to read, ReadFile will continue to report
        // success and return 0 bytes. Same as when we're waiting for a reply. This lets the
        // operation timeout.
        if (GetTickCount() - start_time_ms > 3000)
        {
            // Close handles to force a quicker failure.
            CloseHandle(pCdc->kits[cdcid].read_handle);
            pCdc->kits[cdcid].read_handle = INVALID_HANDLE_VALUE;
            pCdc->kits[cdcid].write_handle = INVALID_HANDLE_VALUE;
            return ATCA_COMM_FAIL;
        }

        result = ReadFile(pCdc->kits[cdcid].read_handle, buffer, CDC_BUFFER_MAX, &bytes_read, NULL);
        if (result == FALSE)
        {
            return ATCA_RX_FAIL;
        }

        // Find the location of the '\n' character in read buffer
        // todo: generalize this read...  it only applies if there is an ascii protocol with an <eom> of \n and if the <eom> exists
        location = strchr((char*)&buffer[0], '\n');
        if (location == NULL)
        {
            // Copy all of the bytes
            bytes_to_copy = bytes_read;
        }
        else
        {
            // Copy only the bytes remaining in the read buffer to the <eom>
            bytes_to_copy = (int)(location - (char*)buffer);

            // The response has been received, stop receiving more data
            continue_read = false;
        }
        // Protect rxdata from overwriting, this will have the result of truncating the returned bytes
        // Remaining space in rxdata
        //bytes_remain = (*rxsize - total_bytes);
        // Use the minimum between number of bytes read and remaining space
        //bytes_to_copy = min(bytes_remain, bytes_to_copy);

        // Copy the received data
        memcpy(&rxdata[total_bytes], &buffer[0], bytes_to_copy);
        total_bytes += bytes_to_copy - size_adjust;
    }
    *rxsize = total_bytes;
    return ATCA_SUCCESS;
}

/** \brief Number of USB CDC devices found
 *  \param[out] num_found Number of USB CDC devices found returned here
 *  \return ATCA_SUCCESS
 */
ATCA_STATUS hal_kit_phy_num_found(int8_t* num_found)
{
    *num_found = _gCdc.num_kits_found;
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of kit protocol send over USB CDC
 *  \param[in] iface instance
 *  \param[in] txdata pointer to bytes to send
 *  \param[in] txlength number of bytes to send
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_cdc_send(ATCAIface iface, uint8_t* txdata, int txlength)
{
    // Call the hal_kit_send() function that will call hal_phy_send() implemented below
    return kit_send(iface, txdata, txlength);
}

/** \brief HAL implementation of kit protocol receive over USB CDC
 * \param[in]    iface   Device to interact with.
 * \param[out]   rxdata  Data received will be returned here.
 * \param[inout] rxsize  As input, the size of the rxdata buffer.
 *                       As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_cdc_receive(ATCAIface iface, uint8_t* rxdata, uint16_t* rxsize)
{
    // Call the hal_kit_receive() function that will call hal_phy_receive() implemented below
    return kit_receive(iface, rxdata, rxsize);
}

/** \brief Call the wake for kit protocol over USB CDC
 * \param[in] iface ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_cdc_wake(ATCAIface iface)
{
    // Call the hal_kit_wake() function that will call hal_phy_send() and hal_phy_receive()
    return kit_wake(iface);
}

/** \brief Call the idle for kit protocol over USB CDC
 * \param[in] iface ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_cdc_idle(ATCAIface iface)
{
    // Call the hal_kit_idle() function that will call hal_phy_send() and hal_phy_receive()
    return kit_idle(iface);
}

/** \brief Call the sleep for kit protocol over USB CDC
 * \param[in] iface ATCAIface instance that is the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_cdc_sleep(ATCAIface iface)
{
    // Call the hal_kit_sleep() function that will call hal_phy_send() and hal_phy_receive()
    return kit_sleep(iface);
}

/** \brief Close the physical port for CDC
 * \param[in] hal_data The hardware abstraction data specific to this HAL
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_cdc_release(void* hal_data)
{
    int i = 0;
    atcacdc_t* phaldat = (atcacdc_t*)hal_data;

    if ((hal_data == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    // Close all kit USB devices
    for (i = 0; i < phaldat->num_kits_found; i++)
    {
        if (phaldat->kits[i].read_handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(phaldat->kits[i].read_handle);
            phaldat->kits[i].read_handle  = INVALID_HANDLE_VALUE;
            phaldat->kits[i].write_handle = INVALID_HANDLE_VALUE;
        }
    }
    return ATCA_SUCCESS;
}

/** \brief discover cdc buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge.This function is currently not implemented.
 * \param[in] cdc_buses - an array of logical bus numbers
 * \param[in] max_buses - maximum number of buses the app wants to attempt to discover
 * \return ATCA_UNIMPLEMENTED
 */

ATCA_STATUS hal_kit_cdc_discover_buses(int cdc_buses[], int max_buses)
{
    // TODO: Implement
    return ATCA_UNIMPLEMENTED;
}


/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in] bus_num - logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg[] - pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] *found - number of devices found on this bus
 * \return ATCA_UNIMPLEMENTED
 */
ATCA_STATUS hal_kit_cdc_discover_devices(int bus_num, ATCAIfaceCfg *cfg, int *found)
{
    // TODO: Implement
    *found = 0;
    return ATCA_UNIMPLEMENTED;
}