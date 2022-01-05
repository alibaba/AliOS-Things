/**
 * \file
 * \brief CryptoAuthLib Basic API methods for Lock command.
 *
 * The Lock command prevents future modifications of the Configuration zone,
 * enables configured policies for Data and OTP zones, and can render
 * individual slots read-only regardless of configuration.
 *
 * \note List of devices that support this command - ATSHA204A, ATECC108A,
 *       ATECC508A, ATECC608A. There are differences in the modes that they
 *       support. Refer to device datasheets for full details.
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

#include "atca_basic.h"
#include "atca_execution.h"

/** \brief The Lock command prevents future modifications of the Configuration
 *         and/or Data and OTP zones. If the device is so configured, then
 *         this command can be used to lock individual data slots. This
 *         command fails if the designated area is already locked.
 *
 * \param[in]  mode           Zone, and/or slot, and summary check (bit 7).
 * \param[in]  summary_crc    CRC of the config or data zones. Ignored for
 *                            slot locks or when mode bit 7 is set.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock(uint8_t mode, uint16_t summary_crc)
{
    ATCAPacket packet;
    ATCACommand ca_cmd = _gDevice->mCommands;
    ATCA_STATUS status = ATCA_GEN_FAIL;

    // build command for lock zone and send
    memset(&packet, 0, sizeof(packet));
    packet.param1 = mode;
    packet.param2 = summary_crc;

    do
    {
        if ((status = atLock(ca_cmd, &packet)) != ATCA_SUCCESS)
        {
            break;
        }

        if ((status = atca_execute_command(&packet, _gDevice)) != ATCA_SUCCESS)
        {
            break;
        }

    }
    while (0);

    return status;
}

/** \brief Unconditionally (no CRC required) lock the config zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_config_zone(void)
{
    return atcab_lock(LOCK_ZONE_NO_CRC | LOCK_ZONE_CONFIG, 0);
}

/** \brief Lock the config zone with summary CRC.
 *
 *  The CRC is calculated over the entire config zone contents. 88 bytes for
 *  ATSHA devices, 128 bytes for ATECC devices. Lock will fail if the provided
 *  CRC doesn't match the internally calculated one.
 *
 *  \param[in] summary_crc  Expected CRC over the config zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_config_zone_crc(uint16_t summary_crc)
{
    return atcab_lock(LOCK_ZONE_CONFIG, summary_crc);
}

/** \brief Unconditionally (no CRC required) lock the data zone (slots and OTP).
 *
 *	ConfigZone must be locked and DataZone must be unlocked for the zone to be successfully locked.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_data_zone(void)
{
    return atcab_lock(LOCK_ZONE_NO_CRC | LOCK_ZONE_DATA, 0);
}

/** \brief Lock the data zone (slots and OTP) with summary CRC.
 *
 *  The CRC is calculated over the concatenated contents of all the slots and
 *  OTP at the end. Private keys (KeyConfig.Private=1) are skipped. Lock will
 *  fail if the provided CRC doesn't match the internally calculated one.
 *
 *  \param[in] summary_crc  Expected CRC over the data zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_data_zone_crc(uint16_t summary_crc)
{
    return atcab_lock(LOCK_ZONE_DATA, summary_crc);
}

/** \brief Lock an individual slot in the data zone on an ATECC device. Not
 *         available for ATSHA devices. Slot must be configured to be slot
 *         lockable (KeyConfig.Lockable=1).
 *
 *  \param[in] slot  Slot to be locked in data zone.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_lock_data_slot(uint16_t slot)
{
    return atcab_lock(((uint8_t)slot << 2) | LOCK_ZONE_DATA_SLOT, 0);
}
