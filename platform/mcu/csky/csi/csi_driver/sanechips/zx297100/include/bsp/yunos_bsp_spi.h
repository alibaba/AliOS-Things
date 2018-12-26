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

#ifndef YUNOS_BSP_SPI_H
#define YUNOS_BSP_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define YUNOS_BSP_API_SPI_OPEN              0x0100
#define YUNOS_BSP_API_SPI_CLOSE             0x0101
#define YUNOS_BSP_API_SPI_DMASTART          0x0102
#define YUNOS_BSP_API_SPI_INIT              0x0103
#define YUNOS_BSP_API_SPI_SETDATAWIDTH      0x0104
#define YUNOS_BSP_API_SPI_SETTHRESHOLD      0x0105
#define YUNOS_BSP_API_SPI_READSTATUS        0x0106
#define YUNOS_BSP_API_SPI_ENABLEIRQ         0x010B
#define YUNOS_BSP_API_SPI_DISABLEIRQ        0x010C
#define YUNOS_BSP_API_SPI_DMAENABLE         0x010D
#define YUNOS_BSP_API_SPI_DMADISABLE        0x010E
#define YUNOS_BSP_API_SPI_SETDMARDLR        0x010F
#define YUNOS_BSP_API_SPI_SETDMATDLR        0x0110
#define YUNOS_BSP_API_SPI_SETNDF            0x0111
#define YUNOS_BSP_API_SPI_SEND              0x0113
#define YUNOS_BSP_API_SPI_RECV              0x0114
#define YUNOS_BSP_API_SPI_SETBAUDRATE       0x0115


/*ioctl cmd code, MAX 255*/
#define CMD_SPI_CS_SELECT	('s' << 24 | 'p' << 16 | 'i' <<8 | 0)
#define CMD_SPI_SETFREQ	(CMD_SPI_CS_SELECT + 1)
#define CMD_SPI_SETMODE	(CMD_SPI_SETFREQ + 1)
#define CMD_SPI_SUSPEND	(CMD_SPI_SETMODE + 1)
#define CMD_SPI_RESUME		(CMD_SPI_SUSPEND + 1)

#define SPI_CHANNEL0            0
#define SPI_CHANNEL1            1
//#define SPI_CHANNEL2            2
#define SPI_CHANNEL_MAXNUM      2

#define SPI_TX_THRESHOLD        32
#define SPI_RX_THRESHOLD        16

typedef enum {
    SPI_DMACR_RDMAE = 1,
    SPI_DMACR_TDMAE
}
SPI_STATUS_TYPE;

typedef enum {
	SSP_MS_MASTER = 0,
	SSP_MS_SLAVE,
} SPI_MSMODE_TYPE;

typedef enum {
    SPI_TRANSMODE_TRANSRECV = 0,
    SPI_TRANSMODE_TRANSONLY,
    SPI_TRANSMODE_RECVONLY,
    SPI_TRANSMODE_EEPROMREAD
} SPI_TRANSMODE_TYPE;

typedef enum {
    SPI_POLARITY_LOWER = 0,
    SPI_POLARITY_HIGH
} SPI_POLARITY_TYPE;

typedef enum {
    SPI_PHASE_MIDDLE = 0,
    SPI_PHASE_START
} SPI_PHASE_TYPE;

typedef enum {
    SPI_DATASIZE4 = 3,
    SPI_DATASIZE5,
    SPI_DATASIZE6,
    SPI_DATASIZE7,
    SPI_DATASIZE8,
    SPI_DATASIZE9,
    SPI_DATASIZE10,
    SPI_DATASIZE11,
    SPI_DATASIZE12,
    SPI_DATASIZE13,
    SPI_DATASIZE14,
    SPI_DATASIZE15,
    SPI_DATASIZE16,
    SPI_DATASIZE32=31,
} SPI_DATAWIDTH_TYPE;

typedef enum {
    SPI_SLAVE_CS0 = 1,
    SPI_SLAVE_CS1
} SPI_SLAVE_TYPE;

typedef enum {
    SPIDEV_MODE0 = 0, ///< CPOL=0 CHPHA=0
    SPIDEV_MODE1,     ///< CPOL=0 CHPHA=1
    SPIDEV_MODE2,     ///< CPOL=1 CHPHA=0
    SPIDEV_MODE3      ///< CPOL=1 CHPHA=1
} SPI_MODE_ETYPE;

