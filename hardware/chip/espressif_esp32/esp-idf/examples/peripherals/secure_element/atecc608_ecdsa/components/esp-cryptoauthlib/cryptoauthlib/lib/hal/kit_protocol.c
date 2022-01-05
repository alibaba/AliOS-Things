/**
 * \file
 *
 * \brief  Microchip Crypto Auth hardware interface object
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

#include <stdlib.h>
#include <stdio.h>
#include "atca_compiler.h"
#include "kit_phy.h"
#include "kit_protocol.h"
#include "basic/atca_helpers.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

/* Constants */
#define KIT_MAX_SCAN_COUNT      4
#define KIT_MAX_TX_BUF          32

#ifndef strnchr
// Local implementation of strnchr if it doesn't exist in the system
char * strnchr(const char * s, size_t count, int c)
{
    size_t i;

    for (i = 0; i < count; i++)
    {
        if (s[i] == c)
        {
            // Casting away const intentionally per API
            return (char*)&s[i];
        }
    }
    return NULL;
}
#endif

/** Kit Protocol is key */
const char * kit_id_from_devtype(ATCADeviceType devtype)
{
    switch (devtype)
    {
    case ATSHA204A:
        return "SHA204A";
    case ATECC108A:
        return "ECC108A";
    case ATECC508A:
        return "ECC508A";
    case ATECC608A:
        return "ECC608A";
    default:
        return "unknown";
    }
}


/** Kit interface from device */
const char * kit_interface_from_kittype(ATCAKitType kittype)
{
    switch (kittype)
    {
    case ATCA_KIT_I2C_IFACE:
        return "TWI";
    case ATCA_KIT_SWI_IFACE:
        return "SWI";
    default:
        return "unknown";
    }
}

/** \brief HAL implementation of kit protocol init.  This function calls back to the physical protocol to send the bytes
 *  \param[in] iface  instance
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_init(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    const char kit_device[] = "board:device(%02X)\n";
    const char kit_device_select[] = "%c:physical:select(%02X)\n";
    char txbuf[KIT_MAX_TX_BUF];
    int txlen;
    char rxbuf[KIT_RX_WRAP_SIZE + 4];
    int rxlen;
    char* device_match, *interface_match;
    char *dev_type, *dev_interface;
    char delim[] = " ";
    char *token; /* string token */
    int i;
    int address;

    device_match = kit_id_from_devtype(iface->mIfaceCFG->devtype);
    interface_match = kit_interface_from_kittype(iface->mIfaceCFG->atcahid.dev_interface);

    /* Iterate to find the target device */
    for (i = 0; i < KIT_MAX_SCAN_COUNT; i++)
    {
        txlen = snprintf(txbuf, sizeof(txbuf) - 2, kit_device, i);
        txbuf[sizeof(txbuf) - 1] = '\0';
        if (txlen < 0)
        {
            status = ATCA_INVALID_SIZE;
            break;
        }

        if (ATCA_SUCCESS != (status = kit_phy_send(iface, txbuf, txlen)))
        {
            break;
        }

        rxlen = sizeof(rxbuf);
        memset(rxbuf, 0, rxlen);
        if (ATCA_SUCCESS != (status = kit_phy_receive(iface, rxbuf, &rxlen)))
        {
            break;
        }

        token = rxbuf;
        dev_type = strtok_r(NULL, delim, &token);
        dev_interface = strtok_r(NULL, delim, &token);

        char * addr = strnchr(rxbuf, rxlen, '('); /* Gets the identity from the kit used for selecting the device*/
        address = 0;

        if (!addr)
        {
            status = ATCA_GEN_FAIL;
            break;
        }

        if (1 != sscanf(addr, "(%02X)", &address))
        {
            status = ATCA_GEN_FAIL;
            break;
        }

        /*Selects the first device type if both device interface and device identity is not defined*/
        if (iface->mIfaceCFG->atcahid.dev_interface == ATCA_KIT_AUTO_IFACE && iface->mIfaceCFG->atcahid.dev_identity == 0 && (strcmp(device_match, dev_type) == 0))
        {

            txlen = snprintf(txbuf, sizeof(txbuf) - 1, kit_device_select, device_match[0], address);
            txbuf[sizeof(txbuf) - 1] = '\0';
            if (txlen < 0)
            {
                status = ATCA_INVALID_SIZE;
                break;
            }

            if (ATCA_SUCCESS != (status = kit_phy_send(iface, txbuf, txlen)))
            {
                break;
            }

            rxlen = sizeof(rxbuf);
            status = kit_phy_receive(iface, rxbuf, &rxlen);
            break;


        }

        else
        {


            /*Selects the device only if the device type, device interface and device identity matches*/
            if ((strcmp(device_match, dev_type) == 0) && (iface->mIfaceCFG->atcahid.dev_identity == address) && (strcmp(interface_match, dev_interface) == 0))
            {


                txlen = snprintf(txbuf, sizeof(txbuf) - 1, kit_device_select, device_match[0], address);
                txbuf[sizeof(txbuf) - 1] = '\0';
                if (txlen < 0)
                {
                    status = ATCA_INVALID_SIZE;
                    break;
                }

                if (ATCA_SUCCESS != (status = kit_phy_send(iface, txbuf, txlen)))
                {
                    break;
                }

                rxlen = sizeof(rxbuf);
                status = kit_phy_receive(iface, rxbuf, &rxlen);
                break;
            }
        }
    }

    if ((KIT_MAX_SCAN_COUNT == i) && !status)
    {
        status = ATCA_NO_DEVICES;
    }

    return status;
}

