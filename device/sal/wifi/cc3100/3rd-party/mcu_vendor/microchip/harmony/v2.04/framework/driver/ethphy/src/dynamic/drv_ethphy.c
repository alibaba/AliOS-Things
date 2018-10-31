/*******************************************************************************
  Ethernet PHY Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ethphy.c

  Summary:
    Ethernet PHY Device Driver Dynamic Implementation

  Description:
    The Ethernet PHY device driver provides a simple interface to manage the
    Ethernet PHY modules on Microchip microcontrollers.  This file Implements
    the core interface routines for the Ethernet PHY driver.

    While building the driver from source, ALWAYS use this file in the build.
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


// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "driver/ethphy/src/drv_ethphy_local.h"

#include "system/clk/sys_clk.h"
#include "system/tmr/sys_tmr.h"
#include "system/debug/sys_debug.h"
#include "system/console/sys_console.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************


// Local Definitions
//
#define PROT_802_3  0x01    // IEEE 802.3 capability
#define MAC_COMM_CPBL_MASK  (_BMSTAT_BASE10T_HDX_MASK|_BMSTAT_BASE10T_FDX_MASK|_BMSTAT_BASE100TX_HDX_MASK|_BMSTAT_BASE100TX_FDX_MASK)
// all comm capabilities our MAC supports

// *****************************************************************************
/* Function:        mRegIxPhyAddToEMACxMADR

   PreCondition:    rIx a valid PHY register, 0-31
                    phyAdd a valid PHY address, 0-31

   Input:           rIx:    PHY register to be accessed
                    phyAdd: PHY to be accessed

   Output:          None

   Side Effects:    None

   Overview:        This macro converts a register index and PHY address to a EMACxMADR format;

   Note:            None
*/
// From eth_miim_access.c:
#define  mRegIxPhyAddToEMACxMADR(rIx, phyAdd)  ((rIx)<<_EMACxMADR_REGADDR_POSITION)|((phyAdd)<<_EMACxMADR_PHYADDR_POSITION)


// From eth_miim_access.c:
static const short _MIIClockDivisorTable[]=
{
    4, 6, 8, 10, 14, 20, 28, 40, 
#if defined (__PIC32MZ__)
    48, 50,
#endif  // defined (__PIC32MZ__)
};  // divider values for the Host clock

// local prototypes
// debug
#if ((DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC) != 0)
volatile int _PhyStayAssertLoop = 0;
static void _PhyAssertCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PHY Assert: %s, in line: %d, \r\n", message, lineNo);
        while(_PhyStayAssertLoop != 0);
    }
}
// a debug condition, not really assertion
volatile int _PhyStayCondLoop = 0;
static void _PhyDebugCond(bool cond, const char* message, int lineNo)
{
    if(cond == false)
    {
        SYS_CONSOLE_PRINT("PHY Cond: %s, in line: %d, \r\n", message, lineNo);
        while(_PhyStayCondLoop != 0);
    }
}

#else
#define _PhyAssertCond(cond, message, lineNo)
#define _PhyDebugCond(cond, message, lineNo)
#endif  // (DRV_PHY_DEBUG_LEVEL & DRV_PHY_DEBUG_MASK_BASIC)

