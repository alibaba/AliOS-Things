#ifndef __usb_host_msd_bo_h__
#define __usb_host_msd_bo_h__
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
* $FileName: usb_host_msd_bo.h$
* $Version : 3.0.9.0$
* $Date    : Jun-17-2009$
*
* Comments:
*
*   This file is a header file which defines all necessary structure to
*   be used for sending commands to mass storage devices.
*
*END************************************************************************/
#include "types.h"

/* System include files */
#include "hostapi.h"

/*******************************************************************
**
** Values specific to CLEAR FEATURE commands (must go to common.h later)
*/



/*******************************************************************
**
** Mass Storage definitions
*/

/* Error codes */
#define USB_TRANSFER_CANCELLED         0xF1
#define USB_MASS_INVALID_CBW           -1
#define USB_MASS_QUEUE_FULL            -2
#define USB_MASS_NO_MATCHING_REQUEST   -4
#define USB_MASS_INVALID_INTF_HANDLE   -5
#define USB_MASS_INVALID_BUFFER        -6
#define USB_MASS_FAILED_IN_COMMAND     -7
#define USB_MASS_FAILED_IN_DATA        -8
#define USB_MASS_FAILED_IN_STATUS      -9

/* Constants */
#define MAX_CBWCB_SIZE            16
#define CBW_DIRECTION_IN         0x80
#define CBW_DIRECTION_OUT        0x00
#define MAX_RETRIAL_ATTEMPTS     1
#define QUEUE_SIZE               4
#define DIRECTION_OUT            0x00
#define DIRECTION_IN             0x80
#define MASK_NON_DIRECTION_BITS  0x80

/* Command Status Wrapper Signature 'USBS'*/
#define CBW_SIGNATURE               0x43425355
#define CSW_SIGNATURE               0x53425355
#define CSW_STATUS_GOOD             0x00
#define CSW_STATUS_FAILED           0x01
#define CSW_STATUS_PHASE_ERROR      0x02

/* some useful defines */
#define ZERO_BIT0      0xFE
#define BE_HIGH_BYTE   0    /* LSB last in UFI specs (big endian)    */
#define BE_LOW_BYTE    1    /* LSB last in UFI specs (big endian)    */
#define LE_HIGH_BYTE   1    /* LSB last in UFI specs (little endian) */
#define LE_LOW_BYTE    0    /* LSB last in UFI specs (little endian) */

/*
** high nibble in x should be 1111
** transfer the low nibble of x to low nibble of a
*/
#define TRANSFER_LOW_NIBBLE(x,a)   (a) = (uint_8)(((x) & 0x0F) | ((a) & 0xF0))

#define CSW_SIZE  0x0C
#define CBW_SIZE  0x1F
#define GET_MAX_LUN   0xFE
#define MASS_STORAGE_RESET 0xFF


#define USB_CLASS_MASS_IS_Q_EMPTY(intf_ptr)      (boolean)(intf_ptr->QUEUE.COUNT==0)


/*********************************************************************
**
** Typedefs
*/

/* UFI Typical Command Block Wrapper for Most commands */
typedef struct _UFI_CBWCB
{
    uint_8      BUFIOPCODE;                 /* 0 */
    uint_8      BUFILUN;                    /* 1 */
    uint_8      BUFILOGICALBLOCKADDRESS[4]; /* 2,3,4,5 */
    uint_8      RESERVED6;                  /* 6 Reserved */
    uint_8      BLENGTH[2];                 /* 7,8 length of the data block */
    uint_8      RESERVED9;                  /* 9 Reserved */
    uint_8      RESERVED10;                 /* 10 Reserved */
    uint_8      RESERVED11;                 /* 11 Reserved */
} UFI_CBWCB_STRUCT, _PTR_ UFI_CBWCB_STRUCT_PTR;


/* UFI Typical Command Block Wrapper for Extended commands */
typedef struct _UFI_CBWCB_EXTENDED
{
    uint_8      BUFIOPCODE;                 /* 0 */
    uint_8      BUFILUN;                    /* 1 */
    uint_8      BUFILOGICALBLOCKADDRESS[4]; /* 2,3,4,5 */
    uint_8      BLENGTH[4];                 /* 6,7,8,9 length of the data block */
    uint_8      RESERVED10;                 /* 10 Reserved */
    uint_8      RESERVED11;                 /* 11 Reserved */
} UFI_CBWCB_EXTENDED, _PTR_ UFI_CBWCB_EXTENDED_STRUCT_PTR;


/* define a union that covers all supported protocols.  */

typedef union _CBWCB {
   UFI_CBWCB_STRUCT     UFI_CBWCB;
   UFI_CBWCB_EXTENDED   UFI_CBWCB_EXT;
   uint_8               MAX_COMMAND[16];
} CBWCB, _PTR_ CBWCB_PTR;

/* State machine constants of Class driver */
enum USB_CLASS_MASS_COMMAND_STATUS {
   STATUS_COMPLETED = USB_OK,
   STATUS_CANCELLED,
   STATUS_QUEUED_IN_DRIVER,
   STATUS_QUEUED_CBW_ON_USB,
   STATUS_FINISHED_CBW_ON_USB,
   STATUS_QUEUED_DPHASE_ON_USB,
   STATUS_FINISHED_DPHASE_ON_USB,
   STATUS_QUEUED_CSW_ON_USB,
   STATUS_FINISHED_CSW_ON_USB,
   STATUS_FAILED_IN_CSW,
   STATUS_RESET_BULK_IN,
   STATUS_RESET_BULK_OUT,
   STATUS_RESET_INTERFACE,
   STATUS_RESET_DEVICE,
   STATUS_CLEAR_BULK_PIPE
};

