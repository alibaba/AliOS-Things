/**
 * \file
 *
 * \brief SAM SDHC HPL
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

#include <utils_assert.h>
#include "hpl_mci_sync.h"
#include <hpl_sdhc_config.h>

#define HSMCI_SLOT_0_SIZE 4

static void _mci_reset(const void *const hw);
static void _mci_set_speed(const void *const hw, uint32_t speed, uint8_t prog_clock_mode);
static bool _mci_wait_busy(const void *const hw);
static bool _mci_send_cmd_execute(const void *const hw, uint32_t cmdr, uint32_t cmd, uint32_t arg);

/**
 * \brief Reset the SDHC interface
 *
 * \param hw The pointer to MCI hardware instance
 */
static void _mci_reset(const void *const hw)
{
	ASSERT(hw);
	hri_sdhc_set_SRR_SWRSTCMD_bit(hw);
}

/**
 * \brief Set speed of the SDHC clock.
 *
 * \param hw       The pointer to MCI hardware instance
 * \param speed    SDHC clock speed in Hz.
 * \param prog_clock_mode     Use programmable clock mode
 */
static void _mci_set_speed(const void *const hw, uint32_t speed, uint8_t prog_clock_mode)
{
	uint32_t div;
	uint32_t clkbase;
	uint32_t clkmul;

	if (hri_sdhc_get_CCR_SDCLKEN_bit(hw)) {
		while (hri_sdhc_read_PSR_reg(hw) & (SDHC_PSR_CMDINHC_CANNOT | SDHC_PSR_CMDINHD_CANNOT))
			;
		hri_sdhc_clear_CCR_SDCLKEN_bit(hw);
	}
	//	clkbase = hri_sdhc_read_CA0R_BASECLKF_bf(hw);
	clkbase = CONF_BASE_FREQUENCY;
	clkmul  = hri_sdhc_read_CA1R_CLKMULT_bf(hw);

	/* If programmable clock mode is enabled, baseclk is divided by 2 */
	if (clkmul > 0) {
		clkbase = clkbase / 2;
	}
	if (prog_clock_mode == 0) {
		/* divided clock mode */
		hri_sdhc_clear_CCR_CLKGSEL_bit(hw);
		/* speed = Base Clock / 2*div */
		div = (clkbase / speed) / 2;
	} else {
		/* programmable clock mode */
		hri_sdhc_set_CCR_CLKGSEL_bit(hw);
		/* Specific constraint for SDHC/SDMMC IP
		   speed = Base Clock * Multi Clock / (div+1) */
		div = (clkbase * (clkmul + 1)) / speed;
		if (div > 0) {
			div = div - 1;
		}
	}

	/* Specific constraint for SDHC/SDMMC IP
	The clock divider (DIV) in SDMMC_CCR must be set to a value different from 0 when HSEN is 1. */
	if ((hri_sdhc_get_HC1R_HSEN_bit(hw)) && (div == 0)) {
		div = 1;
	}

	/* Set clock divider */
	hri_sdhc_write_CCR_SDCLKFSEL_bf(hw, div & 0xFF);
	hri_sdhc_write_CCR_USDCLKFSEL_bf(hw, div >> 8);

	hri_sdhc_set_CCR_INTCLKEN_bit(hw);
	/* Repeat this step until Clock Stable is 1 */
	while (hri_sdhc_get_CCR_INTCLKS_bit(hw) == 0)
		;
	/* Output the clock to the card -- Set SD Clock Enable */
	hri_sdhc_set_CCR_SDCLKEN_bit(hw);
}

/**
 * \brief Wait the end of busy signal on data line
 *
 * \param hw       The pointer to MCI hardware instance
 * \return true if success, otherwise false
 */
static bool _mci_wait_busy(const void *const hw)
{
	uint32_t busy_wait = 0xFFFFFFFF;
	uint32_t psr;

	ASSERT(hw);

	do {
		psr = hri_sdhc_read_PSR_reg(hw);

		if (busy_wait-- == 0) {
			_mci_reset(hw);
			return false;
		}
	} while (!(psr & SDHC_PSR_DATLL(1)));
	return true;
}

