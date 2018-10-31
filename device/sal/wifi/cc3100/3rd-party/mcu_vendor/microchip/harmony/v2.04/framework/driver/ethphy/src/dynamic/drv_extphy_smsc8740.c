/*******************************************************************************
  SMSC LAN8740 PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*******************************************************************************
File Name:  drv_extphy_smsc8740.c
Copyright © 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
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
#include "driver/ethphy/src/drv_ethphy_local.h"

#include "driver/ethphy/src/dynamic/drv_extphy_smsc8740.h"


/****************************************************************************
 *                 interface functions
 ****************************************************************************/
#ifdef PHY_WOL_ENABLE
int32_t LEDstate = SYS_USERIO_LED_DEASSERTED;

static void DRV_EXTPHY_WOLWakeupFrameSetup(DRV_HANDLE hClientObj,unsigned char macaddr[]);
static void DRV_EXTPHY_MMDDataWrite(DRV_HANDLE hClientObj,unsigned short regData,eMMDDEVADDRES mmdDevAddr);
static unsigned short DRV_EXTPHY_MMDDataRead(DRV_HANDLE hClientObj,eMMDDEVADDRES mmdDevAddr);
static void DRV_EXTPHY_MMDAddressWrite(DRV_HANDLE hClientObj,unsigned short indexNo,eMMDDEVADDRES mmdDevAddr);
static void DRV_EXTPHY_WOLMACAddressSet(DRV_HANDLE hClientObj,unsigned char *bAddr);
static unsigned short CalculateCrc16(unsigned char * data_p,unsigned long dwLen,bool fBitReverse);
static void LAN8740WOLIsr(void *p);
static void DRV_EXTPHY_WOLWakeupFilterConfiguration(DRV_HANDLE hClientObj,eWOLWakeupFilterSatus filterStatus);
static void DRV_EXTPHY_IntInit(DRV_HANDLE hClientObj,INT_SOURCE src,int intPri, int intSubPri);
static void  DRV_EXTPHY_WOLInterruptMaskSet(DRV_HANDLE hClientObj,int intStaus);
static bool DRV_EXTPHY_WOLWakeUpFrameRXStatusGet(DRV_HANDLE hClientObj,eWOL_WAKEUP_FRAME wuframe);
static void DRV_EXTPHY_WOLWakeUpFrameSet(DRV_HANDLE hClientObj,eWOL_WAKEUP_FRAME wuframe);
static void DRV_EXTPHY_WOLWakeUpFrameClear(DRV_HANDLE hClientObj,eWOL_WAKEUP_FRAME wuframe);
#endif /* PHY_WOL_ENABLE */

/****************************************************************************
 * Function:        DRV_EXTPHY_MIIConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:   		handle - A valid open-instance handle, returned from the driver's open routine   
 *					cFlags - the requested configuration flags: DRV_ETHPHY_CFG_RMII/DRV_ETHPHY_CFG_MII
 *
 * Output:          DRV_ETHPHY_RES_OK - operation completed successfully
 *
 *                  DRV_ETHPHY_RES_PENDING - operation pending; the call needs to be re-issued until
 *                                    DRV_ETHPHY_RES_OK or an error reported
 *
 *
 *                  < 0         - an error occurred and the operation was aborted
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the PHY in one of MII/RMII operation modes.
 *
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_EXTPHY_MIIConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj,DRV_ETHPHY_CONFIG_FLAGS cFlags)
{
    union
    {
        struct
        {
            uint16_t low;
            uint16_t high;
        };
        uint32_t    w;
    }vendorData = {};

    uint16_t    phyReg = 0;
    uint16_t    miiConfPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &vendorData.w);

    if(res < 0)
    {   // some error occurred
        return res;
    }

    miiConfPhase = vendorData.low;

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, &phyAddress);

    switch(miiConfPhase)
    {
        case 0:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_SPECIAL_MODE, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }
            
            // advance to the next phase
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++miiConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 1:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_SPECIAL_MODE result
            // not used bits should be 0
            phyReg &= _SPECIALMODE_PHYAD_MASK | _SPECIALMODE_MODE_MASK;
            if(cFlags & DRV_ETHPHY_CFG_RMII)
            {
                phyReg |= _SPECIALMODE_MIIMODE_MASK;
            }
            else
            {
                phyReg &= ~_SPECIALMODE_MIIMODE_MASK;
            }

            // save value for the next state
            vendorData.low = miiConfPhase + 1;
            vendorData.high = phyReg;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;

        case 2:
            phyReg = vendorData.high;
            // update the Special Modes reg
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_SPECIAL_MODE, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done    
            return DRV_ETHPHY_RES_OK;


        default:
            // shouldn't happen
            return DRV_ETHPHY_RES_OPERATION_ERR; 
    }

}


/****************************************************************************
 * Function:        DRV_EXTPHY_MDIXConfigure
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:           handle - A valid open-instance handle, returned from the driver's open routine
 *					oFlags - the requested open flags: TCPIP_ETH_OPEN_MDIX_AUTO, TCPIP_ETH_OPEN_MDIX_NORM/TCPIP_ETH_OPEN_MDIX_SWAP
 *
 * Output:          DRV_ETHPHY_RES_OK - operation completed successfully
 *
 *                  DRV_ETHPHY_RES_PENDING - operation pending; the call needs to be re-issued until
 *                                    DRV_ETHPHY_RES_OK or an error reported
 *
 *
 *                  < 0         - an error occurred and the operation was aborted
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the MDIX mode for the PHY.
 *
 * Note:            None
 *****************************************************************************/
