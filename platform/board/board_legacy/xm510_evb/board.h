#ifndef __BOARD_H__
#define __BOARD_H__

#include <xm510.h>

extern int __bss_end;
#define HEAP_BEGIN      ((void*)&__bss_end)

#define HEAP_END        (void*)(0x82000000)  //TODO

#define __REG32(x)  (*((volatile unsigned int *)(x)))
#define __REG16(x)  (*((volatile unsigned short *)(x)))
#define __arch_getb(a)			(*(volatile unsigned char *)(a))
#define __arch_getw(a)			(*(volatile unsigned short *)(a))
#define __arch_getl(a)			(*(volatile unsigned int *)(a))

#define __arch_putb(v,a)		(*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v,a)		(*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v,a)		(*(volatile unsigned int *)(a) = (v))

#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#define __iormb()	dmb()
#define __iowmb()	dmb()


#define writeb(v,c)	({ unsigned char  __v = v; __iowmb(); __arch_putb(__v,c); __v; })
#define writew(v,c)	({ unsigned short __v = v; __iowmb(); __arch_putw(__v,c); __v; })
#define writel(v,c)	({ unsigned int __v = v; __iowmb(); __arch_putl(__v,c); __v; })

#define readb(c)	({ unsigned char  __v = __arch_getb(c); __iormb(); __v; })
#define readw(c)	({ unsigned short __v = __arch_getw(c); __iormb(); __v; })
#define readl(c)	({ unsigned int __v = __arch_getl(c); __iormb(); __v; })
void aos_board_init(void);

#endif