/**
 * \brief Send a command
 *
 * \param hw         The pointer to MCI hardware instance
 * \param cmdr       CMDR resgister bit to use for this command
 * \param cmd        Command definition
 * \param arg        Argument of the command
 *
 * \return true if success, otherwise false
 */
static bool _mci_send_cmd_execute(const void *const hw, uint32_t cmdr, uint32_t cmd, uint32_t arg)
{
	uint32_t sr;
	ASSERT(hw);

	cmdr |= SDHC_CR_CMDIDX(cmd) | SDHC_CR_CMDTYP_NORMAL;

	if (cmd & MCI_RESP_PRESENT) {

		if (cmd & MCI_RESP_136) {
			cmdr |= SDHC_CR_RESPTYP_136_BIT;
		} else if (cmd & MCI_RESP_BUSY) {
			cmdr |= SDHC_CR_RESPTYP_48_BIT_BUSY;
		} else {
			cmdr |= SDHC_CR_RESPTYP_48_BIT;
		}
	}

	if (cmd & MCI_CMD_OPENDRAIN) {
		hri_sdhc_set_MC1R_OPD_bit(hw);
	} else {
		hri_sdhc_clear_MC1R_OPD_bit(hw);
	}

	hri_sdhc_write_ARG1R_reg(hw, arg);
	delay_us(10);
	hri_sdhc_write_CR_reg(hw, cmdr);
	delay_us(10);

	/* Wait end of command */
	do {
		sr = hri_sdhc_read_EISTR_reg(hw);

		if (cmd & MCI_RESP_CRC) {
			if (sr
			    & (SDHC_EISTR_CMDTEO | SDHC_EISTR_CMDEND | SDHC_EISTR_CMDIDX | SDHC_EISTR_DATTEO | SDHC_EISTR_DATEND
			       | SDHC_EISTR_ADMA)) {
				_mci_reset(hw);
				hri_sdhc_set_EISTR_reg(hw, SDHC_EISTR_MASK);
				return false;
			}
		} else {
			if (sr
			    & (SDHC_EISTR_CMDTEO | SDHC_EISTR_CMDEND | SDHC_EISTR_CMDIDX | SDHC_EISTR_CMDCRC | SDHC_EISTR_DATCRC
			       | SDHC_EISTR_DATTEO | SDHC_EISTR_DATEND | SDHC_EISTR_ADMA)) {
				_mci_reset(hw);
				hri_sdhc_set_EISTR_reg(hw, SDHC_EISTR_MASK);
				return false;
			}
		}
	} while (!hri_sdhc_get_NISTR_CMDC_bit(hw));
	if (!(cmdr & SDHC_CR_DPSEL_DATA)) {
		hri_sdhc_set_NISTR_CMDC_bit(hw);
	}
	if (cmd & MCI_RESP_BUSY) {
		if (!_mci_wait_busy(hw)) {
			return false;
		}
	}

	return true;
}

/**
 *  \brief Initialize MCI low level driver.
 */
int32_t _mci_sync_init(struct _mci_sync_device *const mci_dev, void *const hw)
{
	ASSERT(mci_dev && hw);

	mci_dev->hw = hw;

	hri_sdhc_set_SRR_SWRSTALL_bit(hw);
	while (hri_sdhc_get_SRR_SWRSTALL_bit(hw))
		;

	/* Set the Data Timeout Register to 2 Mega Cycles */
	hri_sdhc_write_TCR_reg(hw, SDHC_TCR_DTCVAL(0xE));

	/* Set 3v3 power supply */
	hri_sdhc_write_PCR_reg(hw, SDHC_PCR_SDBPWR_ON | SDHC_PCR_SDBVSEL_3V3);

	hri_sdhc_set_NISTER_reg(hw, SDHC_NISTER_MASK);
	hri_sdhc_set_EISTER_reg(hw, SDHC_EISTER_MASK);

	return ERR_NONE;
}

/**
 *  \brief Deinitialize MCI low level driver.
 */
int32_t _mci_sync_deinit(struct _mci_sync_device *const mci_dev)
{
	ASSERT(mci_dev);

	mci_dev->hw = NULL;
	return ERR_NONE;
}

/**
 *  \brief Select a device and initialize it
 */
