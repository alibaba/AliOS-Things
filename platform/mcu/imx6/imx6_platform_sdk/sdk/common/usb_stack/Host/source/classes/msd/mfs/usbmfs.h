#ifndef _usbmfs_h_
#define _usbmfs_h_
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
* $FileName: usbmfs.h$
* $Version : 3.0.11.0$
* $Date    : Jun-24-2009$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for USB MFS mass storage link driver.
*
*END************************************************************************/

#include "io.h"
#include "ioctl.h"
#include "host_main.h"

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/*
** USB_MFS IOCTL calls
*/
#define USB_MFS_IOCTL_GET_NUM_SECTORS     _IO(IO_TYPE_USBMFS,0x02)
//#define USB_MFS_IOCTL_GET_BLOCK_SIZE        (0x0103)
//#define USB_MFS_IOCTL_GET_SECTOR_SIZE     (0x0103)
#define USB_MFS_IOCTL_GET_DRIVE_PARAMS    _IO(IO_TYPE_USBMFS,0x05)
#define USB_MFS_IOCTL_DEVICE_STOP         _IO(IO_TYPE_USBMFS,0x09)
#define USB_MFS_IOCTL_SET_BLOCK_MODE      _IO(IO_TYPE_USBMFS,0x0A)

//#define IO_DEV_TYPE_PHYS_USB_MFS          (0x0)

#define USB_MFS_VERY_LARGE_DRIVE           (3)

#ifdef PSP_CACHE_LINE_SIZE
   #define CACHE_LINE_FILLER(x)   (PSP_CACHE_LINE_SIZE-(x%PSP_CACHE_LINE_SIZE))
#else
   #define CACHE_LINE_FILLER(x)
#endif

/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/

typedef struct usb_mfs_drive_info_struct
{
   uint_32  NUMBER_OF_HEADS; 
   uint_32  NUMBER_OF_TRACKS;
   uint_32  SECTORS_PER_TRACK;
} USB_MFS_DRIVE_INFO_STRUCT, _PTR_ USB_MFS_DRIVE_INFO_STRUCT_PTR;
    
/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif

uint_32 _io_usb_mfs_install(char_ptr, uint_8, CLASS_CALL_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */