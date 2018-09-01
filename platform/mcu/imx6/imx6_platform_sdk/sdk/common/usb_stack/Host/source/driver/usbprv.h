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
* $FileName: usbprv.h$
* $Version : 3.4.15.0$
* $Date    : Sep-14-2009$
*
* Comments:
*
*   This file contains the internal USB specific type definitions
*
*END************************************************************************/
#ifndef __usbprv_h__
#define __usbprv_h__ 1
#include "derivative.h"
#include "psptypes.h"

#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
	#include "usb_lock.h"

	#define USB_lock()   usb_int_dis()
	#define USB_unlock() usb_int_en()
#else
	#ifndef _MK_xxx_H_
		#define USB_lock()                           asm { move.w SR,D0; ori.l #0x0700,D0; move.w D0,SR;  }
		#define USB_unlock()                         asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }
	#else
		#ifdef __CC_ARM
			#define USB_lock()                           __disable_irq();
			#define USB_unlock()                         __enable_irq();
		#else
			#define USB_lock()                           asm(" CPSID i");
			#define USB_unlock()                         asm(" CPSIE i");
		#endif
	#endif
#endif

#define USB_dcache_invalidate()
#define USB_dcache_invalidate_line(p)
/* Based on the targets it should be modified, for ColdFire it is MBYTES */
#define USB_dcache_invalidate_mlines(p,n)
#define USB_dcache_flush_line(p)
/* Based on the targets it should be modified, for ColdFire it is MBYTES */
#define USB_dcache_flush_mlines(p,n)


#ifndef USB_mem_alloc_uncached
#define USB_mem_alloc_uncached             USB_mem_alloc_word_aligned
#endif

#ifndef USB_mem_alloc_uncached_zero
#define USB_mem_alloc_uncached_zero        USB_mem_alloc_zero
#endif


#if defined __MCF52277_H__ || defined MCU_MK70F12 
	extern void* USB_mem_alloc_zero(uint_32 size);
#else
	/* todo AI: change this to USB_mem_alloc_zero */
	#define USB_mem_alloc_zero(n)             USB_mem_alloc_word_aligned(n)
#endif
#define USB_mem_free(ptr)                     free(ptr)
#define USB_mem_zero(ptr,n)                   memset((ptr),(0),(n))
#define USB_mem_copy(src,dst,n)               memcpy((dst),(src),(n))

#define USB_uncached	                      volatile

#endif

/* EOF */

