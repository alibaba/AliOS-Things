/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/debug.h"

#include "lwip/arch.h"

#include "lwip/def.h"
#include "lwip/inet.h"


/*-----------------------------------------------------------------------------------*/
/* lwip_chksum:
 *
 * Sums up all 16 bit words in a memory portion. Also includes any odd byte.
 * This function is used by the other checksum functions.
 *
 */
/*-----------------------------------------------------------------------------------*/
#if 0
u16_t
lwip_chksum(void *dataptr, int len)
{
  u32_t acc;
    
  for(acc = 0; len > 1; len -= 2) {
    acc += *((u16_t *)dataptr)++;
  }

  /* add up any odd byte */
  if (len == 1) {
    acc += htons((u16_t)((*(u8_t *)dataptr) & 0xff) << 8);
    LWIP_DEBUGF(INET_DEBUG, ("inet: chksum: odd byte %d\n", *(u8_t *)dataptr));
  }
  acc = (acc >> 16) + (acc & 0xffffUL);

  if (acc & 0xffff0000 != 0) {
    acc = (acc >> 16) + (acc & 0xffffUL);
  }

  return (u16_t)acc;
}
/*-----------------------------------------------------------------------------------*/
#endif