static DRV_ETHPHY_RESULT DRV_EXTPHY_MDIXConfigure(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE hClientObj, TCPIP_ETH_OPEN_FLAGS oFlags)
{
    union
    {
        struct
        {
            uint16_t low;
            uint16_t high;
        };
        uint32_t    w;
    }vendorData = {};

    uint16_t    phyReg = 0;
    uint16_t    mdixConfPhase = 0;
    int         phyAddress = 0;

    DRV_ETHPHY_RESULT res = pBaseObj->DRV_ETHPHY_VendorDataGet(hClientObj, &vendorData.w);

    if(res < 0)
    {   // some error occurred
        return res;
    }

    mdixConfPhase = vendorData.low;

    pBaseObj->DRV_ETHPHY_PhyAddressGet(hClientObj, &phyAddress);

    switch(mdixConfPhase)
    {
        case 0:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadStart(hClientObj, PHY_REG_SPECIAL_CTRL, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }
            
            // advance to the next phase
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, ++mdixConfPhase);
            return DRV_ETHPHY_RES_PENDING;

        case 1:
            res = pBaseObj->DRV_ETHPHY_VendorSMIReadResultGet(hClientObj, &phyReg);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // got PHY_REG_SPECIAL_CTRL result
            // mask off not used bits
            phyReg &= _SPECIALCTRL_XPOL_MASK;

            if(oFlags & TCPIP_ETH_OPEN_MDIX_AUTO)
            {	// enable Auto-MDIX
                phyReg &= ~_SPECIALCTRL_AMDIXCTRL_MASK;
            }
            else
            {	// no Auto-MDIX
                phyReg |= _SPECIALCTRL_AMDIXCTRL_MASK;	// disable Auto-MDIX
                if(oFlags & TCPIP_ETH_OPEN_MDIX_SWAP)
                {
                    phyReg |= _SPECIALCTRL_CH_SELECT_MASK;	// swap  - MDIX
                }
                else
                {
                    phyReg &= ~_SPECIALCTRL_CH_SELECT_MASK;	// normal - MDI
                }
            }
            // save value for the next state
            vendorData.low = mdixConfPhase + 1;
            vendorData.high = phyReg;
            pBaseObj->DRV_ETHPHY_VendorDataSet(hClientObj, vendorData.w);
            return DRV_ETHPHY_RES_PENDING;
            
        case 2:
            phyReg = vendorData.high;
            // update the PHY_REG_SPECIAL_CTRL Register
            res = pBaseObj->DRV_ETHPHY_VendorSMIWriteStart(hClientObj, PHY_REG_SPECIAL_CTRL, phyReg, phyAddress);
            if(res < 0)
            {   // some error
                return res;
            }
            else if(res == DRV_ETHPHY_RES_PENDING)
            {   // retry
                return DRV_ETHPHY_RES_PENDING;
            }

            // done    
            return DRV_ETHPHY_RES_OK;


        default:
            // shouldn't happen
            return DRV_ETHPHY_RES_OPERATION_ERR; 
    }
}

/****************************************************************************
 * Function:        EthPhyMIIMClock
 *
 * PreCondition:    None
 *
 * Input:           handle - A valid open-instance handle, returned from the driver's open routine
 *
 * Output:          PHY MIIM clock, Hz
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the maximum clock frequency that the PHY can use for the MIIM transactions
 *
 * Note:            None
 *****************************************************************************/