typedef void (*_DRV_PHY_OperPhaseF)(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void _DRV_ETHPHY_ClientOpNone(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpSetup(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpLinkStat(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpNegComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpNegResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpNegRestart(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpSmiRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpSmiWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ClientOpSmiScan(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const _DRV_PHY_OperPhaseF _DRV_PHY_ClientOpTbl[] =
{
    _DRV_ETHPHY_ClientOpNone,           // DRV_ETHPHY_CLIENT_OP_TYPE_NONE 
    _DRV_ETHPHY_ClientOpSetup,          // DRV_ETHPHY_CLIENT_OP_TYPE_SETUP
    _DRV_ETHPHY_ClientOpLinkStat,       // DRV_ETHPHY_CLIENT_OP_TYPE_LINK_STAT
    _DRV_ETHPHY_ClientOpNegComplete,    // DRV_ETHPHY_CLIENT_OP_TYPE_NEG_COMPLETE
    _DRV_ETHPHY_ClientOpNegResult,      // DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESULT
    _DRV_ETHPHY_ClientOpNegRestart,     // DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESTART
    _DRV_ETHPHY_ClientOpReset,          // DRV_ETHPHY_CLIENT_OP_TYPE_RESET
    _DRV_ETHPHY_ClientOpSmiRead,        // DRV_ETHPHY_CLIENT_OP_SMI_READ
    _DRV_ETHPHY_ClientOpSmiWrite,       // DRV_ETHPHY_CLIENT_OP_SMI_WRITE
    _DRV_ETHPHY_ClientOpSmiScan,        // DRV_ETHPHY_CLIENT_OP_SMI_SCAN
};


static void _DRV_ETHPHY_SetupPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseDetect(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate(DRV_ETHPHY_CLIENT_OBJ * hClientObj);



static const _DRV_PHY_OperPhaseF _DRV_PHY_SetupPhasesTbl[] = 
{
    _DRV_ETHPHY_SetupPhaseIdle,
    _DRV_ETHPHY_SetupPhaseDetect,
    _DRV_ETHPHY_SetupPhaseReset,
    _DRV_ETHPHY_SetupPhaseNegotiate,
};


static void _DRV_ETHPHY_LinkStatPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_LinkStatPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_LinkStatPhaseRefresh(DRV_ETHPHY_CLIENT_OBJ * hClientObj);



static const _DRV_PHY_OperPhaseF _DRV_PHY_LinkStatPhasesTbl[] = 
{
    _DRV_ETHPHY_LinkStatPhaseIdle,
    _DRV_ETHPHY_LinkStatPhaseRead,
    _DRV_ETHPHY_LinkStatPhaseRefresh,
};


static void _DRV_ETHPHY_NegCompletePhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseRead1(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseRead2(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseResultNoWait(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseAN_Restart(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegCompletePhaseAN_Complete(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void _DRV_PHY_NegCompleteSetOperResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t phyBmConReg, uint16_t phyStatReg);

static const _DRV_PHY_OperPhaseF _DRV_PHY_NegCompletePhasesTbl[] = 
{
    _DRV_ETHPHY_NegCompletePhaseIdle,
    _DRV_ETHPHY_NegCompletePhaseRead1,
    _DRV_ETHPHY_NegCompletePhaseRead2,
    _DRV_ETHPHY_NegCompletePhaseResultNoWait,
    _DRV_ETHPHY_NegCompletePhaseAN_Restart,
    _DRV_ETHPHY_NegCompletePhaseAN_Complete,
};


static void _DRV_ETHPHY_NegResultPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_BMSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANEXP(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANLPAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegResultPhase_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static void _DRV_PHY_NegResultSetOperResult(DRV_ETHPHY_CLIENT_OBJ* hClientObj, DRV_ETHPHY_LINK_STATUS linkStatus,
                                    TCPIP_ETH_OPEN_FLAGS openFlags, TCPIP_ETH_PAUSE_TYPE pauseType, DRV_ETHPHY_RESULT res);

static const _DRV_PHY_OperPhaseF _DRV_PHY_NegResultPhasesTbl[] = 
{
    _DRV_ETHPHY_NegResultPhaseIdle,
    _DRV_ETHPHY_NegResultPhase_BMSTAT,
    _DRV_ETHPHY_NegResultPhase_ANEXP,
    _DRV_ETHPHY_NegResultPhase_ANLPAD,
    _DRV_ETHPHY_NegResultPhase_ANAD,
};

static void _DRV_ETHPHY_NegRestartPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegRestartPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_NegRestartPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const _DRV_PHY_OperPhaseF _DRV_PHY_NegRestartPhasesTbl[] = 
{
    _DRV_ETHPHY_NegRestartPhaseIdle,
    _DRV_ETHPHY_NegRestartPhaseRead,
    _DRV_ETHPHY_NegRestartPhaseWrite,
};

static void _DRV_ETHPHY_ResetPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ResetPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_ResetPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj);

static const _DRV_PHY_OperPhaseF _DRV_PHY_ResetPhasesTbl[] = 
{
    _DRV_ETHPHY_ResetPhaseIdle,
    _DRV_ETHPHY_ResetPhaseWrite,
    _DRV_ETHPHY_ResetPhaseRead,
};

// local functions
//

static DRV_ETHPHY_LINK_STATUS _Phy2LinkStat(__BMSTATbits_t phyStat)
{
    DRV_ETHPHY_LINK_STATUS  linkStat;

    linkStat = (phyStat.LINK_STAT)?DRV_ETHPHY_LINK_ST_UP:DRV_ETHPHY_LINK_ST_DOWN;
    if(phyStat.REM_FAULT)
    {
        linkStat|=DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
    }

    return linkStat;

}

#if (DRV_ETHPHY_USE_DRV_MIIM)
// TODO aa: apparently this op never failed before...
// well, that's changed; smth needs to be done about it! but it's not easy...
static DRV_ETHPHY_SMI_TXFER_OP_STATUS _DRV_PHY_SMITransferDo(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_MIIM_OPERATION_HANDLE miimOpHandle;
    DRV_MIIM_RESULT miimRes;
    DRV_ETHPHY_SMI_TXFER_OP_STATUS newTxferStatus;
    DRV_MIIM_OPERATION_FLAGS opFlags;
    uint16_t  opData;
    DRV_ETHPHY_SMI_XFER_TYPE txType = hClientObj->smiTxferType;

    switch(hClientObj->smiTxferStatus)
    {
        case DRV_ETHPHY_SMI_TXFER_OP_START:

            if(hClientObj->miimOpHandle != 0)
            {   // no operations queued, need to wait the end of the previous one
                return DRV_ETHPHY_SMI_TXFER_OP_START;
            }

            if(txType == DRV_ETHPHY_SMI_XFER_TYPE_READ)
            {   // start read
                miimOpHandle = hClientObj->pMiimBase->DRV_MIIM_Read(hClientObj->miimHandle, hClientObj->smiRIx, hClientObj->smiPhyAddress, DRV_MIIM_OPERATION_FLAG_NONE, &miimRes);
                newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_BUSY_COMPLETE;
            }
            else if(txType == DRV_ETHPHY_SMI_XFER_TYPE_WRITE || txType == DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE)
            {   // start write
                if(txType == DRV_ETHPHY_SMI_XFER_TYPE_WRITE)
                {
                    opFlags = DRV_MIIM_OPERATION_FLAG_DISCARD;
                    newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_DONE;
                }
                else
                {
                    opFlags = DRV_MIIM_OPERATION_FLAG_NONE;
                    newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_BUSY_COMPLETE;
                }

                miimOpHandle = hClientObj->pMiimBase->DRV_MIIM_Write(hClientObj->miimHandle, hClientObj->smiRIx, hClientObj->smiPhyAddress, hClientObj->smiData, opFlags, &miimRes);
            }
            else
            {   // no scan operations allowed that take complete control of DRV_MIIM!
                _PhyAssertCond(true, __func__, __LINE__);
                miimOpHandle = 0;
            }

            if(miimOpHandle == 0)
            {   // operation failed
                _PhyDebugCond(true, __func__, __LINE__);
                newTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_DONE;
            }
            else if(newTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_DONE) 
            {
                hClientObj->miimOpHandle = miimOpHandle;
            }

            return (hClientObj->smiTxferStatus = newTxferStatus);


        case DRV_ETHPHY_SMI_TXFER_OP_BUSY_COMPLETE:
            miimRes = DRV_MIIM_OperationResult(hClientObj->miimHandle, hClientObj->miimOpHandle, &opData);
            if(miimRes == DRV_MIIM_RES_PENDING)
            {   // wait op to complete
                return DRV_ETHPHY_SMI_TXFER_OP_BUSY_COMPLETE;
            }

            if(miimRes < 0)
            {   // some error has occurred
                _PhyDebugCond(true, __func__, __LINE__);
            }
            else if(txType == DRV_ETHPHY_SMI_XFER_TYPE_READ)
            {
                hClientObj->smiData = opData; // set the read result
            }

            hClientObj->miimOpHandle = 0;  // operation done
            return (hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_DONE);

        default :   // DRV_ETHPHY_SMI_TXFER_OP_NONE, DRV_ETHPHY_SMI_TXFER_OP_DONE
            return hClientObj->smiTxferStatus;
    }
}
#else
static DRV_ETHPHY_SMI_TXFER_OP_STATUS _DRV_PHY_SMITransferDo(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    ETH_MODULE_ID ethphyId = hClientObj->ethphyId;

    switch(hClientObj->smiTxferStatus)
    {
        case DRV_ETHPHY_SMI_TXFER_OP_START:

            if( PLIB_ETH_MIIMIsBusy(ethphyId) )
            {   // some previous operation; wait
                return DRV_ETHPHY_SMI_TXFER_OP_START;
            }
            if(hClientObj->hDriver->busInUse != 0)
            {   // if this operation is not queued, need to wait the end of the previous one
                return DRV_ETHPHY_SMI_TXFER_OP_START;
            }

            hClientObj->hDriver->busInUse = 1;  // mark as busy
            PLIB_ETH_PHYAddressSet(ethphyId, hClientObj->smiPhyAddress);
            PLIB_ETH_RegisterAddressSet(ethphyId,hClientObj->smiRIx);

            if(hClientObj->smiTxferType == DRV_ETHPHY_SMI_XFER_TYPE_WRITE)
            {
                PLIB_ETH_MIIMWriteDataSet(ethphyId, hClientObj->smiData);
                // no need to wait for write completion
                hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_DONE;
                hClientObj->hDriver->busInUse = 0;  // clear
            }
            else if(hClientObj->smiTxferType == DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE)
            {
                PLIB_ETH_MIIMWriteDataSet(ethphyId, hClientObj->smiData);
                // wait for write completion
                hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_BUSY_COMPLETE;
            }
            else if(hClientObj->smiTxferType == DRV_ETHPHY_SMI_XFER_TYPE_READ)
            {   // start read
                PLIB_ETH_MIIMReadStart(ethphyId);
                hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_BUSY_COMPLETE;
            }
            else if(hClientObj->smiTxferType == DRV_ETHPHY_SMI_XFER_TYPE_SCAN)
            {   // SCAN
                PLIB_ETH_MIIMScanModeEnable(ethphyId);
                hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_DONE;
            }

            return hClientObj->smiTxferStatus;

        case DRV_ETHPHY_SMI_TXFER_OP_BUSY_COMPLETE:
            if ( PLIB_ETH_MIIMIsBusy(ethphyId) )
            {   // wait op to complete
                return DRV_ETHPHY_SMI_TXFER_OP_BUSY_COMPLETE;
            }

            if(hClientObj->smiTxferType == DRV_ETHPHY_SMI_XFER_TYPE_READ)
            {
                PLIB_ETH_MIIMWriteStart(ethphyId);         // Stop read cycle.
                hClientObj->smiData = PLIB_ETH_MIIMReadDataGet(ethphyId); // get the read register
            }

            hClientObj->hDriver->busInUse = 0;  // clear
            return (hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_DONE);

        default :   // DRV_ETHPHY_SMI_TXFER_OP_NONE, DRV_ETHPHY_SMI_TXFER_OP_DONE
            return hClientObj->smiTxferStatus;
    }
}
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)

static DRV_ETHPHY_SMI_TXFER_OP_STATUS _DRV_PHY_SMIReadStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_READ;
    hClientObj->smiPhyAddress = hClientObj->hDriver->phyAddress;

    return _DRV_PHY_SMITransferDo(hClientObj);
}

static DRV_ETHPHY_SMI_TXFER_OP_STATUS _DRV_PHY_SMIReadStartEx(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, int phyAdd)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_READ;
    hClientObj->smiPhyAddress = phyAdd;

    return _DRV_PHY_SMITransferDo(hClientObj);
}

#if !(DRV_ETHPHY_USE_DRV_MIIM)
static DRV_ETHPHY_SMI_TXFER_OP_STATUS _DRV_PHY_SMIScanStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_SCAN;
    hClientObj->smiPhyAddress = hClientObj->hDriver->phyAddress;

    return _DRV_PHY_SMITransferDo(hClientObj);
}
#endif  // !(DRV_ETHPHY_USE_DRV_MIIM)

static DRV_ETHPHY_SMI_TXFER_OP_STATUS _DRV_PHY_SMIWriteStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, uint16_t wData)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_WRITE;
    hClientObj->smiData =  wData;
    hClientObj->smiPhyAddress = hClientObj->hDriver->phyAddress;

   return  _DRV_PHY_SMITransferDo(hClientObj);
}

static DRV_ETHPHY_SMI_TXFER_OP_STATUS _DRV_PHY_SMIWriteStartEx(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, uint16_t wData, int phyAdd)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_WRITE;
    hClientObj->smiData =  wData;
    hClientObj->smiPhyAddress = phyAdd;

   return  _DRV_PHY_SMITransferDo(hClientObj);
}

#if !(DRV_ETHPHY_USE_DRV_MIIM)
static DRV_ETHPHY_SMI_TXFER_OP_STATUS _DRV_PHY_SMIWriteCompleteStartEx(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t rIx, uint16_t wData, int phyAdd)
{
    hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_START;
    hClientObj->smiRIx = rIx;
    hClientObj->smiTxferType = DRV_ETHPHY_SMI_XFER_TYPE_WRITE_COMPLETE;
    hClientObj->smiData =  wData;
    hClientObj->smiPhyAddress = phyAdd;

   return  _DRV_PHY_SMITransferDo(hClientObj);

}
#endif  // !(DRV_ETHPHY_USE_DRV_MIIM)

static void _DRV_PHY_SetOperPhase(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t operPhase, uint16_t operSubPhase)
{
    hClientObj->operPhase = operPhase;
    hClientObj->operSubPhase = operSubPhase;
}

static void _DRV_PHY_SetOperDoneResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_RESULT res)
{
    hClientObj->operPhase =  hClientObj->operSubPhase = 0;
    hClientObj->operType = DRV_ETHPHY_CLIENT_OP_TYPE_NONE;
    hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_READY;
    hClientObj->operRes = res;
    if(res < 0)
    {
        SYS_CONSOLE_PRINT("DRV_PHY operation error: %d\r\n", res);
    }
}

static void _DRV_PHY_SetOperStart(DRV_ETHPHY_CLIENT_OBJ * hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE opType, DRV_ETHPHY_RESULT res)
{
    hClientObj->operType = opType;
    hClientObj->operPhase = hClientObj->operSubPhase = 0; 
    hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_BUSY; 
    hClientObj->operRes = res;
}


// *****************************************************************************
/* Driver Hardware instance objects.

  Summary:
    Defines the hardware instances objects that are available on the part

  Description:
    This data type defines the hardware instance objects that are available on
    the part, so as to capture the hardware state of the instance.

  Remarks:
    Not all modes are available on all micro-controllers.
*/

#if DRV_ETHPHY_INSTANCES_NUMBER < DRV_ETHPHY_INDEX_COUNT 
#define _DRV_ETHPHY_INSTANCES_NUMBER    DRV_ETHPHY_INSTANCES_NUMBER
#else
#define _DRV_ETHPHY_INSTANCES_NUMBER    DRV_ETHPHY_INDEX_COUNT
#endif
static DRV_ETHPHY_OBJ              gDrvETHPHYObj[_DRV_ETHPHY_INSTANCES_NUMBER];


// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static DRV_ETHPHY_CLIENT_OBJ * _DRV_ETHPHY_ClientObjectAllocate( DRV_ETHPHY_OBJ* hPhyObj, int* pCliIx )
{
    int ix;

    DRV_ETHPHY_CLIENT_OBJ* pClientObj = hPhyObj->objClients;
    for(ix = 0; ix < sizeof(hPhyObj->objClients) / sizeof(*hPhyObj->objClients); ix++, pClientObj++)
    {
        if(pClientObj->clientInUse == 0)
        {   // available client
            *pCliIx = ix;
            return pClientObj;
        }
    }

    return 0;
}



// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// The default basic PHY driver object
//
const DRV_ETHPHY_OBJECT_BASE  DRV_ETHPHY_OBJECT_BASE_Default = 
{
    DRV_ETHPHY_Initialize,
    DRV_ETHPHY_Reinitialize,
    DRV_ETHPHY_Deinitialize,
    DRV_ETHPHY_Status,
    DRV_ETHPHY_Tasks,
    DRV_ETHPHY_Open,
    DRV_ETHPHY_Close,
    DRV_ETHPHY_ClientStatus,
    DRV_ETHPHY_ClientOperationResult,
    DRV_ETHPHY_ClientOperationAbort,
    DRV_ETHPHY_SMIRead,
    DRV_ETHPHY_SMIWrite,
    DRV_ETHPHY_SMIScanStart,
    DRV_ETHPHY_SMIScanStop,
    DRV_ETHPHY_SMIScanStatusGet,
    DRV_ETHPHY_SMIScanDataGet,
    DRV_ETHPHY_SMIStatus,
    DRV_ETHPHY_SMIClockSet,
    DRV_ETHPHY_PhyAddressGet,
    DRV_ETHPHY_Setup,
    DRV_ETHPHY_RestartNegotiation,
    DRV_ETHPHY_HWConfigFlagsGet,
    DRV_ETHPHY_NegotiationIsComplete,
    DRV_ETHPHY_NegotiationResultGet,
    DRV_ETHPHY_LinkStatusGet,
    DRV_ETHPHY_Reset,
    DRV_ETHPHY_VendorDataGet,
    DRV_ETHPHY_VendorDataSet,
    DRV_ETHPHY_VendorSMIReadStart,
    DRV_ETHPHY_VendorSMIReadResultGet,
    DRV_ETHPHY_VendorSMIWriteStart,
};



// self reference to the base object itself
static const DRV_ETHPHY_OBJECT_BASE* gDrvEthBaseObj = &DRV_ETHPHY_OBJECT_BASE_Default;


//******************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_ETHPHY_Initialize( const SYS_MODULE_INDEX    drvIndex,
                                          const SYS_MODULE_INIT     * const init )

  Summary:
    Initializes hardware and data for the given instance of the ETHPHY module

  Description:
    This routine initializes hardware for the instance of the ETHPHY module,
    using the hardware initialization given data.  It also initializes all
    necessary internal data.

  Parameters:
    drvIndex        - Identifies the driver instance to be initialized
    init            - Pointer to the data structure containing all data
                      necessary to initialize the hardware. This pointer may
                      be null if no data is required and static initialization
                      values are to be used.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, it returns SYS_MODULE_OBJ_INVALID.
*/

SYS_MODULE_OBJ DRV_ETHPHY_Initialize( const SYS_MODULE_INDEX  iModule,
                                      const SYS_MODULE_INIT   * const init )
{
    DRV_ETHPHY_OBJ * hSysObj;
    DRV_ETHPHY_INIT *ethphyInit = NULL;

    if ( iModule >= _DRV_ETHPHY_INSTANCES_NUMBER || gDrvETHPHYObj[iModule].objInUse == true )
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    hSysObj = gDrvETHPHYObj + iModule;

    /* Assign to the local pointer the init data passed */
    ethphyInit = ( DRV_ETHPHY_INIT * ) init;

#if (DRV_ETHPHY_USE_DRV_MIIM)
    if((hSysObj->pMiimBase = ethphyInit->pMiimObject) == 0)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    hSysObj->miimIndex = ethphyInit->miimIndex;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)

    hSysObj->objInUse = true;      // Set object to be in use
    hSysObj->busInUse = 0;
    hSysObj->numClients = 0;
    hSysObj->status = SYS_STATUS_READY; // Set module state
    hSysObj->iModule  = iModule;  // Store driver instance
    hSysObj->ethphyId = ethphyInit->ethphyId; // Store PLIB ID

    // Assign External PHY Service Functions
    hSysObj->pPhyObj = ethphyInit->pPhyObject;

    memset(hSysObj->objClients, 0, sizeof(hSysObj->objClients));

    // hush compiler warning
    _PhyAssertCond(true, __func__, __LINE__);
    _PhyDebugCond(true, __func__, __LINE__);

    /* Return the driver handle */
    return ( SYS_MODULE_OBJ )hSysObj ;

}


void DRV_ETHPHY_Reinitialize( SYS_MODULE_OBJ        object ,
                              const SYS_MODULE_INIT * const init )
{
    DRV_ETHPHY_OBJ * hSysObj = (DRV_ETHPHY_OBJ *) object ;
    DRV_ETHPHY_INIT * ethphyInit = (DRV_ETHPHY_INIT *)init;

    /* Check for the valid driver object passed */
    if( NULL != hSysObj)
    {

        hSysObj->numClients = 0;
        hSysObj->status = SYS_STATUS_READY; // Set module state
        hSysObj->ethphyId = ethphyInit->ethphyId; // Store PLIB ID

        // Assign External PHY Service Functions
        hSysObj->pPhyObj = ethphyInit->pPhyObject;

        memset(hSysObj->objClients, 0, sizeof(hSysObj->objClients));
    }
}



void DRV_ETHPHY_Deinitialize( SYS_MODULE_OBJ hSysObj )
{
    DRV_ETHPHY_OBJ * hObj = (DRV_ETHPHY_OBJ *) hSysObj ;

    /* Check for the valid driver object passed */
    if( NULL != hObj)
    {

        /* Set the Device Status */
        hObj->status  = SYS_STATUS_UNINITIALIZED;

        /* Remove the driver usage */
        hObj->objInUse  = false;

        hObj->pPhyObj = 0;
    }

} 


SYS_STATUS DRV_ETHPHY_Status( SYS_MODULE_OBJ hSysObj )
{
    DRV_ETHPHY_OBJ * hObj = (DRV_ETHPHY_OBJ *) hSysObj ;

    /* Check for the valid driver object passed */
    if( NULL != hObj)
    {
        /* Return the status associated with the driver handle */
        return( hObj->status ) ;
    }

    return SYS_STATUS_ERROR;
} 


void DRV_ETHPHY_Tasks( SYS_MODULE_OBJ hSysObj )
{
    DRV_ETHPHY_OBJ * hObj = (DRV_ETHPHY_OBJ *) hSysObj ;

    /* Check for the valid driver object passed */
    if( NULL !=  hObj)
    {
        int ix;
        DRV_ETHPHY_CLIENT_OBJ* hClientObj = hObj->objClients;

        for(ix = 0; ix < sizeof(hObj->objClients) / sizeof(*hObj->objClients); ix++, hClientObj++)
        {
            if(hClientObj->clientInUse != 0)
            {   // active client
                (*_DRV_PHY_ClientOpTbl[hClientObj->operType])(hClientObj);
            }
        }
    }
} 


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client Level
// *****************************************************************************
// *****************************************************************************


DRV_HANDLE  DRV_ETHPHY_Open ( const SYS_MODULE_INDEX iModule,
                              const DRV_IO_INTENT ioIntent )
{
    /* Multi client variables are removed from single client builds. */
    DRV_ETHPHY_OBJ * hPhyObj;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj;
    int  clientIx;

    /* Validate the driver index */
    /* If there is anything specific to the module & needs to be checked, should
       be handled in this section with an || condition.
       May be something like ioIntent test for Exclusive access */
    if ( iModule >= _DRV_ETHPHY_INSTANCES_NUMBER || gDrvETHPHYObj[iModule].objInUse == false )
    {
        return DRV_HANDLE_INVALID;
    }

    /* Setup client operations */

    /* TODO: OSAL - Lock Mutex */

    hPhyObj = gDrvETHPHYObj + iModule;
    /* Allocate the client object and set the flag as in use */
    hClientObj = _DRV_ETHPHY_ClientObjectAllocate(hPhyObj, &clientIx) ;
    while(hClientObj)
    {
#if (DRV_ETHPHY_USE_DRV_MIIM)
        DRV_HANDLE miimHandle = hPhyObj->pMiimBase->DRV_MIIM_Open(hPhyObj->miimIndex, DRV_IO_INTENT_SHARED);
        if(miimHandle == DRV_HANDLE_INVALID)
        {
            _PhyDebugCond(true, __func__, __LINE__);
            break;
        }
        hClientObj->pMiimBase = hPhyObj->pMiimBase;
        hClientObj->miimHandle = miimHandle;
        hClientObj->miimOpHandle = 0;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)

        hClientObj->clientInUse    = true;
        hClientObj->clientIx = clientIx;
        hClientObj->hDriver  = hPhyObj;
        hClientObj->ethphyId = hPhyObj->ethphyId;
        hClientObj->operType = DRV_ETHPHY_CLIENT_OP_TYPE_NONE;
        hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
        _DRV_PHY_SetOperPhase(hClientObj, 0, 0);
        hPhyObj->numClients++;

        /* TODO: OSAL - Unlock Mutex */

        /* Update the Client Status */
        hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_READY;

        /* Return the client object */
        return ( ( DRV_HANDLE ) hClientObj );
    }

    return DRV_HANDLE_INVALID;

}


//******************************************************************************
/* Function:
    void DRV_ETHPHY_Close( DRV_HANDLE handle )

  Summary:
    Closes an opened-instance of a driver

  Description:
    This routine closes an opened-instance of a driver, invalidating the given
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None
*/

void DRV_ETHPHY_Close( DRV_HANDLE handle )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *)handle;

    if(hClientObj != 0)
    {
        /* TODO: OSAL - lock Mutex */

#if (DRV_ETHPHY_USE_DRV_MIIM)
        hClientObj->pMiimBase->DRV_MIIM_Close(hClientObj->miimHandle);
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)

        /* Free the Client Instance */
        hClientObj->clientInUse = false ;
        hClientObj->hDriver->numClients--;

        /* TODO: OSAL - unlock Mutex */

        /* Update the Client Status */
        hClientObj->status = DRV_ETHPHY_CLIENT_STATUS_CLOSED;
    }

}

DRV_ETHPHY_RESULT DRV_ETHPHY_Setup( DRV_HANDLE handle, DRV_ETHPHY_SETUP* pSetUp, TCPIP_ETH_OPEN_FLAGS* pSetupFlags)
{

    DRV_ETHPHY_OBJ * hDriver;
    const DRV_ETHPHY_OBJECT* pPhyObj;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    // basic sanity check
    if(pSetUp == 0 || pSetupFlags == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    // check the PHY set up parameters
    hDriver = hClientObj->hDriver;
    pPhyObj = hDriver->pPhyObj;

    if(pPhyObj == 0 || pPhyObj->miiConfigure == 0 || pPhyObj->mdixConfigure == 0 || pPhyObj->smiClockGet == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if(pSetUp->resetFunction)
    {
        (*pSetUp->resetFunction)(gDrvEthBaseObj);
    }
#if (DRV_ETHPHY_USE_DRV_MIIM)
    DRV_MIIM_SETUP  miimSetup;
#if defined (__PIC32MZ__)
    miimSetup.hostClockFreq = SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_5);
#elif defined (__PIC32C__)
    miimSetup.hostClockFreq = SYS_CLK_FrequencyGet(SYS_CLK_MASTER);
#else
    miimSetup.hostClockFreq = SYS_CLK_SystemFrequencyGet();
#endif

    miimSetup.maxBusFreq = (*pPhyObj->smiClockGet)(gDrvEthBaseObj, handle);
    miimSetup.setupFlags = DRV_MIIM_SETUP_FLAG_NONE;

    DRV_MIIM_RESULT miimRes = hClientObj->pMiimBase->DRV_MIIM_Setup(hClientObj->miimHandle, &miimSetup); 
    if(miimRes < 0)
    {   // error occurred
        _PhyDebugCond(true, __func__, __LINE__);
        return DRV_ETHPHY_RES_MIIM_ERR;
    }
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)

    hDriver->phyAddress = pSetUp->phyAddress;
    hDriver->openFlags = pSetUp->openFlags;
    hDriver->configFlags = pSetUp->configFlags;
    hDriver->macPauseType = pSetUp->macPauseType;
    
    hClientObj->operParam = (uintptr_t)pSetupFlags;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_SETUP, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}

