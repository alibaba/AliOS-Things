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
 * $FileName: diskio.h$
 * $Version : 
 * $Date    : 
 *
 * Comments: Low level disk interface module include file
 *
 *   
 *
 *END************************************************************************/

#include "psptypes.h"
#include "host_common.h"

#ifndef _DISKIO
#define _DISKIO

#define _READONLY	0	/* 1: Remove write functions */
#define _USE_IOCTL	1	/* 1: Use disk_ioctl function */
/* Status of Disk Functions */
typedef uint_8	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,	       	/* 0: Successful */
	RES_ERROR,	      	/* 1: R/W Error */
	RES_WRPRT,	      	/* 2: Write Protected */
	RES_NOTRDY,	      	/* 3: Not Ready */
	RES_PARERR,	      	/* 4: Invalid Parameter */
	RES_NOT_ENOUGH_CORE /* 5: Not enough heap memory */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */
/*---------------------------------------*/

DSTATUS disk_initialize (uint_8);
DRESULT disk_read (uint_8, uint_8*, uint_32, uint_8);
DRESULT disk_write (uint_8, uint_8*, uint_32, uint_8);
DRESULT disk_ioctl (uint_8 ,uint_8 ,void* );
DSTATUS disk_status ( uint_8 Drive);
uint_32 get_fattime (void);

/* Disk Status Bits (DSTATUS) */
#define STA_OK			0x00	/* Drive is ready */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */

/* Command code for disk_ioctrl function */
/* Generic command (mandatory for FatFs) */
#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */
#define ERASE_BLOCK_SIZE	1 	/* Unknown erasable block size*/

#define REQUEST_SENSE_CMD				4
#define UFI_INQUIRY_CMD					5
#define UFI_READ_FORMAT_CAPACITY_CMD	6
#define UFI_READ_CAPACITY_CMD			7
#define UFI_TEST_UNIT_READY_CMD			8
#endif

/* Application-specific definitions */
#define  MAX_PENDING_TRANSACTIONS   16
#define  MAX_FRAME_SIZE             1024
#define  HOST_CONTROLLER_NUMBER     0
#define  BUFF_IN_SIZE 				0x40c

/*
 ** Following structs contain all states and pointers
 ** used by the application to control/operate devices.
 */
/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif
	void callback_bulk_pipe (USB_STATUS status,
			pointer p1,
			pointer  p2,
			uint_32 buffer_length
	);

#ifdef __cplusplus
}
#endif



typedef struct device_struct
{
	uint_32                          dev_state;  /* Attach/detach state */
	PIPE_BUNDLE_STRUCT_PTR           pbs;        /* Device & pipes */
	_usb_device_instance_handle      dev_handle;
	_usb_interface_descriptor_handle intf_handle;
	CLASS_CALL_STRUCT                class_intf; /* Class-specific info */

} DEVICE_STRUCT,  _PTR_ DEVICE_STRUCT_PTR;



/* EOF */






