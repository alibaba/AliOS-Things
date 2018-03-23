/**
 *  spi_interface.c
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
 * @file spi_interface.c
 * @brief Defines and Macros for the SPI.
 */

#include "spi_interface.h"
#include "esp8266/eagle_soc.h"
#include "esp8266/ets_sys.h"
#include "esp8266/pin_mux_register.h"
#include "esp_libc.h"
//*****************************************************************************
//
// Make sure all of the definitions in this header have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

// Show the spi registers.
#define SHOWDEBUG

void __ShowRegValue(const char * func, uint32_t line)
{
#ifndef SHOWDEBUG
    int i;
    uint32_t regAddr = 0x60000140; // SPI--0x60000240, HSPI--0x60000140;
    printf("\r\n FUNC[%s],line[%d]\r\n", func, line);
    printf(" SPI_ADDR      [0x%08x]\r\n", READ_PERI_REG(SPI_ADDR(SpiNum_HSPI)));
    printf(" SPI_CMD       [0x%08x]\r\n", READ_PERI_REG(SPI_CMD(SpiNum_HSPI)));
    printf(" SPI_CTRL      [0x%08x]\r\n", READ_PERI_REG(SPI_CTRL(SpiNum_HSPI)));
    printf(" SPI_CTRL2     [0x%08x]\r\n", READ_PERI_REG(SPI_CTRL2(SpiNum_HSPI)));
    printf(" SPI_CLOCK     [0x%08x]\r\n", READ_PERI_REG(SPI_CLOCK(SpiNum_HSPI)));
    printf(" SPI_RD_STATUS [0x%08x]\r\n", READ_PERI_REG(SPI_RD_STATUS(SpiNum_HSPI)));
    printf(" SPI_WR_STATUS [0x%08x]\r\n", READ_PERI_REG(SPI_WR_STATUS(SpiNum_HSPI)));
    printf(" SPI_USER      [0x%08x]\r\n", READ_PERI_REG(SPI_USER(SpiNum_HSPI)));
    printf(" SPI_USER1     [0x%08x]\r\n", READ_PERI_REG(SPI_USER1(SpiNum_HSPI)));
    printf(" SPI_USER2     [0x%08x]\r\n", READ_PERI_REG(SPI_USER2(SpiNum_HSPI)));
    printf(" SPI_PIN       [0x%08x]\r\n", READ_PERI_REG(SPI_PIN(SpiNum_HSPI)));
    printf(" SPI_SLAVE     [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE(SpiNum_HSPI)));
    printf(" SPI_SLAVE1    [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE1(SpiNum_HSPI)));
    printf(" SPI_SLAVE2    [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE2(SpiNum_HSPI)));

    for (i = 0; i < 16; ++i) {
        printf(" ADDR[0x%08x],Value[0x%08x]\r\n", regAddr, READ_PERI_REG(regAddr));
        regAddr += 4;
    }
#endif
}

// Define SPI interrupt enable macro
#define ETS_SPI_INTR_ENABLE()  _xt_isr_unmask(1 << ETS_SPI_INUM)

/**
 * @brief Based on pAttr initialize SPI module.
 *
 */
void ICACHE_FLASH_ATTR SPIInit(SpiNum spiNum, SpiAttr* pAttr)
{
    if ((spiNum > SpiNum_HSPI)
        || (NULL == pAttr)) {
        return;
    }
    // SPI_CPOL & SPI_CPHA
    switch (pAttr->subMode) {
    case SpiSubMode_1:
        CLEAR_PERI_REG_MASK(SPI_PIN(spiNum), SPI_IDLE_EDGE);
        SET_PERI_REG_MASK(SPI_USER(spiNum),  SPI_CK_OUT_EDGE); // CHPA_FALLING_EDGE_SAMPLE
        break;
    case SpiSubMode_2:
        SET_PERI_REG_MASK(SPI_PIN(spiNum), SPI_IDLE_EDGE);
        SET_PERI_REG_MASK(SPI_USER(spiNum),  SPI_CK_OUT_EDGE); // CHPA_FALLING_EDGE_SAMPLE
        break;
    case SpiSubMode_3:
        SET_PERI_REG_MASK(SPI_PIN(spiNum), SPI_IDLE_EDGE);
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum),  SPI_CK_OUT_EDGE);
        break;
    case SpiSubMode_0:
    default:
        CLEAR_PERI_REG_MASK(SPI_PIN(spiNum), SPI_IDLE_EDGE);
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum),  SPI_CK_OUT_EDGE);
        // To do nothing
        break;
    }

    // SPI bit order
    if (SpiBitOrder_MSBFirst == pAttr->bitOrder) {
        CLEAR_PERI_REG_MASK(SPI_CTRL(spiNum), SPI_WR_BIT_ORDER);
        CLEAR_PERI_REG_MASK(SPI_CTRL(spiNum), SPI_RD_BIT_ORDER);
    } else if (SpiBitOrder_LSBFirst == pAttr->bitOrder) {
        SET_PERI_REG_MASK(SPI_CTRL(spiNum), SPI_WR_BIT_ORDER);
        SET_PERI_REG_MASK(SPI_CTRL(spiNum), SPI_RD_BIT_ORDER);
    } else {
        // To do nothing
    }

    // Disable flash operation mode
    // As earlier as better, if not SPI_CTRL2 can not to be set delay cycles.
    CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_FLASH_MODE);

    // SPI mode type
    if (SpiMode_Master == pAttr->mode) {
        // SPI mode type
        CLEAR_PERI_REG_MASK(SPI_SLAVE(spiNum), SPI_SLAVE_MODE);
        // SPI Send buffer
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MISO_HIGHPART );// By default slave send buffer C0-C7
        // SPI Speed
        if (1 < (pAttr->speed)) {
            CLEAR_PERI_REG_MASK(SPI_CLOCK(spiNum), SPI_CLK_EQU_SYSCLK);
            if (spiNum == SpiNum_HSPI) {
                CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_CONF_U, SPI1_CLK_EQU_SYS_CLK);
            }

            WRITE_PERI_REG(SPI_CLOCK(spiNum),
                           ((pAttr->speed & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |
                           ((((pAttr->speed + 1) / 2 - 1) & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |
                           ((pAttr->speed & SPI_CLKCNT_L) << SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div
        } else {
            WRITE_PERI_REG(SPI_CLOCK(spiNum), SPI_CLK_EQU_SYSCLK); // 80Mhz speed
        }
        // By default format:CMD+ADDR+DATA
        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_CS_SETUP | SPI_CS_HOLD | SPI_USR_MOSI );

        //delay num
        SET_PERI_REG_MASK(SPI_CTRL2(spiNum), ((0x1 & SPI_MISO_DELAY_NUM) << SPI_MISO_DELAY_NUM_S));
    } else if (SpiMode_Slave == pAttr->mode) {
        // BIT19 must do
        SET_PERI_REG_MASK(SPI_PIN(spiNum), BIT19);

        // SPI mode type
        SET_PERI_REG_MASK(SPI_SLAVE(spiNum), SPI_SLAVE_MODE);
        // SPI Send buffer
        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MISO_HIGHPART);// By default slave send buffer C8-C15

        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MOSI);

        // If do not set delay cycles, slave not working,master cann't get the data.
        SET_PERI_REG_MASK(SPI_CTRL2(spiNum), ((0x1 & SPI_MOSI_DELAY_NUM) << SPI_MOSI_DELAY_NUM_S)); //delay num
        // SPI Speed
        WRITE_PERI_REG(SPI_CLOCK(spiNum), 0);

        // By default format::CMD(8bits)+ADDR(8bits)+DATA(32bytes).
        SET_PERI_REG_BITS(SPI_USER2(spiNum), SPI_USR_COMMAND_BITLEN,
                          7, SPI_USR_COMMAND_BITLEN_S);
        SET_PERI_REG_BITS(SPI_SLAVE1(spiNum), SPI_SLV_WR_ADDR_BITLEN,
                          7, SPI_SLV_WR_ADDR_BITLEN_S);
        SET_PERI_REG_BITS(SPI_SLAVE1(spiNum), SPI_SLV_RD_ADDR_BITLEN,
                          7, SPI_SLV_RD_ADDR_BITLEN_S);
        SET_PERI_REG_BITS(SPI_SLAVE1(spiNum), SPI_SLV_BUF_BITLEN,
                          (32 * 8 - 1), SPI_SLV_BUF_BITLEN_S);
        // For 8266 work on slave mode.
        SET_PERI_REG_BITS(SPI_SLAVE1(spiNum), SPI_SLV_STATUS_BITLEN,
                          7, SPI_SLV_STATUS_BITLEN_S);
    } else {
        // To do nothing
    }

    //clear Daul or Quad lines transmission mode
    CLEAR_PERI_REG_MASK(SPI_CTRL(spiNum), SPI_QIO_MODE | SPI_DIO_MODE | SPI_DOUT_MODE | SPI_QOUT_MODE);
    SET_PERI_REG_MASK(SPI_CTRL(spiNum), SPI_FASTRD_MODE);

}