static void _DRV_ETHPHY_ClientOpNone(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
}

static void _DRV_ETHPHY_ClientOpSetup(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if((unsigned int)hClientObj->operPhase >= sizeof(_DRV_PHY_SetupPhasesTbl)/sizeof(*_DRV_PHY_SetupPhasesTbl))
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
    else
    {
        (*_DRV_PHY_SetupPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
}


static void _DRV_ETHPHY_SetupPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    DRV_ETHPHY_OBJ * hDriver = NULL;
    DRV_ETHPHY_CONFIG_FLAGS configFlags = DRV_ETHPHY_CFG_DEFAULT, hwFlags = DRV_ETHPHY_CFG_DEFAULT, swFlags = DRV_ETHPHY_CFG_DEFAULT;
    TCPIP_ETH_OPEN_FLAGS      openFlags = TCPIP_ETH_OPEN_DEFAULT;      // flags required at open time

    hDriver = hClientObj->hDriver;

    configFlags = hDriver->configFlags;

    // get the way the hw is configured
    DRV_ETHPHY_HWConfigFlagsGet((DRV_HANDLE)hClientObj, &hwFlags);

    if(configFlags & DRV_ETHPHY_CFG_AUTO)
    {
        hDriver->configFlags = configFlags = hwFlags;
    }
    else
    {   // some minimal check against the way the hw is configured
        swFlags = configFlags & (DRV_ETHPHY_CFG_RMII|DRV_ETHPHY_CFG_ALTERNATE);

        if((swFlags ^ hwFlags) != 0)
        {   // hw-sw configuration mismatch MII/RMII, ALT/DEF config
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_CFG_ERR);
        }
    }

    openFlags = hDriver->openFlags;
    if(openFlags & (TCPIP_ETH_OPEN_PHY_LOOPBACK | TCPIP_ETH_OPEN_MAC_LOOPBACK))
    {
        openFlags &= ~TCPIP_ETH_OPEN_AUTO; // no negotiation in loopback mode!
    }

    if(!(openFlags & TCPIP_ETH_OPEN_AUTO))
    {
        openFlags &= ~TCPIP_ETH_OPEN_MDIX_AUTO;        // Auto-MDIX has to be in auto negotiation only
    }

    openFlags |= (configFlags & DRV_ETHPHY_CFG_RMII) ? TCPIP_ETH_OPEN_RMII : TCPIP_ETH_OPEN_MII;

    hDriver->openFlags = openFlags;


#if !(DRV_ETHPHY_USE_DRV_MIIM)
    // Set SMI clock
    DRV_ETHPHY_VENDOR_SMI_CLOCK_GET phySMIClockGet = hDriver->pPhyObj->smiClockGet;

#if defined (__PIC32MZ__)
    DRV_ETHPHY_SMIClockSet((DRV_HANDLE)hClientObj, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_5), (*phySMIClockGet)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj) );
#elif defined (__PIC32C__)
    DRV_ETHPHY_SMIClockSet((DRV_HANDLE)hClientObj, SYS_CLK_FrequencyGet(SYS_CLK_MASTER), (*phySMIClockGet)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj) );
#else
    DRV_ETHPHY_SMIClockSet((DRV_HANDLE)hClientObj, SYS_CLK_SystemFrequencyGet(), (*phySMIClockGet)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj) );
#endif
#endif  // !(DRV_ETHPHY_USE_DRV_MIIM)

    hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_RESET_CLR_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 0);
}

    
static void _DRV_ETHPHY_SetupPhaseDetect(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // try to detect the PHY and reset it
    __BMCONbits_t bmcon;

    // all phases except 0 wait for the completion of an SMI transaction

    if(hClientObj->operSubPhase == 0)
    {   // initiate a read of the BMCON register
        _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 1);
    }

    // wait the BMCON read/write to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    switch (hClientObj->operSubPhase)
    {
        case 1:
            bmcon.w = hClientObj->smiData;
            if(bmcon.RESET)
            {   // that is suspicios...but give it a chance to clear itself
                if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
                {   // not yet tmo
                    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 0);
                }
                else
                {
                    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
                }
                break;
            }

            // success
            // ok, reset bit is low
            // try to see if we can write smth to the PHY
            // we use BMCON::Loopback and Isolate bits
            _DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, _BMCON_LOOPBACK_MASK|_BMCON_ISOLATE_MASK);
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 2);
            break;

        case 2:
            // re-read the BMCON
            _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 3);
            break;

        case 3:
            bmcon.w = hClientObj->smiData;
            if( (bmcon.LOOPBACK == 0) || (bmcon.ISOLATE == 0) )
            {   // failed to set
                 _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
            }
            else
            {   // clear bits and write
                _DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, bmcon.w ^ (_BMCON_LOOPBACK_MASK|_BMCON_ISOLATE_MASK));
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 4);
            }
            break;

        case 4:
            _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_DETECT, 5);
            break;

        case 5:
            bmcon.w = hClientObj->smiData;
            if(bmcon.LOOPBACK || bmcon.ISOLATE)
            {   // failed to clear
                _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
            }
            else
            {   // everything seems to be fine
                // initiate the PHY reset
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 0);
            }

            break;

        default:
            // shouldn't happen
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

// performs the PHY reset
static void _DRV_ETHPHY_SetupPhaseReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t bmcon;

    if(hClientObj->operSubPhase == 0)
    {   // initiate Soft Reset the PHY
        _DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, _BMCON_RESET_MASK);
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 1);
        return;
    }

    // all other phases wait for some transaction to finish
    // wait the BMCON write/read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    switch(hClientObj->operSubPhase)
    {
        case 1:
            // read the BMCON
            _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
            hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_RESET_CLR_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 2);
            break;

        case 2:
            bmcon.w = hClientObj->smiData;
            if(bmcon.RESET)
            {   // wait to clear itself
                if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
                {   // not yet tmo; re-read
                    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_RESET, 1);
                }
                else
                {
                    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
                }
            }
            else
            {   // success
                _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 0);
            }
            break;

        default:
            // shouldn't happen
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
            break;
    }

}

static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase0(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase1(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase2(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase3(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase4(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase5(DRV_ETHPHY_CLIENT_OBJ * hClientObj);
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase6(DRV_ETHPHY_CLIENT_OBJ * hClientObj);




static const _DRV_PHY_OperPhaseF _DRV_PHY_SetupPhaseNegotiateSubPhaseTbl[] = 
{
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase0,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase1,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase2,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase3,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase4,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase5,
    _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase6,


};

static void _DRV_ETHPHY_SetupPhaseNegotiate(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if(hClientObj->operSubPhase <  sizeof(_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl) / sizeof(*_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl))
    {
        (*_DRV_PHY_SetupPhaseNegotiateSubPhaseTbl[hClientObj->operSubPhase])(hClientObj);
    } 
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase0(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    TCPIP_ETH_OPEN_FLAGS  openFlags;      // flags required at open time
    uint16_t  openReqs;
    
    // provide some defaults
    openFlags = hClientObj->hDriver->openFlags;
    if(!(openFlags & (TCPIP_ETH_OPEN_FDUPLEX|TCPIP_ETH_OPEN_HDUPLEX)))
    {
        openFlags |= TCPIP_ETH_OPEN_HDUPLEX;
    }
    if(!(openFlags & (TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_10)))
    {
        openFlags |= TCPIP_ETH_OPEN_10;
    }

    if(openFlags & TCPIP_ETH_OPEN_AUTO)
    {   // advertise auto negotiation
        openReqs = _BMSTAT_AN_ABLE_MASK;

        if(openFlags & TCPIP_ETH_OPEN_100)
        {
            if(openFlags & TCPIP_ETH_OPEN_FDUPLEX)
            {
                openReqs |= _BMSTAT_BASE100TX_FDX_MASK;
            }
            if(openFlags & TCPIP_ETH_OPEN_HDUPLEX)
            {
                openReqs |= _BMSTAT_BASE100TX_HDX_MASK;
            }
        }

        if(openFlags & TCPIP_ETH_OPEN_10)
        {
            if(openFlags & TCPIP_ETH_OPEN_FDUPLEX)
            {
                openReqs |= _BMSTAT_BASE10T_FDX_MASK;
            }
            if(openFlags & TCPIP_ETH_OPEN_HDUPLEX)
            {
                openReqs |= _BMSTAT_BASE10T_HDX_MASK;
            }
        }
    }
    else
    {   // no auto negotiation
        if(openFlags & TCPIP_ETH_OPEN_100)
        {
            openReqs = (openFlags & TCPIP_ETH_OPEN_FDUPLEX) ? _BMSTAT_BASE100TX_FDX_MASK : _BMSTAT_BASE100TX_HDX_MASK;
        }
        else
        {
            openReqs = (openFlags & TCPIP_ETH_OPEN_FDUPLEX) ? _BMSTAT_BASE10T_FDX_MASK : _BMSTAT_BASE10T_HDX_MASK;
        }
    }

    hClientObj->hDriver->openFlags = openFlags;
    hClientObj->operReg[0] = openReqs;
    // try to match the openFlags with the PHY capabilities
    // initiate the read of the PHY capabilities
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 1);

}

static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase1(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the BMCON PHY_REG_BMSTAT read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    uint16_t  phyCpbl, matchCpbl;

    phyCpbl   = hClientObj->smiData;

    matchCpbl = (hClientObj->operReg[0] & (MAC_COMM_CPBL_MASK | _BMSTAT_AN_ABLE_MASK)) & phyCpbl; // common features
    if(!(matchCpbl & MAC_COMM_CPBL_MASK))
    {   // no match?
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_CPBL_ERR);
        return;
    }

    // we're ok, we can configure the PHY
    hClientObj->operReg[1] = matchCpbl;
    hClientObj->vendorData = 0;
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 2);

}

// MII/RMII configuration phase
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase2(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_OBJ *hDriver;
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;

    hDriver = hClientObj->hDriver;
    res = (*hDriver->pPhyObj->miiConfigure)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj, hDriver->configFlags);

    if(res == DRV_ETHPHY_RES_OK)
    {   // success, advance
        // next phase: configure the MDIX
        hClientObj->vendorData = 0;
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 3);
    }
    else if(res != DRV_ETHPHY_RES_PENDING)
    {   // some error occurred
        _DRV_PHY_SetOperDoneResult(hClientObj, res);
    }

}


// MDIX configure
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase3(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    DRV_ETHPHY_RESULT res = DRV_ETHPHY_RES_PENDING;
    DRV_ETHPHY_OBJ *hDriver = hClientObj->hDriver;

    res = (*hDriver->pPhyObj->mdixConfigure)(gDrvEthBaseObj, (DRV_HANDLE)hClientObj, hClientObj->hDriver->openFlags);

    while(true)
    {
        if(res == DRV_ETHPHY_RES_OK)
        {   // success, advance;
            break;
        }
        else
        {
            if(res != DRV_ETHPHY_RES_PENDING)
            {   // some error occurred
                _DRV_PHY_SetOperDoneResult(hClientObj, res);
            }
            return;
        }
    }


    uint16_t  matchCpbl = hClientObj->operReg[1];

    if(matchCpbl &_BMSTAT_AN_ABLE_MASK)
    {   // ok, we can perform auto negotiation
        uint16_t anadReg;

        anadReg = (((matchCpbl >> _BMSTAT_NEGOTIATION_POS) << _ANAD_NEGOTIATION_POS) & _ANAD_NEGOTIATION_MASK) | PROT_802_3;
        if(hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_PAUSE)
        {
            anadReg |= _ANAD_PAUSE_MASK;
        }

        if(hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_ASM_DIR)
        {
            anadReg |= _ANAD_ASM_DIR_MASK;
        }

        _DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_ANAD, anadReg);      // advertise our capabilities
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 4);
    }
    else
    {   // ok, just don't use negotiation
        uint16_t  ctrlReg;

        ctrlReg = 0;
        if(matchCpbl & (_BMSTAT_BASE100TX_HDX_MASK | _BMSTAT_BASE100TX_FDX_MASK))   // set 100Mbps request/capability
        {
            ctrlReg |= _BMCON_SPEED_MASK;
        }

        if(matchCpbl & (_BMSTAT_BASE10T_FDX_MASK | _BMSTAT_BASE100TX_FDX_MASK))
        {
            ctrlReg |= _BMCON_DUPLEX_MASK;
        }

        if(hClientObj->hDriver->openFlags & TCPIP_ETH_OPEN_PHY_LOOPBACK)
        {
            ctrlReg |= _BMCON_LOOPBACK_MASK;
        }

        _DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, ctrlReg);      // update the configuration
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 6);
    }

}

// advertise, negotiation restart phase
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase4(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the PHY_REG_ANAD write to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);      // read capabilities initiate
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 5);
}
    
// auto-negotiation perform
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase5(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the PHY_REG_BMSTAT read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    __BMSTATbits_t  phyCpbl;

    phyCpbl.w = hClientObj->smiData;

    if(phyCpbl.AN_ABLE)
    {   // ok, we can perform auto negotiation
        // restart negotiation and we'll have to wait
        _DRV_PHY_SMIWriteStart( hClientObj, PHY_REG_BMCON, _BMCON_AN_ENABLE_MASK | _BMCON_AN_RESTART_MASK );
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_SETUP_PHASE_NEGOTIATE, 6);
    }
    else
    {   // no negotiation ability!
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_UNABLE);
    }
}

// final update stage
static void _DRV_ETHPHY_SetupPhaseNegotiate_SubPhase6(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    TCPIP_ETH_OPEN_FLAGS openFlags = TCPIP_ETH_OPEN_DEFAULT;
    uint16_t    matchCpbl = 0 ;
    // wait the BMCON write/read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // now update the open flags
    // the upper layer needs to know the PHY set-up to further set-up the MAC.
    openFlags = hClientObj->hDriver->openFlags;
    matchCpbl = hClientObj->operReg[1];

    // clear the capabilities
    openFlags &= ~(TCPIP_ETH_OPEN_AUTO | TCPIP_ETH_OPEN_FDUPLEX | TCPIP_ETH_OPEN_HDUPLEX | TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_10);

    if(matchCpbl & _BMSTAT_AN_ABLE_MASK)
    {
        openFlags |= TCPIP_ETH_OPEN_AUTO;
    }
    if(matchCpbl & (_BMSTAT_BASE100TX_HDX_MASK | _BMSTAT_BASE100TX_FDX_MASK))   // set 100Mbps request/capability
    {
        openFlags |= TCPIP_ETH_OPEN_100;
    }
    if(matchCpbl & (_BMSTAT_BASE10T_HDX_MASK | _BMSTAT_BASE10T_FDX_MASK))   // set 10Mbps request/capability
    {
        openFlags |= TCPIP_ETH_OPEN_10;
    }
    if(matchCpbl & (_BMSTAT_BASE10T_FDX_MASK | _BMSTAT_BASE100TX_FDX_MASK))
    {
        openFlags |= TCPIP_ETH_OPEN_FDUPLEX;
    }
    if(matchCpbl & (_BMSTAT_BASE10T_HDX_MASK | _BMSTAT_BASE100TX_HDX_MASK))
    {
        openFlags |= TCPIP_ETH_OPEN_HDUPLEX;
    }

    // store the openFlags!
    // upper layer needs to know the PHY set-up to further set-up the MAC.
    hClientObj->hDriver->openFlags = openFlags;

    // we're done
    // deposit result
    TCPIP_ETH_OPEN_FLAGS* pSetUpFlags = (TCPIP_ETH_OPEN_FLAGS*)hClientObj->operParam;
    *pSetUpFlags = openFlags;
    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);

} 



DRV_ETHPHY_CLIENT_STATUS DRV_ETHPHY_ClientStatus( DRV_HANDLE handle )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( NULL != hClientObj)
    {
        /* Return the client status associated with the handle passed */
        return( hClientObj->status );
    }

    return DRV_ETHPHY_CLIENT_STATUS_ERROR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationResult( DRV_HANDLE handle)  
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( NULL != hClientObj)
    {
        /* Return the client status associated with the handle passed */
        return( hClientObj->operRes );
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_ClientOperationAbort( DRV_HANDLE handle)  
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if( NULL != hClientObj)
    {
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_ABORTED);
        return DRV_ETHPHY_RES_OK;
    }

    return DRV_ETHPHY_RES_HANDLE_ERR;
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - SMI/MIIM Interface
// *****************************************************************************
// *****************************************************************************


