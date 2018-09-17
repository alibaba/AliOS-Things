#ifndef __USB_HOST_PHDC_H__
#define __USB_HOST_PHDC_H__

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
* $FileName: usb_host_phdc.h$
* $Version : 1.0.0.0$
* $Date    : Jan-18-2010$
*
* Comments:
*
*   This file contains definitions for  USB PHDC class.
*   Reference Specification:
*   "Universal Serial Bus Device Class Definition for Personal Healthcare Devices" version 1.0
*   November 2008, from USB Implementers Forum.
*
*END************************************************************************/

#include "hostapi.h"

/* Constant Definitions*********************************************************/

#define METADATA_PREAMBLE_SIGNATURE_SIZE    16

/* Descriptor related */

/* Standard descriptors */
#define USB_DEVICE_DESCRIPTOR               0x01
#define USB_CONFIG_DESCRIPTOR               0x02
#define USB_STRING_DESCRIPTOR               0x03
#define USB_IFACE_DESCRIPTOR                0x04
#define USB_ENDPOINT_DESCRIPTOR             0x05

/* PHDC descriptors */
#define USB_PHDC_CLASSFUNCTION_DESCRIPTOR   0x20
#define USB_PHDC_QOS_DESCRIPTOR             0x21
#define PHDC_11073PHD_FUNCTION_DESCRIPTOR   0x30
#define PHDC_METADATA_DESCRIPTOR            0x22

#define USB_CLASS_PHDC                      0x0F

/* Control Requests Types */
#define PHDC_GET_STATUS_BMREQTYPE			0xA1
#define PHDC_GET_STATUS_BREQ				0x00
#define PHDC_SET_FEATURE_BMREQTYPE			0x21
#define PHDC_SET_FEATURE_BREQ				0x03
#define PHDC_CLEAR_FEATURE_BMREQTYPE		0x21
#define PHDC_CLEAR_FEATURE_BREQ				0x01

/* Metadata feature */
#define PHDC_FEATURE_METADATA               0x01  
/* QoS information encoding feature */
#define PHDC_QoS_ENCODING_VERSION           0x01 
/* PHDC specific status codes for the application callbacks (passed through the callback_param) */
#define USB_PHDC_RX_OK      				0x00   /* Rx OK code. All went fine on this transfer */ 
#define USB_PHDC_TX_OK						0x00   /* Tx OK code. All went fine on this transfer */
#define USB_PHDC_CTRL_OK					0x00   /* CTRL OK code. All went fine on this transfer */
#define USB_PHDC_RX_ERR_METADATA_EXPECTED	0x01   /* Rx ERR code. Metadata was expected, instead plain data was received */
#define USB_PHDC_ERR						0x7F   /* USB Standard error returned. Check the USB_STATUS code */
#define USB_PHDC_ERR_ENDP_CLEAR_STALL		0xFF   /* The PHDC failed in clearing the Endpoint STALL (due to an USB STALL ERROR) */

/* Standard USB endpoint related */
#define ENDPOINT_TRANSFER_TYPE          0x03
#define ENDPOINT_DIRECTION              0x80

#define ENDPOINT_CONTROL                0x00
#define ENDPOINT_ISOCHRONOUS            0x01
#define ENDPOINT_BULK                   0x02
#define ENSPOINT_INTERRUPT              0x03

#define ENDPOINT_DIR_IN                 0x80
#define ENDPOINT_DIR_OUT                0x00

/* Type Definitions*********************************************************/

/* Endpoint descriptor structure as defined by the USB standard */
typedef struct usb_phdc_desc_endpoint_type
{
   uint_8  bLength;
   uint_8  bDescriptorType;
   uint_8  bEndpointAddress;
   uint_8  bmAttributes;
   uint_16 wMaxPacketSize;
   uint_8  bInterval;
}USB_PHDC_DESC_ENDPOINT;

/* PHDC class descriptor structure as defined by the Healthcare standard */
typedef struct usb_phdc_desc_class_type
{
   uint_8  bLength;
   uint_8  bDescriptorType;
   uint_8  bPHDCDataCode;
   uint_8  bmCapability;
}USB_PHDC_CLASS_TYPE;

/* QoS descriptor structure as defined by the PHDC class specification. */
typedef struct usb_phdc_desc_qos_type
{
   uint_8  bLength;
   uint_8  bDescriptorType;
   uint_8  bQoSEncodingVersion;
   uint_8  bmLatencyReliability;
}USB_PHDC_DESC_QOS;

/* Metadata descriptor structure as defined by the PHDC class specification. */
typedef struct usb_phdc_desc_metadata_type
{
   uint_8  bLength;
   uint_8  bDescriptorType;
   uint_8  bOpaqueData[];
}USB_PHDC_DESC_METADATA;

/* QoS and Metadata Linked-List element */
typedef struct usb_phdc_qos_metadata_list_type 
{
   USB_PHDC_DESC_ENDPOINT                  *pEndpointDesc;
   USB_PHDC_DESC_QOS                       *pQosDesc;
   USB_PHDC_DESC_METADATA                  *pMetadataDesc;
   struct usb_phdc_qos_metadata_list_type  *pNext;
}USB_PHDC_DESC_QOS_METADATA_LIST;

/* Function extension descriptor (device specialization) structure as defined by the PHDC class specification. */
typedef struct USB_PHDC_DESC_FCN_EXT_type
{
   uint_8  bLength;
   uint_8  bDescriptorType;
   uint_8  bReserved;
   uint_8  bNumDevSpecs;
   uint_16 wDevSpecializations[];
}USB_PHDC_DESC_FCN_EXT;


/* PHDC required type for parameter passing to the PHDC transfer functions (Send / Receive/ Ctrl). 
 * A pointer to this type is required when those functions are called, 
 * pointer which will be also transmitted back to the application when the corresponding callback 
 * function is called by the PHDC through the callback_param_ptr.
 */
typedef struct usb_phdc_param_type
{
  /* [APP->PHDC] phdc call struct pointer */
  CLASS_CALL_STRUCT_PTR          ccs_ptr;

  /* [APP->PHDC] the type of the request (only for PHDC Ctrl requests) */
  uint_8                         classRequestType;

  /* [APP->PHDC] Boolean for metadata transfers (only for PHDC Send request) */
  boolean                        metadata;

  /* [APP->PHDC] QoS for receive transfers (only for PHDC Recv request) */
  uint_8                         qos;
  
  /* [PHDC->APP] USB PHDC status code. USB PHDC (specific) code when the transfer is finished. Not valid until the callback is called */
  uint_8                         usb_phdc_status;  
  
  /* [PHDC->APP] USB status code. USB_STATUS (standard) code when the transfer is finished. Not valid until the callback is called */
  USB_STATUS                     usb_status;  

  /* [APP->PHDC] data buffer (only for PHDC Send/Recv requests) */
  uint_8*                        buff_ptr;

  /* [APP->PHDC] length of buffer (only for PHDC Send/Recv requests) */
  uint_32                        buff_size;

  /* [PHDC->APP] USB transaction index. Used to identify the Send/Recv transaction */
  uint_32                        tr_index;
  
    /* [PHDC->APP] USB transaction index. Used to identify the Send/Recv transaction */
  _usb_pipe_handle               tr_pipe_handle;

  
}USB_PHDC_PARAM;

/* Callback function pointer keeping the current transaction parameters. It contains a pointer to a USB_PHDC_PARAM struct. */
typedef  void  (* phdc_callback)(USB_PHDC_PARAM *call_param);

/*
 * PHDC Class Interface structure. This structure will be passed to
 * all commands to this class driver.
 */
typedef struct usb_phdc_class_intf_struct_type 
{
   /* Each class must start with a GENERAL_CLASS struct */
   GENERAL_CLASS  			         	generalClass;
   
   /* Pipes */
   _usb_pipe_handle  		        	controlPipe; /* control pipe */
   _usb_pipe_handle  			        bulkInPipe;   /* bulk IN pipe */
   _usb_pipe_handle  			        bulkOutPipe;  /* bulk OUT pipe */
   _usb_pipe_handle  			        intInPipe;    /* interrupt IN pipe */
   
   /* Descriptors */
   USB_PHDC_DESC_QOS_METADATA_LIST  	*qosMetadataList;
   USB_PHDC_DESC_FCN_EXT				*fcnExtDesc;
   /* Callbacks */
   phdc_callback						sendCallback;    /* Send app callback */
   phdc_callback						recvCallback;    /* Receive app callback */
   phdc_callback						ctrlCallback;    /* Send ctrl app callback */

   /* Configuration */
   boolean								preambleCapability;
   uint_8                               phdcDataCode;
   /* Status */
   /* Only one SET_FEATURE/CLEAR_FEATURE request at the time */
   boolean                              setClearRequestPending;
   
   /* Metadata feature set for the device */
   boolean                              deviceFeatureSet;

   /* Number of transfers until next metadata on the Bulk IN. Required on receive */
   uint_8                               numTransfBulkIn;

   /* Number of transfers until next metadata on the Bulk OUT. Required on transmit */
   uint_8                               numTransfBulkOut;

} USB_PHDC_CLASS_INTF_STRUCT;

/* Internal structure keeping the Metadata preamble fields as defined by the PHDC class specification. */
typedef struct usb_phdc_metadata_preamble_type
{
   uint_8  aSignature[METADATA_PREAMBLE_SIGNATURE_SIZE];
   uint_8  bNumTransfers;
   uint_8  bQoSEncodingVersion;
   uint_8  bmLatencyReliability;
   uint_8  bOpaqueDataSize;
   uint_8  bOpaqueData[];
} USB_PHDC_METADATA_PREAMBLE;


/* Function Declarations *********************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void 	   usb_class_phdc_init(PIPE_BUNDLE_STRUCT_PTR pbs_ptr, CLASS_CALL_STRUCT_PTR ccs_ptr);

void       usb_class_phdc_uninit(_usb_device_instance_handle dev_handle, _usb_interface_descriptor_handle intf_handle);

USB_STATUS usb_class_phdc_set_callbacks(CLASS_CALL_STRUCT_PTR ccs_ptr, 
										phdc_callback sendCallback, 
										phdc_callback recvCallback, 
										phdc_callback ctrlCallback);

USB_STATUS usb_class_phdc_send_control_request(USB_PHDC_PARAM *call_param);

USB_STATUS usb_class_phdc_recv_data(USB_PHDC_PARAM *call_param);
									
USB_STATUS usb_class_phdc_send_data(USB_PHDC_PARAM *call_param);

USB_STATUS usb_class_phdc_cancel_transfer(CLASS_CALL_STRUCT_PTR ccs_ptr,
										  uint_32			 tr_index,
										  _usb_pipe_handle   tr_pipe_handle);

#ifdef __cplusplus
}
#endif


#endif /* __USB_HOST_PHDC_H__ */


