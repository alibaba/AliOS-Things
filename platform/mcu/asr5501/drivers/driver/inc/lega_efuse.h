#ifndef _LEGA_EFUSE_H_
#define _LEGA_EFUSE_H_

#define EFUSE_READ_OP_WAIT_CYCLE        5
#define EFUSE_WRITE_OP_WAIT_CYCLE       (SYSTEM_CLOCK/100000) //10us
#define EFUSE_DIV_OP_WAIT_CYCLE         31

#define EFUSE_LDO25_OPEN  1
#define EFUSE_LDO25_CLOSE 0

void lega_efuse_init(uint8_t ldo25_open);

uint8_t lega_efuse_byte_read(uint16_t addr);

uint32_t lega_efuse_word_read(uint16_t addr);

void lega_efuse_byte_write(uint16_t addr, uint8_t data);

void lega_efuse_word_write(uint16_t addr, uint32_t data);

void lega_efuse_multi_read(uint16_t start_addr, uint16_t size_in_bytes, uint8_t *pData);

#endif //_LEGA_EFUSE_H_
