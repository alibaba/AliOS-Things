/**
 *  spi_interface.h
 *
 * Defines and Macros for the SPI.
 *
 * Copyright @ 2015 Espressif System Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Espressif System Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
/**
 * @file spi_interface.h
 * @brief Defines and Macros for the SPI.
 */
#ifndef __SPI_INTERFACE_H__
#define __SPI_INTERFACE_H__

#include "spi_register.h"
#include "c_types.h"

//*****************************************************************************
//
// Make sure all of the definitions in this header have a C binding.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Defines slave commands. Default value based on slave ESP8266.
 */
#define MASTER_WRITE_DATA_TO_SLAVE_CMD                      2
#define MASTER_READ_DATA_FROM_SLAVE_CMD                     3

#define MASTER_WRITE_STATUS_TO_SLAVE_CMD                    1
#define MASTER_READ_STATUS_FROM_SLAVE_CMD                   4

/**
 * @brief Support HSPI and SPI module.
 *
 */
typedef enum
{
    SpiNum_SPI   = 0,
    SpiNum_HSPI  = 1,
} SpiNum;

/**
 * @brief The SPI module can work in either master or slave mode.
 *
 */
typedef enum
{
    SpiMode_Master = 0,
    SpiMode_Slave  = 1,
} SpiMode;

/**
 *  @brief SPI sub mode
 *
 * Support 4 sub modes based on SPI clock polarity and phase.
 * SPI_CPOL SPI_CPHA  SubMode
 *   0        0        0
 *   0        1        1
 *   1        0        2
 *   1        1        3
 */
typedef enum
{
    SpiSubMode_0 = 0,
    SpiSubMode_1 = 1,
    SpiSubMode_2 = 2,
    SpiSubMode_3 = 3,
} SpiSubMode;

/**
 * @brief The SPI module working speed.
 *
 * @attention Max speed 80MHz
 *
 */
typedef enum
{
    SpiSpeed_2MHz  = 40 - 1,
    SpiSpeed_5MHz  = 16 - 1,
    SpiSpeed_10MHz = 8 - 1,
    SpiSpeed_16MHz = 5 - 1,
    SpiSpeed_20MHz = 4 - 1,
} SpiSpeed;

/**
 * @brief The SPI mode working speed.
 *
 */
typedef enum
{
    SpiBitOrder_MSBFirst = 0,
    SpiBitOrder_LSBFirst = 1,
} SpiBitOrder;

// @brief SPI interrupt soource defined.
typedef enum
{
    SpiIntSrc_TransDoneEn = SPI_TRANS_DONE_EN,
    SpiIntSrc_WrStaDoneEn = SPI_SLV_WR_STA_DONE_EN,
    SpiIntSrc_RdStaDoneEn = SPI_SLV_RD_STA_DONE_EN,
    SpiIntSrc_WrBufDoneEn = SPI_SLV_WR_BUF_DONE_EN,
    SpiIntSrc_RdBufDoneEn = SPI_SLV_RD_BUF_DONE_EN,
} SpiIntSrc;

// @brief SPI CS pin.
typedef enum
{
    SpiPinCS_0 = 0,
    SpiPinCS_1 = 1,
    SpiPinCS_2 = 2,
} SpiPinCS;

#pragma pack (1)

/**
 * @brief SPI attribute
 */
typedef struct
{
    SpiMode        mode;           ///< Master or slave mode
    SpiSubMode     subMode;        ///< SPI SPI_CPOL SPI_CPHA mode
    SpiSpeed       speed;          ///< SPI Clock
    SpiBitOrder    bitOrder;       ///< SPI bit order
} SpiAttr;

/**
 * @brief SPI attribute
 */
typedef struct
{
    uint16_t    cmd;            ///< Command value
    uint8_t     cmdLen;         ///< Command byte length
    uint32_t    *addr;          ///< Point to address value
    uint8_t     addrLen;        ///< Address byte length
    uint32_t    *data;          ///< Point to data buffer
    uint8_t     dataLen;        ///< Data byte length.
} SpiData;

#pragma upack (1)

#define SHOWREG() __ShowRegValue(__func__, __LINE__);

/**
 * @brief Print debug information.
 *
 */
void __ShowRegValue(const char * func, uint32_t line);

/**
 * @brief Initialize SPI module.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] pAttr
 *             Pointer to a struct SpiAttr that indicates SPI working attribution.
 *
 * @return void.
 */
void SPIInit(SpiNum spiNum, SpiAttr* pAttr);

/**
 * @brief Set slave address value by master.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] addr
 *             Slave address to be set.
 *
 * @return void.
 */
void SPIMasterCfgAddr(SpiNum spiNum, uint32_t addr);

/**
 * @brief Set command value by master.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] cmd
 *             Command will be send to slave.
 *
 * @return void.
 */
void SPIMasterCfgCmd(SpiNum spiNum, uint32_t cmd);

/**
 * @brief Send data to slave from master.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] pInData
 *             Pointer to a strcuture that will be send.
 *
 * @return int, -1:indicates failure,others indicates success.
 */
 int SPIMasterSendData(SpiNum spiNum, SpiData* pInData);

/**
 * @brief Receive data from slave by master.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] pOutData
 *             Point to data buffer.
 *
 * @return int, -1:indicates failure,others indicates success.
 *
 */
 int SPIMasterRecvData(SpiNum spiNum, SpiData* pOutData);

/**
 * @brief Load data to slave send buffer.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] pInData
 *             Point to data buffer.
 * @param [in] outLen
 *             The number of bytes to be set.
 *
 * @return int, -1:indicates failure,others indicates success.
 */
int SPISlaveSendData(SpiNum spiNum, uint32_t *pInData, uint8_t outLen);

/**
 * @brief Receive data by slave.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] isrFunc
 *             isrFunc is a pointer to the function to be called when the SPI interrupt occurs.
 *
 * @return int, -1:indicates failure,others indicates success.
 */
int SPISlaveRecvData(SpiNum spiNum, void(*isrFunc)(void*));

/**
 * @brief Set slave status by master.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] data
 *             Data will be write to slave SPI_WR_STATUS.
 *
 * @return void.
 *
 * @attention Just for ESP8266(slave) register of RD_STATUS or WR_STATUS.
 */
void SPIMasterSendStatus(SpiNum spiNum, uint8_t data);

/**
 * @brief Get salve status by master.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 *
 * @return int, -1: indicates failure; other value in slave status.
 *
 * @attention Just for ESP8266(slave) register of RD_STATUS or WR_STATUS.
 */
int SPIMasterRecvStatus(SpiNum spiNum);

/**
 * @brief Select SPI CS pin.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] pinCs
 *             Indicates which SPI pin to choose.
 *
 * @return void.
 */
void SPICsPinSelect(SpiNum spiNum, SpiPinCS pinCs);

/**
 * @brief Enable SPI module interrupt source.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] intSrc
 *             Indicates which interrupt source to enable.
 *
 * @return void.
 */
void SPIIntEnable(SpiNum spiNum, SpiIntSrc intSrc);

/**
 * @brief Disable SPI module interrupt source.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 * @param [in] intSrc
 *             Indicates which interrupt source to disable.
 *
 * @return void.
 */
void SPIIntDisable(SpiNum spiNum, SpiIntSrc intSrc);

/**
 * @brief Clear all of spi interrupt.
 *
 * @param [in] spiNum
 *             Indicates which submode to be used, SPI or HSPI.
 *
 * @return void.
 */
void SPIIntClear(SpiNum spiNum);

#ifdef __cplusplus
}
#endif

#endif //  __SPI_INTERFACE_H__