#include "stm32f1xx_hal.h"

SRAM_HandleTypeDef SRAM_Handler;    //

void sram_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;

    __HAL_RCC_FSMC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    //PD0,1,4,5,8~15
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|\
                    GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|\
                    GPIO_PIN_14|GPIO_PIN_15;
    GPIO_Initure.Mode  = GPIO_MODE_AF_PP;
    GPIO_Initure.Pull  = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    //PE0,1,7~15
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|\
                    GPIO_PIN_10| GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|\
                    GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);

    //PF0~5,12~15
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|\
                    GPIO_PIN_5|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);

    //PG0~5,10
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);

    SRAM_Handler.Instance=FSMC_NORSRAM_DEVICE;
    SRAM_Handler.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;

    SRAM_Handler.Init.NSBank=FSMC_NORSRAM_BANK3;
    SRAM_Handler.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE;
    SRAM_Handler.Init.MemoryType=FSMC_MEMORY_TYPE_SRAM;
    SRAM_Handler.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    SRAM_Handler.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE;
    SRAM_Handler.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;
    SRAM_Handler.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;
    SRAM_Handler.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;
    SRAM_Handler.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;
    SRAM_Handler.Init.ExtendedMode=FSMC_EXTENDED_MODE_DISABLE;
    SRAM_Handler.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    SRAM_Handler.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;

    FSMC_ReadWriteTim.AddressSetupTime=0x00;
    FSMC_ReadWriteTim.AddressHoldTime=0x00;
    FSMC_ReadWriteTim.DataSetupTime=0x03;
    FSMC_ReadWriteTim.BusTurnAroundDuration=0X00;
    FSMC_ReadWriteTim.AccessMode=FSMC_ACCESS_MODE_A;
    HAL_SRAM_Init(&SRAM_Handler, &FSMC_ReadWriteTim,&FSMC_ReadWriteTim);
}

#define Bank1_SRAM3_ADDR    ((unsigned int)(0x68000000))

void fsmc_sram_write(unsigned char* pBuffer, unsigned int WriteAddr, unsigned int n)
{
#if 0
    HAL_SRAM_Write_8b(&SRAM_Handler, Bank1_SRAM3_ADDR + WriteAddr, pBuffer, n);
#else
    for(; n != 0; n--)
    {
        *(volatile unsigned char*)(Bank1_SRAM3_ADDR + WriteAddr) = *pBuffer;
        WriteAddr ++;
        pBuffer ++;
    }
#endif
}

void fsmc_sram_read(unsigned char* pBuffer, unsigned int ReadAddr, unsigned int n)
{
#if 0
    HAL_SRAM_Read_8b(&SRAM_Handler, Bank1_SRAM3_ADDR + WriteAddr, pBuffer, n);
#else
    for(; n != 0; n--)
    {
        *pBuffer ++ = *(volatile unsigned char*)(Bank1_SRAM3_ADDR + ReadAddr);
        ReadAddr ++;
    }
#endif
}