static unsigned int DRV_EXTPHY_SMIClockGet(const DRV_ETHPHY_OBJECT_BASE* pBaseObj, DRV_HANDLE handle)
{
	return 2500000;		//  2.5 MHz max clock supported
}


// the DRV_ETHPHY_OBJECT

const DRV_ETHPHY_OBJECT  DRV_ETHPHY_OBJECT_SMSC_LAN8740 = 
{
    DRV_EXTPHY_MIIConfigure,
    DRV_EXTPHY_MDIXConfigure,
    DRV_EXTPHY_SMIClockGet,
    0,                          // no WOL functionality yet
};



#ifdef PHY_WOL_ENABLE
/****************************************************************************
 * Function:        DRV_EXTPHY_MMDAddressWrite
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					indexNo - Specified Register index value
 *					mmdDevAddr - MMD device register value
 * Output:          none
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to configure the MMD control register for address write. 
 *
 * Note:            None
 *****************************************************************************/
static void DRV_EXTPHY_MMDAddressWrite(DRV_HANDLE hClientObj,unsigned short indexNo,eMMDDEVADDRES mmdDevAddr)
{
    // Write 00b to MMD access control register for Address
	unsigned short	mmdStatus;
    unsigned short mmdRegData;
    
	DRV_ETHPHY_SMIReadStart(hClientObj,PHY_MMD_ACCESS_CONTROL);
	mmdStatus=DRV_ETHPHY_SMIReadResultGet(hClientObj);	
	mmdStatus &= _PHY_MMD_RESERVED_MASK;
    mmdStatus |= _PHY_MMD_CNTL_ACCESS_ADDRESS_MASK;
    // Write 3 for PCS ( for WOL configuration ) and 7 for Auto negotiation and 30 for Vendor specific
    mmdStatus |= mmdDevAddr ;
      
	DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_MMD_ACCESS_CONTROL, mmdStatus);
    
    // write MMD access address/data register with 16 bit address of the desired MMD register
	DRV_ETHPHY_SMIReadStart(hClientObj,PHY_MMD_ACCESS_DATA_ADDR);
	mmdRegData=DRV_ETHPHY_SMIReadResultGet(hClientObj)&0xFFFF;	// mask off not used bits
    mmdRegData =  indexNo;
	DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_MMD_ACCESS_DATA_ADDR, mmdRegData);	
    
}
/****************************************************************************
 * Function:        DRV_EXTPHY_MMDDataRead
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					mmdDevAddr - MMD device register value
 * Output:          unsigned short - 16 bit data read from MMD register.
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to enable the MMD control register for data read.
 *
 * Note:            None
 *****************************************************************************/
static unsigned short DRV_EXTPHY_MMDDataRead(DRV_HANDLE hClientObj,eMMDDEVADDRES mmdDevAddr)
{
    // Write 01b to MMD access control register for Data
	unsigned short	mmdStatus;
    unsigned short mmdRegData;
    
	DRV_ETHPHY_SMIReadStart(hClientObj,PHY_MMD_ACCESS_CONTROL);
	mmdStatus=DRV_ETHPHY_SMIReadResultGet(hClientObj);	
	mmdStatus &= _PHY_MMD_RESERVED_MASK;
    mmdStatus |= _PHY_MMD_CNTL_ACCESS_DATA_MASK;
    // Write 3 for PCS ( for WOL configuration ) and 7 for Auto negotiation and 30 for Vendor specific
    mmdStatus |= mmdDevAddr ;
      
	DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_MMD_ACCESS_CONTROL, mmdStatus);
    
    // write MMD access address/data register with 16 bit address of the desired MMD register
	DRV_ETHPHY_SMIReadStart(hClientObj,PHY_MMD_ACCESS_DATA_ADDR);
	mmdRegData=DRV_ETHPHY_SMIReadResultGet(hClientObj);	

    return mmdRegData;
    
}

/****************************************************************************
 * Function:        DRV_EXTPHY_MMDDataWrite
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					mmdDevAddr - MMD device register value
 *					regData - Data need to be written
 * Output:          void
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to write the desired data with the configurations of MMD register.
 *
 * Note:            None
 *****************************************************************************/