DRV_ETHPHY_RESULT DRV_ETHPHY_SMIRead( DRV_HANDLE handle, unsigned int rIx, uint16_t* pSmiRes, int phyAdd)
{
#if (DRV_ETHPHY_USE_DRV_MIIM)
    return DRV_ETHPHY_RES_OPERATION_ERR; 
#else

    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    hClientObj->operParam = (uintptr_t)pSmiRes;

    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_SMI_READ, DRV_ETHPHY_RES_PENDING);
    _DRV_PHY_SMIReadStartEx(hClientObj, rIx, phyAdd);

    return DRV_ETHPHY_RES_PENDING;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)
}

static void _DRV_ETHPHY_ClientOpSmiRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    uint16_t* pSmiRes = NULL;
    // wait for read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    pSmiRes = (uint16_t*)hClientObj->operParam;
    if(pSmiRes != 0)
    {
        *pSmiRes = hClientObj->smiData;
    }

    // done
    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_SMIWrite( DRV_HANDLE handle, unsigned int rIx, uint16_t wData, int phyAdd, bool waitComplete)
{
#if (DRV_ETHPHY_USE_DRV_MIIM)
    return DRV_ETHPHY_RES_OPERATION_ERR; 
#else
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    hClientObj->operParam = waitComplete;


    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_SMI_WRITE, DRV_ETHPHY_RES_PENDING);
    if(waitComplete)
    {
        _DRV_PHY_SMIWriteCompleteStartEx(hClientObj, rIx, wData, phyAdd);
    }
    else
    {
        _DRV_PHY_SMIWriteStartEx(hClientObj, rIx, wData, phyAdd);
    }

    return DRV_ETHPHY_RES_PENDING;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)
}


static void _DRV_ETHPHY_ClientOpSmiWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait for write transfer to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // done
    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_SMIScanStart( DRV_HANDLE handle, unsigned int rIx)
{
#if (DRV_ETHPHY_USE_DRV_MIIM)
    return DRV_ETHPHY_RES_OPERATION_ERR; 
#else
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_SMI_SCAN, DRV_ETHPHY_RES_PENDING);
    _DRV_PHY_SMIScanStart(hClientObj, rIx);


    return DRV_ETHPHY_RES_PENDING;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)
}

static void _DRV_ETHPHY_ClientOpSmiScan(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait for scan transfer to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // done; remain busy as to not start another SMI operation before aborting this one

}


DRV_ETHPHY_RESULT DRV_ETHPHY_SMIScanStop( DRV_HANDLE handle )
{
#if (DRV_ETHPHY_USE_DRV_MIIM)
    return DRV_ETHPHY_RES_OPERATION_ERR; 
#else
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || hClientObj->operType != DRV_ETHPHY_CLIENT_OP_SMI_SCAN)
    {   // not scanning?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    PLIB_ETH_MIIMScanModeDisable(hClientObj->ethphyId);
    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
    hClientObj->hDriver->busInUse = 0;  // clear
    return DRV_ETHPHY_RES_OK;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)
}


DRV_ETHPHY_RESULT DRV_ETHPHY_SMIScanStatusGet( DRV_HANDLE handle )
{
#if (DRV_ETHPHY_USE_DRV_MIIM)
    return DRV_ETHPHY_RES_OPERATION_ERR; 
#else
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || hClientObj->operType != DRV_ETHPHY_CLIENT_OP_SMI_SCAN)
    {   // not scanning?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    return PLIB_ETH_DataNotValid(hClientObj->ethphyId) ? DRV_ETHPHY_RES_PENDING : DRV_ETHPHY_RES_OK;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)
}

DRV_ETHPHY_RESULT DRV_ETHPHY_SMIScanDataGet( DRV_HANDLE handle, uint16_t* pScanRes )
{
#if (DRV_ETHPHY_USE_DRV_MIIM)
    return DRV_ETHPHY_RES_OPERATION_ERR; 
#else
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || hClientObj->operType != DRV_ETHPHY_CLIENT_OP_SMI_SCAN)
    {   // not scanning?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    if( PLIB_ETH_DataNotValid(hClientObj->ethphyId) )
    {   // no data available yet
        return DRV_ETHPHY_RES_PENDING;
    }

    uint16_t scanData = PLIB_ETH_MIIMReadDataGet(hClientObj->ethphyId);
    if(pScanRes)
    {
        *pScanRes = scanData;
    }
    
    return DRV_ETHPHY_RES_OK;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)
}

DRV_ETHPHY_RESULT DRV_ETHPHY_SMIStatus( DRV_HANDLE handle )
{
#if (DRV_ETHPHY_USE_DRV_MIIM)
    return DRV_ETHPHY_RES_OPERATION_ERR; 
#else
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    return PLIB_ETH_MIIMIsBusy(hClientObj->ethphyId) ? DRV_ETHPHY_RES_BUSY : DRV_ETHPHY_RES_OK;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)
} 

DRV_ETHPHY_RESULT DRV_ETHPHY_SMIClockSet( DRV_HANDLE handle,
                             uint32_t hostClock,
                             uint32_t maxSMIClock )
{
#if (DRV_ETHPHY_USE_DRV_MIIM)
    return DRV_ETHPHY_RES_OPERATION_ERR; 
#else
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    int  ix;
    ETH_MODULE_ID    ethphyId; 

    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    ethphyId = hClientObj->ethphyId;
    PLIB_ETH_MIIMResetEnable(ethphyId); // Issue MIIM reset
    PLIB_ETH_MIIMResetDisable(ethphyId); // Clear MIIM reset

    for(ix=0; ix<sizeof(_MIIClockDivisorTable)/sizeof(*_MIIClockDivisorTable); ix++)
    {
        if(hostClock/_MIIClockDivisorTable[ix]<=maxSMIClock)
        {   // found it
            break;
        }
    }

    if(ix == sizeof(_MIIClockDivisorTable)/sizeof(*_MIIClockDivisorTable))
    {
        ix--;   // max divider; best we can do
    }
    PLIB_ETH_MIIMClockSet(ethphyId,ix+1);  // program the clock

    return DRV_ETHPHY_RES_OK;
#endif  // (DRV_ETHPHY_USE_DRV_MIIM)
} 

DRV_ETHPHY_RESULT DRV_ETHPHY_PhyAddressGet( DRV_HANDLE handle, int* pPhyAddress)
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    if( hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(pPhyAddress)
    {
        *pPhyAddress = hClientObj->hDriver->phyAddress;
    }
    return DRV_ETHPHY_RES_OK;
}


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - Client & Module Level
// *****************************************************************************
// *****************************************************************************

DRV_ETHPHY_RESULT DRV_ETHPHY_RestartNegotiation( DRV_HANDLE hClient )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) hClient;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESTART, DRV_ETHPHY_RES_PENDING);
    return DRV_ETHPHY_RES_PENDING;
}

static void _DRV_ETHPHY_ClientOpNegRestart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegRestartPhasesTbl)/sizeof(*_DRV_PHY_NegRestartPhasesTbl))
    {
        (*_DRV_PHY_NegRestartPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_NegRestartPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the capabilities control reg: BMSTAT
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESTART_PHASE_READ, 0);
}

static void _DRV_ETHPHY_NegRestartPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMSTATbits_t  phyCpbl;

    // wait the BMSTAT read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    phyCpbl.w = hClientObj->smiData;
    if(phyCpbl.AN_ABLE == 0)
    {   // no negotiation ability!
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_UNABLE);
        return;
    }

    // we can perform auto negotiation
    // restart negotiation and we'll have to wait
    _DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, _BMCON_AN_ENABLE_MASK | _BMCON_AN_RESTART_MASK );
    
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESTART_PHASE_WRITE, 0);
}

static void _DRV_ETHPHY_NegRestartPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // all other phases wait for some transaction to finish
    // wait the BMCON write to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}

DRV_ETHPHY_RESULT DRV_ETHPHY_HWConfigFlagsGet( DRV_HANDLE handle, DRV_ETHPHY_CONFIG_FLAGS* pFlags )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;
    DRV_ETHPHY_CONFIG_FLAGS hwFlags;
    DRV_ETHPHY_RESULT ethRes;
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }


    // the way the hw is configured
#if defined (__PIC32MX__) || defined(__PIC32MZ__)
    hwFlags =  (DEVCFG3bits.FMIIEN != 0) ?     DRV_ETHPHY_CFG_MII : DRV_ETHPHY_CFG_RMII;
    hwFlags |= (DEVCFG3bits.FETHIO != 0) ? DRV_ETHPHY_CFG_DEFAULT : DRV_ETHPHY_CFG_ALTERNATE;
    ethRes = DRV_ETHPHY_RES_OK;
#elif defined (__PIC32C__)
    hwFlags = ((_GMAC_REGS->GMAC_UR.w & GMAC_UR_Msk)!= 0) ?  DRV_ETHPHY_CFG_MII : DRV_ETHPHY_CFG_RMII;
    ethRes = DRV_ETHPHY_RES_OK;
#else
    hwFlags = 0;
    ethRes = DRV_ETHPHY_RES_CFG_ERR;
#endif  // defined (__PIC32MX__)

    if(pFlags)
    {

        *pFlags = hwFlags;
    }

    return ethRes;
}


DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationIsComplete( DRV_HANDLE hClient, bool waitComplete )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) hClient;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    hClientObj->operParam = waitComplete;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_COMPLETE, DRV_ETHPHY_RES_PENDING);


    return DRV_ETHPHY_RES_PENDING;
}