typedef enum USB_CLASS_MASS_COMMAND_STATUS USB_CLASS_MASS_COMMAND_STATUS;


/* Define the representation of a circular queue */
typedef struct _MASS_QUEUE_STRUCT
{
   pointer     ELEMENTS[QUEUE_SIZE]; /* storage for the queue*/
   // CR TBD
   uint_32     COUNT;
   uint_8      FIRST;         /* index of the first element in the queue*/
   uint_8      LAST;          /* index of the last element in the queue*/
} MASS_QUEUE_STRUCT, _PTR_ MASS_QUEUE_STRUCT_PTR;


/* Command Block Wrapper (see USB Mass Storage specs) */
#pragma pack(1)
typedef struct _CBW
{
   uint_8      DCBWSIGNATURE[4];            /* 0-3   */
   uint_8      DCBWTAG[4];                  /* 4-7   */
   uint_8      DCBWDATATRANSFERLENGTH[4];   /* 8-11  */
   uint_8      BMCBWFLAGS;                  /* 12    */
   uint_8      BCBWLUN;                     /* 13    */
   uint_8      BCBWCBLENGTH;                /* 14    */
   CBWCB       CMD_BLOCK;                    /* 15-31 */
} CBW_STRUCT, _PTR_ CBW_STRUCT_PTR;


/* Command Status Wrapper   (see USB Mass Storage specs) */
typedef struct _CSW
{
    uint_8     DCSWSIGNATURE[4];         /* 0-3    */
    uint_8     DCSWTAG[4];               /* 4-7    */
    uint_8     DCSWDATARESIDUE[4];       /* 8-11   */
    uchar      BCSWSTATUS;                /* 12     */
} CSW_STRUCT, _PTR_ CSW_STRUCT_PTR;


/* USB Mass Class  One Single Command Object for all protocols */
typedef struct _COMMAND_OBJECT {
   CLASS_CALL_STRUCT_PTR             CALL_PTR;      /* Class intf data pointer and key    */
   uint_32                           LUN;           /* Logical unit number on device      */
   CBW_STRUCT_PTR                    CBW_PTR;       /* current CBW being constructed      */
   CSW_STRUCT_PTR                    CSW_PTR;       /* CSW for this command               */
   void (_CODE_PTR_                  CALLBACK)
      (USB_STATUS,                            /* status of this command                       */
       pointer,                               /* pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT */
       pointer,                               /* pointer to the command object                */
       uint_32                                /* length of the data transfered if any         */
      );

   pointer                           DATA_BUFFER;   /* buffer for IN/OUT for the command  */
   uint_32                           BUFFER_LEN;    /* length of data buffer              */
   USB_CLASS_MASS_COMMAND_STATUS     STATUS;        /* current status of this command     */
   USB_CLASS_MASS_COMMAND_STATUS     PREV_STATUS;   /* previous status of this command    */
   uint_32                           TR_BUF_LEN;    /* length of the buffer received in
                                                       currently executed TR              */
   uint_8                            RETRY_COUNT;   /* Number of times this command tried  */
   uint_8                            TR_INDEX;      /* TR_INDEX of the TR used for search */
} COMMAND_OBJECT_STRUCT, _PTR_ COMMAND_OBJECT_PTR;


/*
** USB Mass Class Interface structure. This structure will be passed to all
** commands to this class driver. The structure holds all information
** pertaining to an interface on storage device. This allows the class driver
** to know which interface the command is directed for.
*/

typedef struct _Usb_Mass_Intf_Struct {
   GENERAL_CLASS                       G;                 // This is general class containing following
   _usb_pipe_handle                    CONTROL_PIPE;      /* control pipe handle*/
   _usb_pipe_handle                    BULK_IN_PIPE;      /* Bulk in pipe handle*/
   _usb_pipe_handle                    BULK_OUT_PIPE;     /* Bulk out pipe handle*/
   MASS_QUEUE_STRUCT                   QUEUE;             /* structure that queues requests*/
   uint_8                              INTERFACE_NUM;     /* interface number*/
   uint_8                              ALTERNATE_SETTING; /* Alternate setting*/
} USB_MASS_CLASS_INTF_STRUCT, _PTR_ USB_MASS_CLASS_INTF_STRUCT_PTR;


/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern int_32 usb_class_mass_q_insert(USB_MASS_CLASS_INTF_STRUCT_PTR,
   COMMAND_OBJECT_PTR);
extern void usb_class_mass_deleteq(USB_MASS_CLASS_INTF_STRUCT_PTR);
extern void usb_class_mass_get_pending_request(USB_MASS_CLASS_INTF_STRUCT_PTR,
   COMMAND_OBJECT_PTR _PTR_);
extern void usb_class_mass_q_init(USB_MASS_CLASS_INTF_STRUCT_PTR);
extern void usb_class_mass_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
extern void usb_class_mass_exit(void);
extern USB_STATUS usb_class_mass_storage_device_command(CLASS_CALL_STRUCT_PTR,
   COMMAND_OBJECT_PTR);
extern boolean usb_class_mass_storage_device_command_cancel(CLASS_CALL_STRUCT_PTR,
   COMMAND_OBJECT_PTR);
extern USB_STATUS usb_class_mass_getmaxlun_bulkonly(CLASS_CALL_STRUCT_PTR,
   uint_8_ptr, tr_callback);
extern USB_STATUS usb_class_mass_reset_recovery_on_usb(
   USB_MASS_CLASS_INTF_STRUCT_PTR);
extern boolean usb_class_mass_cancelq
   (
      USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr,
      COMMAND_OBJECT_PTR pCmd
   );
#ifdef __cplusplus
}
#endif

#endif
