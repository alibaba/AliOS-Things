#ifndef _LEGA_COMM_H_
#define _LEGA_COMM_H_

void jumpToApp(int addr);
void lega_memset(char *buf,int value, int size);
void lega_memcpy(char *dst, char *src, int size);
void delay(unsigned int cycles);
int convert_str_to_int(char *str);
void convert_int_to_str(unsigned int val, unsigned int type, char *ch);
void roll_index(volatile unsigned char *idx, int size);

/// Macro to read a register
#define REG_RD(addr)              (*(volatile uint32_t *)(addr))
/// Macro to write a register
#define REG_WR(addr, value)       (*(volatile uint32_t *)(addr)) = (value)

void lega_write32_bit(uint32_t reg, uint8_t start_bit, uint8_t len, uint32_t src_val);
uint32_t lega_read32_bit(uint32_t reg, uint8_t start_bit, uint8_t len);
#endif //_LEGA_COMM_H_