/** \brief HAL implementation of kit protocol send.  This function calls back to the physical protocol to send the bytes
 *  \param[in] iface     instance
 *  \param[in] txdata    pointer to bytes to send
 *  \param[in] txlength  number of bytes to send
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_send(ATCAIface iface, const uint8_t* txdata, int txlength)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    int nkitbuf = txlength * 2 + KIT_TX_WRAP_SIZE;
    char* pkitbuf = NULL;
    char *target;

    // Check the pointers
    if (txdata == NULL)
    {
        return ATCA_BAD_PARAM;
    }
    // Wrap in kit protocol
    pkitbuf = malloc(nkitbuf);
    memset(pkitbuf, 0, nkitbuf);
    target = kit_id_from_devtype(iface->mIfaceCFG->devtype);
    status = kit_wrap_cmd(&txdata[1], txlength, pkitbuf, &nkitbuf, target[0]);
    if (status != ATCA_SUCCESS)
    {
        free(pkitbuf);
        return ATCA_GEN_FAIL;
    }
    // Send the bytes
    status = kit_phy_send(iface, pkitbuf, nkitbuf);

#ifdef KIT_DEBUG
    // Print the bytes
    printf("\nKit Write: %s", pkitbuf);
#endif

    // Free the bytes
    free(pkitbuf);

    return status;
}

/** \brief HAL implementation to receive bytes and unwrap from kit protocol.
 *         This function calls back to the physical protocol to receive the bytes
 * \param[in]    iface   instance
 * \param[in]    rxdata  pointer to space to receive the data
 * \param[inout] rxsize  ptr to expected number of receive bytes to request
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_receive(ATCAIface iface, uint8_t* rxdata, uint16_t* rxsize)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t kitstatus = 0;
    int nkitbuf = 0;
    int dataSize;
    char pkitbuf[256 * 2 + KIT_RX_WRAP_SIZE];

    // Check the pointers
    if ((rxdata == NULL) || (rxsize == NULL))
    {
        return ATCA_BAD_PARAM;
    }
    dataSize = *rxsize;
    *rxsize = 0;

    memset(pkitbuf, 0, sizeof(pkitbuf));

    // Receive the bytes
    nkitbuf = sizeof(pkitbuf);
    status = kit_phy_receive(iface, pkitbuf, &nkitbuf);
    if (status != ATCA_SUCCESS)
    {
        return ATCA_GEN_FAIL;
    }

#ifdef KIT_DEBUG
    // Print the bytes
    printf("Kit Read: %s\r", pkitbuf);
#endif

    // Unwrap from kit protocol
    status = kit_parse_rsp(pkitbuf, nkitbuf, &kitstatus, rxdata, &dataSize);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    *rxsize = (uint16_t)dataSize;

    return ATCA_SUCCESS;
}

/** \brief Call the wake for kit protocol
 * \param[in] iface  the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_wake(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t kitstatus = 0;
    char wake[] = "d:w()\n";
    int wakesize = sizeof(wake);
    char reply[KIT_RX_WRAP_SIZE + 4];
    int replysize = sizeof(reply);
    uint8_t rxdata[10];
    int rxsize = sizeof(rxdata);
    char *target;

    target = kit_id_from_devtype(iface->mIfaceCFG->devtype);
    wake[0] = target[0];

    // Send the bytes
    status = kit_phy_send(iface, wake, wakesize);

#ifdef KIT_DEBUG
    // Print the bytes
    printf("\nKit Write: %s", wake);
#endif

    // Receive the reply to wake "00(04...)\n"
    memset(reply, 0, replysize);
    status = kit_phy_receive(iface, reply, &replysize);
    if (status != ATCA_SUCCESS)
    {
        return ATCA_GEN_FAIL;
    }

#ifdef KIT_DEBUG
    // Print the bytes
    printf("Kit Read: %s\n", reply);
#endif

    // Unwrap from kit protocol
    memset(rxdata, 0, rxsize);
    status = kit_parse_rsp(reply, replysize, &kitstatus, rxdata, &rxsize);

    return hal_check_wake(rxdata, rxsize);
}

/** \brief Call the idle for kit protocol
 * \param[in] iface  the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_idle(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t kitstatus = 0;
    char idle[] = "d:i()\n";
    int idlesize = sizeof(idle);
    char reply[KIT_RX_WRAP_SIZE];
    int replysize = sizeof(reply);
    uint8_t rxdata[10];
    int rxsize = sizeof(rxdata);
    char *target;

    target = kit_id_from_devtype(iface->mIfaceCFG->devtype);
    idle[0] = target[0];

    // Send the bytes
    status = kit_phy_send(iface, idle, idlesize);

#ifdef KIT_DEBUG
    // Print the bytes
    printf("\nKit Write: %s", idle);
#endif

    // Receive the reply to sleep "00()\n"
    memset(reply, 0, replysize);
    status = kit_phy_receive(iface, reply, &replysize);
    if (status != ATCA_SUCCESS)
    {
        return ATCA_GEN_FAIL;
    }

#ifdef KIT_DEBUG
    // Print the bytes
    printf("Kit Read: %s\r", reply);
#endif

    // Unwrap from kit protocol
    memset(rxdata, 0, rxsize);
    status = kit_parse_rsp(reply, replysize, &kitstatus, rxdata, &rxsize);

    return status;
}

/** \brief Call the sleep for kit protocol
 * \param[in] iface  the interface object to send the bytes over
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_sleep(ATCAIface iface)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t kitstatus = 0;

    char sleep[] = "d:s()\n";
    int sleepsize = sizeof(sleep);
    char reply[KIT_RX_WRAP_SIZE];
    int replysize = sizeof(reply);
    uint8_t rxdata[10];
    int rxsize = sizeof(rxdata);
    char* target;

    target = kit_id_from_devtype(iface->mIfaceCFG->devtype);
    sleep[0] = target[0];

    // Send the bytes
    status = kit_phy_send(iface, sleep, sleepsize);

#ifdef KIT_DEBUG
    // Print the bytes
    printf("\nKit Write: %s", sleep);
#endif

    // Receive the reply to sleep "00()\n"
    memset(reply, 0, replysize);
    status = kit_phy_receive(iface, reply, &replysize);
    if (status != ATCA_SUCCESS)
    {
        return ATCA_GEN_FAIL;
    }

#ifdef KIT_DEBUG
    // Print the bytes
    printf("Kit Read: %s\r", reply);
#endif

    // Unwrap from kit protocol
    memset(rxdata, 0, rxsize);
    status = kit_parse_rsp(reply, replysize, &kitstatus, rxdata, &rxsize);

    return status;
}

/** \brief Wrap binary bytes in ascii kit protocol
 * \param[in]    txdata   Binary data to wrap.
 * \param[in]    txlen    Length of binary data in bytes.
 * \param[out]   pkitcmd  ASCII kit protocol wrapped data is return here.
 * \param[inout] nkitcmd  As input, the size of the pkitcmd buffer.
 *                        As output, the number of bytes returned in the
 *                        pkitcmd buffer.
 * \param[in]    target   Target char to use 's' for SHA devices, 'e' for ECC
                          devices.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_wrap_cmd(const uint8_t* txdata, int txlen, char* pkitcmd, int* nkitcmd, char target)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    char cmdpre[] = "d:t(";
    char cmdpost[] = ")\n";
    size_t cmdAsciiLen = txlen * 2;
    size_t cmdlen = txlen * 2 + sizeof(cmdpre) + sizeof(cmdpost) - 1;
    size_t cpylen = 0;
    size_t cpyindex = 0;

    // Check the variables
    if (txdata == NULL || pkitcmd == NULL || nkitcmd == NULL)
    {
        return ATCA_BAD_PARAM;
    }
    if (*nkitcmd < (int)cmdlen)
    {
        return ATCA_SMALL_BUFFER;
    }

    // Wrap in kit protocol
    memset(pkitcmd, 0, *nkitcmd);

    // Copy the prefix
    cpylen = strlen(cmdpre);
    memcpy(&pkitcmd[cpyindex], cmdpre, cpylen);
    cpyindex += cpylen;

    pkitcmd[0] = target;

    // Copy the ascii binary bytes
    status = atcab_bin2hex_(txdata, txlen, &pkitcmd[cpyindex], &cmdAsciiLen, false, false, true);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }
    cpyindex += cmdAsciiLen;

    // Copy the postfix
    cpylen = strlen(cmdpost);
    memcpy(&pkitcmd[cpyindex], cmdpost, cpylen);
    cpyindex += cpylen;

    *nkitcmd = (int)cpyindex;

    return status;
}

/** \brief Parse the response ascii from the kit
 * \param[out] pkitbuf pointer to ascii kit protocol data to parse
 * \param[in] nkitbuf length of the ascii kit protocol data
 * \param[in] kitstatus status of the ascii device
 * \param[in] rxdata pointer to the binary data buffer
 * \param[in] datasize size of the pointer to the binary data buffer
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS kit_parse_rsp(const char* pkitbuf, int nkitbuf, uint8_t* kitstatus, uint8_t* rxdata, int* datasize)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    int statusId = 0;
    int dataId = 3;
    size_t binSize = 1;
    size_t asciiDataSize = 0;
    size_t datasizeTemp = *datasize;
    char* endDataPtr = 0;

    // First get the kit status
    status = atcab_hex2bin(&pkitbuf[statusId], 2, kitstatus, &binSize);
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    // Next get the binary data bytes
    endDataPtr = strnchr((char*)pkitbuf, nkitbuf, ')');
    if (endDataPtr < (&pkitbuf[dataId]))
    {
        return ATCA_GEN_FAIL;
    }
    asciiDataSize = endDataPtr - (&pkitbuf[dataId]);
    status = atcab_hex2bin(&pkitbuf[dataId], asciiDataSize, rxdata, &datasizeTemp);
    *datasize = (int)datasizeTemp;
    if (status != ATCA_SUCCESS)
    {
        return status;
    }

    return ATCA_SUCCESS;
}

/** @} */
