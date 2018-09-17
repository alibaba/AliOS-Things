#ifndef __usb_host_msd_ufi_h__
#define __usb_host_msd_ufi_h__
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
* $FileName: usb_host_msd_ufi.h$
* $Version : 3.0.5.0$
* $Date    : Jun-17-2009$
*
* Comments:
*
*   This file is a supporting header file for UFI.c which implements the UFI
*   command set for mass storage applications
*
*END************************************************************************/

#include "usb_host_msd_bo.h"

/* UFI specific error codes */
#define INVALID_UFI_REQUEST               (-1)

/* a should be two byte number */
#define HIGH_BYTE(a)                      ((a) >> 8)
#define LOW_BYTE(a)                       ((a) & 0xFF)

/*some useful macros for UFI command block*/
#define TRANSFER_LUN_TO_HIGH_3_BITS(x,l)   x &= ((l << 5)   | 0x1F)


/* Operation code for the UFI Commands (floppy drives) */
#define UFI_FORMAT_UNIT                   (0x04)
#define FORMAT_LUN_HEADER_BITS            (0x17)
#define UFI_READ_CAPACITY                 (0x25)
#define READ_CAPACITY_LUN_HEADER_BITS     (0x00)
#define UFI_READ_FORMAT_CAPACITY          (0x23)
#define UFI_INQUIRY                       (0x12)
#define INQUIRY_LUN_HEADER_BITS           (0x00)
#define UFI_MODE_SELECT                   (0x55)
#define MODE_SELECT_LUN_HEADER_BITS       (0x10)
#define UFI_MODE_SENSE                    (0x5A)
#define MODE_SENSE_LUN_HEADER_BITS        (0x00)
#define PAGE_CURRENT_VALUE                (0x00)
#define PAGE_CHANGEABLE_VALUE             (0x01)
#define PAGE_DEFAULT_VALUE                (0x10)
#define PAGE_SAVED_VALUE                  (0x11)

#define PAGE_CODE_ALL_PAGES               (0x3F)

#define UFI_PREVENT_ALLOW_MEDIUM_ROMVAL   (0x1E)
#define UFI_READ10                        (0x28)
#define READ10_LUN_HEADER_BITS            (0x00)
#define UFI_READ12                        (0xA8)
#define READ12_LUN_HEADER_BITS            (0x00)
#define UFI_REQUEST_SENSE                 (0x03)
#define UFI_REZERO_UNIT                   (0x01)
#define UFI_SEEK10                        (0x2B)
#define UFI_SEND_DIAGNOSTIC               (0x1D)
#define SEND_DIAG_LUN_HEADER_BITS         (0x00)
#define UFI_START_STOP                    (0x1B)
#define UFI_STOP_MEDIA                    (0x00)
#define UFI_START_MEDIA_AND_GET_FORMAT    (0x01)

#define UFI_TEST_UNIT_READY               (0x00)
#define UFI_VERIFY                        (0x2F)
#define VERIFY_LUN_HEADER_BITS            (0x00)

#define UFI_WRITE10                       (0x2A)
#define UFI_WRITE_LUN_HEADER_BITS         (0x00)
#define UFI_WRITE12                       (0xAA)
#define UFI_WRITE_AND_VERIFY              (0x2E)

#define DEFAULT_INTERLEAVE                (0x00)


/* USB Mass storage FORMAT UNIT Command information */

typedef struct _DEFECT_LIST_HEADER
{
   uint_8   RESERVED1;
   uint_8   BBIT_INFO_HEADER;
   #define DEFAULT_BIT_INFO               (0xA0)
   uint_8   BLENGTH_MSB;
   uint_8   BLENGTH_LSB;
   #define DEFAULT_LENGTH_MSB             (0x00)
   #define DEFAULT_LENGTH_LSB             (0x08)
} DEFECT_LIST_HEADER, _PTR_ DEFECT_LIST_HEADER_PTR;