/**
 * This function will set data width.
 * @param[in]   id          spi channel index
 * @param[in]   datawidth   data width
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_set_datawidth(uint8_t id, SPI_DATAWIDTH_TYPE datawidth);

/**
 * This function will set spi tx and rx threshold.
 * @param[in]   id          spi channel index
 * @param[in]   txftl       transfer FIFO threshold
 * @param[in]   rxftl       receive FIFO threshold
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_set_threshold(uint8_t id, uint32_t txftl, uint32_t rxftl);

/**
 * This function will set spi number data frame.
 * @param[in]   id          spi channel index
 * @param[in]   dnf         data number frame
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_set_numberdataframe(uint8_t id, uint32_t dnf);

/**
 * This function will set dma recv data count.
 * @param[in]   id          spi channel index
 * @param[in]   count       recv data count
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_set_dma_recvdatacount(uint8_t id, uint32_t count);

/**
 * This function will set dma transmit data count.
 * @param[in]   id          spi channel index
 * @param[in]   count       trans data count
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_set_dma_transdatacount(uint8_t id, uint32_t count);

/**
 * This function will enable dma receive or transmit.
 * @param[in]   id          spi channel index
 * @param[in]   type        dma receive or transmit
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_dma_enable(uint8_t id, SPI_STATUS_TYPE type);

/**
 * This function will disable dma receive or transmit.
 * @param[in]   id          spi channel index
 * @param[in]   type        dma receive or transmit
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_dma_disable(uint8_t id, SPI_STATUS_TYPE type);

/**
 * This function will start dma.
 * @param[in]   id          spi channel index
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_dma_start(uint8_t id);

/**
 * This function will enable spi irq.
 * @param[in]   id          spi channel index
 * @param[in]   irq         spi irq type
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_enableirq(uint8_t id, uint32_t irq);

/**
 * This function will disable spi irq.
 * @param[in]   id          spi channel index
 * @param[in]   irq         spi irq type
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_disableirq(uint8_t id, uint32_t irq);

/**
 * This function will select the spi slave.
 * @return      Zero on success, -1 on failed
 */
//int yunos_bsp_spi_select(void);
int yunos_bsp_spi_select(uint8_t id);

/**
 * This function will set frequenc.
 * @param[in]   id          spi channel index
 * @param[in]   freq        The SPI frequency requested
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_set_frequency(uint32_t id, uint32_t freq);

/***
 * This function will Set the SPI direction mode.
 * @param[in]   id          spi channel index
 * @param[in]   dir_mode    The SPI transfer mode
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_set_direction_mode(uint32_t id, SPI_TRANSMODE_TYPE dirct_mode);

/**
 * This function will Set the SPI mode.  see enum sspi_mode_e for mode definitions.
 * @param[in]   id          spi channel index
 * @param[in]   mode        the software spi mode requested
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_set_mode(uint32_t id, SPI_MODE_ETYPE mode);

/**
 * This function will read spi status.
 * @param[in]   id          spi channel index
 * @param[in]   status      status of spi
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_readstatus(uint8_t id, uint32_t *status);

/**
 * This function will send bytes to spi slave.
 * @param[in]   id          spi channel index
 * @param[in]   data        A pointer to a buffer in which to receive data
 * @param[in]   len         the length of data to be exchaned in units of words
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_send(uint8_t id, const char *data, uint32_t len);

/**
 * This function will read bytes from spi slave.
 * @param[in]   id          spi channel index
 * @param[in]   data        A pointer to a buffer in which to receive data
 * @param[in]   len         the length of data to be exchaned in units of words
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_recv(uint8_t id, char *data, uint32_t len);

/**
 * This function will open spi device.
 * @param[in]   id          spi channel index
 * @param[in]   slaveid     slave spi device
 * @return      Zero on success, -1 on failed
 */
//int yunos_bsp_spi_open(uint8_t id, SPI_SLAVE_TYPE slaveid);
int yunos_bsp_spi_open(uint8_t id);

/**
 * This function will close spi device.
 * @param[in]   id          spi channel index
 * @param[in]   slaveid     slave spi device
 * @return      Zero on success, -1 on failed
 */
//int yunos_bsp_spi_close(uint8_t id, SPI_SLAVE_TYPE slaveid);
int yunos_bsp_spi_close(uint8_t id);

 /**
 * This function will suspend spi device, called by psm.
 * @param[in]  id  spi channel index
 * @return the operation status, YUNOS_SUCCESS is OK, others is error
 */
int yunos_bsp_spi_suspend(uint8_t id);
 
/**
 * This function will resume spi device.
 * @param[in]  id  spi channel index
 * @return the operation status, YUNOS_SUCCESS is OK, others is error
 */
int yunos_bsp_spi_resume(uint8_t id);

/**
 * Init spi module.
 * @param[in]   id          spi channel index
 * @return      Zero on success, -1 on failed
 */
int yunos_bsp_spi_init(uint8_t id);

#ifdef __cplusplus
}
#endif

#endif /* YUNOS_BSP_SPI_H */