static void _DRV_ETHPHY_ClientOpNegComplete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegCompletePhasesTbl)/sizeof(*_DRV_PHY_NegCompletePhasesTbl))
    {
        (*_DRV_PHY_NegCompletePhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_NegCompletePhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the control reg: PHY_REG_BMCON
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_READ1, 0);
}

static void _DRV_ETHPHY_NegCompletePhaseRead1(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;
    // wait the BMCON read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    phyBMCon.w = hClientObj->smiData;
    if(phyBMCon.AN_ENABLE == 0)
    {   // no negotiation is taking place; we're done
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_INACTIVE);
        return;
    }

    // re-issue another BMCON read
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_READ2, 0);
}

static void _DRV_ETHPHY_NegCompletePhaseRead2(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;

    // wait the BMCON read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // save BMCON read for later
    hClientObj->operReg[1] = phyBMCon.w = hClientObj->smiData;

    if(hClientObj->operParam == 0)
    {   // no wait complete
        // issue a PHY_REG_BMSTAT read
        _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_RESULT_NOWAIT, 0);
        return;
    }

    // wait for negotiation completion
    // and launch another BMCON read op
    hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_NEG_INIT_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_AN_RESTART, 0);
}

static void _DRV_ETHPHY_NegCompletePhaseAN_Restart(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;

    // wait the BMCON read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // save BMCON read for later
    hClientObj->operReg[1] = phyBMCon.w = hClientObj->smiData;

    if(phyBMCon.AN_RESTART == 0)
    {   // negotiation started; wait to complete
        // and launch another BMSTAT read op
        hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_NEG_DONE_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
        _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_COMPLETE_PHASE_AN_COMPLETE, 0);
        return;
    }

    if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
    {   // wait some more; re-read
        _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
    }
    else
    {   // timeout
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED);
    }
}

static void _DRV_ETHPHY_NegCompletePhaseAN_Complete(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;
    __BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    phyStat.w = hClientObj->smiData;

    if(phyStat.AN_COMPLETE == 0)
    {   // not done yet
        if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
        {   // wait some more
            _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
        }
        else
        {   // timeout
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
        }
    }
    else
    {
        phyBMCon.w = hClientObj->operReg[1];   // restore BMCON read
        _DRV_PHY_NegCompleteSetOperResult(hClientObj, phyBMCon.w, phyStat.w);
    }

}

static void _DRV_ETHPHY_NegCompletePhaseResultNoWait(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t   phyBMCon;
    __BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    phyStat.w = hClientObj->smiData;
    phyBMCon.w = hClientObj->operReg[1];   // restore BMCON read

    _DRV_PHY_NegCompleteSetOperResult(hClientObj, phyBMCon.w, phyStat.w);
}

static void _DRV_PHY_NegCompleteSetOperResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj, uint16_t phyBmConReg, uint16_t phyStatReg)
{
    DRV_ETHPHY_RESULT res;
    __BMCONbits_t   phyBMCon;
    __BMSTATbits_t  phyStat;

    phyBMCon.w = phyBmConReg;
    phyStat.w = phyStatReg;

    if(!phyBMCon.AN_ENABLE)
    {
        res = DRV_ETHPHY_RES_NEGOTIATION_INACTIVE;       // no negotiation is taking place!
    }
    else if(phyBMCon.AN_RESTART)
    {
        res = DRV_ETHPHY_RES_NEGOTIATION_NOT_STARTED;        // not started yet/tmo
    }
    else
    {
        res = (phyStat.AN_COMPLETE == 0) ? DRV_ETHPHY_RES_NEGOTIATION_ACTIVE : DRV_ETHPHY_RES_OK;    // active/tmo/ok
    }

    _DRV_PHY_SetOperDoneResult(hClientObj, res);
}



DRV_ETHPHY_RESULT DRV_ETHPHY_NegotiationResultGet( DRV_HANDLE handle, DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult)
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    // basic sanity check
    if(pNegResult == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    hClientObj->operParam = (uintptr_t)pNegResult;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_NEG_RESULT, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}


static void _DRV_ETHPHY_ClientOpNegResult(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_NegResultPhasesTbl)/sizeof(*_DRV_PHY_NegResultPhasesTbl))
    {
        (*_DRV_PHY_NegResultPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_NegResultPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the control reg: PHY_REG_BMSTAT
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_BMSTAT, 0);
}

static void _DRV_ETHPHY_NegResultPhase_BMSTAT(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMSTATbits_t   phyStat;
    // wait the BMSTAT read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    phyStat.w = hClientObj->smiData;
    if(phyStat.AN_COMPLETE == 0)
    {   // negotiation is not yet done; we're finished
        _DRV_PHY_NegResultSetOperResult(hClientObj, ( DRV_ETHPHY_LINK_ST_DOWN | DRV_ETHPHY_LINK_ST_NEG_TMO), 0, TCPIP_ETH_PAUSE_TYPE_NONE, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
    }
    else if(phyStat.LINK_STAT == 0)
    {
        _DRV_PHY_NegResultSetOperResult(hClientObj, DRV_ETHPHY_LINK_ST_DOWN, 0, TCPIP_ETH_PAUSE_TYPE_NONE, DRV_ETHPHY_RES_NEGOTIATION_ACTIVE);
    }
    else
    {   // we're up and running
        // read the control reg: ANEXP
        _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_ANEXP);
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_ANEXP, 0);
    }
}


static void _DRV_ETHPHY_NegResultPhase_ANEXP(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the ANEXP read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // save the ANEXP for later
    hClientObj->operReg[0] = hClientObj->smiData;

    // read the control reg: ANLPAD
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_ANLPAD);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_ANLPAD, 0);

}

static void _DRV_ETHPHY_NegResultPhase_ANLPAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the ANLPAD read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // save the ANLPAD for later
    hClientObj->operReg[1] = hClientObj->smiData;

    // read the control reg: ANAD
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_ANAD);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_NEG_RESULT_PHASE_ANAD, 0);
}

static void _DRV_ETHPHY_NegResultPhase_ANAD(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __ANEXPbits_t    phyExp;
    __ANLPADbits_t   lpAD;
    __ANADbits_t     anadReg;
    int              lcl_Pause, lcl_AsmDir, lp_Pause, lp_AsmDir;     // pause capabilities, local and LP
    DRV_ETHPHY_LINK_STATUS  linkStat;
    TCPIP_ETH_OPEN_FLAGS   oFlags;
    TCPIP_ETH_PAUSE_TYPE   pauseType;

    // wait the ANAD read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    anadReg.w = hClientObj->smiData;
    // restore ANEXP
    phyExp.w = hClientObj->operReg[0];
    // restore ANLPAD
    lpAD.w = hClientObj->operReg[1];


    oFlags = 0;   // don't know the result yet
    pauseType = TCPIP_ETH_PAUSE_TYPE_NONE;
    linkStat = DRV_ETHPHY_LINK_ST_UP;

    lcl_Pause  = (hClientObj->hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_PAUSE) ? 1 : 0;
    lcl_AsmDir = (hClientObj->hDriver->macPauseType & TCPIP_ETH_PAUSE_TYPE_ASM_DIR) ? 1 : 0;
    lp_Pause   = lp_AsmDir = 0;         // in case negotiation failed

    if(phyExp.LP_AN_ABLE)
    {   // ok,valid auto negotiation info

        if(lpAD.REM_FAULT)
        {
            linkStat |= DRV_ETHPHY_LINK_ST_REMOTE_FAULT;
        }

        if(lpAD.PAUSE)
        {
            linkStat |= DRV_ETHPHY_LINK_ST_LP_PAUSE;
            lp_Pause = 1;
        }
        if(lpAD.ASM_DIR)
        {
            linkStat |= DRV_ETHPHY_LINK_ST_LP_ASM_DIR;
            lp_AsmDir = 1;
        }
    }
    else
    {
        lpAD.w     = _ANAD_BASE10T_MASK;  // lowest priority resolution
        linkStat |= DRV_ETHPHY_LINK_ST_LP_NEG_UNABLE;
        if(phyExp.PDF)
        {
            linkStat |= DRV_ETHPHY_LINK_ST_PDF;
        }
    }

    // set the PHY connection params
    // advertised capabilities & the matching ones
    anadReg.w &= lpAD.w;
    // get the settings, according to IEEE 802.3 Annex 28B.3 Priority Resolution
    // Note: we don't support 100BaseT4 !

    if(anadReg.w & _ANAD_BASE100TX_FDX_MASK)
    {
        oFlags = (TCPIP_ETH_OPEN_100|TCPIP_ETH_OPEN_FDUPLEX);
    }
    else if(anadReg.w & _ANAD_BASE100TX_MASK)
    {
        oFlags = (TCPIP_ETH_OPEN_100 | TCPIP_ETH_OPEN_HDUPLEX);
    }
    else if(anadReg.w & _ANAD_BASE10T_FDX_MASK)
    {
        oFlags = (TCPIP_ETH_OPEN_10 | TCPIP_ETH_OPEN_FDUPLEX);
    }
    else if(anadReg.w & _ANAD_BASE10T_MASK)
    {
        oFlags = (TCPIP_ETH_OPEN_10 | TCPIP_ETH_OPEN_HDUPLEX);
    }
    else
    {   // this should NOT happen!
        linkStat |= DRV_ETHPHY_LINK_ST_NEG_FATAL_ERR;
        linkStat &= ~DRV_ETHPHY_LINK_ST_UP;      // make sure we stop...!
    }

    // set the pause type for the MAC
    // according to IEEE Std 802.3-2002 Tables 28B-2, 28B-3
    if(oFlags & TCPIP_ETH_OPEN_FDUPLEX)
    {   // pause type relevant for full duplex only
        if(lp_Pause & (lcl_Pause | (lcl_AsmDir & lp_AsmDir)))
        {
            pauseType = TCPIP_ETH_PAUSE_TYPE_EN_TX;
        }
        if(lcl_Pause & (lp_Pause | (lcl_AsmDir & lp_AsmDir)))
        {
            pauseType |= TCPIP_ETH_PAUSE_TYPE_EN_RX;
        }
    }

    _DRV_PHY_NegResultSetOperResult(hClientObj, linkStat, oFlags, pauseType, DRV_ETHPHY_RES_OK);
}