typedef struct _FORMAT_CAPACITY_DESCRIPTOR
{
   uint_8   NNUM_BLOCKS[4];
   uint_8   RESERVED;
   uint_8   NBLOCK_LENGTH[3];
} FORMAT_CAPACITY_DESCRIPTOR, _PTR_ FORMAT_CAPACITY_DESCRIPTOR_PTR;

typedef  struct _FORMAT_UNIT_PARAMETER_BLOCK
{
   DEFECT_LIST_HEADER             DEF_LIST_HEADER;
   FORMAT_CAPACITY_DESCRIPTOR     FMT_CAPACITY_DESC;
}  FORMAT_UNIT_PARAMETER_BLOCK, _PTR_ FORMAT_UNIT_PARAMETER_BLOCK_PTR;


/* USB Mass storage READ CAPACITY Command information */

/* USB Mass storage Read10 Command information */
typedef struct _MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO
{
   uint_8  BLLBA[4];    /* Last Logical Block Address */
   uint_8  BLENGTH[4];    /*Block length */
} MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO,
_PTR_ MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO_PTR;


/* USB Mass storage READ FORMAT CAPACITY Command information */

typedef struct _CAPACITY_LIST_HEADER {
   uint_8  RESERVED[3];
   uint_8  BLENGTH[1];    /* Capacity list length */
} CAPACITY_LIST_HEADER,_PTR_ CAPACITY_LIST_HEADER_PTR;


typedef struct _CURRENT_CAPACITY_DESCRIPTOR {
   uint_8   NNUM_BLOCKS[4];
   uint_8   BDESC_CODE;    /* Only last two bits are used */
   #define  UNFORMATTED_MEDIA          (0x01)
   #define  FORMATTED_MEDIA            (0x02)
   #define  NO_MEDIA                   (0x03)
   uint_8   NBLOCK_LENGTH[3];
} CURRENT_CAPACITY_DESCRIPTOR,_PTR_ CURRENT_CAPACITY_DESCRIPTOR_PTR;


typedef struct _CAPACITY_LIST {
   CAPACITY_LIST_HEADER            LIST_HEADER;          /* 4 bytes */
   CURRENT_CAPACITY_DESCRIPTOR     CAPACITY_DESCRIPTOR;  /* 8 bytes */
   FORMAT_CAPACITY_DESCRIPTOR      FMT_CAPACITY_DESC;    /* 8 bytes */
} CAPACITY_LIST, _PTR_ CAPACITY_LIST_PTR;


/* USB Mass storage INQUIRY Command information */

typedef struct _INQUIRY_DATA_FORMAT
{
   /*device type currently connected to Logical Unit */
   uint_8   BDEVICE_TYPE;
   #define DIRECT_ACCESS_DEVICE     (0x00)
   #define   NONE                   (0x1F)

   /*removable media bit only bit 7 is used. rest reserved*/
   uint_8   BRMB;

   /*ISO, ECMA, ANSI Version bits*/
   uint_8   BVERSION_SPEC;
   #define DEFAULT_VERSION_SPEC     (0x00)

   /*Response data format */
   uint_8   BRESP_DATA_FORMAt;
   #define DEFAULT_RESNPOSE_FORMAT  (0x01)

   /* length of parameters */
   uint_8   BADITIONAL_LENGTH;
   #define DEFAULT_LENGTH           (0x1F)

   uint_8   RESERVED1;
   uint_8   RESERVED2;
   uint_8   RESERVED3;

   uint_8   BVID[8];         /* Vendor Identification String */
   uint_8   BPID[16];        /* Product Identification String */
   uint_8   BPRODUCT_REV[4]; /* Product revision level */

} INQUIRY_DATA_FORMAT, _PTR_ INQUIRY_DATA_FORMAT_PTR;


