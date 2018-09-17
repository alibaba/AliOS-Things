#ifndef __usbprv_host_h__
#define __usbprv_host_h__ 1
/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: usbprv_host.h$
* $Version : 3.5.8.0$
* $Date    : Jan-7-2010$
*
* Comments:
*
*   This file contains the internal USB Host specific type definitions
*
*END************************************************************************/
#include "ehci_cache.h"
#include "types.h"
#include "ehci.h"
#include "hostapi.h"

#include "host_cnfg.h"

#define  HUB_LS_SETUP                        (333)

#define  MAX_QH_DESCRS                       (8)
#define  MAX_QTD_DESCRS                      (16)
#define  MAX_ITD_DESCRS                      (16)
#define  MAX_SITD_DESCRS                     (16)


#define  USB_MAX_INTERFACES                  (10)
// jm struct item with same name #define  RX_BUFFER_SIZE                      (0xFF)

/***************************************
**
** Code macros
**
*/

#define  BitStuffTime(x)                     (7* 8 * x / 6)

typedef struct pipe_tr_struct
{
   TR_INFO_FIELDS				  /* 28 bytes in size */
   struct pipe_tr_struct   _PTR_ next;
   uint_32                       status;
   /* Second phase of setup packet: 
   ** Send/Receive 
   */
   boolean                       SEND_PHASE;
  /* number of ITds/SITDs used for this transfer */
   uint_32                       no_of_itds_sitds;

   /****************************************************
   Data cache requirement is that any buffer that stores
   the data should be cache aligned. This means that the
   setup packet buffer must be cache aligned too. Structure
   fields above this have occupied 28 + 16 = 44 bytes.
   We add a 20 bytes padding to makesure that it is aligned
   on a 32 byte boundry since 44 + 20 = 64; We also
   add a padding at the end to ensure that we don't corrupt
   someother memory when we flush the setup packet
   from cache.
   ****************************************************/
#if PSP_HAS_DATA_CACHE  // TODO check align
   	uint_8						 RESERVED1[USB_CACHE_ALIGN(44) - 44];
#endif // PSP_HAS_DATA_CACHE
   
   USB_SETUP        			 setup_packet;
   
#if PSP_HAS_DATA_CACHE  // TODO check align
   	uint_8               RESERVED2[USB_CACHE_ALIGN(8) - 8];
#endif


} PIPE_TR_STRUCT, _PTR_ PIPE_TR_STRUCT_PTR;
    
typedef struct pipe_descriptor_struct
{
   PIPE_INFO_FIELDS		/* 34 bytes in size */
   uint_8               CURRENT_INTERVAL;
   uint_8               NEXTDATA01;       /* Data 0/1 synch bit for the PID */
   uint_8               HUB_DEVICE_ADDR;
   uint_8               HUB_PORT_NUM;
   int_16               PIPE_ID;          /* Pipe ID */
   uint_32              SOFAR;            /* number of bytes sent/recv'd so far */
   boolean              OPEN;
   uint_32              CURRENT_NAK_COUNT;
   PIPE_TR_STRUCT_PTR   tr_list_ptr;
   pointer              QH_FOR_THIS_PIPE; /* Queue head for this pipe */

   /* Special fields defined for periodic transfers */
   uint_32              START_FRAME;    /*frame number from which this pipe is started to schedule*/
   uint_32              START_UFRAME;   /*Micro frame number from which this pipe is started to schedule*/
   uint_32              NO_OF_START_SPLIT; /* store the number of start splits (used for ISO OUT )*/
   uint_8               START_SPLIT;    /* store the start split slots for full speed devices*/
   uint_8               COMPLETE_SPLIT; /* store the complete split slots for full speed devices*/
   uint_16              BWIDTH;         /* time take by this pipe (for periodic pipes only */
   uint_8               BWIDTH_SLOTS[8];/*micro frame slots budgeted for transaction */
   
   /* 84 bytes so far add padding to ensure cache alignment*/
#if PSP_HAS_DATA_CACHE  // TODO check align
   uint_8				reserved[USB_CACHE_ALIGN(74) - 74]; 
#endif	
   	
} PIPE_DESCRIPTOR_STRUCT, _PTR_ PIPE_DESCRIPTOR_STRUCT_PTR;

