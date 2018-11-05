/*******************************************************************************
  MIIM Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_miim_local.h

  Summary:
    MIIM driver local declarations and definitions.

  Description:
    This file contains the MIIM driver's local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_MIIM_MAPPING_H
#define _DRV_MIIM_MAPPING_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************




#if defined (__PIC32C__)
    //*****************************************************************************
    /* MII Clock Selection

      Summary:
        Lists the possible speed selection for the Reduced Media Independent 
        Interface (RMII).

      Description:
        This enumeration lists the possible speed selection for RMII.
        The body contains only two states: RMII_10Mbps or RMII_100Mbps.

      Remarks:
        This enumeration is processor specific and is defined in the processor-
        specific header files (see processor.h).
    */
    typedef enum {

        GMAC_MIIM_SYSCLK_DIV_BY_8   /*DOM-IGNORE-BEGIN*/ = 0x00 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_16  /*DOM-IGNORE-BEGIN*/ = 0x01 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_32  /*DOM-IGNORE-BEGIN*/ = 0x02 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_48  /*DOM-IGNORE-BEGIN*/ = 0x03 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_64  /*DOM-IGNORE-BEGIN*/ = 0x04 /*DOM-IGNORE-END*/ ,
        GMAC_MIIM_SYSCLK_DIV_BY_96  /*DOM-IGNORE-BEGIN*/ = 0x05 /*DOM-IGNORE-END*/ ,

    } GMAC_MIIM_CLK;
    
    static  __inline__ DRV_MIIM_RESULT __attribute__((always_inline))_DRV_MIIM_ETH_ENABLE(uintptr_t ethphyId)
    {
        return DRV_MIIM_RES_OK;
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MII_RELEASE_RESET(uintptr_t ethphyId)
    {
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SETUP_PERAMBLE(uintptr_t ethphyId,const DRV_MIIM_SETUP* pSetUp)
    {
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_INCREMENT(uintptr_t ethphyId,const DRV_MIIM_SETUP* pSetUp)
    {
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_ENABLE(uintptr_t ethphyId)
    {
        _GMAC_REGS->GMAC_NCR.w |=	GMAC_NCR_MPE_Msk;
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_DISABLE(uintptr_t ethphyId)
    {
       _GMAC_REGS->GMAC_NCR.w &= ~GMAC_NCR_MPE_Msk;
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_BUSY(uintptr_t ethphyId)
    {
       if ((_GMAC_REGS->GMAC_NSR.w & GMAC_NSR_IDLE_Msk) != GMAC_NSR_IDLE_Msk)
           return true;
       else
           return false;          
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_PHYADDR_SET(uintptr_t ethphyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
      
    }

    static  __inline__ DRV_MIIM_TXFER_STAT __attribute__((always_inline))_DRV_MIIM_OP_SCAN_ENABLE(uintptr_t ethphyId)
    {        
         return DRV_MIIM_TXFER_SCAN_STALE;
    }

    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_WRITE_DATA(uintptr_t ethphyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        _GMAC_REGS->GMAC_MAN.w = 
                                (~GMAC_MAN_WZO_Msk & GMAC_MAN_CLTTO_Msk)
                                 | (GMAC_MAN_OP(0x1)) 
                                 | GMAC_MAN_WTN(0x02) 
                                 | GMAC_MAN_PHYA(pOpDcpt->phyAdd) 
                                 | GMAC_MAN_REGA(pOpDcpt->regIx) 
                                 | GMAC_MAN_DATA(pOpDcpt->opData);
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_WRITE_START(uintptr_t ethphyId)
    {
    }
    
   
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_READ_START(uintptr_t ethphyId, DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        _GMAC_REGS->GMAC_MAN.w =    (~GMAC_MAN_WZO_Msk & GMAC_MAN_CLTTO_Msk) 
                                    | (GMAC_MAN_OP(0x2)) 
                                    | GMAC_MAN_WTN(0x02) 
                                    | GMAC_MAN_PHYA(pOpDcpt->phyAdd) 
                                    | GMAC_MAN_REGA(pOpDcpt->regIx) 
                                    | GMAC_MAN_DATA(0);
    }
    
                                 
    static  __inline__ uint16_t __attribute__((always_inline))_DRV_MIIM_OP_READ_DATA_GET(uintptr_t ethphyId)
    {
       return (uint16_t)(_GMAC_REGS->GMAC_MAN.w & GMAC_MAN_DATA_Msk) ;
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_CLEAR_DATA_VALID(uintptr_t ethphyId)
    {
    }
  
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_DATA_VALID(uintptr_t ethphyId)
    {
        return false;
    }  

 
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_DIABLE(uintptr_t ethphyId)
    {
    } 
    
    static  __inline__ void __attribute__((always_inline)) _DRV_MIIM_SMI_CLOCK_SET(uintptr_t ethphyId, uint32_t hostClock, uint32_t maxMIIMClock )
    { 
    	uint32_t mdc_div; 
        GMAC_MIIM_CLK clock_dividor ; 
        mdc_div = hostClock/maxMIIMClock; 
        if (mdc_div <= 8 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_8; 
        } 
        else if (mdc_div <= 16 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_16; 
        } 
        else if (mdc_div <= 32 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_32; 
        } 
        else if (mdc_div <= 48 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_48; 
        } 
        else if (mdc_div <= 64 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_64; 
        } 
        else if (mdc_div <= 96 ) 
        { 
            clock_dividor = GMAC_MIIM_SYSCLK_DIV_BY_96; 
        } 
        else 
        { 
            clock_dividor = 0; 
        } 
        _GMAC_REGS->GMAC_NCR.w &= ~GMAC_NCR_TXEN_Msk; 
        _GMAC_REGS->GMAC_NCR.w &= ~GMAC_NCR_RXEN_Msk;	
        _GMAC_REGS->GMAC_NCFGR.w = 
                   (_GMAC_REGS->GMAC_NCFGR.w & 
                        (~GMAC_NCFGR_CLK_Msk)) | (clock_dividor << GMAC_NCFGR_CLK_Pos); 
        _GMAC_REGS->GMAC_NCR.w |= GMAC_NCR_TXEN_Msk; 
        _GMAC_REGS->GMAC_NCR.w |= GMAC_NCR_RXEN_Msk;	
    } 
    
#else    
   static const short _MIIMClockDivisorTable[]=
    {
        4, 6, 8, 10, 14, 20, 28, 40, 
#if defined (__PIC32MZ__)
        48, 50,
#endif  // defined (__PIC32MZ__)
    };  // divider values for the Host clock
   
    static  __inline__ DRV_MIIM_RESULT __attribute__((always_inline))_DRV_MIIM_ETH_ENABLE(uintptr_t ethphyId)
    {
        DRV_MIIM_RESULT res = DRV_MIIM_RES_OK;
        if(!PLIB_ETH_IsEnabled(ethphyId)) 
        { 
            PLIB_ETH_Enable(ethphyId);  
            res = DRV_MIIM_RES_INIT_WARNING;
        } 
        return res;
    }
    

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MII_RELEASE_RESET(uintptr_t ethphyId)
    {
        PLIB_ETH_MIIResetDisable(ethphyId);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SETUP_PERAMBLE(uintptr_t ethphyId, const DRV_MIIM_SETUP* pSetUp)
    {
        if((pSetUp->setupFlags & DRV_MIIM_SETUP_FLAG_PREAMBLE_SUPPRESSED) != 0)
        {
            PLIB_ETH_MIIMNoPreEnable(ethphyId);
        }
        else
        {
            PLIB_ETH_MIIMNoPreDisable(ethphyId);
        }
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_INCREMENT(uintptr_t ethphyId, const DRV_MIIM_SETUP* pSetUp)
    {
        if((pSetUp->setupFlags & DRV_MIIM_SETUP_FLAG_SCAN_ADDRESS_INCREMENT) != 0)
        {
            PLIB_ETH_MIIMScanIncrEnable(ethphyId);
        }
        else
        {
            PLIB_ETH_MIIMScanIncrDisable(ethphyId);
        }
    }

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_ENABLE(uintptr_t ethphyId)
    {
       
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_MNGMNT_PORT_DISABLE(uintptr_t ethphyId)
    {
       
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_BUSY(uintptr_t ethphyId)
    {
        return PLIB_ETH_MIIMIsBusy(ethphyId);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_PHYADDR_SET(uintptr_t ethphyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
       PLIB_ETH_PHYAddressSet(ethphyId, pOpDcpt->phyAdd);
       PLIB_ETH_RegisterAddressSet(ethphyId,pOpDcpt->regIx);
    }
                                    
    static  __inline__ void __attribute__((always_inline))PLIB_ETH_ClearDataValid(uintptr_t ethphyId)
    {
        volatile eth_registers_t * eth = ((eth_registers_t *)(ethphyId));	
        eth->EMACxMINDSET = _EMACxMIND_NOTVALID_MASK;
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_CLEAR_DATA_VALID(uintptr_t ethphyId)
    {
        PLIB_ETH_ClearDataValid(ethphyId);
    }
   
    static  __inline__ DRV_MIIM_TXFER_STAT __attribute__((always_inline))_DRV_MIIM_OP_SCAN_ENABLE(uintptr_t ethphyId)
    {        
        PLIB_ETH_ClearDataValid(ethphyId);
        PLIB_ETH_MIIMScanModeEnable(ethphyId);
        return DRV_MIIM_TXFER_SCAN_STALE;
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_WRITE_DATA(uintptr_t ethphyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        PLIB_ETH_MIIMWriteDataSet(ethphyId, pOpDcpt->opData);
    }
    

    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_WRITE_START(uintptr_t ethphyId)
    {
        PLIB_ETH_MIIMWriteStart(ethphyId);
    }
    
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_OP_READ_START(uintptr_t ethphyId,DRV_MIIM_OP_DCPT* pOpDcpt)
    {
        PLIB_ETH_MIIMReadStart(ethphyId);
    }

    static  __inline__ uint16_t __attribute__((always_inline))_DRV_MIIM_OP_READ_DATA_GET(uintptr_t ethphyId)
    {
        return PLIB_ETH_MIIMReadDataGet(ethphyId);
    }
    
    static  __inline__ bool __attribute__((always_inline))_DRV_MIIM_IS_DATA_VALID(uintptr_t ethphyId)
    {
        return !PLIB_ETH_DataNotValid(ethphyId);
    }
      
    static  __inline__ void __attribute__((always_inline))_DRV_MIIM_SCAN_DIABLE(uintptr_t ethphyId)
    {    
        PLIB_ETH_MIIMScanModeDisable(ethphyId);
    }
    
    static  __inline__ void __attribute__((always_inline)) _DRV_MIIM_SMI_CLOCK_SET(uintptr_t ethphyId, uint32_t hostClock, uint32_t maxMIIMClock )
    {
        int  ix;

        PLIB_ETH_MIIMResetEnable(ethphyId); // Issue MIIM reset
        PLIB_ETH_MIIMResetDisable(ethphyId); // Clear MIIM reset

        for(ix = 0; ix < sizeof(_MIIMClockDivisorTable) / sizeof(*_MIIMClockDivisorTable); ix++)
        {
            if(hostClock / _MIIMClockDivisorTable[ix] <= maxMIIMClock)
            {   // found it
                break;
            }
        }

        if(ix == sizeof(_MIIMClockDivisorTable) / sizeof(*_MIIMClockDivisorTable))
        {
            ix--;   // max divider; best we can do
        }

        PLIB_ETH_MIIMClockSet(ethphyId, ix + 1);  // program the clock*/
    }

#endif  //defined (__PIC32C__)
#endif //#ifndef _DRV_MIIM_MAPPING_H

/*******************************************************************************
 End of File
*/