/* USB Mass storage MODE SELECT Command information */
typedef union _READ_WRITE_RECOVERY_PAGE {
   uint_8   FIELDS[12];
} READ_WRITE_RECOVERY_PAGE, _PTR_ READ_WRITE_RECOVERY_PAGE_PTR;


typedef union _MODE_SELECT_PAGE {
   READ_WRITE_RECOVERY_PAGE  READ_WRITE_PAGE;
} MODE_SELECT_PAGE, _PTR_ MODE_SELECT_PAGE_PTR;

typedef struct _MODE_PARAM_HEADER {
   uint_8  BLENGTH[2];    /* Mode Data Length */
   uint_8  BMEDIA_TYPE;   /* Media type code */
   #define DEFAULT_MEDIA_TYPE       (0x00)
   #define DD_720KB                 (0x1E)
   #define HD_125MB                 (0x93)
   #define DD_144MB                 (0x94)
   uint_8  WP_DPOFUA;
   #define WP_DPOFUA_HEADER         (0xEF)
} MODE_PARAM_HEADER,_PTR_ MODE_PARAM_HEADER_PTR;


typedef struct _MODE_SELECT_PARAMETER_LIST {
   MODE_PARAM_HEADER   MODE_PARAM_HEADER;
   #define      DEFAULT_NO_OF_PAGES (0x01)
   MODE_SELECT_PAGE   PAGES[DEFAULT_NO_OF_PAGES];
} MODE_SELECT_PARAMETER_LIST, _PTR_ MODE_SELECT_PARAMETER_LIST_PTR;



/* USB Mass storage REQUEST SENSE Command information */

typedef struct _REQ_SENSE_DATA_FORMAT
{
   /* Valid and Error code*/
   uint_8   BERROR_CODE;           /* 0 */
   #define REQUEST_SENSE_NOT_VALID  (0x00)
   #define REQUEST_SENSE_IS_VALID   (0x01)
   #define REQUEST_SENSE_ERROR_CODE (0x70)
   uint_8   RESERVED1;             /* 1 */
   uint_8   BSENSE_KEY;            /* 2  Only lower 4 bits are used */
   uint_8   BINFO[4];              /* 3,4,5,6 Information  data format */
   uint_8   BADITIONAL_LENGTH;     /* 7 additional sense length */
   uint_8   RESERVED2[4];          /* 8,9,10,11*/
   uint_8   BADITIONAL_CODE;       /* 12 Additional sense code */
   uint_8   BADITIONAL_QUALIFIER;  /* 13 Additional sense code qualifier */
   uint_8   RESERVED3[4];          /* 14,15,16,17*/
} REQ_SENSE_DATA_FORMAT, _PTR_ REQ_SENSE_DATA_FORMAT_PTR;


/* USB Mass storage WRITE10 Command information */
typedef struct _MASS_STORAGE_WRITE10_CMD_STRUCT_INFO {
   uint_8  BLBA[4];
   uint_8  BTRANSFER_LENGTH[2];
} MASS_STORAGE_WRITE10_CMD_STRUCT_INFO,
   _PTR_ MASS_STORAGE_WRITE10_CMD_STRUCT_INFO_PTR;


/* USB Mass storage WRITE12 Command information */
typedef struct _MASS_STORAGE_WRITE12_CMD_STRUCT_INFO {
   uint_8  BLBA[4];
   uint_8  BTRANSFER_LENGTH;
} MASS_STORAGE_WRITE12_CMD_STRUCT_INFO,
_PTR_ MASS_STORAGE_WRITE12_CMD_STRUCT_INFO_PTR;

/* USB Mass storage SEEK10 Command information */
typedef struct MASS_STORAGE_SEEK10_STRUCT_INFO {
   uint_8  BLBA[4];
} MASS_STORAGE_SEEK10_STRUCT_INFO, _PTR_ MASS_STORAGE_SEEK10_STRUCT_INFO_PTR;

