/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: diskio.c$
* $Version : 
* $Date    : 
*
* Comments: This file implements low level disk interface module
*
*   
*
*END************************************************************************/

#include "diskio.h"		/* Common include file for FatFs and disk I/O layer */
#include "usb_host_msd_ufi.h"
#include "khci.h"
#include "hidef.h"
#include "poll.h"

#if (defined (__CWCC__) && defined(_MK_xxx_H_))
#define _ALIGNED_ __attribute__((aligned (4)))
#elif (defined (__CWCC__) && !defined(_MK_xxx_H_))
#define _ALIGNED_
#pragma pack(4)
#elif (defined __IAR_SYSTEMS_ICC__) || defined(__CC_ARM)
#pragma data_alignment=4
#define _ALIGNED_
#else
#define _ALIGNED_
#error "Toolchain not supported!"
#endif

/**************************************************************************
   Global variables
**************************************************************************/
volatile DEVICE_STRUCT   mass_device = { 0 };   /* mass storage device struct */
extern uint_8 endpoint_stalled;
extern uint_8 sense_error;

/**************************************************************************
*   Local variables
**************************************************************************/
static volatile boolean       bCallBack = FALSE;
static volatile USB_STATUS    bStatus = USB_OK;
/* Command object used in SCSI commands */
static _ALIGNED_ COMMAND_OBJECT_STRUCT  pCmd;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : callback_bulk_pipe
* Returned Value : 
* Comments       : Initialize Disk Drive (always successful) 
*
*END*--------------------------------------------------------------------*/
DSTATUS disk_initialize 
	(
	  /* [IN] Physical drive number (0) */
		uint_8 drv		
	)
{
	static CBW_STRUCT_PTR cbw_ptr = NULL; 
	static CSW_STRUCT_PTR csw_ptr = NULL;

	/* Allocate memory for Command Block Wrapper and Command Status Wrapper */
	if(NULL == cbw_ptr)
	{
		cbw_ptr = (CBW_STRUCT_PTR) malloc(sizeof(CBW_STRUCT));      
	}

	pCmd.CBW_PTR = cbw_ptr;

	if(NULL == pCmd.CBW_PTR)
	{
		return RES_NOT_ENOUGH_CORE;
	}

	if(NULL == csw_ptr)
	{
		csw_ptr = (CSW_STRUCT_PTR) malloc(sizeof(CSW_STRUCT));
	}

	pCmd.CSW_PTR = csw_ptr;

	if(NULL == pCmd.CSW_PTR)
	{
		return RES_NOT_ENOUGH_CORE;    
	}

	/* Store the address of CBW and CSW */
	cbw_ptr = pCmd.CBW_PTR;
	csw_ptr = pCmd.CSW_PTR;

	memset(pCmd.CSW_PTR, 0, sizeof(CSW_STRUCT));
	memset(pCmd.CBW_PTR, 0, sizeof(CBW_STRUCT));
	memset(&pCmd, 0, sizeof(COMMAND_OBJECT_STRUCT));

	/* Init SCSI command object */
	pCmd.CBW_PTR  = cbw_ptr;
	pCmd.CSW_PTR  = csw_ptr;
	pCmd.LUN      = drv;
	pCmd.CALL_PTR = (pointer)&mass_device.class_intf;
	pCmd.CALLBACK = callback_bulk_pipe;

	return RES_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : disk_read
* Returned Value : 
* Comments       : Read Sector(s) 
*
*END*--------------------------------------------------------------------*/
DRESULT disk_read 
(
		/* [IN] Physical drive number (0) */
		uint_8 drv,			
		/* [OUT] Pointer to the data buffer to store read data */
		uint_8 *buff,			
		/* [IN] Start sector number (LBA) */
		uint_32 sector,		
		/* [IN] Sector count (1..128) */
		uint_8 count			
)
{
	DRESULT 	res = RES_OK;
	USB_STATUS      status = USB_OK;

	UNUSED(drv)

	/* Check sector count */
	if (!count) return RES_PARERR;

	/* Send read_10 SCSI command */
	bCallBack = FALSE;
	status = usb_mass_ufi_read_10(&pCmd, sector, buff, (uint_32)(512*count), count);
	if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
	{
		res = RES_ERROR;
	}
	else
	{
		while(!bCallBack)	/* Wait till command comes back */
		{
			Poll();
		}
                
                if(endpoint_stalled)
		{
                        endpoint_stalled = FALSE;                                                             				
                        res = disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
                                
		}                
                
		if (!bStatus) 
		{
			res = RES_OK;
		}
		else 
		{
			res = RES_NOTRDY;
		}         
	}
	return res;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : disk_write
* Returned Value : 
* Comments       : Write Sector(s) 
*
*END*--------------------------------------------------------------------*/

DRESULT disk_write 
  (
    /* [IN] Physical drive nmuber (0) */
  	uint_8 drv,		
  	/* [IN] Pointer to the data to be written */	
  	uint_8 *buff,	
  	/* [IN] Start sector number (LBA) */
  	uint_32 sector,		
  	/* [IN] Sector count (1..128) */
  	uint_8 count			
  )
{
	DRESULT         res = RES_OK;
	USB_STATUS      status = USB_OK;
	
	UNUSED(drv)
	
	/* Check sector count */
	if (!count) return RES_PARERR;

        /* Send Write_10 SCSI command */
	bCallBack = FALSE;
        status = usb_mass_ufi_write_10(&pCmd, sector, buff, (uint_32)(512*count), count);
	if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
	{
		 res = RES_ERROR;
	}
	else
	{
		while(!bCallBack)	/* Wait till command comes back */
                {
                        Poll();
                }
                
                if(endpoint_stalled)
		{
                        endpoint_stalled = FALSE;                                                             				
                        res = disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
                                
		}                
                
		if (!bStatus) 
		{
			 res = RES_OK;
		}
		else 
		{
			res = RES_NOTRDY;
		}         
	}		      	
	return res;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : disk_ioctl
* Returned Value : 
* Comments       : The disk_ioctl function controls device specified features
*                  and miscellaneous functions other than disk read/write 
*
*END*--------------------------------------------------------------------*/
DRESULT disk_ioctl 
(
  /* [IN] Drive number */
  uint_8 Drive,      
  /* [IN] Control command code */
  uint_8 Command, 
  /* [IN/OUT] Parameter and data buffer */   
  void* Buff
)
{
	_ALIGNED_ DRESULT										res = RES_OK;
	_ALIGNED_ USB_STATUS   								    status = USB_OK;
	_ALIGNED_ MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO 	read_capacity;
	static _ALIGNED_ CAPACITY_LIST                          capacity_list;
	static _ALIGNED_ INQUIRY_DATA_FORMAT                    inquiry;
	static _ALIGNED_ REQ_SENSE_DATA_FORMAT                  req_sense;

	UNUSED(Drive);

        switch (Command)
        {
            case UFI_TEST_UNIT_READY_CMD:
            /* Send test unit ready SCSI command */
            bCallBack = FALSE;
            status = usb_mass_ufi_test_unit_ready(&pCmd);
            if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
            {
                res = RES_ERROR;         
            }
            else
            {
                /* Wait till command comes back */
                while(!bCallBack)	
                {
                    Poll();
                }
                
                /*
                * workaround AI:
                * After the endpoint STALL flag is cleared we have to issue another 
                * REQUEST_SENSE commmand
                */
		if(endpoint_stalled)
		{
                      endpoint_stalled = FALSE;                                                             				
                      res = disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
                }

		if (!bStatus) 
		{                                
                      res = RES_OK;  
		}
		else 
		{              
                      res = RES_NOTRDY;				    
		}         
          }		          
          break;
        case UFI_READ_CAPACITY_CMD:
          /* Send read_capacity SCSI command */
          bCallBack = FALSE;
          status = usb_mass_ufi_read_capacity(&pCmd, (uchar_ptr)&capacity_list,
                   sizeof(CAPACITY_LIST));
          if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
          {
                res = RES_ERROR;         
          }
          else
          {
                /* Wait till command comes back */
                while(!bCallBack)	
		{
                        Poll();
		}     
			
                if(endpoint_stalled)
		{
			endpoint_stalled = FALSE;                                
			res = disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
		}                        

		if (!bStatus) 
		{
			res = RES_OK;   
		}
		else 
		{
			res = RES_NOTRDY;  
		}         
          }		          
          break;
        case UFI_READ_FORMAT_CAPACITY_CMD:
          /* Send read_capacity SCSI command */
          bCallBack = FALSE;
          status = usb_mass_ufi_format_capacity(&pCmd, (uchar_ptr)&read_capacity,\
                   sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
          if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
          {
                res = RES_ERROR;   
          }
          else
          {
		/* Wait till command comes back */
		while(!bCallBack)	
		{
			Poll();
		}
			
                if(endpoint_stalled)
		{
			endpoint_stalled = FALSE;                                
			res = disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
		}                        
                        
		if (!bStatus) 
		{
			res = RES_OK;    
		}
		else 
		{
			res = RES_NOTRDY;  
		}         
	  }		          
          break;
        case UFI_INQUIRY_CMD:
          /* Send read_capacity SCSI command */
          bCallBack = FALSE;
          status = usb_mass_ufi_inquiry(&pCmd, (uchar_ptr) &inquiry,
                   sizeof(INQUIRY_DATA_FORMAT));
          if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
          {
		res = RES_ERROR;  
          }
          else
          {
		/* Wait till command comes back */
		while(!bCallBack)	
		{
			Poll();
		}

		if(endpoint_stalled)
		{
			endpoint_stalled = FALSE;                                
			res = disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
		}
                        
		if (!bStatus) 
		{
			res = RES_OK;   
		}
		else 
		{
			res = RES_NOTRDY;  
		}         
          }		          
          break;
        case REQUEST_SENSE_CMD:
          /* Send read_capacity SCSI command */
          bCallBack = FALSE;
          status = usb_mass_ufi_request_sense(&pCmd, &req_sense,
                   sizeof(REQ_SENSE_DATA_FORMAT));
          if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
          {
                res = RES_ERROR;  
          }
          else
          {
		/* Wait till command comes back */
		while(!bCallBack)
		{
                        Poll();
		}
                        
                if (!bStatus)
		{
			res = RES_OK; 
		}
		else 
		{
                        res = RES_NOTRDY; 
		}
          }          
          break;
        case GET_SECTOR_COUNT:
        case GET_SECTOR_SIZE:
          /* Send read_capacity SCSI command */
          bCallBack = FALSE;
          status = usb_mass_ufi_read_capacity(&pCmd, (uchar_ptr)&read_capacity,\
                   sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
          if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
          {
                res = RES_ERROR;   
          }
          else
          {
		/* Wait till command comes back */
		while(!bCallBack)
		{
			Poll();
		}
                        
		if(endpoint_stalled)
		{
			endpoint_stalled = FALSE;                                
			res = disk_ioctl(0, REQUEST_SENSE_CMD, NULL);
		}                        
                        
		if (!bStatus)
		{
                        res = RES_OK;    
		}
		else
		{
			res = RES_NOTRDY;  
		} 
          }

          if(!Buff)
		return res = RES_ERROR;
		
          /* Get number of sectors on the disk (DWORD) */
          if (GET_SECTOR_COUNT == Command)
          {
		*(uint_32 *)Buff = *(uint_32 *)read_capacity.BLLBA +1 ;
          }
          /* Get the sector size in byte */
          else
          {
		*(uint_32 *)Buff = *(uint_32 *)read_capacity.BLENGTH;
          }
          break;          
        case GET_BLOCK_SIZE:
          if(!Buff)
		return res = RES_ERROR;
		
          *(uint_32*)Buff = ERASE_BLOCK_SIZE;
          res = RES_OK;           
          break;
        case CTRL_SYNC:
          res = RES_OK;    
          break;
        default:
          res = RES_PARERR; 
          break;
        }  
        return res;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : disk_status
* Returned Value : STA_OK: drive is ready to write and read
*                  STA_NOINIT: dis is not initialized
* Comments       : Get the status of drive
*
*END*--------------------------------------------------------------------*/
DSTATUS disk_status
  (
    /* [IN] Physical drive number */
    uint_8 Drive     
  )
{
  if (Drive) return STA_NOINIT;		/* Supports only drive 0 */
  
  return STA_OK;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : callback_bulk_pipe
* Returned Value : None
* Comments       : Called on completion of a control-pipe transaction.
*
*END*--------------------------------------------------------------------*/
void callback_bulk_pipe
   (
      /* [IN] Status of this command */
      USB_STATUS status,

      /* [IN] pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
      pointer p1,

      /* [IN] pointer to the command object*/
      pointer  p2,

      /* [IN] Length of data transmitted */
      uint_32 buffer_length
   )
{ /* Body */

   UNUSED(p1)
   UNUSED(p2)
   UNUSED(buffer_length)

   bCallBack = TRUE;
   bStatus = status;

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : get_fattime
* Returned Value : Currnet time is returned with packed into a unit_32 value. 
*                  The bit field is as follows:
*                       bit31:25  Year from 1980 (0..127)
*                       bit24:21  Month (1..12)
*                       bit20:16  Day in month(1..31)
*                       bit15:11  Hour (0..23)
*                       bit10:5   Minute (0..59)
*                       bit4:0    Second / 2 (0..29) 
* Comments       : User Provided Timer Function for FatFs module
*
*END*--------------------------------------------------------------------*/
uint_32 get_fattime (void)
{
	return	  ((uint_32)(2010 - 1980) << 25)	/* Fixed to Jan. 1, 2010 */
			| ((uint_32)1 << 21)
			| ((uint_32)1 << 16)
			| ((uint_32)0 << 11)
			| ((uint_32)0 << 5)
			| ((uint_32)0 >> 1);
}

