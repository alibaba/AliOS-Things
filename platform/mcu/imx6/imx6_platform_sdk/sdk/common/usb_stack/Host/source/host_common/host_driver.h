#ifndef __usb_mqx_h__
#define __usb_mqx_h__
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
* $FileName: host_driver.h$
* $Version : 3.4.8.0$
* $Date    : Sep-11-2009$
*
* Comments:
*
*   This file contains MQX-specific code used by the USB stack
*
*END************************************************************************/


#undef MQX_DISABLE_CONFIG_CHECK

#define USB_RTOS_HOST_NUM_PIPE_MESSAGES         32

#define USB_RTOS_HOST_TASK_NUM_MESSAGES         32

#define USB_RTOS_HOST_TASK_TEMPLATE_INDEX       0
#define USB_RTOS_HOST_TASK_ADDRESS              _usb_rtos_host_task
#define USB_RTOS_HOST_TASK_STACKSIZE            2048
#define USB_RTOS_HOST_TASK_PRIORITY             3
#define USB_RTOS_HOST_TASK_NAME                 "USB Host Task"
#define USB_RTOS_HOST_TASK_ATTRIBUTES           0
#define USB_RTOS_HOST_TASK_CREATION_PARAMETER   USB_RTOS_HOST_TASK_NUM_MESSAGES
#define USB_RTOS_HOST_TASK_DEFAULT_TIME_SLICE   0

#define USB_RTOS_HOST_IO_TASK_NUM_MESSAGES         32

#define USB_RTOS_HOST_IO_TASK_TEMPLATE_INDEX       0
#define USB_RTOS_HOST_IO_TASK_ADDRESS              _usb_rtos_host_io_task
#define USB_RTOS_HOST_IO_TASK_STACKSIZE            2048
#define USB_RTOS_HOST_IO_TASK_PRIORITY             2
#define USB_RTOS_HOST_IO_TASK_NAME                 "USB Int Task"
#define USB_RTOS_HOST_IO_TASK_ATTRIBUTES           0
#define USB_RTOS_HOST_IO_TASK_CREATION_PARAMETER   0
#define USB_RTOS_HOST_IO_TASK_DEFAULT_TIME_SLICE   0


#ifdef __cplusplus
extern "C" {
#endif



extern uint_8 _usb_host_driver_install
(
   uint_32,
   pointer
);


#ifdef __cplusplus
}
#endif

#endif