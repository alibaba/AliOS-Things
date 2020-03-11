/*********************************************************************************************************************
 * @file     XMC1000_RomFunctionTable.h
 * @brief    ROM functions prototypes for the XMC1400-Series
 * @version  V1.0
 * @date     08 April 2013
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2013-2016, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided with the distribution.                       
 * 
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written permission.                                           
 *                                                                              
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                  
 *                                                                              
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
 * Infineon Technologies AG dave@infineon.com).                                                          
 *********************************************************************************************************************
 *
 **************************** Change history *********************************
 * V1.0, 08 Jan 2013, GD, First version with XMC1000 devices
 *****************************************************************************
 * @endcond 
 */

#ifndef ROM_FUNCTION_TABLE_H
#define ROM_FUNCTION_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ***************************************************************************
******************************* Constants *******************************
*************************************************************************** */
/* Start address of the ROM function table */
#define ROM_FUNCTION_TABLE_START    (0x00000100U)

/* Pointer to Erase Flash Page routine */
#define _NvmErase                    (ROM_FUNCTION_TABLE_START + 0x00U)

/* Pointer to Erase, Program & Verify Flash Page routine */
#define _NvmProgVerify               (ROM_FUNCTION_TABLE_START + 0x04U)

/* Pointer to Request BMI installation routine */
#define _BmiInstallationReq          (ROM_FUNCTION_TABLE_START + 0x08U)


/* ***************************************************************************
******************************** Enumerations ********************************
*************************************************************************** */
typedef enum TagNVMStatus
{

/* The function succeeded */
 NVM_PASS               = (int32_t)0x00010000U,
 
/* Generic error code */ 
 NVM_E_FAIL             = (int32_t)0x80010001U,
 
/* Source data not in RAM */ 
 NVM_E_SRC_AREA_EXCCEED = (int32_t)0x80010003U,
 
/* Source data is not 4 byte aligned */ 
 NVM_E_SRC_ALIGNMENT    = (int32_t)0x80010004U,
 
/* NVM module cannot be physically accessed */ 
 NVM_E_NVM_FAIL         = (int32_t)0x80010005U,
 
/* Verification of written page not successful */ 
 NVM_E_VERIFY           = (int32_t)0x80010006U,
 
/* Destination data is not (completely) located in NVM */ 
 NVM_E_DST_AREA_EXCEED  = (int32_t)0x80010009U,
 
/* Destination data is not properly aligned */ 
 NVM_E_DST_ALIGNMENT    = (int32_t)0x80010010U,

} NVM_STATUS;


/* ***************************************************************************
*********************************** Macros ***********************************
*************************************************************************** */

/* ***************************************************************************
Description: Erase granularity = 1 Page of  16 blocks of 16 Bytes
                               = Equivalent to 256 Bytes using this routine.

Input parameters:                                                                                                              
– Logical address of the Flash Page to be erased which must be page aligned 
and in NVM address range

Return status:
– OK (NVM_PASS)
– Invalid address (NVM_E_DST_ALIGNMENT or NVM_E_DST_AREA_EXCEED)
– Operation failed (Error during low level NVM programming driver):
                                                                NVM_E_FAIL 
                                                                NVM_E_VERIFY 
                                                                NVM_E_NVM_FAIL

Prototype: 
    NVM_STATUS XMC1000_NvmErasePage(uint32_t *pageAddr)
*************************************************************************** */
#define XMC1000_NvmErasePage (*((NVM_STATUS (**) (uint32_t * )) \
                                                                   _NvmErase))

/* ***************************************************************************
Description: This procedure performs erase (skipped if not necessary), program 
and verify of selected Flash page.

Input parameter:                                                                                                              
– Logical address of the target Flash Page, must be page aligned and in NVM 
address range

– Address in SRAM where the data starts, must be 4-byte aligned

Return status:
–  OK (NVM_PASS)
–  Invalid addresses 
                    NVM_E_DST_ALIGNMENT 
                    NVM_E_SRC_ALIGNMENT 
                    NVM_E_DST_AREA_EXCEED 
                    NVM_E_SRC_AREA_EXCCEED
–  Operation failed (Error during low level NVM programming driver): 
                                                                 NVM_E_FAIL
                                                                 NVM_E_VERIFY 
                                                                 NVM_E_NVM_FAIL

Prototype: 
  NVM_STATUS XMC1000_NvmProgVerify(const uint32_t *srcAddr, uint32_t *dstAddr)
*************************************************************************** */
#define XMC1000_NvmProgVerify (*((NVM_STATUS (**) (const uint32_t * ,\
                                                   uint32_t * ))\
                                                               _NvmProgVerify))


/* ***************************************************************************
Description: This procedure initiates installation of a new BMI value. In 
particular, it can be used as well as to restore the state upon delivery for a
device already in User Productive mode. 

Input parameter:                                                                                                              
– BMI value to be installed

Return status:
–  wrong input BMI value (0x01) - only upon error, if OK the procedure triggers
a reset respectively does not return to calling routine !

Prototype: 
  unsigned long XMC1000_BmiInstallationReq(unsigned short requestedBmiValue)
**************************************************************************** */
#define XMC1000_BmiInstallationReq (*((uint32_t (**) (uint16_t)) \
                                                          _BmiInstallationReq))

#ifdef __cplusplus
}
#endif

#endif /* ROM_FUNCTION_TABLE_H */
