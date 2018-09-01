#ifndef __usb_host_cdc_h__
#define __usb_host_cdc_h__
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
* $FileName: usb_host_cdc.h$
* $Version : 3.4.5.0$
* $Date    : Sep-10-2009$
*
* Comments:
*
*   This file defines a template structure for Class Drivers.
*
*END************************************************************************/
/* System Include files */
#include "usb.h"
#include "hostapi.h"
#include "host_ch9.h"
#include "host_dev_list.h"
#include "host_common.h"
#include "usbprv.h"
#include "usbprv_host.h"
#include "usbevent.h"
#include "fio.h"
#include "io.h"

#define USB_CDC_SET_LINE_CODING              0x20
#define USB_CDC_GET_LINE_CODING              0x21
typedef struct {
    uint_32      baudrate;
    uint_8       stopbits; /* 1 ~ 1bit, 2 ~ 2bits, 3 ~ 1.5bit */
    uint_8       parity;   /* 1 ~ even, -1 ~ odd, 0 ~ no parity */
    uint_8       databits;
} USB_CDC_UART_CODING, _PTR_ USB_CDC_UART_CODING_PTR;

/* set ouput pin state */
#define USB_CDC_SET_CTRL_LINE_STATE          0x22
typedef struct {
#define USB_ACM_LINE_STATE_DTR               0x01
#define USB_ACM_LINE_STATE_RTS               0x02
    uint_8       bmStates[2];
} USB_CDC_CTRL_STATE, _PTR_ USB_CDC_CTRL_STATE_PTR;

/* receive interrupt state */
typedef struct {
    uint_8       reservedA[8];
#define USB_ACM_STATE_RX_CARRIER             0x01
#define USB_ACM_STATE_TX_CARRIER             0x02
#define USB_ACM_STATE_BREAK                  0x04
#define USB_ACM_STATE_RING_SIGNAL            0x08
#define USB_ACM_STATE_FRAMING                0x10
#define USB_ACM_STATE_PARITY                 0x20
#define USB_ACM_STATE_OVERRUN                0x40
    uint_8       bmStates;
    uint_8       reservedB[1];
} USB_CDC_ACM_STATE, _PTR_ USB_CDC_ACM_STATE_PTR;

#define USB_DESC_SUBTYPE_CS_HEADER           0x00
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bcdCDC[2];
} USB_CDC_DESC_HEADER, _PTR_ USB_CDC_DESC_HEADER_PTR;

#define USB_DESC_SUBTYPE_CS_CM               0x01
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
#define USB_ACM_CM_CAP_HANDLE_MANAGEMENT     0x01
#define USB_ACM_CM_CAP_DATA_CLASS            0x02
    uint_8   bmCapabilities;
    uint_8   bDataInterface;
} USB_CDC_DESC_CM, _PTR_ USB_CDC_DESC_CM_PTR;

#define USB_DESC_SUBTYPE_CS_ACM              0x02
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
#define USB_ACM_CAP_COMM_FEATURE             0x01
#define USB_ACM_CAP_LINE_CODING              0x02
#define USB_ACM_CAP_SEND_BREAK               0x04
#define USB_ACM_CAP_NET_NOTIFY               0x08
    uint_8   bmCapabilities;
} USB_CDC_DESC_ACM, _PTR_ USB_CDC_DESC_ACM_PTR;

#define USB_DESC_SUBTYPE_CS_DL               0x03
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bmCapabilities;
} USB_CDC_DESC_DL, _PTR_ USB_CDC_DESC_DL_PTR;

#define USB_DESC_SUBTYPE_CS_TR               0x04
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bRingerVolSteps;
    uint_8   bNumRingerPatterns;
} USB_CDC_DESC_TR, _PTR_ USB_CDC_DESC_TR_PTR;

#define USB_DESC_SUBTYPE_CS_TCS              0x05
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bmCapabilities[4];
} USB_CDC_DESC_TCS, _PTR_ USB_CDC_DESC_TCS_PTR;