static void DRV_EXTPHY_MMDDataWrite(DRV_HANDLE hClientObj,unsigned short regData,eMMDDEVADDRES mmdDevAddr)
{
    // Write 01b to MMD access control register for Data
	unsigned short	mmdStatus;
    unsigned short mmdRegData;
    
	DRV_ETHPHY_SMIReadStart(hClientObj,PHY_MMD_ACCESS_CONTROL);
	mmdStatus=DRV_ETHPHY_SMIReadResultGet(hClientObj);	
	mmdStatus &= _PHY_MMD_RESERVED_MASK;
    mmdStatus |= _PHY_MMD_CNTL_ACCESS_DATA_MASK;
    // Write 3 for PCS ( for WOL configuration ) and 7 for Auto negotiation and 30 for Vendor specific
    mmdStatus |= mmdDevAddr ;
      
	DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_MMD_ACCESS_CONTROL, mmdStatus);
    
    // write MMD access address/data register with 16 bit address of the desired MMD register
	DRV_ETHPHY_SMIReadStart(hClientObj,PHY_MMD_ACCESS_DATA_ADDR);
	mmdRegData=DRV_ETHPHY_SMIReadResultGet(hClientObj)&0xFFFF;	// mask off not used bits
    mmdRegData =  regData;
	DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_MMD_ACCESS_DATA_ADDR, mmdRegData);	
  
    
}

/****************************************************************************
 * Function:        CalculateCrc16
 *
 * PreCondition:    None
 *
 * Input:           data_p - Data for CRC16 calculation
 *					dwLen - Length of data
 *					fBitReverse - Boolean value . If required for Bit reverse
 * Output:          unsigned short - CRC16 value
 *
 *
 * Side Effects:    None
 *
 * Overview:        CRC calucation for 16 bit wide and it is used for WOL CRC calculation.
 *					This is the CCITT CRC 16 polynomial x^16 + x^15 + x^2 + 1.
 * 					This works out to be 0x1021, but the way the algorithm work
 *					lets us use 0x8005 . 
 *
 * Note:            None
 *****************************************************************************/

 static unsigned short CalculateCrc16(unsigned char * data_p,unsigned long dwLen,bool fBitReverse)
{
    unsigned short i, j, bit1;
    unsigned short wCrc = 0xFFFFU;
    unsigned short wMsb;
    unsigned char bCurrentByte;
    unsigned short wNumOfBits = 16U;
    unsigned short wCrcOut=0;

    for (i=0; i<(unsigned short)dwLen; i++)
    {
        bCurrentByte = *data_p++;
        for (bit1=(unsigned short)0U; bit1<(unsigned short)8U; bit1++)
        {
            wMsb = wCrc >> 15;
            wCrc <<= 1;
            if (wMsb ^ (unsigned short)(bCurrentByte & 1))
            {
                wCrc ^= WOl_CRC_POLY;
                wCrc |= (unsigned short)0x0001U;
            }
            bCurrentByte >>= 1;
        }
    }
    /* bit reverse if needed */
    if (fBitReverse)
    {
        j = 1;
        for (i=(unsigned short)(1<<(wNumOfBits-(unsigned short)1U)); i; i = i>>1) 
        {
            if (wCrc & i)
            {
                wCrcOut |= j;
            }
            j <<= 1;
        }
        wCrc = wCrcOut;
    }
    return wCrc;
}

/****************************************************************************
 * Function:        DRV_EXTPHY_WOLWakeupFrameSetup
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					macaddr - 6 byte MAC address which will be used for WOL support

 * Output:          void
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to configure for the WOL wakeup frame.
 *
 * Note:            None
 *****************************************************************************/
