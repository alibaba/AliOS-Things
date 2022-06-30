// 一些常用的、共用的接口

#ifndef __OPENLOONGSON_PUBLIC_H
#define __OPENLOONGSON_PUBLIC_H


#include <stdio.h>


#define MIN(a, b)           ((a) > (b) ? (b) : (a))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

#define DIV_ROUND_UP(n, d)      (((n) + (d) - 1) / (d))

typedef enum
{
    FALSE=0, 
    TRUE=1
}BOOL;

/*
 * 将指定寄存器的指定位置1
 * @reg 寄存器地址
 * @bit 需要置1的那一bit
 */
void reg_set_one_bit(volatile unsigned int *reg, unsigned int bit);


/*
 * 将指定寄存器的指定位清零
 * @reg 寄存器地址
 * @bit 需要清零的那一bit
 */
void reg_clr_one_bit(volatile unsigned int *reg, unsigned int bit);


/*
 * 获取指定寄存器的指定位的值
 * @reg 寄存器地址
 * @bit 需要读取值的那一bit
 * @ret 指定位的值
 */
unsigned int reg_get_bit(volatile unsigned int *reg, unsigned int bit);


/*
 * 向寄存器中写入8bit(一个字节)数据
 * @data 待写入的数据
 * @addr 寄存器地址
 */
void reg_write_8(unsigned char data, volatile unsigned char *addr);


/*
 * 从寄存器读出8bit(一个字节)数据
 * @addr 寄存器地址
 * @ret 读出的数据
 */
unsigned char reg_read_8(volatile unsigned char *addr);


/*
 * 向寄存器中写一个32bit的数据
 * @data 待写入的数据
 * @addr 寄存器地址
 */
void reg_write_32(unsigned int data, volatile unsigned int *addr);


/*
 * 从寄存器读出一个32bit数据
 * @addr 寄存器地址
 * @ret 读出的数据
 */
unsigned int reg_read_32(volatile unsigned int *addr);



#endif

