/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_RNG_H
#define HAL_RNG_H
#include "stdint.h"
#include "aos/errno.h"
#include "fm33a0xx_include_all.h"
#include "board.h"

// ============== Fm33A0X Gpio Define Begin =============== //

//init random 
int32_t hal_random_init()
{
    RCC_PERCLK_SetableEx( ANACCLK,        ENABLE );       //ANAC clk enable
    RCC_PERCLK_SetableEx( TRNGCLK,      ENABLE );       //TRNG clk enable
  
    TRNG_RNGIE_RNGIE_Setable(DISABLE);            //RNG IE disable
    TRNG_RNGIF_RNF_Clr();                         //RNG IF clr
  
    return 0;
}

//deinit random
int32_t hal_random_deinit()
{
    RCC_PERCLK_SetableEx( TRNGCLK,      DISABLE );      //TRNG clk disable
  
    TRNG_Deinit();
  
    return 0;
}

//get random byte
uint8_t hal_random_get_byte()
{
    uint16_t i;
    uint32_t random_num;

    TRNG_RNGIF_RNF_Clr();
    TRNG_TRNGCON_RNGEN_Setable(ENABLE); 
    
    //wait for rng success
    for(i = 0; i < 10; i++)
    {       
        if(RESET == TRNG_RNGIF_LFSREN_Chk())//rng success
        {
            break;
        }
    }
    
    TRNG_TRNGCON_RNGEN_Setable(DISABLE);
    random_num = TRNG_RNGOUT_Read();
  
    //crc32
    TRNG_CRCIN_Write(random_num);
    TRNG_CRCCON_CRCEN_Setable(ENABLE);
  
    for(i = 0; i < 100; i++)
    {
        if(SET == TRNG_CRCFLAG_CRCD_Chk())
        {
            break;
        }      
    }
    TRNG_CRCFLAG_CRCD_Clr();
    
    random_num = TRNG_RNGOUT_Read();
  
    return (random_num & 0xFF);
}

// ============== Fm33A0X Gpio Define End =============== //


typedef struct {
    uint8_t port; /* random device port */
    void   *priv; /* priv data */
} random_dev_t;

/**
 * Fill in a memory buffer with random data
 *
 * @param[in]   random       the random device
 * @param[out]  inBuffer     Point to a valid memory buffer, this function will fill
 *                           in this memory with random numbers after executed
 * @param[in]   inByteCount  Length of the memory buffer (bytes)
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_random_num_read(random_dev_t random, void *buf, int32_t bytes)
{
  uint8_t *local_buf = (uint8_t *)buf;
  int32_t i;
  
  if (NULL == buf)
  {
    return EIO;
  }
  
  hal_random_init();
  
  for (i = 0; i < bytes; i++)
  {
    local_buf[i] = hal_random_get_byte();
  }
  
  hal_random_deinit();
  
  return 0;
}

#endif /* HAL_RNG_H */