int32_t _mci_sync_select_device(struct _mci_sync_device *const mci_dev, uint8_t slot, uint32_t clock, uint8_t bus_width,
                                bool high_speed)
{
	(void)(slot);
	void *hw;

	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	if (high_speed) {
		hri_sdhc_set_HC1R_HSEN_bit(hw);
	} else {
		hri_sdhc_clear_HC1R_HSEN_bit(hw);
	}

	if (hri_sdhc_get_HC2R_PVALEN_bit(hw) == 0) {
		_mci_set_speed(hw, clock, CONF_SDHC1_CLK_GEN_SEL);
	}

	switch (bus_width) {
	case 1:
		hri_sdhc_clear_HC1R_DW_bit(hw);
		break;

	case 4:
		hri_sdhc_set_HC1R_DW_bit(hw);
		break;

	default:
		return ERR_INVALID_ARG;
	}

	return ERR_NONE;
}

/**
 *  \brief Deselect a device by an assigned slot
 */
int32_t _mci_sync_deselect_device(struct _mci_sync_device *const mci_dev, uint8_t slot)
{
	/* Nothing to do */
	(void)(mci_dev);
	(void)(slot);

	return ERR_NONE;
}

/**
 *  \brief Get the maximum bus width of a device
 *         by a selected slot
 */
uint8_t _mci_sync_get_bus_width(struct _mci_sync_device *const mci_dev, uint8_t slot)
{
	(void)(mci_dev);

	switch (slot) {
	case 0:
		return HSMCI_SLOT_0_SIZE;

	default:
		/* Slot number wrong */
		return 0;
	}
}

/**
 *  \brief Get the high speed capability of the device.
 */
bool _mci_sync_is_high_speed_capable(struct _mci_sync_device *const mci_dev)
{
	return hri_sdhc_get_CA0R_HSSUP_bit(mci_dev->hw);
}

/**
 *  \brief Send 74 clock cycles on the line.
 *   Note: It is required after card plug and before card install.
 */
void _mci_sync_send_clock(struct _mci_sync_device *const mci_dev)
{
	ASSERT(mci_dev);

	volatile uint32_t i;
	for (i = 0; i < 5000; i++)
		;
}

/**
 *  \brief Send a command on the selected slot
 */
bool _mci_sync_send_cmd(struct _mci_sync_device *const mci_dev, uint32_t cmd, uint32_t arg)
{
	void *hw;
	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	/* Check Command Inhibit (CMD) in the Present State register */
	if (hri_sdhc_get_PSR_CMDINHC_bit(hw)) {
		return false;
	}

	hri_sdhc_clear_TMR_DMAEN_bit(hw);
	hri_sdhc_write_BCR_reg(hw, 0);

	return _mci_send_cmd_execute(hw, 0, cmd, arg);
}

/**
 *  \brief Get 32 bits response of the last command.
 */
uint32_t _mci_sync_get_response(struct _mci_sync_device *const mci_dev)
{
	void *hw;
	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	return hri_sdhc_read_RR_reg(hw, 0);
}

/**
 *  \brief Get 128 bits response of the last command.
 */
void _mci_sync_get_response_128(struct _mci_sync_device *const mci_dev, uint8_t *response)
{
	uint32_t response_32;
	void *   hw;
	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	for (int8_t i = 3; i >= 0; i--) {
		response_32 = hri_sdhc_read_RR_reg(hw, i);
		if (i != 3) {
			*response = (response_32 >> 24) & 0xFF;
			response++;
		}
		*response = (response_32 >> 16) & 0xFF;
		response++;
		*response = (response_32 >> 8) & 0xFF;
		response++;
		*response = (response_32 >> 0) & 0xFF;
		response++;
	}
}

/**
 *  \brief Send an ADTC command on the selected slot
 *         An ADTC (Addressed Data Transfer Commands)
 *         command is used for read/write access.
 */
