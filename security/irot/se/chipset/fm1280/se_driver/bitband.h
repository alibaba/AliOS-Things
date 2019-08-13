
#include "stm32f10x.h"


#define BITBAND(addr,bitnum)  	((addr&0xF0000000)+0x2000000+((addr&0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  		*((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x4001080C
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010808
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001080C
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40010808
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001080C
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40010808
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001080C
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40010808
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x4001080C
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40010808
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x4001080C
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40010808


#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入
#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入
#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入