/* USB Mass storage SEND DIAGNOSTIC Command information */
typedef struct _MASS_STORAGE_SEND_DIAGNOSTIC_STRUCT_INFO {
   boolean  SELF_TEST;
} MASS_STORAGE_SEND_DIAGNOSTIC_STRUCT_INFO,
_PTR_ MASS_STORAGE_SEND_DIAGNOSTIC_STRUCT_INFO_PTR;

/* USB Mass storage START-STOP UNIT Command information */
typedef struct _MASS_STORAGE_START_STOP_UNIT_STRUCT_INFO {
   boolean   START;
   boolean   LOEJ;
} MASS_STORAGE_START_STOP_UNIT_STRUCT_INFO,
_PTR_ MASS_STORAGE_START_STOP_UNIT_STRUCT_INFO_PTR;

/* USB Mass storage VERIFY Command information */
typedef struct _MASS_STORAGE_VERIFY_CMD_STRUCT_INFO {
   uint_8  BLBA[4];
   uint_8  BTRANSFER_LENGTH[2];
} MASS_STORAGE_VERIFY_CMD_STRUCT_INFO,
_PTR_ MASS_STORAGE_VERIFY_CMD_STRUCT_INFO_PTR;


/* UFI commands */

/* Used by host-side to send the READ CAPACITY command */
#define usb_mass_ufi_read_capacity(                                       \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_READ_CAPACITY,                                          \
      (uint_8)(((cmd_ptr)->LUN << 5) | READ_CAPACITY_LUN_HEADER_BITS),    \
      (uint_32)0,                                                         \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the FORMAT UNIT command */
#define usb_mass_ufi_format_unit(                                         \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR  */             cmd_ptr,                      \
                                                                          \
      /* [IN] Track number to be formatted (see UFI specs) */             \
      /* uint_8 */                          track_num,                    \
                                                                          \
      /* [IN] Interleave number (see UFI specs) */                        \
      /* uint_8 */                          interleave,                   \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* FORMAT_UNIT_PARAMETER_BLOCK_PTR */ format_ptr                    \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_FORMAT_UNIT,                                            \
      (uint_8)(((cmd_ptr)->LUN << 5) | FORMAT_LUN_HEADER_BITS),           \
      (uint_32)(((track_num) << 24) | (HIGH_BYTE((interleave)) << 16) |   \
         (LOW_BYTE((interleave)) << 8)),                                  \
      (uint_32)((format_ptr) ? (sizeof(FORMAT_LUN_HEADER_BITS) << 8) : 0),\
      (uint_8)CBW_DIRECTION_OUT,                                          \
      (uchar_ptr)(format_ptr),                                            \
      (format_ptr) ? (sizeof(FORMAT_LUN_HEADER_BITS) << 8) : 0)


/* Used by host-side to send the FORMAT CAPACITY command */
#define usb_mass_ufi_format_capacity(                                     \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_READ_FORMAT_CAPACITY,                                   \
      (uint_8)((cmd_ptr)->LUN << 5),                                      \
      (uint_32)0,                                                         \
      (uint_32)(((buf_len) & 0xFFFF) << 8),                               \
      (uint_8)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/*
** Used by host-side to send the INQUIRY command, to request information
** regarding parameters of the UFI device itself
*/
#define usb_mass_ufi_inquiry(                                             \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */         cmd_ptr,                           \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_INQUIRY,                                                \
      (uint_8)(((cmd_ptr)->LUN << 5) | INQUIRY_LUN_HEADER_BITS),          \
      (uint_32)(((buf_len) & 0xFF) << 8),                                 \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the MODE SELECT command */
#define usb_mass_ufi_mode_select(                                         \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */             cmd_ptr,                       \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* pointer */                        buf_ptr,                       \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                        buf_len                        \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_MODE_SELECT,                                            \
      (uint_8)(((cmd_ptr)->LUN << 5) | MODE_SELECT_LUN_HEADER_BITS),      \
      (uint_32)0,                                                         \
      (uint_32)((buf_ptr) ? ((buf_len) << 8) : 0),                        \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      (uchar_ptr)(buf_ptr),                                               \
      (buf_ptr) ? (buf_len) : 0)