static void _DRV_PHY_NegResultSetOperResult(DRV_ETHPHY_CLIENT_OBJ* hClientObj, DRV_ETHPHY_LINK_STATUS linkStatus,
       TCPIP_ETH_OPEN_FLAGS openFlags, TCPIP_ETH_PAUSE_TYPE pauseType, DRV_ETHPHY_RESULT res)
{
    DRV_ETHPHY_NEGOTIATION_RESULT* pNegResult = (DRV_ETHPHY_NEGOTIATION_RESULT*)hClientObj->operParam;

    if(pNegResult)
    {
        pNegResult->linkStatus = linkStatus;
        pNegResult->linkFlags = openFlags;
        pNegResult->pauseType = pauseType;
    }

    _DRV_PHY_SetOperDoneResult(hClientObj, res);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_LinkStatusGet( DRV_HANDLE handle, DRV_ETHPHY_LINK_STATUS* pLinkStat, bool refresh )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }


    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    // basic sanity check
    if(pLinkStat == 0)
    {
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }

    hClientObj->operParam = (uintptr_t)pLinkStat;
    hClientObj->operReg[0] = refresh;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_LINK_STAT, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}
 
static void _DRV_ETHPHY_ClientOpLinkStat(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_LinkStatPhasesTbl)/sizeof(*_DRV_PHY_LinkStatPhasesTbl))
    {
        (*_DRV_PHY_LinkStatPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_LinkStatPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // read the link status: PHY_REG_BMSTAT
    _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_LINK_STAT_PHASE_READ, 0);
}

// read from PHY_REG_BMSTAT
static void _DRV_ETHPHY_LinkStatPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // read the link status
    phyStat.w = hClientObj->smiData;
    if( phyStat.LINK_STAT != 0 || hClientObj->operReg[0] == 0)
    {   // link up or no refresh needed
        DRV_ETHPHY_LINK_STATUS* pLinkStat = (DRV_ETHPHY_LINK_STATUS*)hClientObj->operParam;
        if(pLinkStat)
        {
            *pLinkStat = _Phy2LinkStat(phyStat);
        }
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
    }
    else
    {   // initiate another read
        _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMSTAT);
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_LINK_STAT_PHASE_REFRESH, 0);
    }
}

// read from PHY_REG_BMSTAT
static void _DRV_ETHPHY_LinkStatPhaseRefresh(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMSTATbits_t  phyStat;

    // wait the BMSTAT read to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    // read the link status
    phyStat.w = hClientObj->smiData;

    DRV_ETHPHY_LINK_STATUS* pLinkStat = (DRV_ETHPHY_LINK_STATUS*)hClientObj->operParam;
    if(pLinkStat)
    {
        *pLinkStat = _Phy2LinkStat(phyStat);
    }

    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
}


DRV_ETHPHY_RESULT DRV_ETHPHY_Reset( DRV_HANDLE handle, bool waitComplete )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    /* Check for the Client validity */
    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_READY)
    {   // another op going on
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    hClientObj->operParam = waitComplete;
    _DRV_PHY_SetOperStart(hClientObj, DRV_ETHPHY_CLIENT_OP_TYPE_RESET, DRV_ETHPHY_RES_PENDING);

    return DRV_ETHPHY_RES_PENDING;
}


static void _DRV_ETHPHY_ClientOpReset(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{

    if((unsigned int)hClientObj->operPhase < sizeof(_DRV_PHY_ResetPhasesTbl)/sizeof(*_DRV_PHY_ResetPhasesTbl))
    {
        (*_DRV_PHY_ResetPhasesTbl[hClientObj->operPhase])(hClientObj);
    }
    else
    {   // shouldn't happen
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

static void _DRV_ETHPHY_ResetPhaseIdle(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // initiate Soft Reset of the PHY
    _DRV_PHY_SMIWriteStart(hClientObj, PHY_REG_BMCON, _BMCON_RESET_MASK);
    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_RESET_PHASE_WRITE, 0);
}

static void _DRV_ETHPHY_ResetPhaseWrite(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    // wait the BMCON write to complete
    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {
        return;
    }

    if( hClientObj->operParam == 0)
    {   // no need to wait, we're done
        _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
    }
    else
    {   // read the BMCON
        hClientObj->operTStamp = SYS_TMR_TickCountGet() + ((DRV_ETHPHY_RESET_CLR_TMO * SYS_TMR_TickCounterFrequencyGet()) + 999) / 1000;
        _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_RESET_PHASE_READ, 0);
    }
}

static void _DRV_ETHPHY_ResetPhaseRead(DRV_ETHPHY_CLIENT_OBJ * hClientObj)
{
    __BMCONbits_t bmcon;

    switch(hClientObj->operSubPhase)
    {
        case 0:
            // initiate BMCON read
            _DRV_PHY_SMIReadStart(hClientObj, PHY_REG_BMCON);
            _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_RESET_PHASE_READ, 1);
            break;

        case 1:
            // wait the BMCON read to complete
            if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
            {
                break;
            }


            bmcon.w = hClientObj->smiData;
            if(bmcon.RESET)
            {   // wait to clear itself
                if(SYS_TMR_TickCountGet() < hClientObj->operTStamp)
                {   // not yet tmo; re-read
                    _DRV_PHY_SetOperPhase(hClientObj, DRV_ETHPHY_RESET_PHASE_READ, 0);
                }
                else
                {   // failed by timeout
                    _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_DTCT_ERR);
                }
            }
            else
            {   // reset cleared itself; success
                _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OK);
            }
            break;

        default:
            // shouldn't happen
            _DRV_PHY_SetOperDoneResult(hClientObj, DRV_ETHPHY_RES_OPERATION_ERR);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Vendor interface Routines
// *****************************************************************************
// *****************************************************************************


DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataGet( DRV_HANDLE handle, uint32_t* pVendorData )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    if(pVendorData)
    {
        *pVendorData = hClientObj->vendorData;
    }

    return DRV_ETHPHY_RES_OK;

}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorDataSet( DRV_HANDLE handle, uint32_t vendorData )
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    hClientObj->vendorData = vendorData;

    return DRV_ETHPHY_RES_OK;

}



DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadStart( DRV_HANDLE handle, uint16_t rIx,  int phyAddress )
{
    DRV_ETHPHY_SMI_TXFER_OP_STATUS opStatus;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    
    if( hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || 
        hClientObj->operType != DRV_ETHPHY_CLIENT_OP_TYPE_SETUP || 
        hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_NEGOTIATE)
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_DONE && hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_NONE)
    {   // calling this before the previous SMI done?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }


    opStatus = _DRV_PHY_SMIReadStartEx(hClientObj, rIx, phyAddress);
    if(opStatus == DRV_ETHPHY_SMI_TXFER_OP_START)
    {   // needs to be retried
        // reset SMI transfer status so we can retry operation
        hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
        return DRV_ETHPHY_RES_PENDING;
    }

    return DRV_ETHPHY_RES_OK;
}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIReadResultGet( DRV_HANDLE handle, uint16_t* pSmiRes)
{
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    
    if( hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || 
        hClientObj->operType != DRV_ETHPHY_CLIENT_OP_TYPE_SETUP || 
        hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_NEGOTIATE)
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(_DRV_PHY_SMITransferDo(hClientObj) != DRV_ETHPHY_SMI_TXFER_OP_DONE)
    {   // wait some more
        return DRV_ETHPHY_RES_PENDING;
    }

    if(pSmiRes)
    {
        *pSmiRes = hClientObj->smiData;
    }
    
    return DRV_ETHPHY_RES_OK;

}

DRV_ETHPHY_RESULT DRV_ETHPHY_VendorSMIWriteStart( DRV_HANDLE handle, uint16_t rIx,  uint16_t wData, int phyAddress )
{
    DRV_ETHPHY_SMI_TXFER_OP_STATUS opStatus;
    DRV_ETHPHY_CLIENT_OBJ * hClientObj = (DRV_ETHPHY_CLIENT_OBJ *) handle;

    if(hClientObj == 0)
    {
        return DRV_ETHPHY_RES_HANDLE_ERR;
    }

    
    if( hClientObj->status != DRV_ETHPHY_CLIENT_STATUS_BUSY || 
        hClientObj->operType != DRV_ETHPHY_CLIENT_OP_TYPE_SETUP || 
        hClientObj->operPhase != DRV_ETHPHY_SETUP_PHASE_NEGOTIATE)
    {   // not allowed
        return DRV_ETHPHY_RES_NOT_READY_ERR;
    }

    if(hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_DONE && hClientObj->smiTxferStatus != DRV_ETHPHY_SMI_TXFER_OP_NONE)
    {   // calling this before the previous SMI done?
        return DRV_ETHPHY_RES_OPERATION_ERR;
    }


    opStatus = _DRV_PHY_SMIWriteStartEx(hClientObj, rIx, wData, phyAddress);
    if(opStatus == DRV_ETHPHY_SMI_TXFER_OP_START)
    {   // needs to be retried
        // reset SMI transfer status so we can retry operation
        hClientObj->smiTxferStatus = DRV_ETHPHY_SMI_TXFER_OP_NONE;
        return DRV_ETHPHY_RES_PENDING;
    }

    return DRV_ETHPHY_RES_OK;

}


/*******************************************************************************
End of File
*/
