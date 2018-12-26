/**
  ******************************************************************************
  * @file    nvm.h
  * @author  Sean
  * @version V1.0.0
  * @date    3-June-2013
  * @brief   Nvm read and write functions declare
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
  */

#ifndef __NVM_H__
#define __NVM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * @brief  Read data from NVM Memory
 * @param  NvmAddr: the NVM buffer index which you read start,its range [0:179]
 * @param  Buf: the data address which you read and save data form the NVM
 * @param  Length: the NVM buffer length which you read,Addr+Length must be not more than 180
 * @return 0:failure,1:success
 */
bool NvmRead(uint8_t NvmAddr, uint8_t* Buf, uint8_t Length);

/**
 * @brief  Write data to NVM Memory
 * @param  NvmAddr: NVM buffer index which you write start,its range [0~179]
 * @param  Buf: the data address which you will write to NVM
 * @param  Length: the NVM buffer length which you will write,Addr+Length must be not more than 180
 * @return 0:failure,1:success
 */
bool NvmWrite(uint8_t NvmAddr, uint8_t* Buf, uint8_t Length);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__NVM_H__