/* Used by host-side to send the MODE SENSE command */
#define usb_mass_ufi_mode_sense(                                          \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */         cmd_ptr,                           \
                                                                          \
      /*[IN] Page control byte 2 bits   */                                \
      /* uint_8 */                     bPC,                               \
                                                                          \
      /*[IN] Page code 6 bit          */                                  \
      /* uint_8 */                     bPage_code,                        \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                    buf_ptr,                           \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                    buf_len                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_MODE_SENSE,                                             \
      (uint_8)(((cmd_ptr)->LUN << 5) | MODE_SENSE_LUN_HEADER_BITS),       \
      (uint_32)((((bPC) << 6) | ((bPage_code) & 0x3F)) << 24),            \
      (uint_32)((buf_ptr) ? ((buf_len) << 8) : 0),                        \
      (uint_8)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_ptr) ? (buf_len) : 0)


/* Used by host-side to send the PREVENT-ALLOW MEDIUM REMOVAL command */
#define usb_mass_ufi_prevent_allow_medium_removal(                        \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* uint_8 */                  bPrevent /*1 or 0 values */           \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_PREVENT_ALLOW_MEDIUM_ROMVAL,                            \
      (uint_8)((cmd_ptr)->LUN << 5),                                      \
      (uint_32)(((bPrevent) & 0x01) << 8),                                \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the READ(10) command, to get data from device */
#define usb_mass_ufi_read_10(                                             \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint_32 */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to read */          \
      /* uint_32 */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_READ10,                                                 \
      (uint_8)(((cmd_ptr)->LUN << 5) | READ10_LUN_HEADER_BITS),           \
      (uint_32)(bBlock_address),                                          \
      (uint_32)((num_of_blocks) << 8),                                    \
      (uint_8)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the READ(12) command, to get data from device */
#define usb_mass_ufi_read_12(                                             \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint_32 */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to read */          \
      /* uint_32 */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_READ12,                                                 \
      (uint_8)(((cmd_ptr)->LUN << 5) | READ12_LUN_HEADER_BITS),           \
      (uint_32)(bBlock_address),                                          \
      (uint_32)(num_of_blocks),                                           \
      (uint_8)CBW_DIRECTION_IN,                                           \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the REQUEST SENSE command */
#define usb_mass_ufi_request_sense(                                       \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                 buf_len                               \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_REQUEST_SENSE,                                          \
      (uint_8)((cmd_ptr)->LUN << 5),                                      \
      (uint_32)(((buf_len) & 0xFF) << 8),                                 \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_IN,                                           \
      (uchar_ptr)(buf_ptr),                                               \
      (buf_len))


/* Used by host-side to send the REZERO UNIT command */
#define usb_mass_ufi_rezero_unit(                                         \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr                               \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_REZERO_UNIT,                                            \
      (uint_8)((cmd_ptr)->LUN << 5),                                      \
      (uint_32)0,                                                         \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the SEEK command */
#define usb_mass_ufi_seek_10(                                             \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] block address (see UFI specs) */                            \
      /* uint_32 */                 bBlock_address                        \
   )                                                                      \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_SEEK10,                                                 \
      (uint_8)((cmd_ptr)->LUN << 5),                                      \
      (uint_32)(bBlock_address),                                          \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the SEND DIAGNOSTIC command */
#define usb_mass_ufi_send_diagnostic(                                     \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] command specific parameters (see UFI specs) */              \
      /* uint_8 */                  bSelf_test                            \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_SEND_DIAGNOSTIC,                                        \
      (uint_8)((((cmd_ptr)->LUN << 5) | SEND_DIAG_LUN_HEADER_BITS) |      \
         (((bSelf_test) & 0xFF) << 2)),                                   \
      (uint_32)0,                                                         \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the START-STOP command */
#define usb_mass_ufi_start_stop(                                          \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* Load Object Bit */                                               \
      /* uint_8 */                  bLoej,                                \
                                                                          \
      /* Start Stop bit  */                                               \
      /* uint_8 */                  bStart                                \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_START_STOP,                                             \
      (uint_8)(((cmd_ptr)->LUN << 5) | SEND_DIAG_LUN_HEADER_BITS),        \
      (uint_32)(((((bLoej) << 1) | (bStart)) & 0x3) << 8),                \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the TEST UNIT READY command */
#define usb_mass_ufi_test_unit_ready(                                     \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr                               \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_TEST_UNIT_READY,                                        \
      (uint_8)((cmd_ptr)->LUN << 5),                                      \
      (uint_32)0,                                                         \
      (uint_32)0,                                                         \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the VERIFY command */
#define usb_mass_ufi_verify(                                              \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] address of the block to verify */                           \
      /* uint_32 */                 bBlock_address,                       \
                                                                          \
      /* Length of the data to verify */                                  \
      /*uint_16  */               dLength                                 \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_VERIFY,                                                 \
      (uint_8)(((cmd_ptr)->LUN << 5) | VERIFY_LUN_HEADER_BITS),           \
      (uint_32)(bBlock_address),                                          \
      (uint_32)((dLength) << 8),                                          \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      NULL,                                                               \
      0)


/* Used by host-side to send the WRITE(10) command, to send data to device */
#define usb_mass_ufi_write_10(                                            \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint_32 */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to write */         \
      /* uint_32 */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_WRITE10,                                                \
      (uint_8)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint_32)(bBlock_address),                                          \
      (uint_32)((num_of_blocks) << 8),                                    \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the WRITE(12) command, to send data to device */
#define usb_mass_ufi_write_12(                                            \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint_32 */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks to write */         \
      /* uint_32 */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_WRITE12,                                                \
      (uint_8)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint_32)(bBlock_address),                                          \
      (uint_32)(num_of_blocks),                                          \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))


/* Used by host-side to send the WRITE AND VERIFY command */
#define usb_mass_ufi_write_and_verify(                                    \
      /* [IN] command object allocated by application*/                   \
      /* COMMAND_OBJECT_PTR */      cmd_ptr,                              \
                                                                          \
      /* [IN] Address of the block to be read */                          \
      /* uint_32 */                 bBlock_address,                       \
                                                                          \
      /* [IN] buffer to send or receive data*/                            \
      /* pointer */                 buf_ptr,                              \
                                                                          \
      /* [IN] length of the expected data */                              \
      /* uint_32 */                 buf_len,                              \
                                                                          \
      /* [IN] the number of contiguous logical blocks */                  \
      /* uint_32 */                 num_of_blocks                         \
   )                                                                      \
                                                                          \
   usb_mass_ufi_generic((cmd_ptr),                                        \
      (uint_8)UFI_WRITE_AND_VERIFY,                                       \
      (uint_8)(((cmd_ptr)->LUN << 5) | UFI_WRITE_LUN_HEADER_BITS),        \
      (uint_32)(bBlock_address),                                          \
      (uint_32)((num_of_blocks) << 8),                                    \
      (uint_8)CBW_DIRECTION_OUT,                                          \
      (buf_ptr),                                                          \
      (buf_len))


/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern USB_STATUS usb_mass_ufi_generic
   (
      /* [IN] command object allocated by application*/
      COMMAND_OBJECT_PTR         cmd_ptr,
      uint_8                     opcode,
      uint_8                     lun,
      uint_32                    lbaddr,
      uint_32                    blen,

      uint_8                     cbwflags,

      uchar_ptr                  buf,
      uint_32                    buf_len
   );

extern boolean usb_mass_ufi_cancel
   (
      COMMAND_OBJECT_PTR         cmd_ptr   );
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
