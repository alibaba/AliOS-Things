#ifndef _DRV_DMA_H
#define _DRV_DMA_H

#include <stdint.h>
#include "soc_defs.h"

#define SRAM3 0x200000
#define SRAM_ADR 0x23000
#define MAX_DATA_LEN 0x1000

#define DMA_CH0 0x101
#define DMA_CH1 0x202
#define DMA_CH01 0x303

//void dma_setenv(work_unit *unit);
//uint32_t dma_main(work_unit *unit);

typedef struct _dma_config{
    uint32_t source0;
    uint32_t dest0;
    uint32_t source1;
    uint32_t dest1;
    uint32_t src_burst_len0:3;
    uint32_t dst_burst_len0:3;
    uint32_t tsr_size0:12;
    uint32_t prior0:1;
    uint32_t prior1:1;
    uint32_t tsr_size1:12;
    uint32_t src_burst_len1:3;
    uint32_t dst_burst_len1:3;
    uint32_t src_inc0:2;
    uint32_t dst_inc0:2;
    uint32_t src_inc1:2;
    uint32_t dst_inc1:2;
    uint32_t fc_mode0:3;
    uint32_t fc_mode1:3;
    uint32_t reserve:2;
    uint32_t ch_demask;

} dma_config;

typedef enum {
    SRAM_DMA_BLOCK_SIZE_IN2IN,
    SRAM_DMA_BLOCK_SIZE_IN2DE,
    SRAM_DMA_BLOCK_SIZE_DE2DE,
    SRAM_DMA_BLOCK_SIZE_DE2IN,
    SRAM2PKB_DMA_BLOCK_SIZE_IN2IN,
    SRAM2PKB_DMA_BLOCK_SIZE_IN2DE,
    SRAM2PKB_DMA_BLOCK_SIZE_DE2DE,
    SRAM2PKB_DMA_BLOCK_SIZE_DE2IN,
    PKB_DMA_BLOCK_SIZE_IN2IN,
    PKB_DMA_BLOCK_SIZE_IN2DE,
    PKB_DMA_BLOCK_SIZE_DE2DE,
    PKB_DMA_BLOCK_SIZE_DE2IN,
    PKB2SRAM_DMA_BLOCK_SIZE_IN2IN,
    PKB2SRAM_DMA_BLOCK_SIZE_IN2DE,
    PKB2SRAM_DMA_BLOCK_SIZE_DE2DE,
    PKB2SRAM_DMA_BLOCK_SIZE_DE2IN,
    DMA_MAX,
};

uint32_t dma_memcpy(uint32_t des_data_adr,uint32_t src_data_adr,uint32_t data_len);


#endif //_DRV_DMA_H