bool _mci_sync_adtc_start_dma(struct _mci_sync_device *const mci_dev, uint32_t cmd, uint32_t arg, uint16_t block_size,
                          uint16_t nb_block, bool access_block, uint32_t sys_addr)
{
	uint32_t tmr;
	void *   hw;
	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	/* No use without dma support */
	(void)access_block;

	/* Check Command Inhibit (CMD/DAT) in the Present State register */
	if (hri_sdhc_get_PSR_CMDINHC_bit(hw) || hri_sdhc_get_PSR_CMDINHD_bit(hw)) {
		return false;
	}

	if (cmd & MCI_CMD_WRITE) {
		tmr = SDHC_TMR_DTDSEL_WRITE;
	} else {
		tmr = SDHC_TMR_DTDSEL_READ;
	}

	if (cmd & MCI_CMD_SDIO_BYTE) {
		tmr |= SDHC_TMR_MSBSEL_SINGLE;
	} else if (cmd & MCI_CMD_SDIO_BLOCK) {
		tmr |= SDHC_TMR_BCEN | SDHC_TMR_MSBSEL_MULTIPLE;
	} else if (cmd & MCI_CMD_SINGLE_BLOCK) {
		tmr |= SDHC_TMR_MSBSEL_SINGLE;
	} else if (cmd & MCI_CMD_MULTI_BLOCK) {
		tmr |= SDHC_TMR_BCEN | SDHC_TMR_MSBSEL_MULTIPLE;
	} else {
		return false;
	}
	tmr |= SDHC_TMR_DMAEN;
	hri_sdhc_write_SSAR_ADDR_bf(hw, sys_addr);
	hri_sdhc_write_TMR_reg(hw, tmr);
	hri_sdhc_write_BSR_reg(hw, SDHC_BSR_BLOCKSIZE(block_size) | SDHC_BSR_BOUNDARY_256K);
	hri_sdhc_write_BCR_reg(hw, SDHC_BCR_BCNT(nb_block));

	mci_dev->mci_sync_trans_pos  = 0;
	mci_dev->mci_sync_block_size = block_size;
	mci_dev->mci_sync_nb_block   = nb_block;

	return _mci_send_cmd_execute(hw, SDHC_CR_DPSEL_DATA, cmd, arg);
}

bool _mci_sync_adtc_start(struct _mci_sync_device *const mci_dev, uint32_t cmd, uint32_t arg, uint16_t block_size,
uint16_t nb_block, bool access_block)
{
	uint32_t tmr;
	void *   hw;
	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	/* No use without dma support */
	(void)access_block;

	/* Check Command Inhibit (CMD/DAT) in the Present State register */
	if (hri_sdhc_get_PSR_CMDINHC_bit(hw) || hri_sdhc_get_PSR_CMDINHD_bit(hw)) {
		return false;
	}

	if (cmd & MCI_CMD_WRITE) {
		tmr = SDHC_TMR_DTDSEL_WRITE;
		} else {
		tmr = SDHC_TMR_DTDSEL_READ;
	}

	if (cmd & MCI_CMD_SDIO_BYTE) {
		tmr |= SDHC_TMR_MSBSEL_SINGLE;
		} else if (cmd & MCI_CMD_SDIO_BLOCK) {
		tmr |= SDHC_TMR_BCEN | SDHC_TMR_MSBSEL_MULTIPLE;
		} else if (cmd & MCI_CMD_SINGLE_BLOCK) {
		tmr |= SDHC_TMR_MSBSEL_SINGLE;
		} else if (cmd & MCI_CMD_MULTI_BLOCK) {
		tmr |= SDHC_TMR_BCEN | SDHC_TMR_MSBSEL_MULTIPLE;
		} else {
		return false;
	}
	hri_sdhc_write_TMR_reg(hw, tmr);
	hri_sdhc_write_BSR_reg(hw, SDHC_BSR_BLOCKSIZE(block_size) | SDHC_BSR_BOUNDARY_4K);
	hri_sdhc_write_BCR_reg(hw, SDHC_BCR_BCNT(nb_block));

	mci_dev->mci_sync_trans_pos  = 0;
	mci_dev->mci_sync_block_size = block_size;
	mci_dev->mci_sync_nb_block   = nb_block;

	return _mci_send_cmd_execute(hw, SDHC_CR_DPSEL_DATA, cmd, arg);
}
/**
 *  \brief Send a command to stop an ADTC command on the selected slot.
 */
bool _mci_sync_adtc_stop(struct _mci_sync_device *const mci_dev, uint32_t cmd, uint32_t arg)
{
	ASSERT(mci_dev && mci_dev->hw);
	/* Nothing to do */
	(void)(cmd);
	(void)(arg);

	return true;
}