#define USB_DESC_SUBTYPE_CS_UNION            0x06
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bMasterInterface;
    uint_8   bSlaveInterface[];
} USB_CDC_DESC_UNION, _PTR_ USB_CDC_DESC_UNION_PTR;

#define USB_DESC_SUBTYPE_CS_TOM              0x08
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bmCapabilities;
} USB_CDC_DESC_TOM, _PTR_ USB_CDC_DESC_TOM_PTR;

typedef union {
    USB_CDC_DESC_HEADER         header;
    USB_CDC_DESC_CM             cm;
    USB_CDC_DESC_ACM            acm;
    USB_CDC_DESC_DL             dl;
    USB_CDC_DESC_TR             tr;
    USB_CDC_DESC_TCS            tcs;
    USB_CDC_DESC_TOM            tom;
    USB_CDC_DESC_UNION          uni;
} USB_CDC_FUNC_DESC, _PTR_ USB_CDC_FUNC_DESC_PTR;

/*
** CDC Class Interface structure. This structure will be passed to
** all commands to this class driver.
*/
typedef struct usb_cdc_general_class {
    /* Each class must start with a GENERAL_CLASS struct */
    GENERAL_CLASS                             G;

    /* Higher level callback and parameter */
    tr_callback                               USER_CALLBACK;
    pointer                                   USER_PARAM;

    uint_8                                    IFNUM;

} USB_CDC_GENERAL_CLASS, _PTR_ USB_CDC_GENERAL_CLASS_PTR;

typedef struct {
    /* Each CDC subclass must start with a USB_CDC_GENERAL_CLASS struct */
    USB_CDC_GENERAL_CLASS                     CDC_G;
    USB_CDC_DESC_ACM_PTR                      acm_desc;
    USB_CDC_DESC_CM_PTR                       cm_desc;
    USB_CDC_DESC_HEADER_PTR                   header_desc;
    USB_CDC_DESC_UNION_PTR                    union_desc;
   
    USB_CDC_UART_CODING                       uart_coding;

    _usb_pipe_handle                          interrupt_pipe;
    USB_CDC_ACM_STATE                         interrupt_buffer;
    
    USB_CDC_CTRL_STATE                        ctrl_state;

#define USB_ACM_DETACH           0x01
#define USB_ACM_CTRL_PIPE_FREE   0x02
#define USB_ACM_INT_PIPE_FREE    0x04
    USB_EVENT_STRUCT                            acm_event;

} USB_ACM_CLASS_INTF_STRUCT, _PTR_ USB_ACM_CLASS_INTF_STRUCT_PTR;

typedef struct {
    /* Each CDC subclass must start with a USB_CDC_GENERAL_CLASS struct */
    USB_CDC_GENERAL_CLASS                     CDC_G;
    /* Address of bound control interface */
    CLASS_CALL_STRUCT_PTR                     BOUND_CONTROL_INTERFACE;

    char_ptr                                  RX_BUFFER;
    char_ptr                                  RX_BUFFER_DRV;
    char_ptr                                  RX_BUFFER_APP;
    uint_32                                   RX_BUFFER_SIZE;
    uint_32                                   RX_READ;
    
    uint_32                                   TX_SENT;

    _usb_pipe_handle                          in_pipe;
    _usb_pipe_handle                          out_pipe;
   
    char_ptr                                  device_name;

#define USB_DATA_DETACH            0x01
#define USB_DATA_READ_COMPLETE     0x02
#define USB_DATA_READ_PIPE_FREE    0x04
#define USB_DATA_SEND_COMPLETE     0x08
#define USB_DATA_SEND_PIPE_FREE    0x10
    USB_EVENT_STRUCT                            data_event;

} USB_DATA_CLASS_INTF_STRUCT, _PTR_ USB_DATA_CLASS_INTF_STRUCT_PTR;