/**
 * @brief Set address value by master mode.
 *
 */
void ICACHE_FLASH_ATTR SPIMasterCfgAddr(SpiNum spiNum, uint32_t addr)
{
    if (spiNum > SpiNum_HSPI) {
        return;
    }
    // Set address
    WRITE_PERI_REG(SPI_ADDR(spiNum), addr);
}

/**
 * @brief Set command value by master mode.
 *
 */
void ICACHE_FLASH_ATTR SPIMasterCfgCmd(SpiNum spiNum, uint32_t cmd)
{
    if (spiNum > SpiNum_HSPI) {
        return;
    }
    // SPI_USER2 bit28-31 is cmd length,cmd bit length is value(0-15)+1,
    // bit15-0 is cmd value.
    SET_PERI_REG_BITS(SPI_USER2(spiNum), SPI_USR_COMMAND_VALUE, cmd, SPI_USR_COMMAND_VALUE_S);
}

/**
 * @brief Send data to slave.
 *
 */
int ICACHE_FLASH_ATTR SPIMasterSendData(SpiNum spiNum, SpiData* pInData)
{
    char idx = 0;
    if ((spiNum > SpiNum_HSPI)
        || (NULL == pInData)
        || (64 < pInData->dataLen)) {
        return -1;
    }
    uint32_t *value = pInData->data;
    while (READ_PERI_REG(SPI_CMD(spiNum))&SPI_USR);
    // Set command by user.
    if (pInData->cmdLen != 0) {
        // Max command length 16 bits.
        SET_PERI_REG_BITS(SPI_USER2(spiNum), SPI_USR_COMMAND_BITLEN,
                          ((pInData->cmdLen << 3) - 1), SPI_USR_COMMAND_BITLEN_S);
        // Enable command
        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_COMMAND);
        // Load command
        SPIMasterCfgCmd(spiNum, pInData->cmd);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_USER2(spiNum), SPI_USR_COMMAND_BITLEN,
                          0, SPI_USR_COMMAND_BITLEN_S);
    }
    // Set Address by user.
    if (pInData->addrLen == 0) {
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_ADDR);
        SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_ADDR_BITLEN,
                          0, SPI_USR_ADDR_BITLEN_S);
    } else {
        if (NULL == pInData->addr) {
            return -1;
        }
        SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_ADDR_BITLEN,
                          ((pInData->addrLen << 3) - 1), SPI_USR_ADDR_BITLEN_S);
        // Enable address
        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_ADDR);
        // Load address
        SPIMasterCfgAddr(spiNum, *pInData->addr);
    }
    // Set data by user.
    if (pInData->dataLen != 0) {
        if (NULL == value) {
            return -1;
        }
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MOSI);
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MISO);
        // Load send buffer
        do {
            WRITE_PERI_REG((SPI_W0(spiNum) + (idx << 2)), *value++);
        } while (++idx < (pInData->dataLen / 4));
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_MOSI_BITLEN, ((pInData->dataLen << 3) - 1), SPI_USR_MOSI_BITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MOSI);
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_MOSI_BITLEN,
                          0, SPI_USR_MOSI_BITLEN_S);
    }
    // Start send data
    SET_PERI_REG_MASK(SPI_CMD(spiNum), SPI_USR);

    SHOWREG();
    return 0;
}