static void DRV_EXTPHY_WOLWakeupFrameSetup(DRV_HANDLE hClientObj,unsigned char macaddr[])
{
    unsigned char  pattern[WOL_WAKEUP_FRAME_PATTERN_LEN] = WOL_WAKEUP_FRAME_PATTERN;
    unsigned short  byteMask[WOL_WAKEUP_FRAME_BYTEMASK_SIZE]=
        {WOL_WAKEUP_BYTEMASK_REG0,WOL_WAKEUP_BYTEMASK_REG1,WOL_WAKEUP_BYTEMASK_REG2,WOL_WAKEUP_BYTEMASK_REG3,
        WOL_WAKEUP_BYTEMASK_REG4,WOL_WAKEUP_BYTEMASK_REG5,WOL_WAKEUP_BYTEMASK_REG6,WOL_WAKEUP_BYTEMASK_REG7
        };
    unsigned short  u16data=0;
    unsigned short  crc16=0;
    unsigned long   patternOffset=0;
    unsigned short  addrMatchBits = WAKEUP_FILTER_NONE;
    
    int i=0;
    
    /*1. Calculate CRC of pattern buffer */
    crc16 = CalculateCrc16(pattern, WOL_WAKEUP_FRAME_PATTERN_LEN, false);
    
    /* Pattern offset */
    patternOffset = WOL_WAKEUP_FRAME_START_OFFSET;

    //2. update wakeup filter Mask registers ( WUF_MASK)
    for(i=0;i<WOL_WAKEUP_FRAME_BYTEMASK_SIZE;i++)
    {
        // write address and index -
        DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_FILTER_BYTEMASK_1-i,MMD_PCS);
        // Write  Data Write - 
        DRV_EXTPHY_MMDDataWrite(hClientObj,byteMask[i],MMD_PCS);
    }

    // 3. write CRC value to WUF_CFGB
    // write address and index -
    DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_FILTER_CRC_REGISTER_B,MMD_PCS);
    // Write  Data Write - 
    DRV_EXTPHY_MMDDataWrite(hClientObj,crc16,MMD_PCS);

    //4. Update Offset in WUF_CFGA
    // write address and index -
    DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_FILTER_REGISTER_A,MMD_PCS);
    //read the Data from that address
    u16data = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS)&(~_WOL_WAKEUP_PATTERN_OFFSET_MASK);
    // Write Data Write - 
    u16data = u16data | (patternOffset&0xFF);
    DRV_EXTPHY_MMDDataWrite(hClientObj,u16data,MMD_PCS);

    //5. Add Wake up filter
    if(addrMatchBits & UNICAST_ADDRESS_MATCH)
    {
        // Set MAC address 
        DRV_EXTPHY_WOLMACAddressSet(hClientObj,macaddr);
        // write address and index -
        DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_FILTER_REGISTER_A,MMD_PCS);
        //read the Data from that address  and disable other filters
        u16data = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS)& ~(MULTICAST_ADDRESS_MATCH|BROADCAST_ADDRESS_MATCH);
        // Write Data Write - 
        u16data = u16data | UNICAST_ADDRESS_MATCH;
        DRV_EXTPHY_MMDDataWrite(hClientObj,u16data,MMD_PCS);
        
    }

    
    if(addrMatchBits & BROADCAST_ADDRESS_MATCH)
    {
        // write address and index -
        DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_FILTER_REGISTER_A,MMD_PCS);
        //read the Data from that address  and disable other filters
        u16data = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS)& ~(MULTICAST_ADDRESS_MATCH|UNICAST_ADDRESS_MATCH);
        // Write Data Write - 
        u16data = u16data | BROADCAST_ADDRESS_MATCH ;
        DRV_EXTPHY_MMDDataWrite(hClientObj,u16data,MMD_PCS);
        
    }

    
    if(addrMatchBits & MULTICAST_ADDRESS_MATCH)
    {
        // write address and index -
        DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_FILTER_REGISTER_A,MMD_PCS);
        //read the Data from that address  and disable other filters
        u16data = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS)& ~(BROADCAST_ADDRESS_MATCH|UNICAST_ADDRESS_MATCH);
        // Write Data Write - 
        u16data = u16data | MULTICAST_ADDRESS_MATCH ;
        DRV_EXTPHY_MMDDataWrite(hClientObj,u16data,MMD_PCS);
        
    }

    // 6. Enable filter
    DRV_EXTPHY_WOLWakeupFilterConfiguration(hClientObj,WOL_WAKEUP_FILTER_ENA);

    //7.

}

/****************************************************************************
 * Function:        DRV_EXTPHY_WOLWakeupFilterConfiguration
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					filterStatus - Enable or Disable WOL wake up frame Filter

 * Output:          void
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to enable or disable for the WOL wakeup frame filter.
 *
 * Note:            None
 *****************************************************************************/
