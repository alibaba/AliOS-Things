/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_SPI_STM32F4_H
#define __HAL_SPI_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"

#include "board.h"

#define HAL_SPI_GPIO_NEED_MAP 0xfe

typedef enum{
    SPI_POLLING_MODE,
    SPI_INTERRUPT_MODE,
    SPI_DMA_MODE
}SPI_OPERATING_MODE_TYPE;

typedef struct{
    uint32_t Direction; //Specifies the SPI bidirectional mode state,
                        //the parameter can be:SPI_DIRECTION_2LINES,SPI_DIRECTION_2LINES_RXONLY,SPI_DIRECTION_1LINE
    uint32_t DataSize;  //Specifies the SPI data size,
                        //the parameter can be:SPI_DATASIZE_8BIT,SPI_DATASIZE_16BIT
    uint32_t CLKPolarity; //Specifies the serial clock steady state,
                            //the parameter can be:SPI_POLARITY_LOW,SPI_POLARITY_HIGH
    uint32_t CLKPhase;  //Specifies the clock active edge for the bit capture
                            //the parameter can be:SPI_PHASE_1EDGE,SPI_PHASE_2EDGE
    uint32_t NSS;   //Specifies whether the NSS signal is managed by hardware (NSS pin) or by software using the SSI bit
                        //the parameter can be:SPI_NSS_SOFT,SPI_NSS_HARD_INPUT,SPI_NSS_HARD_OUTPUT
    uint32_t FirstBit;  //Specifies whether data transfers start from MSB or LSB bit
                            //SPI_FIRSTBIT_MSB,SPI_FIRSTBIT_LSB
    uint32_t TIMode;    //Specifies if the TI mode is enabled or not
                            //SPI_TIMODE_DISABLE,SPI_TIMODE_ENABLE
    uint32_t CRCCalculation;//Specifies if the CRC calculation is enabled or not
                            //SPI_CRCCALCULATION_DISABLE, SPI_CRCCALCULATION_ENABLE
    uint32_t CRCPolynomial; //Specifies the polynomial used for the CRC calculation.
                            //This parameter must be a number between Min_Data = 0 and Max_Data = 65535
}spiAttribute;
/*
typedef struct{
    uint16_t clk_pin;
    uint16_t cs_pin;
    uint16_t mosi_pin;
    uint16_t miso_pin;
}spi_pins_map_t;*/

typedef struct{
    PORT_SPI_TYPE  spiFuncP;
    void*          spiPhyP;
    spiAttribute  attr;
    uint16_t needmap;
    uint16_t gpiomaps[4];
}SPI_MAPPING;

extern SPI_MAPPING SPI_MAPPING_TABLE[PORT_SPI_SIZE];
extern void SPI_DMA_RX_IRQHandler(const void* spiIns);
extern void SPI_DMA_TX_IRQHandler(const void* spiIns);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_SPI_STM32L4_H */
