#ifndef _usbmfspr_h_
#define _usbmfspr_h_
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
* $FileName: usbmfspr.h$
* $Version : 3.0.5.0$
* $Date    : Nov-27-2008$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions private to the USB mass storage link driver
*   to MFS.
*
*END************************************************************************/


#include "usbsem.h"
/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/* Error codes from lower layers */
#define USB_MFS_DEFAULT_SECTOR_SIZE  (512)

/*----------------------------------------------------------------------*/
/*
**                          ERROR CODES
*/

/* Error codes from lower layers */
//#define USB_MFS_NO_ERROR             MQX_OK
//#define USB_MFS_READ_ERROR           IO_ERROR_READ
//#define USB_MFS_WRITE_ERROR          IO_ERROR_WRITE
//#define USB_MFS_INVALID_SECTOR       IO_ERROR_SEEK

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/


/*
** USB_MFS_INFO_STRUCT
** Run time state information for each USB mass storage device
*/
typedef struct io_usb_mfs_info_struct
{
   COMMAND_OBJECT_STRUCT      COMMAND;    // MUST BE FIRST
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER1[CACHE_LINE_FILLER( sizeof(COMMAND_OBJECT_STRUCT))];
#endif
   CBW_STRUCT                 CBW;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER2[CACHE_LINE_FILLER( sizeof(CBW_STRUCT))];
#endif
   CSW_STRUCT                 CSW;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER3[CACHE_LINE_FILLER( sizeof(CSW_STRUCT))];
#endif
   INQUIRY_DATA_FORMAT        INQUIRY_DATA;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER4[CACHE_LINE_FILLER( sizeof(INQUIRY_DATA_FORMAT))];
#endif
  MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO   CAPACITY;
#if PSP_HAS_DATA_CACHE
   uchar                      FILLER5[CACHE_LINE_FILLER( sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO))];
#endif
   /* Handle for mass storage class  calls */
   pointer     MSC_STREAM;
   
   /* 8 bytes of ASCI Data identifying the vendor of the product */
   
   /* 16 bytes of ASCI Data defined by the vendor */
  
   /* 4 bytes of ASCI Data defined by the vendor */
   
   /* CBW tag used for commands */
   uint_32       CBW_TAG;         
   
   /* Drive number to associate with this slot */
   uint_8        LUN;
   
   uint_32       BLENGTH;	// logic block length
   uint_32       BCOUNT;	// logic blocks count

   /* Total size of Drive in bytes */
   uint_32       SIZE_BYTES; 

   /* The number of heads as reported  */
   uint_32       NUMBER_OF_HEADS;          

   /* The number of tracks as reported  */
   uint_32       NUMBER_OF_TRACKS;

   /* The number of sectos per cylinder as reported */
   uint_32       SECTORS_PER_TRACK;

   /* Light weight semaphore struct */
   USB_SEM_STRUCT  LWSEM;
   
   /* The address of temp buffer */

   /* The current error code for the device */
   uint_32       ERROR_CODE;
   
   /* Start CR 812 */
   /* Indicates if the device is running in block mode or character mode */
   boolean       BLOCK_MODE;
   /* End   CR 812 */

   uint_32                    COMMAND_STATUS;
   USB_SEM_STRUCT           COMMAND_DONE;
} IO_USB_MFS_STRUCT, _PTR_ IO_USB_MFS_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif
extern void _io_usb_mfs_callback  (USB_STATUS,      /*status of this command*/
               pointer,     /*pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
               pointer,     /*pointer to the command object*/
               uint_32       /* length of the data transfered if any */
               );
extern int_32 _io_usb_mfs_open(FILE_PTR, char _PTR_, char _PTR_);
extern int_32 _io_usb_mfs_close(FILE_PTR);
extern int_32 _io_usb_mfs_read (FILE_PTR, char_ptr, int_32);
extern int_32 _io_usb_mfs_write(FILE_PTR, char_ptr, int_32);
extern int_32 _io_usb_mfs_ioctl(FILE_PTR, int_32, pointer);
extern int_32 _io_usb_mfs_uninstall_internal
   (
      IO_DEVICE_STRUCT_PTR   io_dev_ptr
   );
extern int_32 _io_usb_mfs_read_sector_internal(IO_USB_MFS_STRUCT_PTR, uint_32, uint_16, 
   uchar_ptr);
extern int_32 _io_usb_mfs_read_sector_long_internal(IO_USB_MFS_STRUCT_PTR, uint_32, uint_32, 
   uchar_ptr);
   
extern int_32  _io_usb_mfs_read_partial_sector_internal(FILE_PTR, uint_32, 
   uint_32, uint_32, uchar_ptr);
extern int_32 _io_usb_mfs_write_sector_internal(IO_USB_MFS_STRUCT_PTR, uint_32, uint_16, 
   uchar_ptr);
extern int_32 _io_usb_mfs_write_sector_long_internal(IO_USB_MFS_STRUCT_PTR, uint_32, uint_32,
   uchar_ptr);
   
extern int_32  _io_usb_mfs_write_partial_sector_internal(FILE_PTR, 
   uint_32, uint_32, uint_32, uchar_ptr);
   
/* Start CR 812 */
extern int_32 _io_usb_mfs_read_write_sectors_internal(FILE_PTR, 
   IO_USB_MFS_STRUCT_PTR, char_ptr, uint_32, boolean);
/* End   CR 812 */
   
#ifdef __cplusplus
}
#endif

#endif
/* EOF */