static void DRV_EXTPHY_WOLWakeupFilterConfiguration(DRV_HANDLE hClientObj,eWOLWakeupFilterSatus filterStatus)
{
    unsigned short  u16data=0;
    if(filterStatus == WOL_WAKEUP_FILTER_ENA)
    {
        // write address and index -
        DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_FILTER_REGISTER_A,MMD_PCS);
        //read the Data from that address  
        u16data = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS);
        // Write Data Write - 
        u16data = u16data | _WOL_WAKEUP_FILTER_ENABLE;
        DRV_EXTPHY_MMDDataWrite(hClientObj,u16data,MMD_PCS);
    }
    else
    {
        // write address and index -
        DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_FILTER_REGISTER_A,MMD_PCS);
        //read the Data from that address  
        u16data = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS);
        // Write Data Write - 
        u16data = u16data & ~_WOL_WAKEUP_FILTER_ENABLE;
        DRV_EXTPHY_MMDDataWrite(hClientObj,u16data,MMD_PCS);
    }
}
/*
void WOLMACAddressSet(uint8_t *bAddr)
{
    unsigned short u16dat=0;
    
    // write MAC ADDRESS A address and index -
    DRV_EXTPHY_MMDAddressWrite((unsigned short)WOL_MAC_ADDRESS_A,MMD_PCS);
    u16dat = ( ((*bAddr)<<8) | (*(bAddr+1)) );
    // Write MAC Address A Data Write - 
    DRV_EXTPHY_MMDDataWrite(u16dat,MMD_PCS);

    bAddr+=2;
    // write MAC ADDRESS B address and index -
    DRV_EXTPHY_MMDAddressWrite((unsigned short)WOL_MAC_ADDRESS_B,MMD_PCS);
    u16dat = ( ((*bAddr)<<8) | (*(bAddr+1)) );
    // Write MAC Address B Data Write - 
    DRV_EXTPHY_MMDDataWrite(u16dat,MMD_PCS);

    bAddr+=2;
    // write MAC ADDRESS C address and index -
    DRV_EXTPHY_MMDAddressWrite((unsigned short)WOL_MAC_ADDRESS_C,MMD_PCS);
    u16dat = ( ((*bAddr)<<8) | (*(bAddr+1)) );
    // Write MAC Address C Data Write - 
    DRV_EXTPHY_MMDDataWrite(u16dat,MMD_PCS);

}
*/

/****************************************************************************
 * Function:        DRV_EXTPHY_WOLMACAddressSet
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					bAddr - Mac Address

 * Output:          void
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to set the macaddress for the WOL .
 *
 * Note:            None
 *****************************************************************************/
static void DRV_EXTPHY_WOLMACAddressSet(DRV_HANDLE hClientObj,unsigned char *bAddr)
{
    unsigned short u16dat=0;

    union
    {
        double          align;          // 8 bytes aligned
        unsigned char   addr[6];        // MAC address, network order
    }uA;       // aligned MAC address
    
    memcpy(uA.addr, bAddr, sizeof(uA.addr)); // align properly
    unsigned short* pS=(unsigned short*)uA.addr;
    unsigned short macAddr01Byte = *pS++;
    unsigned short macAddr23Byte = *pS++;    
    unsigned short macAddr45Byte = *pS;
    // write MAC ADDRESS A address and index -
    DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_MAC_ADDRESS_A,MMD_PCS);
    // Write MAC Address A Data Write - 
    DRV_EXTPHY_MMDDataWrite(hClientObj,macAddr45Byte,MMD_PCS);
    u16dat = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS);

    // write MAC ADDRESS B address and index -
    DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_MAC_ADDRESS_B,MMD_PCS);
    // Write MAC Address B Data Write - 
    DRV_EXTPHY_MMDDataWrite(hClientObj,macAddr23Byte,MMD_PCS);
    u16dat = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS);
    // write MAC ADDRESS C address and index -
    DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_MAC_ADDRESS_C,MMD_PCS);
    // Write MAC Address C Data Write - 
    DRV_EXTPHY_MMDDataWrite(hClientObj,macAddr01Byte,MMD_PCS);
    u16dat = DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS);
}

/****************************************************************************
 * Function:        DRV_EXTPHY_WOLWakeUpFrameSet
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					wuframe - Set WOL wake up frame

 * Output:          void
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to set the wakeup frame for the WOL .
 *
 * Note:            None
 *****************************************************************************/
static void DRV_EXTPHY_WOLWakeUpFrameSet(DRV_HANDLE hClientObj,eWOL_WAKEUP_FRAME wuframe)
{
    unsigned short wolContrlval=0;
    
    // write  address and index -
    DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_CONTROL_STATUS,MMD_PCS);
    wolContrlval =  DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS);
    switch(wuframe)
    {
        case PERFECT_DA_FRAME : 
            wolContrlval |= _WOL_DA_WAKEUP_FRAME_ENABLE_MASK;
            break;
        case REMOTE_WAKEUP_FRAME:
            wolContrlval |= _WOL_WAKEUP_FRAME_ENABLE_MASK;
            break;
        case MAGIC_PKT_RECEIVED:
            wolContrlval |= _WOL_MAGIC_PKT_ENABLE_MASK;
            break;
        case BROADCAST_FRAME_RCVD:
            wolContrlval |= _WOL_BROADCAST_WAKEU_ENABLE_MASK;
            break;
    }
    // Write Data  - 
    DRV_EXTPHY_MMDDataWrite(hClientObj,wolContrlval,MMD_PCS);
}

/****************************************************************************
 * Function:        DRV_EXTPHY_WOLWakeUpFrameClear
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					wuframe - Clear WOL wake up frame

 * Output:          void
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to clear the wakeup frame for the WOL .
 *
 * Note:            None
 *****************************************************************************/
static void DRV_EXTPHY_WOLWakeUpFrameClear(DRV_HANDLE hClientObj,eWOL_WAKEUP_FRAME wuframe)
{
    unsigned short wolContrlval=0;
    
    // write address and index -
    DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_CONTROL_STATUS,MMD_PCS);
    wolContrlval =  DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS);
    switch(wuframe)
    {
        case PERFECT_DA_FRAME : 
            wolContrlval &= ~_WOL_DA_WAKEUP_FRAME_ENABLE_MASK;
            break;
        case REMOTE_WAKEUP_FRAME:
            wolContrlval &= ~_WOL_WAKEUP_FRAME_ENABLE_MASK;
            break;
        case MAGIC_PKT_RECEIVED:
            wolContrlval &= ~_WOL_MAGIC_PKT_ENABLE_MASK;
            break;
        case BROADCAST_FRAME_RCVD:
            wolContrlval &= ~_WOL_BROADCAST_WAKEU_ENABLE_MASK;
            break;
    }
    // Write Data  - 
    DRV_EXTPHY_MMDDataWrite(hClientObj,wolContrlval,MMD_PCS);
}

/****************************************************************************
 * Function:        DRV_EXTPHY_WOLWakeUpFrameRXStatusGet
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *					wuframe - Get WOL wake up frame RX status

 * Output:          bool
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to check the wakeup frame RX status for the WOL .
 *
 * Note:            None
 *****************************************************************************/
static bool DRV_EXTPHY_WOLWakeUpFrameRXStatusGet(DRV_HANDLE hClientObj,eWOL_WAKEUP_FRAME wuframe)
{
    unsigned short wolContrlval=0;
    
    // write  address and index -
    DRV_EXTPHY_MMDAddressWrite(hClientObj,(unsigned short)WOL_WAKEUP_CONTROL_STATUS,MMD_PCS);
    wolContrlval =  DRV_EXTPHY_MMDDataRead(hClientObj,MMD_PCS);
    switch(wuframe)
    {
        case PERFECT_DA_FRAME : 
            return (wolContrlval & _WOL_DA_FRAME_RECEIVED_MASK) ? true : false;
        case REMOTE_WAKEUP_FRAME:
            return (wolContrlval & _WOL_REMOTE_WAKEUP_FRAME_RECEIVED_MASK) ? true : false;
        case MAGIC_PKT_RECEIVED:
            return (wolContrlval & _WOL_MAGIC_PACKET_RECEIVED_MASK) ? true : false;
        case BROADCAST_FRAME_RCVD:
            return (wolContrlval & _WOL_BROADCAST_FRAME_RECEIVED_MASK) ? true : false;
    }
    return false;
}

/****************************************************************************
 * Function:        DRV_EXTPHY_WOLInterruptMaskSet
 *
 * PreCondition:    None
 *
 * Input:           hClientObj - A valid open-instance handle, returned from the driver's open routine
 *			   intStaus - Set interrpu status.

 * Output:          bool
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to set the WOL interrupt  .
 *
 * Note:            None
 *****************************************************************************/

static void  DRV_EXTPHY_WOLInterruptMaskSet(DRV_HANDLE hClientObj,int intStaus)
{
	unsigned short	intmask;
    
	DRV_ETHPHY_SMIReadStart(hClientObj,PHY_REG_INT_MASK);
	intmask=DRV_ETHPHY_SMIReadResultGet(hClientObj)&(~0xFE01);	
    if(intStaus == WOL_INT_ENABLED)
    {
        intmask |= WOL_INT8_EN;
    }
    else
    {
        intmask &= ~WOL_INT8_EN;
    }
	DRV_ETHPHY_SMIWriteStart(hClientObj,PHY_REG_INT_MASK, intmask);
}

/****************************************************************************
 * Function:        DRV_EXTPHY_IntInit
 *
 * PreCondition:    EthInit and EthPhyInit should have been called.
 *
 * Input:         hClientObj - A valid open-instance handle, returned from the driver's open routine
 *                  src - interrupt source
 *                  intPri - intrrupt priority
 *                  intSubPri - Interuupt Sub Priority
 *
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       Initialize External Interrupt 3.	One can take this as refreence for their WOL 
 *                      Interrupt.
 *
 * Note:            None
 *****************************************************************************/
