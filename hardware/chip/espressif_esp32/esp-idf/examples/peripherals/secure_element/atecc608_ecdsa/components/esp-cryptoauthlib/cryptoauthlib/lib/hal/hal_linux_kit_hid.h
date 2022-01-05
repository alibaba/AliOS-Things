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

#ifndef HAL_LINUX_KIT_HID_H_
#define HAL_LINUX_KIT_HID_H_

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

// Kit USB defines
#define HID_DEVICES_MAX     10      //! Maximum number of supported Kit USB devices
#define HID_PACKET_MAX      512     //! Maximum number of bytes for a HID send/receive packet (typically 64)

// Each device that is found will have a read handle and a write handle
typedef struct hid_device
{
    FILE *read_handle;         //! The kit USB read file handle
    FILE *write_handle;        //! The kit USB write file handle
} hid_device_t;


// A structure to hold HID information
typedef struct atcahid
{
    hid_device_t kits[HID_DEVICES_MAX];
    int8_t       num_kits_found;
} atcahid_t;

/** @} */
#endif /* HAL_LINUX_KIT_HID_H_ */