/**
 * @brief Receive data from slave.
 *
 */
int ICACHE_FLASH_ATTR SPIMasterRecvData(SpiNum spiNum, SpiData* pOutData)
{
    char idx = 0;
    if ((spiNum > SpiNum_HSPI)
        || (NULL == pOutData)) {
        return -1;
    }

    uint32_t *value = pOutData->data;
    while (READ_PERI_REG(SPI_CMD(spiNum))&SPI_USR);
    // Set command by user.
    if (pOutData->cmdLen != 0) {
        // Max command length 16 bits.
        SET_PERI_REG_BITS(SPI_USER2(spiNum), SPI_USR_COMMAND_BITLEN,
                          ((pOutData->cmdLen << 3) - 1), SPI_USR_COMMAND_BITLEN_S);
        // Enable command
        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_COMMAND);
        // Load command
        SPIMasterCfgCmd(spiNum, pOutData->cmd);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_USER2(spiNum), SPI_USR_COMMAND_BITLEN,
                          0, SPI_USR_COMMAND_BITLEN_S);
    }
    // Set Address by user.
    if (pOutData->addrLen == 0) {
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_ADDR);
        SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_ADDR_BITLEN,
                          0, SPI_USR_ADDR_BITLEN_S);
    } else {
        if (NULL == pOutData->addr) {
            return -1;
        }
        SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_ADDR_BITLEN,
                          ((pOutData->addrLen << 3) - 1), SPI_USR_ADDR_BITLEN_S);
        // Enable address
        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_ADDR);
        // Load address
        SPIMasterCfgAddr(spiNum, *pOutData->addr);
    }
    // Set data by user.
    if (pOutData->dataLen != 0) {
        if (NULL == value) {
            return -1;
        }
        // Clear MOSI enable
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MOSI);
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MISO);
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_MISO_BITLEN, ((pOutData->dataLen << 3) - 1), SPI_USR_MISO_BITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MOSI);
        CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_MISO_BITLEN,
                          0, SPI_USR_MISO_BITLEN_S);
    }

    //CLEAR FIFO DATA
    int fifo_idx = 0;
    do {
        WRITE_PERI_REG(SPI_W0(spiNum) + (fifo_idx << 2), 0);
    } while (++fifo_idx < (pOutData->dataLen / 4));

    // Start send data
    SET_PERI_REG_MASK(SPI_CMD(spiNum), SPI_USR);
    while (READ_PERI_REG(SPI_CMD(spiNum))&SPI_USR);
    // Read data out
    do {
        *pOutData->data++ = READ_PERI_REG(SPI_W0(spiNum) + (idx << 2));
    } while (++idx < (pOutData->dataLen / 4));

    SHOWREG();
    return 0;
}

/**
 * @brief Load data to send buffer by slave mode.
 *
 */
int ICACHE_FLASH_ATTR SPISlaveSendData(SpiNum spiNum, uint32_t *pInData, uint8_t outLen)
{
    if (NULL == pInData) {
        return -1;
    }
    char i;
    for (i = 0; i < outLen; ++i) {
        WRITE_PERI_REG((SPI_W8(spiNum) + (i << 2)), *pInData++);
    }
    return 0;
}

/**
 * @brief Configurate slave prepare for receive data.
 *
 */
int ICACHE_FLASH_ATTR SPISlaveRecvData(SpiNum spiNum, void(*isrFunc)(void*))
{
    if ((spiNum > SpiNum_HSPI)) {
        return -1;
    }

    SPIIntEnable(SpiNum_HSPI, SpiIntSrc_WrStaDoneEn
                 | SpiIntSrc_RdStaDoneEn | SpiIntSrc_WrBufDoneEn | SpiIntSrc_RdBufDoneEn);
    SPIIntDisable(SpiNum_HSPI, SpiIntSrc_TransDoneEn);

    // Maybe enable slave transmission liston
    SET_PERI_REG_MASK(SPI_CMD(spiNum), SPI_USR);
    //
     _xt_isr_attach(ETS_SPI_INUM, isrFunc, NULL);
   // ETS_SPI_INTR_ATTACH(isrFunc, NULL);
    // Enable isr
    ETS_SPI_INTR_ENABLE();


    SHOWREG();

    return 0;
}

/**
 * @brief Send data to slave(ESP8266 register of RD_STATUS or WR_STATUS).
 *
 */
void ICACHE_FLASH_ATTR SPIMasterSendStatus(SpiNum spiNum, uint8_t data)
{
    if (spiNum > SpiNum_HSPI) {
        return;
    }
    while (READ_PERI_REG(SPI_CMD(spiNum))&SPI_USR);
    // Enable MOSI
    SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MOSI);
    CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MISO | SPI_USR_DUMMY | SPI_USR_ADDR);

    // 8bits cmd, 0x04 is eps8266 slave write cmd value
    WRITE_PERI_REG(SPI_USER2(spiNum),
                   ((7 & SPI_USR_COMMAND_BITLEN) << SPI_USR_COMMAND_BITLEN_S)
                   | MASTER_WRITE_STATUS_TO_SLAVE_CMD);
    // Set data send buffer length.
    SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_MOSI_BITLEN,
                      ((sizeof(data) << 3) - 1), SPI_USR_MOSI_BITLEN_S);

    WRITE_PERI_REG(SPI_W0(spiNum), (uint32)(data));
    // Start SPI
    SET_PERI_REG_MASK(SPI_CMD(spiNum), SPI_USR);

    SHOWREG();
}

/**
 * @brief Receive status register from slave(ESP8266).
 *
 */
int ICACHE_FLASH_ATTR SPIMasterRecvStatus(SpiNum spiNum)
{
    if (spiNum > SpiNum_HSPI) {
        return -1;
    }

    while (READ_PERI_REG(SPI_CMD(spiNum))&SPI_USR);
    // Enable MISO
    SET_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MISO);
    CLEAR_PERI_REG_MASK(SPI_USER(spiNum), SPI_USR_MOSI | SPI_USR_DUMMY | SPI_USR_ADDR);

    // 8bits cmd, 0x06 is eps8266 slave read status cmd value
    WRITE_PERI_REG(SPI_USER2(spiNum),
                   ((7 & SPI_USR_COMMAND_BITLEN) << SPI_USR_COMMAND_BITLEN_S)
                   | MASTER_READ_STATUS_FROM_SLAVE_CMD);
    // Set revcive buffer length.
    SET_PERI_REG_BITS(SPI_USER1(spiNum), SPI_USR_MISO_BITLEN,
                      7, SPI_USR_MISO_BITLEN_S);

    // start spi module.
    SET_PERI_REG_MASK(SPI_CMD(spiNum), SPI_USR);

    while (READ_PERI_REG(SPI_CMD(spiNum))&SPI_USR);

    uint8_t data = (uint8)(READ_PERI_REG(SPI_W0(spiNum)) & 0xff);
    SHOWREG();

    return (uint8)(READ_PERI_REG(SPI_W0(spiNum)) & 0xff);
}

/**
 * @brief Select SPI CS pin.
 *
 */
void ICACHE_FLASH_ATTR SPICsPinSelect(SpiNum spiNum, SpiPinCS pinCs)
{
    if (spiNum > SpiNum_HSPI) {
        return;
    }
    // clear select
    SET_PERI_REG_BITS(SPI_PIN(spiNum), 3, 0, 0);
    SET_PERI_REG_MASK(SPI_PIN(spiNum), pinCs);
}

/**
 * @brief Enable SPI interrupt source.
 *
 */
void ICACHE_FLASH_ATTR SPIIntEnable(SpiNum spiNum, SpiIntSrc intSrc)
{
    if (spiNum > SpiNum_HSPI) {
        return;
    }
    SET_PERI_REG_MASK(SPI_SLAVE(spiNum), intSrc);
}

/**
 * @brief Disable SPI interrupt source.
 *
 */
void ICACHE_FLASH_ATTR SPIIntDisable(SpiNum spiNum, SpiIntSrc intSrc)
{
    if (spiNum > SpiNum_HSPI) {
        return;
    }
    CLEAR_PERI_REG_MASK(SPI_SLAVE(spiNum), intSrc);
}

/**
 * @brief Clear all of SPI interrupt source.
 *
 */
void ICACHE_FLASH_ATTR SPIIntClear(SpiNum spiNum)
{
    if (spiNum > SpiNum_HSPI) {
        return;
    }
    CLEAR_PERI_REG_MASK(SPI_SLAVE(spiNum), SpiIntSrc_TransDoneEn
                        | SpiIntSrc_WrStaDoneEn
                        | SpiIntSrc_RdStaDoneEn
                        | SpiIntSrc_WrBufDoneEn
                        | SpiIntSrc_RdBufDoneEn);
}


#ifdef __cplusplus
}
#endif