static void DRV_EXTPHY_IntInit(DRV_HANDLE hClientObj,INT_SOURCE src,int intPri, int intSubPri)
{
	// set up the External Interrupt 3 with a prioirty of 5 and 1 sub-priority    

    SYS_INT_SourceDisable(src);      // stop Eth ints
    SYS_INT_SourceStatusClear(src);
    
    SYS_INT_VectorPrioritySet(src, intPri);
    SYS_INT_VectorSubprioritySet(src, intSubPri);
    SYS_INT_DynamicRegisterPrvt(src, LAN8740WOLIsr, (void*)&hClientObj);
    
    SYS_INT_SourceEnable(src);
}



static void LAN8740WOLIsr(void *p)
{
    DRV_HANDLE hClientObj = (DRV_HANDLE)p;

    eWOL_WAKEUP_FRAME wuframe = REMOTE_WAKEUP_FRAME;
    //eWOL_WAKEUP_FRAME wuframe = MAGIC_PKT_RECEIVED;
    // eWOL_WAKEUP_FRAME wuframe = BROADCAST_FRAME_RCVD;    
    //eWOL_WAKEUP_FRAME wuframe = PERFECT_DA_FRAME;
    bool wol_status;
    wol_status = DRV_EXTPHY_WOLWakeUpFrameRXStatusGet(hClientObj,wuframe);
   
    INT_SOURCE src = DRV_ETHPHY_INTERRUPT_SOURCE;

    LEDstate ^= SYS_USERIO_LED_ASSERTED;
    SYS_USERIO_SetLED(SYS_USERIO_LED_1, LEDstate);
    DRV_EXTPHY_WOLInterruptMaskSet(hClientObj,WOL_INT_DISABLED);
    // clear the WOL wake up frame
    DRV_EXTPHY_WOLWakeUpFrameClear(hClientObj,wuframe);
    SYS_INT_SourceDisable(src);
}

/****************************************************************************
 * Function:        DRV_EXTPHY_WOLConfiguration
 *
 * PreCondition:    EthInit and EthPhyInit should have been called.
 *
 * Input:          hClientObj - A valid open-instance handle, returned from the driver's open routine
 *                    bAddr[] -  Source Mac Address , or a Magic Packet MAC address
 *
 * Output:         none
 *
 *
 * Side Effects:    None
 *
 * Overview:       Configure WOL for SMSC8740 with a Source MAC address or a 6 byte magic packet mac address.
 *                      wuFrame = Frame type need to be set.
 *
 * Note:            TODO: This function needs to be converted to the non-blocking Harmony model!
 *                      
 *****************************************************************************/
void  DRV_EXTPHY_WOLConfiguration(DRV_HANDLE hClientObj,unsigned char bAddr[])
{
    unsigned char bAddr1[6] = {0x11,0x22,0x33,0x44,0x55,0x66};
    //eWOL_WAKEUP_FRAME wuframe = REMOTE_WAKEUP_FRAME;
    eWOL_WAKEUP_FRAME wuframe = MAGIC_PKT_RECEIVED;
    // eWOL_WAKEUP_FRAME wuframe = BROADCAST_FRAME_RCVD;
    //eWOL_WAKEUP_FRAME wuframe = PERFECT_DA_FRAME;
    
    INT_SOURCE src = DRV_ETHPHY_INTERRUPT_SOURCE;

    DRV_EXTPHY_WOLWakeUpFrameClear(hClientObj,PERFECT_DA_FRAME);
    DRV_EXTPHY_WOLWakeUpFrameClear(hClientObj,MAGIC_PKT_RECEIVED);
    DRV_EXTPHY_WOLWakeUpFrameClear(hClientObj,REMOTE_WAKEUP_FRAME);
    DRV_EXTPHY_WOLWakeUpFrameClear(hClientObj,BROADCAST_FRAME_RCVD);
    
    DRV_EXTPHY_WOLMACAddressSet(hClientObj,bAddr1);
    if(wuframe == REMOTE_WAKEUP_FRAME)
    {
        DRV_EXTPHY_WOLWakeupFrameSetup(hClientObj,bAddr);
    }
    DRV_EXTPHY_WOLWakeUpFrameSet(hClientObj,wuframe);
    DRV_EXTPHY_WOLInterruptMaskSet(hClientObj,WOL_INT_ENABLED);
	DRV_EXTPHY_IntInit(hClientObj,src,3,0);
}

#endif /* PHY_WOL_ENABLE */

