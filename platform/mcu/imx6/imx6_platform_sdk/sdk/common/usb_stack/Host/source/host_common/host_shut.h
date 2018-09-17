#ifndef __host_shut_h__
#define __host_shut_h__
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
* $FileName: host_shut.h$
* $Version : 3.0.2.0$
* $Date    : Sep-12-2008$
*
* Comments:
*
*   
*
*END************************************************************************/
#include "psptypes.h"
#include "host_common.h"

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern void _usb_host_shutdown (_usb_host_handle);
extern void _usb_host_bus_control (_usb_host_handle, uint_8);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */