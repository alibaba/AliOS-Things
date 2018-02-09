/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YUNOS_SPI_CTRL_H
#define YUNOS_SPI_CTRL_H

#include <spi.h>
#include <board.h>

/**
 * Select the software spi slave.
 * @param[in]   id          the id of spi
 * @param[in]   selected    slave select
 * @return      Returns OK or NO-ZERO
 */
int yunos_bsp_sspi_select(uint32_t id, int selected);

/**
 * Set the SPI frequency.
 * @param[in]   id          the id of spi
 * @param[in]   frequency   the SPI frequency requested
 * @return      Returns OK or NO-ZERO
 */
int yunos_bsp_sspi_set_frequency(uint32_t id, uint32_t freq);

/**
 * Set the SPI mode.  see enum spi_mode_e for mode definitions
 * @param[in]   id          the id of spi
 * @param[in]   mode        the SPI mode requested
 * @return      Returns OK or NO-ZERO
 */
int yunos_bsp_sspi_set_mode(uint32_t id, enum spi_mode_e mode);

/**
 * Set the number of bits per word.
 * @param[in]   id          the id of spi
 * @param[in]   nbits       the number of bits requested
 * @return      Returns OK or NO_ZERO
 */
int yunos_bsp_sspi_set_bits(uint32_t id, uint8_t nbits);

/**
 * Transfer data on spi.
 * @param[in]   id          the id of spi
 * @param[in]   txbuf       the source data address.
 * @param[in]   rxbuf       the destination data address.
 * @param[in]   txlen       send data numbers.
 * @param[in]   rxlen       recieve data numbers.
 * @param[in]   tx_relen    the number that send data success.
 * @param[in]   rx_relen    the number that recieve data success.
 * @return      OK or ERROR CODE.
 */
int yunos_bsp_sspi_transfer(uint32_t id, uint8_t *txbuf, size_t txlen, uint32_t *tx_relen,  uint8_t *rxbuf, size_t rxlen, uint32_t *rx_relen);

/**
 * Send data on spi.
 * @param[in]   id          the id of spi
 * @param[in]   buf         the source data address.
 * @param[in]   len         write data numbers.
 * @parma[in]   re_len      the number that send data success.
 * @return      OK or ERROR CODE.
 */
int yunos_bsp_sspi_send(uint32_t id, uint8_t *buf, size_t len, uint32_t *re_len);

/**
 * Recieve data on spi.
 * @param[in]   id          the spi num
 * @param[out]  buf         the destination data address.
 * @param[in]   len         read data numbers.
 * @param[out]  re_len      the number the read data success.
 * @return      OK or ERROR CODE.
 */
int yunos_bsp_sspi_recieve(uint32_t id, uint8_t *buf, size_t len,  uint32_t *re_len);

/**
 * Open SPI
 * @param[in]   id          the spi num.
 * @return      OK or ERROR CODE.
 */
int yunos_bsp_sspi_open(uint32_t id);

/**
 * Close SPI
 * @param[in]   id          the spi num.
 * @return      OK or error code
 */
int yunos_bsp_sspi_close(uint32_t id);

/**
 * Initialize the selected SPI port
 * @param[in]   Port number (for hardware that has mutiple SPI interfaces)
 * @return      Valid SPI device structure reference on succcess; a NULL on failure
 */
int yunos_bsp_sspi_initialize(uint32_t *num);

#endif /* YUNOS_SPI_CTRL_H */
