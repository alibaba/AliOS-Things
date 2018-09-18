#ifndef __phdc_mng_h__
#define __phdc_mng_h__
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
* $FileName: phdcmng.h$
* $Version : 1.0.0.0$
* $Date    : Jan-29-2010$
*
* Comments:
*
*   This file contains phdc-application types and definitions.
*
*END************************************************************************/


#ifdef LITTLE_ENDIAN
#include "ieee11073_phd_types_kinetis.h"
#include "ieee11073_nom_codes_kinetis.h"
#else
#include "ieee11073_phd_types.h"
#include "ieee11073_nom_codes.h"
#endif

/***************************************
**
** Application-specific definitions
*/

#define USB_EVENT_CTRL 0x01
#define USB_EVENT_DATA 0x02

/* Used to initialize USB controller */
#define MAX_FRAME_SIZE           			1024
#define HOST_CONTROLLER_NUMBER      		0

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_SETTING_PROTOCOL       (5)
#define  USB_DEVICE_INUSE                  (6)
#define  USB_DEVICE_DETACHED               (7)
#define  USB_DEVICE_OTHER                  (8)

#define  PHD_MNG_UNASSOCIATED              (0)
#define  PHD_MNG_WAIT_CONFIG			   (1)
#define  PHD_MNG_OPERATING				   (2)

/* Request/Response Sizes */
#define ASSOC_RES_SIZE                      48
#define CONFIG_RES_SIZE                     26
#define GET_MDS_ATTR_SIZE					18
#define SCAN_RES_SIZE                       22
#define REL_REQ_SIZE                        6
#define REL_RES_SIZE                        6
#define ABRT_SIZE                           6
#define CNFG_EVT_RPT_SIZE                   166
#define DIM_GET_RSP_SIZE                    113
#define DIM_DATA_TX_SIZE                    94

#define UPPER_BYTE_SHIFT                    8
#define LOW_NIBBLE_MASK                     0x0f
#define LOW_BYTE_MASK                       0xff

/* APDU Header Size */
#define APDU_HEADER_SIZE                    4

/* APDU Max buffer size */
#define APDU_MAX_BUFFER_SIZE				255

/* Add here the manager known attributes */
#define PHD_MDC_PART_SCADA_MASS_BODY_ACTUAL 	1
#define PHD_MDC_PART_SCADA_RATIO_MASS_BODY_LEN_SQ 	1

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/

typedef struct device_struct {
   uint_32                          DEV_STATE;  /* Attach/detach state */
   _usb_device_instance_handle      DEV_HANDLE;
   _usb_interface_descriptor_handle INTF_HANDLE;
   CLASS_CALL_STRUCT                CLASS_INTF; /* Class-specific info */

   /* Application specific info */
   uint_8  *phd_buffer;
   uint_8  phd_recv_size;
   boolean phd_recv_in_progress;
   uint_8  phd_manager_state;
   ConfigObjectList *confObjList;

} DEVICE_STRUCT,  _PTR_ DEVICE_STRUCT_PTR;


#endif /* __phdc_mng_h__ */

/* EOF */