typedef struct {
    CLASS_CALL_STRUCT_PTR   CLASS_PTR;
    tr_callback             CALLBACK_FN;
    pointer                 CALLBACK_PARAM;
} CDC_COMMAND, _PTR_ CDC_COMMAND_PTR;

typedef struct cdc_serial_init {
#define USB_UART_BLOCKING          (0x0000)
#define USB_UART_NO_BLOCKING       (0x0001)
/* NOTE!!!
** HW control flow has one weakness: we dont know exactly when DCD signal got log. 0 or
** log.1. This is because interrupts pipe is polled in time intervals in some milliseconds.
** HW flow control is here only just for compatibility purposes.
** As for output pins, we set always DTR and RTS signal for HW flow control. The user can
** still use usb_class_cdc_set_acm_ctrl_state to control those signals manually.
** Note also, that additional HW control flow is done on USB,
** which implies from USB specification (NAK responding).
*/
#define USB_UART_HW_FLOW           (0x0002)
    uint_32               FLAGS;
} CDC_SERIAL_INIT, _PTR_ CDC_SERIAL_INIT_PTR;

/* Class specific functions exported by CDC class driver */
#ifdef __cplusplus
extern "C" {
#endif

extern void usb_class_cdc_acm_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
extern void usb_class_cdc_data_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
extern CLASS_CALL_STRUCT_PTR usb_class_cdc_get_ctrl_interface(pointer);
extern CLASS_CALL_STRUCT_PTR usb_class_cdc_get_data_interface(pointer);
extern USB_STATUS usb_class_cdc_get_acm_line_coding(CLASS_CALL_STRUCT_PTR, USB_CDC_UART_CODING_PTR);
extern USB_STATUS usb_class_cdc_set_acm_line_coding(CLASS_CALL_STRUCT_PTR, USB_CDC_UART_CODING_PTR);
extern USB_STATUS usb_class_cdc_set_acm_ctrl_state(CLASS_CALL_STRUCT_PTR, uint_8, uint_8);
extern USB_STATUS usb_class_cdc_get_acm_descriptors(_usb_device_instance_handle, _usb_interface_descriptor_handle, USB_CDC_DESC_ACM_PTR _PTR_, USB_CDC_DESC_CM_PTR _PTR_, USB_CDC_DESC_HEADER_PTR _PTR_, USB_CDC_DESC_UNION_PTR _PTR_);
extern USB_STATUS usb_class_cdc_set_acm_descriptors(CLASS_CALL_STRUCT_PTR, USB_CDC_DESC_ACM_PTR, USB_CDC_DESC_CM_PTR, USB_CDC_DESC_HEADER_PTR, USB_CDC_DESC_UNION_PTR);
extern USB_STATUS usb_class_cdc_get_ctrl_descriptor(_usb_device_instance_handle, _usb_interface_descriptor_handle, INTERFACE_DESCRIPTOR_PTR _PTR_);
extern USB_STATUS usb_class_cdc_bind_data_interfaces(_usb_device_instance_handle, CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_unbind_data_interfaces(CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_bind_acm_interface(CLASS_CALL_STRUCT_PTR, INTERFACE_DESCRIPTOR_PTR);
extern USB_STATUS usb_class_cdc_unbind_acm_interface(CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_init_ipipe(CLASS_CALL_STRUCT_PTR);
extern USB_STATUS usb_class_cdc_install_driver(CLASS_CALL_STRUCT_PTR, char_ptr);
extern USB_STATUS usb_class_cdc_uninstall_driver(CLASS_CALL_STRUCT_PTR);
extern int_32 _io_cdc_serial_open(FILE_CDC_PTR, char _PTR_, char _PTR_);
extern int_32 _io_cdc_serial_read(FILE_CDC_PTR, char_ptr, int_32);
extern int_32 _io_cdc_serial_write(FILE_CDC_PTR, char_ptr, int_32);

#ifdef __cplusplus
}
#endif


#endif