/**
 *  \brief Read a word on the line.
 */
bool _mci_sync_read_word(struct _mci_sync_device *const mci_dev, uint32_t *value)
{
	uint32_t sr;
	uint8_t  nbytes;
	void *   hw;

	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	/* Wait data available */
	nbytes = (mci_dev->mci_sync_block_size * mci_dev->mci_sync_nb_block - mci_dev->mci_sync_trans_pos < 4)
	             ? (mci_dev->mci_sync_block_size % 4)
	             : 4;

	if (mci_dev->mci_sync_trans_pos % mci_dev->mci_sync_block_size == 0) {
		do {
			sr = hri_sdhc_read_EISTR_reg(hw);

			if (sr & (SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND)) {
				_mci_reset(hw);
				return false;
			}
		} while (!hri_sdhc_get_NISTR_BRDRDY_bit(hw));
		hri_sdhc_set_NISTR_BRDRDY_bit(hw);
	}

	/* Read data */
	if (nbytes == 4) {
		*value = hri_sdhc_read_BDPR_reg(hw);
	} else {
		sr = hri_sdhc_read_BDPR_reg(hw);
		switch (nbytes) {
		case 3:
			value[2] = sr >> 16 & 0xFF;
		case 2:
			value[1] = sr >> 8 & 0xFF;
		case 1:
			value[0] = sr & 0xFF;
			break;
		}
	}
	mci_dev->mci_sync_trans_pos += nbytes;

	if (((uint64_t)mci_dev->mci_sync_block_size * mci_dev->mci_sync_nb_block) > mci_dev->mci_sync_trans_pos) {
		return true;
	}

	/* Wait end of transfer */
	do {
		sr = hri_sdhc_read_EISTR_reg(hw);

		if (sr & (SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND)) {
			_mci_reset(hw);
			return false;
		}
	} while (!hri_sdhc_get_NISTR_TRFC_bit(hw));
	hri_sdhc_set_NISTR_TRFC_bit(hw);
	return true;
}

/**
 *  \brief Write a word on the line
 */
bool _mci_sync_write_word(struct _mci_sync_device *const mci_dev, uint32_t value)
{
	uint32_t sr;
	uint8_t  nbytes;
	void *   hw;

	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	/* Wait data available */
	nbytes = 4; //( mci_dev->mci_sync_block_size & 0x3 ) ? 1 : 4;
	if (mci_dev->mci_sync_trans_pos % mci_dev->mci_sync_block_size == 0) {
		do {
			sr = hri_sdhc_read_EISTR_reg(hw);

			if (sr & (SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND)) {
				_mci_reset(hw);
				return false;
			}
		} while (!hri_sdhc_get_NISTR_BWRRDY_bit(hw));
		hri_sdhc_set_NISTR_BWRRDY_bit(hw);
	}
	/* Write data */
	hri_sdhc_write_BDPR_reg(hw, value);
	mci_dev->mci_sync_trans_pos += nbytes;

	if (((uint64_t)mci_dev->mci_sync_block_size * mci_dev->mci_sync_nb_block) > mci_dev->mci_sync_trans_pos) {
		return true;
	}

	/* Wait end of transfer */
	do {
		sr = hri_sdhc_read_EISTR_reg(hw);

		if (sr & (SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND)) {
			_mci_reset(hw);
			return false;
		}
	} while (!hri_sdhc_get_NISTR_TRFC_bit(hw));
	hri_sdhc_set_NISTR_TRFC_bit(hw);
	return true;
}

/**
 *  \brief Start a read blocks transfer on the line
 *  Note: The driver will use the DMA available to speed up the transfer.
 */
bool _mci_sync_start_read_blocks(struct _mci_sync_device *const mci_dev, void *dst, uint16_t nb_block)
{
	uint32_t nb_data;
	uint8_t *ptr    = (uint8_t *)dst;
	uint8_t  nbytes = 4;

	ASSERT(mci_dev && mci_dev->hw);
	ASSERT(nb_block);
	ASSERT(dst);

	nb_data = nb_block * mci_dev->mci_sync_block_size;

	while (nb_data) {
		_mci_sync_read_word(mci_dev, (uint32_t *)ptr);
		if (nb_data < nbytes) {
			nbytes = mci_dev->mci_sync_block_size % nbytes;
		}
		nb_data -= nbytes;
		ptr += nbytes;
	}

	return true;
}

