/**
 * \file
 *
 * \brief Multimedia Card/ Memory Card Interface related functionality declaration.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef _HAL_MCI_INCLUDED
#define _HAL_MCI_INCLUDED

#include <hpl_mci_sync.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup doc_driver_hal_mci_sync
 *
 *@{
 */

/**
 * \brief MCI descriptor structure
 */
struct mci_sync_desc {
	struct _mci_sync_device device;
};

/**
 *  \brief Initialize MCI low level driver.
 *
 *  \return Operation status.
 *  \retval 0 Success.
 *  \retval <0 Error code.
 */
int32_t mci_sync_init(struct mci_sync_desc *mci, void *hw);

/**
 *  \brief Deinitialize MCI low level driver.
 *
 *  \return Operation status.
 *  \retval 0 Success.
 *  \retval <0 Error code.
 */
int32_t mci_sync_deinit(struct mci_sync_desc *mci);

/**
 *  \brief Select a device and initialize it
 *
 *  \param[in] slot    Selected slot
 *  \param[in] clock   Maximum clock to use (Hz)
 *  \param[in] bus_width  Bus width to use (1, 4, or 8)
 *  \param[in] high_speed True, to enable high speed mode
 *  \return Operation status.
 *  \retval 0 Success.
 *  \retval <0 Error code.
 */
int32_t mci_sync_select_device(struct mci_sync_desc *mci, uint8_t slot, uint32_t clock, uint8_t bus_width,
                               bool high_speed);

/**
 *  \brief Deselect a device by an assigned slot
 *
 *  \param[in] slot    Selected slot
 *  \return Operation status.
 *  \retval 0 Success.
 *  \retval <0 Error code.
 */
int32_t mci_sync_deselect_device(struct mci_sync_desc *mci, uint8_t slot);

/**
 *  \brief Get the maximum bus width of a device
 *         by a selected slot
 *
 *  \param[in] slot    Selected slot
 *  \return Bus width.
 */
uint8_t mci_sync_get_bus_width(struct mci_sync_desc *mci, uint8_t slot);

/**
 *  \brief Get the high-speed capability of the device.
 *
 *  \return True, if the high-speed is supported.
 */
bool mci_sync_is_high_speed_capable(struct mci_sync_desc *mci);

/**
 *  \brief Send 74 clock cycles on the line.
 *   Note: It is required after card plug and before card install.
 */
void mci_sync_send_clock(struct mci_sync_desc *mci);

/**
 *  \brief Send a command on the selected slot
 *
 *  \param[in] cmd    Command definition
 *  \param[in] arg    Argument of the command
 *  \return True if success, otherwise false
 */
bool mci_sync_send_cmd(struct mci_sync_desc *mci, uint32_t cmd, uint32_t arg);

/**
 *  \brief Get 32-bits response of the last command.
 *
 *  \return 32-bits response.
 */
uint32_t mci_sync_get_response(struct mci_sync_desc *mci);

/**
 *  \brief Get 128-bits response of the last command.
 *
 *  \param[in] response   Pointer on the array to fill
 *                        with the 128-bits response.
 */
void mci_sync_get_response_128(struct mci_sync_desc *mci, uint8_t *response);

/**
 *  \brief Send an ADTC command on the selected slot.
 *         An ADTC (Addressed Data Transfer Commands)
 *         command is used for read/write access.
 *
 *  \param[in] cmd          Command definition.
 *  \param[in] arg          Argument of the command.
 *  \param[in] block_size   Block size used for the transfer.
 *  \param[in] nb_block     Total number of blocks for this transfer
 *  \param[in] access_block If true, the x_read_blocks() and x_write_blocks()
 *                          functions must be used after this function.
 *                          If false, the mci_read_word() and mci_write_word()
 *                          functions must be used after this function.
 *
 * \return True if success, otherwise false
 */
bool mci_sync_adtc_start(struct mci_sync_desc *mci, uint32_t cmd, uint32_t arg, uint16_t block_size, uint16_t nb_block,
                         bool access_block);

/**
 *  \brief Send a command to stop an ADTC command on the selected slot.
 *
 * \param[in] cmd    Command definition
 * \param[in] arg    Argument of the command
 *
 * \return True if success, otherwise false
 */
bool mci_sync_adtc_stop(struct mci_sync_desc *mci, uint32_t cmd, uint32_t arg);

/**
 *  \brief Read a word on the line.
 *
 *  \param[in] value  Pointer on a word to fill.
 *
 *  \return True if success, otherwise false
 */
bool mci_sync_read_word(struct mci_sync_desc *mci, uint32_t *value);

/**
 *  \brief Write a word on the line
 *
 *  \param[in] value  Word to send
 *
 *  \return True if success, otherwise false
 */
bool mci_sync_write_word(struct mci_sync_desc *mci, uint32_t value);

/**
 *  \brief Start a read blocks transfer on the line
 *
 *  Note: The driver will use the DMA available to speed up the transfer.
 *
 *  \param[in] dst        Pointer on the buffer to fill
 *  \param[in] nb_block   Number of block to transfer
 *
 *  \return True if started, otherwise false
 */
bool mci_sync_start_read_blocks(struct mci_sync_desc *mci, void *dst, uint16_t nb_block);

/**
 *  \brief Start a write blocks transfer on the line
 *
 *  Note: The driver will use the DMA available to speed up the transfer.
 *
 *  \param[in] src        Pointer on the buffer to send
 *  \param[in] nb_block   Number of block to transfer
 *
 *  \return True if started, otherwise false
 */
bool mci_sync_start_write_blocks(struct mci_sync_desc *mci, const void *src, uint16_t nb_block);

/**
 *  \brief Wait for the end of transfer to be initiated by the mci_start_read_blocks()
 *
 *  \return True if success, otherwise false
 */
bool mci_sync_wait_end_of_read_blocks(struct mci_sync_desc *mci);

/**
 *  \brief Wait for the end of transfer to be initiated by the mci_start_write_blocks()
 *
 *  \return True if success, otherwise false
 */
bool mci_sync_wait_end_of_write_blocks(struct mci_sync_desc *mci);

/**
 *  \brief Retrieve the current driver version
 *  \return Current driver version.
 */
uint32_t mci_sync_get_version(void);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _HAL_MCI_INCLUDED */
