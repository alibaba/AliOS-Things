/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#include "SWM320.h"
#include "SWM320_sdram.h"

void SDRAM_Init(SDRAM_InitStructure * initStruct)
{
    SYS->CLKEN |= (1 << SYS_CLKEN_SDRAM_Pos);

    SYS->CLKDIV &= ~SYS_CLKDIV_SDRAM_Msk;
    SYS->CLKDIV |= (1 << SYS_CLKDIV_SDRAM_Pos);

    SDRAMC->CR0 = (2 << SDRAMC_CR0_BURSTLEN_Pos) |
                  (2 << SDRAMC_CR0_CASDELAY_Pos);

    SDRAMC->CR1 = (initStruct->CellSize << SDRAMC_CR1_CELLSIZE_Pos) |
                  ((initStruct->CellWidth == 16 ? 0 : 1) << SDRAMC_CR1_CELL32BIT_Pos) |
                  (initStruct->CellBank << SDRAMC_CR1_BANK_Pos) |
                  ((initStruct->DataWidth == 16 ? 0 : 1) << SDRAMC_CR1_32BIT_Pos) |
                  (7 << SDRAMC_CR1_TMRD_Pos) |
                  (3 << SDRAMC_CR1_TRRD_Pos) |
                  (7 << SDRAMC_CR1_TRAS_Pos) |
                  (8 << SDRAMC_CR1_TRC_Pos) |
                  (3 << SDRAMC_CR1_TRCD_Pos) |
                  (3 << SDRAMC_CR1_TRP_Pos);

    SDRAMC->LATCH = 0x02;

    SDRAMC->REFRESH = (1 << SDRAMC_REFRESH_EN_Pos) |
                      (0x0FA << SDRAMC_REFRESH_RATE_Pos);

    while(SDRAMC->REFDONE == 0);
}
