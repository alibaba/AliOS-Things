/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_SPI_H
#define HAL_SPI_H
#include "stdint.h"
#include "aos/errno.h"
#include "fm33a0xx_include_all.h"
#include "board.h"

#define HAL_SPI_MODE_MASTER 1  /* spi communication is master mode */
#define HAL_SPI_MODE_SLAVE  2  /* spi communication is slave mode */

typedef struct {
    uint32_t mode;        /* spi communication mode */
    uint32_t freq;        /* communication frequency Hz */
} spi_config_t;

typedef struct {
    uint8_t      port;    /* spi port */
    spi_config_t config;  /* spi config */
    void        *priv;    /* priv data */
} spi_dev_t;

// ============== Fm33A0X Gpio Define Begin =============== //
/*
 * SPI Name
 */
typedef enum 
{
  SPI_0,
  SPI_1,
  SPI_2
}hal_spi_name_t;

/*
 * SPI Port/Pin
 */
#define SPI0_SSN_Pin                      GPIO_Pin_15
#define SPI0_SSN_Port                     GPIOF
#define SPI0_SCK_Pin                      GPIO_Pin_14
#define SPI0_SCK_Port                     GPIOF
#define SPI0_MISO_Pin                     GPIO_Pin_13
#define SPI0_MISO_Port                    GPIOF
#define SPI0_MOSI_Pin                     GPIO_Pin_12
#define SPI0_MOSI_Port                    GPIOF

#define SPI1_SSN_Pin                      GPIO_Pin_12
#define SPI1_SSN_Port                     GPIOB
#define SPI1_SCK_Pin                      GPIO_Pin_13
#define SPI1_SCK_Port                     GPIOB
#define SPI1_MISO_Pin                     GPIO_Pin_14
#define SPI1_MISO_Port                    GPIOB
#define SPI1_MOSI_Pin                     GPIO_Pin_15
#define SPI1_MOSI_Port                    GPIOB

//#define SPI1_SSN_Pin                        GPIO_Pin_6
//#define SPI1_SSN_Port                       GPIOC
//#define SPI1_SCK_Pin                        GPIO_Pin_7
//#define SPI1_SCK_Port                       GPIOC
//#define SPI1_MISO_Pin                       GPIO_Pin_8
//#define SPI1_MISO_Port                      GPIOC
//#define SPI1_MOSI_Pin                       GPIO_Pin_9
//#define SPI1_MOSI_Port                      GPIOC

#define SPI2_SSN_Pin                      GPIO_Pin_2
#define SPI2_SSN_Port                     GPIOD
#define SPI2_SCK_Pin                      GPIO_Pin_3
#define SPI2_SCK_Port                     GPIOD
#define SPI2_MISO_Pin                     GPIO_Pin_4
#define SPI2_MISO_Port                    GPIOD
#define SPI2_MOSI_Pin                     GPIO_Pin_5
#define SPI2_MOSI_Port                    GPIOD

//get baudrate para
//0-success EIO-fail
int32_t get_baudrate_para(uint8_t port, uint32_t freq, uint32_t *freq_para)
{
    uint32_t AHBClock;
    uint8_t i;
  
    AHBClock = RCC_SYSCLKSEL_AHBPRES_Get();
  
    if (SPI_0 == port)
    {
        for (i = 0; i < 8; i++)
        {
            if (AHBClock / (1 << i) == freq)
            {
                break;
            }
        }
    
        if (8 == i)
        {
            return EIO;
        }
        else
        {
            switch (i)
            {
                case 0:
                    *freq_para = HSPI_SPICR1_BAUD_PCLK;
                    break;
                case 1:
                    *freq_para = HSPI_SPICR1_BAUD_PCLK_2;
                    break;
                case 2:
                    *freq_para = HSPI_SPICR1_BAUD_PCLK_4;
                    break;
                case 3:
                    *freq_para = HSPI_SPICR1_BAUD_PCLK_8;
                    break;
                case 4:
                    *freq_para = HSPI_SPICR1_BAUD_PCLK_16;
                    break;
                case 5:
                    *freq_para = HSPI_SPICR1_BAUD_PCLK_32;
                    break;
                case 6:
                    *freq_para = HSPI_SPICR1_BAUD_PCLK_64;
                    break;
                case 7:
                    *freq_para = HSPI_SPICR1_BAUD_PCLK_128;
                    break;
            }
      
            return 0;
        }
    }
    else
    {
        for (i = 0; i < 8; i++)
        {
            if (AHBClock / (1 << (i + 1)) == freq)
            {
                break;
            }
        }
    
        if (8 == i)
        {
            return EIO;
        }
        else
        {
            switch (i)
            {
                case 0:
                    *freq_para = SPIx_SPICR1_BAUD_PCLK_2;
                    break;
                case 1:
                    *freq_para = SPIx_SPICR1_BAUD_PCLK_4;
                    break;
                case 2:
                    *freq_para = SPIx_SPICR1_BAUD_PCLK_8;
                    break;
                case 3:
                    *freq_para = SPIx_SPICR1_BAUD_PCLK_16;
                    break;
                case 4:
                    *freq_para = SPIx_SPICR1_BAUD_PCLK_32;
                    break;
                case 5:
                    *freq_para = SPIx_SPICR1_BAUD_PCLK_64;
                    break;
                case 6:
                    *freq_para = SPIx_SPICR1_BAUD_PCLK_128;
                    break;
                case 7:
                    *freq_para = SPIx_SPICR1_BAUD_PCLK_256;
                    break;
            }
      
            return 0;
        }
    }
}

//check param
int32_t check_param(spi_dev_t *spi)
{
    uint32_t freq_para;

    if (spi->port > SPI_2)
    {
        return EIO;
    }
  
    if (HAL_SPI_MODE_MASTER != spi->config.mode)
    {
        return EIO;
    }
  
    if (EIO == get_baudrate_para(spi->port, spi->config.freq, &freq_para))
    {
        return EIO;
    }
  
    return 0;
}



//port transform SPIx
SPIx_Type* port_transform_SPIx(spi_dev_t *spi)
{
    SPIx_Type * SPIx = NULL;
  
    if (spi->port > SPI_2)
    {
        return SPIx;
    }
  
    if (SPI_0 == spi->port)
    {
        SPIx = HSPI;
    }
    else if (SPI_1 == spi->port)
    {
        SPIx = SPI1;
    }
    else
    {
        SPIx = SPI2;
    }
  
    return SPIx;
}

// ============== Fm33A0X Gpio Define End =============== //

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_init(spi_dev_t *spi)
{ 
    uint32_t freq_para;
    SPIx_Type * SPIx = NULL;
  
    if (NULL == spi)
    {
        return EIO;
    }
  
    if (EIO == check_param(spi))
    {
        return EIO;
    }
  
    SPIx = port_transform_SPIx(spi);
    if (NULL == SPIx)
    {
        return EIO;
    }
  
    //gpio config
    RCC_PERCLK_SetableEx(PDCCLK, ENABLE);//PAD clk enable
    if (SPI_0 == spi->port)
    {
        HSPI_Master_SInitTypeDef init_para;
        
        AltFunIO(SPI0_SSN_Port, SPI0_SSN_Pin, ALTFUN_NORMAL);//SPI_SSN
        AltFunIO(SPI0_SCK_Port, SPI0_SCK_Pin, ALTFUN_NORMAL);//SPI_SCK
        AltFunIO(SPI0_MISO_Port, SPI0_MISO_Pin, ALTFUN_NORMAL);//SPI_MISO
        AltFunIO(SPI0_MOSI_Port, SPI0_MOSI_Pin, ALTFUN_NORMAL);//SPI_MOSI
        
        RCC_PERCLK_SetableEx(HSPICLK, ENABLE);//HSPI clk enable
        
        //hspi master init
        get_baudrate_para(spi->port, spi->config.freq, &freq_para);
        init_para.BAUD_RATE = freq_para;//freq
        init_para.LSBF = HSPI_SPICR1_LSBF_MSB;//MSB
        init_para.CPHOL = HSPI_SPICR1_CPHOL_HIGH;//CPHOL
        init_para.CPHA = HSPI_SPICR1_CPHA_SECOND;//CPHA
        init_para.SSNM = HSPI_SPICR2_SSNM_LOW;//SSN
        init_para.SSNSEN = HSPI_SPICR2_SSNSEN_SOFT;//SSN Control
          
        HSPI_Master_Init(&init_para);//master init
        HSPI_SPICR2_HSPIEN_Setable(ENABLE);//HSPI enable
    }
    else if (SPI_1 == spi->port)
    {
        SPI_Master_SInitTypeDef init_para;
        
        AltFunIO(SPI1_SSN_Port, SPI1_SSN_Pin, ALTFUN_NORMAL);//SPI_SSN
        AltFunIO(SPI1_SCK_Port, SPI1_SCK_Pin, ALTFUN_NORMAL);//SPI_SCK
        AltFunIO(SPI1_MISO_Port, SPI1_MISO_Pin, ALTFUN_NORMAL);//SPI_MISO
        AltFunIO(SPI1_MOSI_Port, SPI1_MOSI_Pin, ALTFUN_NORMAL);//SPI_MOSI
        
        RCC_PERCLK_SetableEx(SPI1CLK, ENABLE);//SPI clk enable
      
        //spi master init
        get_baudrate_para(spi->port, spi->config.freq, &freq_para);
        init_para.BAUD_RATE = freq_para;//freq
        init_para.LSBF = SPIx_SPICR1_LSBF_MSB;//MSB
        init_para.CPHOL = SPIx_SPICR1_CPHOL_HIGH;//CPHOL
        init_para.CPHA = SPIx_SPICR1_CPHA_SECOND;//CPHA
        init_para.SSNM = SPIx_SPICR2_SSNM_LOW;//SSN
        init_para.SSNSEN = SPIx_SPICR2_SSNSEN_SOFT;//SSN Control

        SPI_Master_Init(SPIx, &init_para);//master init
        SPIx_SPICR2_SPIEN_Setable(SPIx, ENABLE);//SPIx enable
    }
    else
    {
        SPI_Master_SInitTypeDef init_para;
        
        AltFunIO(SPI2_SSN_Port, SPI2_SSN_Pin, ALTFUN_NORMAL);//SPI_SSN
        AltFunIO(SPI2_SCK_Port, SPI2_SCK_Pin, ALTFUN_NORMAL);//SPI_SCK
        AltFunIO(SPI2_MISO_Port, SPI2_MISO_Pin, ALTFUN_NORMAL);//SPI_MISO
        AltFunIO(SPI2_MOSI_Port, SPI2_MOSI_Pin, ALTFUN_NORMAL);//SPI_MOSI
        
        RCC_PERCLK_SetableEx(SPI2CLK, ENABLE);//SPI clk enable
      
        //spi master init
        get_baudrate_para(spi->port, spi->config.freq, &freq_para);
        init_para.BAUD_RATE = freq_para;//freq
        init_para.LSBF = SPIx_SPICR1_LSBF_MSB;//MSB
        init_para.CPHOL = SPIx_SPICR1_CPHOL_HIGH;//CPHOL
        init_para.CPHA = SPIx_SPICR1_CPHA_SECOND;//CPHA
        init_para.SSNM = SPIx_SPICR2_SSNM_LOW;//SSN
        init_para.SSNSEN = SPIx_SPICR2_SSNSEN_SOFT;//SSN Control

        SPI_Master_Init(SPIx, &init_para);//msater init
        SPIx_SPICR2_SPIEN_Setable(SPIx, ENABLE);//SPIx enable
    }
 
    return 0;
}

