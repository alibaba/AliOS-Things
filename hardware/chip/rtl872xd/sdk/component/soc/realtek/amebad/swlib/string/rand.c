/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

HAL_ROM_BSS_SECTION u32 rand_seed[4]; //z1, z2, z3, z4, 
HAL_ROM_BSS_SECTION u32 rand_first;

HAL_ROM_TEXT_SECTION
_LONG_CALL_ u32
Rand (
    VOID
)
{
	u32 b;

	if (!rand_first) {
		rand_seed[0] = 12345;
		rand_seed[1] = 12345;
		rand_seed[2] = 12345;
		rand_seed[3] = 12345;
		rand_first = 1;
	}

	b  = ((rand_seed[0] << 6) ^ rand_seed[0]) >> 13;
	rand_seed[0] = ((rand_seed[0] & 4294967294U) << 18) ^ b;
	b  = ((rand_seed[1] << 2) ^ rand_seed[1]) >> 27; 
	rand_seed[1] = ((rand_seed[1] & 4294967288U) << 2) ^ b;
	b  = ((rand_seed[2]<< 13) ^ rand_seed[2]) >> 21;
	rand_seed[2] = ((rand_seed[2] & 4294967280U) << 7) ^ b;
	b  = ((rand_seed[3] << 3) ^ rand_seed[3]) >> 12;
	rand_seed[3] = ((rand_seed[3] & 4294967168U) << 13) ^ b;
	return (rand_seed[0] ^ rand_seed[1] ^ rand_seed[2] ^ rand_seed[3]);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
int Rand_Arc4(void)
{
	u32 res = SYSTIMER_TickGet();
	
	HAL_ROM_BSS_SECTION static unsigned long seed = 0xDEADB00B;
	
	seed = ((seed & 0x007F00FF) << 7) ^
		((seed & 0x0F80FF00) >> 8) ^ // be sure to stir those low bits
		(res << 13) ^ (res >> 9);    // using the clock too!

	return (int)seed;
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
int RandBytes_Get(void *buf, u32 len)
{
	unsigned int ranbuf;
	unsigned int *lp;
	int i, count;
	
	count = len / sizeof(unsigned int);
	lp = (unsigned int *) buf;

	for(i = 0; i < count; i ++) {
		lp[i] = Rand_Arc4();  
		len -= sizeof(unsigned int);
	}

	if(len > 0) {
		ranbuf = Rand_Arc4();
		_memcpy(&lp[i], &ranbuf, len);
	}
	
	return 0;
}