bool _mci_sync_start_read_blocks_dma(struct _mci_sync_device *const mci_dev, void *dst, uint16_t nb_block)
{
	void *   hw;
	uint32_t sr;
	ASSERT(mci_dev && mci_dev->hw);
	ASSERT(nb_block);
	ASSERT(dst);
	ASSERT(mci_dev && mci_dev->hw);
	hw = mci_dev->hw;

	/* Wait end of transfer */
	do {
		sr = hri_sdhc_read_EISTR_reg(hw);

		if (sr & (SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND)) {
			_mci_reset(hw);
			return false;
		}
	} while (!hri_sdhc_get_NISTR_TRFC_bit(hw) && !hri_sdhc_get_NISTR_DMAINT_bit(hw));
	if(hri_sdhc_get_NISTR_TRFC_bit(hw))
	{
		hri_sdhc_set_NISTR_TRFC_bit(hw);
		hri_sdhc_set_NISTR_DMAINT_bit(hw);
	}
	if(hri_sdhc_get_NISTR_DMAINT_bit(hw))
	{
		hri_sdhc_set_NISTR_DMAINT_bit(hw);
	}

	hri_sdhc_clear_TMR_DMAEN_bit(hw);
	return true;
}

/**
 *  \brief Start a write blocks transfer on the line
 *  Note: The driver will use the DMA available to speed up the transfer.
 */
bool _mci_sync_start_write_blocks(struct _mci_sync_device *const mci_dev, const void *src, uint16_t nb_block)
{
	uint32_t nb_data;
	uint8_t *ptr    = (uint8_t *)src;
	uint8_t  nbytes = 4;

	ASSERT(mci_dev && mci_dev->hw);
	ASSERT(nb_block);
	ASSERT(src);

	nb_data = nb_block * mci_dev->mci_sync_block_size;

	while (nb_data) {
		_mci_sync_write_word(mci_dev, *(uint32_t *)ptr);
		if (nb_data < nbytes) {
			nbytes = mci_dev->mci_sync_block_size % nbytes;
		}
		nb_data -= nbytes;
		ptr += nbytes;
	}

	return true;
}

bool _mci_sync_start_write_blocks_dma(struct _mci_sync_device *const mci_dev, const void *src, uint16_t nb_block)
{
	uint32_t sr;
	void *   hw;

	ASSERT(mci_dev && mci_dev->hw);
	ASSERT(nb_block);
	ASSERT(src);
	ASSERT(mci_dev && mci_dev->hw);

	hw = mci_dev->hw;

	/* Wait end of transfer */
	do {
		sr = hri_sdhc_read_EISTR_reg(hw);

		if (sr & (SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND)) {
			_mci_reset(hw);
			return false;
		}
	}  while (!hri_sdhc_get_NISTR_TRFC_bit(hw) && !hri_sdhc_get_NISTR_DMAINT_bit(hw));
	if(hri_sdhc_get_NISTR_TRFC_bit(hw))
	{
		hri_sdhc_set_NISTR_TRFC_bit(hw);
		hri_sdhc_set_NISTR_DMAINT_bit(hw);
	}
	if(hri_sdhc_get_NISTR_DMAINT_bit(hw))
	hri_sdhc_set_NISTR_DMAINT_bit(hw);

	hri_sdhc_clear_TMR_DMAEN_bit(hw);

	return true;
}

/**
 *  \brief Wait the end of transfer initiated by mci_start_read_blocks()
 */
bool _mci_sync_wait_end_of_read_blocks(struct _mci_sync_device *const mci_dev)
{
	ASSERT(mci_dev && mci_dev->hw);

	/* Always return true for sync read blocks */
	return true;
}

/**
 *  \brief Wait the end of transfer initiated by mci_start_write_blocks()
 */
bool _mci_sync_wait_end_of_write_blocks(struct _mci_sync_device *const mci_dev)
{
	ASSERT(mci_dev && mci_dev->hw);

	/* Always return true for sync write blocks */
	return true;
}
