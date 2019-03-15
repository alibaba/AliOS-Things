/*--------------------------------------------------------------------
 * drafter	:	huangyucai
 * file		:	delay.h
 * date		:	20110614
 * desc		:	supply a task delay(not sleep) waiting,that is says that
 *				this is a busy waiting,so please use it carefully under
 *				a short wait,especially less than 2 ms
 *maintainer: Halley
 *-------------------------------------------------------------------*/
 
/**
* @addtogroup Çý¶¯
* @{
* @defgroup Delay Delay
* @{
*/
 
#ifndef		__DELAY_H__
#define		__DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

extern void __udelay(uint32_t us);
extern void Delay(uint32_t Loops);

/*
 * udelay,delay about a mirosecond time
 */
#define DelayUs(n)	((n) <= 1000 ? __udelay(n) : __udelay(1000))

/*
 * mdelay,udelay times 1000
 */
extern void DelayMs(unsigned int Ms);

/*
 * Oh, if only we had a cycle counter...
 */
#if defined(__CC_ARM)
static __INLINE __ASM  void Delay(uint32_t Loops)
{
__DELAY
	SUBS R0, R0, #1
	BHI	 __DELAY
	BX	 LR
}
#elif defined(__ICCARM__)
static __INLINE void Delay(uint32_t Loops)
{
    asm("__DELAY:           \n"
        "SUBS R0, R0, #1    \n"
        "BHI.W	 __DELAY    \n"
        "BX	 LR             \n");
}
#endif

#define WaitMs(Ms)     DelayMs(Ms)

#define WaitUs(Us)     DelayUs(Us)


#ifdef __cplusplus
}
#endif//__cplusplus

#endif	//_DELAY_H_

/**
 * @}
 * @}
 */