/**
 * Spi send
 *
 * @param[in]  spi      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in ms
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint16_t send_size = 0;
    SPIx_Type* SPIx = NULL;
    uint32_t lastTick;
  
    SPIx = port_transform_SPIx(spi);
    if (NULL == SPIx)
    {
        return EIO;
    }
  
    lastTick = SysTick->VAL;
  
    if (SPI_0 == spi->port)
    {
        HSPI_SSN_Set_Low();//SSN low
    
        while (send_size < size)
        {
            IWDT_Clr(); //feed dog
      
            HSPI_RW_Byte(data[send_size]);
            send_size++;
            if ((lastTick - SysTick->VAL > (timeout * ((__SYSTEM_CLOCK/1000)))) &&
                (send_size < size))
            {
                return EIO;
            }
        }
    
        HSPI_SSN_Set_High();//SSN high
    }
    else
    {
        SPI_SSN_Set_Low(SPIx);//SSN low
    
        while (send_size < size)
        {
            IWDT_Clr(); //feed dog
          
            SPI_RW_Byte(SPIx, data[send_size]);
            send_size++;
            if ((lastTick - SysTick->VAL > (timeout * ((__SYSTEM_CLOCK/1000)))) &&
                (send_size < size))
            {
                return EIO;
            }
        }
    
        SPI_SSN_Set_High(SPIx);//SSN high
    }
  
    return 0;
}

/**
 * spi_recv
 *
 * @param[in]   spi      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeout  timeout in ms
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint16_t recv_size = 0;
    SPIx_Type* SPIx = NULL;
    uint32_t lastTick;
  
    SPIx = port_transform_SPIx(spi);
    if (NULL == SPIx)
    {
        return EIO;
    }
  
    lastTick = SysTick->VAL;
  
    if (SPI_0 == spi->port)
    {
        //HSPI_SSN_Set_Low();//SSN low
    
        while (recv_size < size)
        {
            IWDT_Clr(); //feed dog
          
            while(1)
            {
                if (HSPI_SPIIF_RXBF_Chk() == SET)
                {
                    break;
                }
            }
            data[recv_size] = HSPI_Recv_Byte();
            recv_size++;
            if ((lastTick - SysTick->VAL > (timeout * ((__SYSTEM_CLOCK/1000)))) &&
                (recv_size < size))
            {
                return EIO;
            }
        }
    
        //HSPI_SSN_Set_High();//SSN high
    }
    else
    {
        //SPI_SSN_Set_Low(SPIx);//SSN low
    
        while (recv_size < size)
        {
            IWDT_Clr(); //feed dog
      
            while(1)
            {
                if (SPIx_SPIIF_RXBF_Chk(SPIx) == SET)
                {
                    break;
                }
            }
            data[recv_size] = SPI_Recv_Byte(SPIx);
            recv_size++;
            if ((lastTick - SysTick->VAL > (timeout * ((__SYSTEM_CLOCK/1000)))) &&
                (recv_size < size))
            {
                return EIO;
            }
        }
    
        //SPI_SSN_Set_High(SPIx);//SSN high
    }
  
    return 0;
}

/**
 * spi send data and recv
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  spi send data
 * @param[in]  rx_data  spi recv data
 * @param[in]  size  spi data to be sent and recived
 * @param[in]  timeout  timeout in ms
 *
 * @return  0, on success;  EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
    uint16_t send_size = 0;
    SPIx_Type* SPIx = NULL;
    uint32_t lastTick;
  
    if ((NULL == spi) ||
        (NULL == tx_data) ||
        (NULL == rx_data))
    {
        return EIO;
    }
  
    SPIx = port_transform_SPIx(spi);
    if (NULL == SPIx)
    {
        return EIO;
    }
  
    lastTick = SysTick->VAL;
  
    if (SPI_0 == spi->port)
    {
        HSPI_SSN_Set_Low();//SSN low
    
        while (send_size < size)
        {
            IWDT_Clr(); //feed dog
          
            rx_data[send_size] = HSPI_RW_Byte(tx_data[send_size]);
            send_size++;
            if ((lastTick - SysTick->VAL > (timeout * ((__SYSTEM_CLOCK/1000)))) &&
                (send_size < size))
            {
                return EIO;
            }
        }
    
        HSPI_SSN_Set_High();//SSN high
    }
    else
    {
        SPI_SSN_Set_Low(SPIx);//SSN low
    
        while (send_size < size)
        {
            IWDT_Clr(); //feed dog
          
            rx_data[send_size] = SPI_RW_Byte(SPIx, tx_data[send_size]);
            send_size++;
            if ((lastTick - SysTick->VAL > (timeout * ((__SYSTEM_CLOCK/1000)))) &&
                (send_size < size))
            {
                return EIO;
            }
        }
    
        SPI_SSN_Set_High(SPIx);//SSN high
    }
  
    return 0;
}

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi the SPI device to be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_spi_finalize(spi_dev_t *spi)
{
    SPIx_Type* SPIx = NULL;
  
    if (NULL == spi)
    {
        return EIO;
    }
  
    SPIx = port_transform_SPIx(spi);
    if (NULL == SPIx)
    {
        return EIO;
    }
  
    if (SPI_0 == spi->port)
    {
        HSPI_Deinit();
    }
    else
    {
        SPIx_Deinit(SPIx);
    }
  
    return 0;
}

#endif /* HAL_SPI_H */

