#ifndef __ehci_intr_h__
#define __ehci_intr_h__
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
* $FileName: ehci_intr.h$
* $Version : 3.0.2.0$
* $Date    : Aug-12-2009$
*
* Comments:
*
*   
*
*END************************************************************************/
#include "types.h"
#include "host_common.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint_32 _usb_ehci_add_interrupt_xfer_to_periodic_list (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR, PIPE_TR_STRUCT_PTR);
extern void _usb_ehci_close_interrupt_pipe (_usb_host_handle, PIPE_DESCRIPTOR_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif