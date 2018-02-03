/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <cache.h>



/**
  \brief   Enable I-Cache
  \details Turns on I-Cache
  */
void hal_enable_icache (void)
{
    SCB_EnableICache();
    return;  
}


/**
  \brief   Disable I-Cache
  \details Turns off I-Cache
  */
void hal_disable_icache (void)
{
    SCB_DisableICache();
    return; 
}


/**
  \brief   Invalidate I-Cache
  \details Invalidates I-Cache
  */
void hal_invalidate_icache (void)
{
    SCB_InvalidateICache();
    return;
}


/**
  \brief   Enable D-Cache
  \details Turns on D-Cache
  */
void hal_enable_dcache (void)
{
    SCB_EnableDCache();
    return;
}


/**
  \brief   Disable D-Cache
  \details Turns off D-Cache
  */
void hal_disable_dcache (void)
{
    SCB_DisableDCache();
    return;
}


/**
  \brief   Invalidate D-Cache
  \details Invalidates D-Cache
  */
void hal_invalidate_dcache (void)
{
    SCB_InvalidateDCache();
    return;
}


/**
  \brief   Clean D-Cache
  \details Cleans D-Cache
  */
void hal_clean_dcache (void)
{
    SCB_CleanDCache();
    return;
}


/**
  \brief   Clean & Invalidate D-Cache
  \details Cleans and Invalidates D-Cache
  */
void hal_cleaninvalidate_dcache (void)
{
    SCB_CleanInvalidateDCache();
    return;
}


/**
  \brief   D-Cache Invalidate by address
  \details Invalidates D-Cache for the given address
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void hal_invalidate_dcache_byaddr (uint32_t *addr, int32_t dsize)
{
    SCB_InvalidateDCache_by_Addr (addr, dsize);
    return;
}


/**
  \brief   D-Cache Clean by address
  \details Cleans D-Cache for the given address
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void hal_clean_dcache_byaddr (uint32_t *addr, int32_t dsize)
{
    SCB_CleanDCache_by_Addr (addr, dsize);
    return;
}


/**
  \brief   D-Cache Clean and Invalidate by address
  \details Cleans and invalidates D_Cache for the given address
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
void hal_cleaninvalidate_dcache_byaddr (uint32_t *addr, int32_t dsize)
{
    SCB_CleanInvalidateDCache_by_Addr (addr, dsize);
    return;
}


