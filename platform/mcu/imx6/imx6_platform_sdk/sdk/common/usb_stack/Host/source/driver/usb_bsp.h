#ifndef __usb_bsp_h__
#define __usb_bsp_h__ 1

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
* $FileName: usb_bsp.h$
* $Version : 3.4.9.0$
* $Date    : Sep-22-2009$
*
* Comments:
*
*   Include this header file to use BSP-specific USB initialization code
*
*END************************************************************************/

#include "derivative.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "psptypes.h"

#if (defined MCU_MK20D7) || (defined MCU_MK40D7) 
	#define MCGOUTCLK_72_MHZ	
	//#define MCGOUTCLK_48_MHZ	
#endif

#ifdef MCGOUTCLK_72_MHZ
	#define SYSTEM_CLOCK   ((uint_32)72000000)
#else
	#define SYSTEM_CLOCK   ((uint_32)48000000)
#endif

#define BUS_CLOCK      ((uint_32)(SYSTEM_CLOCK) / 2)


extern void   _bsp_platform_init(void);
extern int_32 _bsp_usb_host_init(pointer);

#ifdef __cplusplus
}
#endif

#endif