/* Queue head management data structure */
typedef struct active_qh_mgmt_struct {
   EHCI_QH_STRUCT_PTR                  QH_PTR;
   EHCI_QTD_STRUCT_PTR                 FIRST_QTD_PTR;
   int_32                              TIME;
   struct active_qh_mgmt_struct _PTR_  NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
} ACTIVE_QH_MGMT_STRUCT, _PTR_ ACTIVE_QH_MGMT_STRUCT_PTR;

/* ITD,SITD list management data structure (doubly link list )*/
typedef struct list_node_struct {
   struct list_node_struct _PTR_  next;        /* next member in the list */
   struct list_node_struct _PTR_  prev;        /* previous member in the list */
   boolean   next_active; /* is next node a active node */
   pointer   member;      /* pointer to the currently active ITD or SITD*/
} LIST_NODE_STRUCT, _PTR_ LIST_NODE_STRUCT_PTR;


/* Class Callback function storage structure */
typedef struct class_service_struct {
   uint_8               CLASS_TYPE;
   uint_8               SUB_CLASS;
   uint_8               PROTOCOL;
   /* index of the instance */
   uint_8               INDEX;
   /* identification string unique to this type */
   char_ptr             CLASS_ID_STRING;
   /* class handle will be NULL if it is not initialized */
   pointer              CLASS_HANDLE;
   pointer (_CODE_PTR_  INIT)(pointer, uint_32);
   void    (_CODE_PTR_  DEINIT)(pointer);
   void    (_CODE_PTR_  CALL_BACK_INIT)(pointer, char_ptr);
   void    (_CODE_PTR_  CALL_BACK_REMOVE)(pointer);
   struct class_service_struct _PTR_ NEXT_INSTANCE;
   struct class_service_struct _PTR_ NEXT;
} USB_HOST_CLASS_DRIVER_STRUCT, _PTR_ USB_HOST_CLASS_DRIVER_STRUCT_PTR;

typedef struct  usb_host_state_structure {
   uint_32                             FRAME_LIST_SIZE;
   pointer                			   DEV_PTR;
   pointer                             DEVICE_LIST_PTR;
   USB_SERVICE_STRUCT_PTR              SERVICE_HEAD_PTR;
   PIPE_DESCRIPTOR_STRUCT_PTR          PIPE_DESCRIPTOR_BASE_PTR;
   uint_8_ptr                          PERIODIC_FRAME_LIST_BW_PTR;
   pointer                             CALLBACK_STRUCT_PTR;
   pointer                			   PERIODIC_LIST_BASE_ADDR;
   pointer                			   ALIGNED_PERIODIC_LIST_BASE_ADDR;
   EHCI_QH_STRUCT_PTR                  ASYNC_LIST_BASE_ADDR;
   EHCI_QH_STRUCT_PTR          		   ALIGNED_ASYNCLIST_ADDR;
   EHCI_QH_STRUCT_PTR				   QH_BASE_PTR;	
   EHCI_QH_STRUCT_PTR				   QH_ALIGNED_BASE_PTR;	
   uint_32                             QH_ENTRIES;
   EHCI_QH_STRUCT_PTR                  QH_HEAD;
   EHCI_QH_STRUCT_PTR                  QH_TAIL;
   ACTIVE_QH_MGMT_STRUCT_PTR           ACTIVE_ASYNC_LIST_PTR;
   ACTIVE_QH_MGMT_STRUCT_PTR           ACTIVE_INTERRUPT_PERIODIC_LIST_PTR;
   EHCI_QTD_STRUCT_PTR                 QTD_BASE_PTR;
   EHCI_QTD_STRUCT_PTR                 QTD_HEAD;
   EHCI_QTD_STRUCT_PTR                 QTD_TAIL;
   EHCI_QTD_STRUCT_PTR                 QTD_ALIGNED_BASE_PTR;
   uint_32                             QTD_ENTRIES;
   boolean                             HIGH_SPEED_ISO_QUEUE_ACTIVE; 
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_ITD_PERIODIC_LIST_HEAD_PTR;
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_ITD_PERIODIC_LIST_TAIL_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_BASE_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_ALIGNED_BASE_PTR;
   EHCI_ITD_STRUCT_PTR                 ITD_HEAD;
   EHCI_ITD_STRUCT_PTR                 ITD_TAIL;
   uint_32                             ITD_ENTRIES;
   boolean                             FULL_SPEED_ISO_QUEUE_ACTIVE; 
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_SITD_PERIODIC_LIST_HEAD_PTR;
   LIST_NODE_STRUCT_PTR                ACTIVE_ISO_SITD_PERIODIC_LIST_TAIL_PTR;
   EHCI_SITD_STRUCT_PTR   			   SITD_BASE_PTR;
   EHCI_SITD_STRUCT_PTR   			   SITD_ALIGNED_BASE_PTR;
   EHCI_SITD_STRUCT_PTR                SITD_HEAD;
   EHCI_SITD_STRUCT_PTR                SITD_TAIL;
   uint_32                             SITD_ENTRIES;
   uint_32                             RESET_RECOVERY_TIMER;
   uint_32                             SPEED;
   uint_32                             PORT_NUM;
   uint_8                              DEV_NUM;
   uint_8                              UFRAME_COUNT;
   pointer                             DEVICE_INFO_TABLE;
   pointer                             CONTROLLER_MEMORY;
   boolean                             PERIODIC_LIST_INITIALIZED;
   boolean                             ITD_LIST_INITIALIZED;
   boolean                             SITD_LIST_INITIALIZED;
   boolean                             IS_RESETTING;
} USB_HOST_STATE_STRUCT, _PTR_ USB_HOST_STATE_STRUCT_PTR;

#define USBHS_PORTSC1_PSPD_FULL_MASK  USBHS_PORTSC1_PSPD(0)
#define USBHS_PORTSC1_PSPD_LOW_MASK   USBHS_PORTSC1_PSPD(1)
#define USBHS_PORTSC1_PSPD_HIGH_MASK  USBHS_PORTSC1_PSPD(2)

#define EHCI_GET_TYPE(data_struct_ptr) \
   (*((USB_Uncached uint_32 _PTR_)data_struct_ptr) & EHCI_ELEMENT_TYPE_MASK)

#define EHCI_ITD_QADD(head, tail, ITD)      \
   if ((head) == NULL) {         \
      (head) = (ITD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (ITD);   \
   } /* Endif */                 \
   (tail) = (ITD);               \
   (ITD)->SCRATCH_PTR = NULL
   
#define EHCI_ITD_QGET(head, tail, ITD)      \
   (ITD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_ITD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_ACTIVE_QUEUE_ADD_NODE(tail,member_ptr)     \
   if (tail->prev != NULL) {                             \
       tail->prev->next_active = TRUE;                   \
   }                                                     \
   tail->member = (pointer) member_ptr;                                  \
   tail->next_active = FALSE;                            \
   tail = tail->next; 

extern void EHCI_QUEUE_FREE_NODE(
		LIST_NODE_STRUCT_PTR head,
		LIST_NODE_STRUCT_PTR tail,
		LIST_NODE_STRUCT_PTR node_ptr
		);

#define EHCI_SITD_QADD(head, tail, SITD)      \
   if ((head) == NULL) {         \
      (head) = (SITD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (SITD);   \
   } /* Endif */                 \
   (tail) = (pointer) (SITD);               \
   (SITD)->SCRATCH_PTR = NULL
   
#define EHCI_SITD_QGET(head, tail, SITD)      \
   (SITD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_SITD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */
   
#define EHCI_QTD_QADD(head, tail, QTD)      \
   if ((head) == NULL) {         \
      (head) = (QTD);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (QTD);   \
   } /* Endif */                 \
   (tail) = (pointer) (QTD);               \
   (QTD)->SCRATCH_PTR = NULL
   
#define EHCI_QTD_QGET(head, tail, QTD)      \
   (QTD) = (head);               \
   if (head) {                   \
      (head) = (EHCI_QTD_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_QH_QADD(head, tail, QH)      \
   if ((head) == NULL) {         \
      (head) = (pointer) (QH);            \
   } else {                      \
      (tail)->SCRATCH_PTR = (void *) (QH);   \
   } /* Endif */                 \
   (tail) = (QH);               \
   (QH)->SCRATCH_PTR = NULL
   
#define EHCI_QH_QGET(head, tail, QH)      \
   (QH) = (head);               \
   if (head) {                   \
      (head) = (EHCI_QH_STRUCT_PTR)((head)->SCRATCH_PTR);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

extern USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;

/* Additional Includes */
#include "ehci_bw.h"
#include "ehci_cncl.h"
#include "ehci_intr.h"
#include "ehci_iso.h"
#include "ehci_main.h"
#include "ehci_shut.h"
#include "ehci_utl.h"

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern uint_8 _bsp_get_usb_vector(uint_8);
extern pointer _bsp_get_usb_capability_register_base(uint_8);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */