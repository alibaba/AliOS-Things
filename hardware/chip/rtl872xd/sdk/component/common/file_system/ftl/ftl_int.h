/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      ftl_rom.h
   * @brief     ftl header file for rom project
   * @details
   * @author
   * @date
   * @version
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
  *                   Define to prevent recursive inclusion
  *============================================================================*/

#ifndef _FTL_ROM_H_
#define _FTL_ROM_H_

/*============================================================================*
  *                               Header Files
  *============================================================================*/
#include "ftl.h"


#ifdef  __cplusplus
extern  "C" {
#endif

#define FTL_SUCCESS       (0x00)
#define FTL_ERROR_INVALID_LOGICAL_ADDR 0xF0
#define FTL_ERROR_OUT_OF_SPACE        (0xF1)
#define FTL_ERROR_PAGE_FORMAT         (0xF2)
#define FTL_ERROR_PAGE_END_FORMAT     (0xF3)


/*============================================================================*
  *                                   Types
  *============================================================================*/

/*============================================================================*
  *                                Functions
  *============================================================================*/
uint32_t ftl_init(uint32_t u32PageStartAddr, uint8_t pagenum);
/**
    * @brief    Save specified value to specified ftl offset
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to store
    * @param    size   size to store
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no confict with ROM
    */
uint32_t ftl_save_to_storage(void *pdata, uint16_t offset, uint16_t size);

/**
    * @brief    Load specified ftl offset parameter to specified buffer
    * @param    pdata  specify data buffer
    * @param    offset specify FTL offset to load
    * @param    size   size to load
    * @return   status
    * @retval   0  status successful
    * @retval   otherwise fail
    * @note     FTL offset is pre-defined and no confict with ROM
    */
uint32_t ftl_load_from_storage(void *pdata, uint16_t offset, uint16_t size);

void ftl_garbage_collect_in_idle(void);



#ifdef  __cplusplus
}
#endif

#endif // _FTL_ROM_H_
