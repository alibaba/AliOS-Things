
#include <stdio.h>
#include "realview.h"
#include "board.h"

#define NORFLASH1_BASE     0x40000000
#define NORFLASH1_SIZE     (64 * 1024 * 1024)
#define NORFLASH2_BASE     0x44000000
#define NORFLASH2_SIZE     (64 * 1024 * 1024)
//remap address for nor flash 1
#define NORFLASH_BASE      NORFLASH1_BASE
#define NORFLASH_SIZE      (64 * 1024 * 1024)

#define PAGE_SIZE          (4 * 1024)
#define BLOCK_SIZE         (256 * 1024)
#define BLOCK_NUM          (NORFLASH_SIZE / BLOCK_SIZE)

void nor_init()
{

}

int nor_flash_info_get(unsigned int chipno)
{
    int vendorid, flashid;
    int hw_base = NORFLASH1_BASE;
    int i, xx[0x40];
    if(1 == chipno)
        hw_base = NORFLASH2_BASE;
    __REG8(hw_base) = 0x90;
    vendorid = __REG32(hw_base);
    flashid = __REG32(hw_base + 0x4);
    __REG8(hw_base) = 0x90;
    __REG8(hw_base) = 0xff;
    printf("flash id is 0x%x 0x%x\n", vendorid, flashid);
    // read cfi register
    __REG8(hw_base + 0x55) = 0x98;
    for(i = 0x10; i < 0x40; i++)
        xx[i] = __REG32(hw_base + i * 4);
    __REG8(hw_base + 0x55) = 0x98;
    //i = __REG32(hw_base + 0x40);
    //i = __REG32(hw_base + 0x44);
    //i = __REG32(hw_base + 0x48);
    __REG8(hw_base) = 0xff;
    printf("cfi register is \n");
    for(i = 0x10; i < 0x40; i++) {
        printf("0x%x ", xx[i]);
        if((i & 0x0f) == 0x0f)
            printf("\n");
    }
    printf("\n");
    return 0;
}

static int nor_flash_write_bytes(unsigned int chipno, unsigned int addr, const unsigned char *buf, unsigned int buf_size)
{
    unsigned int hw_base = NORFLASH1_BASE;
    const unsigned char *pdata = buf;
    unsigned int len;
    unsigned int status;

    if(1 == chipno)
        hw_base = NORFLASH2_BASE;
    addr += hw_base;
    for(len = buf_size; len > 0; len--) {
        __REG8(addr) = 0x10;
        __REG8(addr) = *pdata;
        pdata++;
        addr++;
        while(1) {
            status = __REG8(hw_base);
            if(status & 0x80) {
                break;
            }
        }
    }
    __REG8(hw_base) = 0xff;
    return buf_size;
}

static int nor_flash_write_buffer(unsigned int chipno, unsigned int addr, const unsigned char *buf, unsigned int buf_size)
{
    unsigned int hw_base = NORFLASH1_BASE;
    unsigned int page_addr;
    const unsigned char *pdata = buf;
    unsigned int len;
    unsigned int status;

    if(1 == chipno)
        hw_base = NORFLASH2_BASE;
    page_addr = hw_base + (addr & (~(PAGE_SIZE - 1)));
    addr += hw_base;
    __REG8(page_addr) = 0xe8;
    while(1) {
        status = __REG8(hw_base);
        if(status & 0x80) {
            break;
        }
    }
    __REG16(page_addr) = buf_size - 1;
    for(len = buf_size; len > 0; len--) {
        __REG8(addr) = *pdata;
        pdata++;
        addr++;
    }

    __REG8(hw_base) = 0xd0;
    while(1){
        status = __REG8(hw_base);
        if(status & 0x80) {
            __REG8(hw_base) = 0xff;
            break;
        }
    }
    return buf_size;
}

int nor_flash_write(unsigned int chipno, unsigned int *poff, const void *buf, unsigned int buf_size)
{
    unsigned int addr = *poff;
    unsigned int page_addr = addr & (~(PAGE_SIZE - 1));
    const unsigned char *pdata = (const unsigned char *)buf;
    unsigned int len;
    unsigned int write_len = buf_size;
    unsigned int page_num;
    unsigned int i;

    if((*poff >= NORFLASH_SIZE) || (buf_size == 0))
        return -1;
    if((*poff + buf_size) > NORFLASH_SIZE)
        buf_size = NORFLASH_SIZE - *poff;

    // write first page
    if((addr % PAGE_SIZE) || ((addr + buf_size) < (page_addr + PAGE_SIZE))) {
        len = page_addr + PAGE_SIZE - addr;
        if(len > buf_size)
            len = buf_size;
        nor_flash_write_bytes(chipno, addr, pdata, len);
        buf_size -= len;
        pdata += len;
        addr += len;
    }
    // write whole pages
    page_num = buf_size / PAGE_SIZE;
    for(i = page_num; i > 0; i--) {
        len = PAGE_SIZE;
        nor_flash_write_buffer(chipno, addr, pdata, len);
        buf_size -= len;
        pdata += len;
        addr += len;
    }
    // write last page
    if(buf_size) {
        nor_flash_write_bytes(chipno, addr, pdata, buf_size);
    }
    return write_len;
}

int nor_flash_read(unsigned int chipno, unsigned int *poff, void *buf, unsigned int buf_size)
{
    unsigned int hw_base = NORFLASH1_BASE;
    unsigned int addr;
    unsigned char *pdata = buf;
    unsigned int len;

    if(1 == chipno)
        hw_base = NORFLASH2_BASE;
    addr = hw_base + *poff;
    if((*poff >= NORFLASH_SIZE) || (buf_size == 0))
        return -1;
    if((*poff + buf_size) > NORFLASH_SIZE)
        buf_size = NORFLASH_SIZE - *poff;

    __REG8(addr) = 0xff;
    for(len = buf_size; len > 0; len--) {
        *pdata = __REG8(addr);
        pdata++;
        addr++;
    }
    return buf_size;
}

static int nor_flash_erase_blocks(unsigned int chipno, unsigned int block_addr, unsigned int block_num)
{
    unsigned int hw_base = NORFLASH1_BASE;
    unsigned int status;

    if(1 == chipno)
        hw_base = NORFLASH2_BASE;
    block_addr += hw_base;
    for(; block_num > 0; block_num--) {
        __REG8(block_addr) = 0x20;
        __REG8(block_addr) = 0xd0;
        while(1) {
            status = __REG8(hw_base);
            if(status & 0x80) {
                block_addr += BLOCK_SIZE;
                break;
            }
        }
    }
    __REG8(hw_base) = 0xff;
    return 0;
}
int nor_flash_erase(unsigned int chipno, unsigned int off_set, unsigned int size)
{
    unsigned int block_addr = off_set & (~(BLOCK_SIZE - 1));
    unsigned int block_num = 0;

    if((off_set >= NORFLASH_SIZE) || (size == 0))
        return -1;
    if((off_set + size) > NORFLASH_SIZE)
        size = NORFLASH_SIZE - off_set;

    block_num = ((off_set % BLOCK_SIZE) + size + BLOCK_SIZE - 1)  / BLOCK_SIZE;

    return nor_flash_erase_blocks(chipno, block_addr, block_num);
}

