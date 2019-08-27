/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2010 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for New Device 256kB Flash                            */
/*                                                                     */
/***********************************************************************/
#include <stdlib.h>
#include "lega_flash_alg.h"        // FlashOS Structures
#include "lega_cm4.h"
#include "lega_common.h"


FLASH_DRIVER_SEG void lega_flash_alg_cache_bypass(void)
{
    //cache bypass
    if(!(FLASH_CACHE_CTRL & CACHE_BYPASS))
    {
        FLASH_CACHE_CTRL = CACHE_BYPASS;
    }
}

FLASH_DRIVER_SEG void lega_flash_alg_cache_enable(void)
{
    //cache enable
    if(FLASH_CACHE_CTRL & CACHE_BYPASS)
    {
        //cache flush, set bit0
        FLASH_CACHE_CTRL = CACHE_BYPASS | CACHE_FLUSH;

        //disable cache bypass
        FLASH_CACHE_CTRL = 0x00;
    }
}

FLASH_DRIVER_SEG void lega_flash_alg_cache_flush(void)
{
    if(FLASH_CACHE_CTRL & CACHE_BYPASS) //if cache already bypass, do nothing
    {
        return;
    }
    //enable cache bypass
    FLASH_CACHE_CTRL = CACHE_BYPASS;

    //set bit0
    FLASH_CACHE_CTRL = CACHE_BYPASS | CACHE_FLUSH;

    //disable cache bypass
    FLASH_CACHE_CTRL = 0x00;
}

FLASH_DRIVER_SEG void lega_flash_alg_f_delay(unsigned char cycle)
{
    while(cycle--)
        __asm("nop");
}

#if (defined LEGA_A0V1)
FLASH_DRIVER_SEG int lega_flash_alg_init(void)
{
    int var_rdata = 0;
    int cmp_rdata = 0;

    while((FLASH->BUSY & 0x01) ==1);

    FLASH->CMD    = RDID; //WRITE
    FLASH->LENGTH = 0;
    FLASH->ADDR = 0;
    FLASH->DUMMY = 0;
    FLASH->SBUS_START = 0x01; //WRITE
    lega_flash_alg_f_delay(30);
    while((FLASH->BUSY & 0x01) ==1);

    var_rdata = FLASH->ID;  //READ
    cmp_rdata = var_rdata>>16;
    if(cmp_rdata == 0xC2) {
        FLASH->CFG = 0x110;
    }
    else if((cmp_rdata == 0xEF)||(cmp_rdata == 0x0B)) {
        FLASH->CFG = 0x210;
    }
    else if(cmp_rdata == 0xC8) {
        FLASH->CFG = 0x410;
    }
    FLASH->IBUS_CMD = 0x3B; //2-wire
    return (0);                                  // Finished without Errors
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

FLASH_DRIVER_SEG int lega_flash_alg_erasechip (void) {
    volatile int var_rdata = 0;
    volatile int cycle_count = 0;
    while((FLASH->BUSY & 0x01)==1);
    //configure XTX mode, disable auto ERA nor PRG
    FLASH->CFG = 0x200;
    do
    {   //write enable
        FLASH->CMD = WREN;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        //read Device status register
        FLASH->CMD = RDSR;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        var_rdata = FLASH->STATUS;//READ status
        cycle_count++;
        if(cycle_count == 5)
        {
            return 1;
        }
    }while(((var_rdata>>1) & 0x01 )==0);
    cycle_count = 0;
    //set command CE
    FLASH->CMD = CE;
    FLASH->LENGTH = 0;
    FLASH->DUMMY = 0;
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(30);
    //check busy
    while((FLASH->BUSY & 0x01)==1);
    do
    {   //set command Read Device status reg
        FLASH->CMD = RDSR;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        //read flash controller status register
        var_rdata = FLASH->STATUS;
        cycle_count++;
        //cycle_count value caculated by operation(chip erase) under related Flash clock.
        //time value here used for timeout check is enough
        if(cycle_count == 200000000)
        {
            return 1;
        }
    }while((var_rdata & 0x01)==1);
    cycle_count = 0;
    return (0); // Finished without Errors

}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

FLASH_DRIVER_SEG int lega_flash_alg_erasesector(unsigned long adr)
{
    volatile int var_rdata = 0;
    volatile int cycle_count = 0;
    /* Add your Code */
    while((FLASH->BUSY & 0x01)==1);
    do
    {        //write enable
        FLASH->CMD = WREN;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        //read Device status register
        FLASH->CMD = RDSR;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        var_rdata = FLASH->STATUS;//READ status
        cycle_count++;
        if(cycle_count == 5)
        {
            return adr;
        }
    }while(((var_rdata>>1) & 0x01 )==0);
    cycle_count = 0;
    //set command SE
    FLASH->CMD = SE;
    FLASH->ADDR = adr;
    FLASH->LENGTH = 0;
    FLASH->DUMMY = 0;
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(30);
    //check busy
    while((FLASH->BUSY & 0x01)==1);
    do
    {   //set command Read Device status reg
        FLASH->CMD = RDSR;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        //read flash controller status register
        var_rdata = FLASH->STATUS;
        cycle_count++;
        //time out check
        if(cycle_count == 5000000)
        {
            return adr;
        }
    }while((var_rdata & 0x01)==1);
    cycle_count = 0;
    return (0);
    // Finished without Errors
}

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

FLASH_DRIVER_SEG int lega_flash_alg_programpage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
    unsigned long adr_temp,sz_temp;
    int var_rdata = 0;
    volatile int cycle_count = 0;
    adr_temp = 0;

    if((0 == sz) || (NULL == buf))
    {
        return 1;
    }
    sz_temp = sz - 1;

    //word align
    sz = (sz + 3) & ~3;                           // Adjust size for Words
    //write one page data to Buffer
    while (sz) {
        M32(FLASH_BASE + 0x100 + adr_temp) = *((unsigned long *)buf);        // Program Half Word
        while((FLASH->BUSY & 0x01)==1){
        }
          buf += 4;
          sz  -= 4;
          adr_temp +=4;
    }
    while((FLASH->BUSY & 0x01)==1);

    //configure XTX mode, disable auto ERA nor PRG
    FLASH->CFG = 0x200;
    do
    {   //write enable
        FLASH->CMD = WREN;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        //read Device status register
        FLASH->CMD = RDSR;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        var_rdata = FLASH->STATUS;//READ status
        cycle_count++;
        if(cycle_count == 5)
        {
            return adr;
        }
    }while(((var_rdata>>1) & 0x01 )==0);
    cycle_count = 0;
    //set command PP
    FLASH->CMD    = PP; //WRITE
    FLASH->LENGTH = sz_temp;
    FLASH->ADDR = adr;
    FLASH->DUMMY = 0;
    FLASH->SBUS_START = 0x01; //START WRITE
    lega_flash_alg_f_delay(30);
    while((FLASH->BUSY & 0x01)==1);
    do
    {   //set command Read Device status reg
        FLASH->CMD = RDSR;
        FLASH->SBUS_START = 0x1;
        lega_flash_alg_f_delay(30);
        while((FLASH->BUSY & 0x01)==1);
        //read flash controller status register
        var_rdata = FLASH->STATUS;
        //polling status register
        cycle_count++;
        //timeout check
        if(cycle_count == 5000000)
        {
            return adr;
        }
    }while((var_rdata & 0x01)==1);
    cycle_count = 0;
    return (0);                                  // Finished without Errors
}

#elif (defined LEGA_A0V2)
FLASH_DRIVER_SEG int lega_flash_alg_check_busy (void) {

    int var_rdata = 0;
    int cnt = 0;
    /* Add your Code */
    var_rdata = FLASH->QSPI_SR; //read back

    while(((var_rdata>>5 & 0x01) ==1)){
        var_rdata = FLASH->QSPI_SR; //read back
        if(cnt < 80000000)//erase chip time(10s)/(160MHz*2cycle)
            cnt = cnt+1;
        else
            return (1);
    }
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int lega_flash_alg_check_abort_busy (void) {

    int var_rdata = 0;
    int cnt = 0;
    /* Add your Code */
    var_rdata = FLASH->QSPI_SR; //read back

    while(((var_rdata>>6 & 0x01) ==1)){
        var_rdata = FLASH->QSPI_SR; //read back
        if(cnt < 200)// /160MHz*10cycled
            cnt = cnt+1;
        else
            return (1);
    }
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int lega_flash_alg_clr_flg (void) {
    /* Add your Code */
    FLASH->QSPI_FCR = 0xF001B;//0x1F;
    FLASH->QSPI_FCR = 0x0;

    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int lega_flash_alg_abort_en (void) {
    /* Add your Code */
    int var_rdata;
    var_rdata=FLASH->QSPI_CR;
    FLASH->QSPI_CR = (var_rdata&0xFFFFFFFD)+0x2;
    FLASH->QSPI_CR = var_rdata&0xFFFFFFFD;
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int lega_flash_alg_polling_wip (void){
    int var_rdata = 0;
    //dumode=2'b00,fmode=2'b10,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h05;
    FLASH->QSPI_CCR = 0x9000105;
    var_rdata = FLASH->QSPI_CR;
    FLASH->QSPI_CR = (var_rdata & 0xFFBFFFFF) + 0x400000; //QSPI_CR[22],apms= 1'b1;
    FLASH->QSPI_DLR = 0x0;//one byte
    FLASH->QSPI_PSMKR = 0x1;//mask = 0x1;
    FLASH->QSPI_PSMAR = 0x0;//match = 0x0;
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    lega_flash_alg_clr_flg();
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int lega_flash_alg_polling_wel (void){
    int var_rdata = 0;
    //dumode=2'b00,fmode=2'b10,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h05;
    FLASH->QSPI_CCR = 0x9000105;
    var_rdata = FLASH->QSPI_CR;
    FLASH->QSPI_CR = (var_rdata & 0xFFBFFFFF) + 0x400000; //QSPI_CR[22],apms= 1'b1;
    FLASH->QSPI_DLR = 0x0;//one byte
    FLASH->QSPI_PSMKR = 0x2;//mask = 0x1;
    FLASH->QSPI_PSMAR = 0x2;//match = 0x0;
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    lega_flash_alg_clr_flg();
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG int lega_flash_alg_setqe (unsigned char quad) {
    //bypass enable 0x4000308C bit10 set 1
    //*((volatile unsigned int *)(0x4000308C)) |= (1 << 10);
    FLASH->QSPI_CFGR |= (1 << 10);
    lega_flash_alg_abort_en();
    lega_flash_alg_check_abort_busy();

    FLASH->QSPI_CCR = 0x106;//IMODE=2'b01,INSTRUCTION=WREN
    lega_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    lega_flash_alg_polling_wel();

    FLASH->QSPI_CCR = 0x01000101;//dumode=2'b00,fmode=2'b00,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h01;
    FLASH->QSPI_DLR = 0x1;//two byte
    if(quad == 0x1){
        FLASH->QSPI_DR = 0x200;
    }
    else{
        FLASH->QSPI_DR = 0x0; //0x200;
    }
    lega_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    lega_flash_alg_clr_flg();
    lega_flash_alg_polling_wip();
    //bypass disable 0x4000308C bit10 clr
    //*((volatile unsigned int *)(0x4000308C)) &= ~(1 << 10);
    FLASH->QSPI_CFGR &= ~(1 << 10);
    /* Add your Code */
    return (0);                                  // Finished without Errors
}

//use in unencryption
FLASH_DRIVER_SEG int lega_flash_alg_check_setqe (void) {
    volatile int var_rdata = 0;
    lega_flash_alg_abort_en();
    lega_flash_alg_check_abort_busy();
    lega_flash_alg_check_busy();
    FLASH->QSPI_DLR = 0x0;
    FLASH->QSPI_CCR = 0x5000135;//dumode=2'b00,fmode=2'b01,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h35;
    lega_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    var_rdata = FLASH->QSPI_DR & 0xFF;
    return var_rdata;
}

FLASH_DRIVER_SEG void lega_flash_alg_load_cfg () {
    lega_flash_alg_abort_en();
    lega_flash_alg_check_abort_busy();
#ifdef CFG_CERTIF_TEST
    FLASH->QSPI_CR    = FLASH_QSPI_DIV4;  //div4, //offset 0x00
#else
    FLASH->QSPI_CR    = FLASH_QSPI_DIV2;  //div2, //offset 0x00
#endif
    FLASH->QSPI_DCR   = 0x00140000  ;        //offset 0x04
    FLASH->QSPI_FCR   = 0x0  ;        //offset 0x0C
    FLASH->QSPI_DLR   = 0x0  ;        //offset 0x10
    FLASH->QSPI_CCR   = FLASH_QSPI_L2;//dumode=2'b10,,sio=1'b0,fmode=2'b11,dmode=2'b10,reserved=1'b0,dcyc=5'h7,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h3B;
    FLASH->QSPI_AR    = 0x0  ;        //offset 0x18
    FLASH->QSPI_ABR   = 0x0  ;        //offset 0x1C
    FLASH->QSPI_DR    = 0x0  ;        //offset 0x20
    FLASH->QSPI_PSMKR = 0x0  ;        //offset 0x24
    FLASH->QSPI_PSMAR = 0x0 ;        //offset 0x28
    FLASH->QSPI_PIR   = 0x0 ;        //offset 0x2C
    FLASH->QSPI_TOR   = 0x12FFFF ;        //offset 0x30
    FLASH->QSPI_CFGR  = 0x200 ;        //offset 0x8C

}

// flash crypt bypass test
FLASH_DRIVER_SEG int lega_flash_alg_init ()
{
    //add delay to avoid confilict between flash cfg and instruction fetch by cache
    lega_flash_alg_f_delay(32);
    //lega_flash_alg_abort_en();
    //lega_flash_alg_check_abort_busy();
    //lega_flash_alg_setqe(1);

    lega_flash_alg_load_cfg ();

    //flush cache after flash operation
    lega_flash_alg_cache_flush();
    return (0);                                  // Finished without Errors
}

/*
 *  cmd: CHIP_ERASE_CMD or SECTOR_ERASE_CMD or BLOCK32_ERASE_CMD or BLOCK64_ERASE_CMD
 *  adr: not used for CHIP_ERASE_CMD
 */
FLASH_DRIVER_SEG int lega_flash_alg_erase(unsigned int cmd, unsigned long adr) {
    //add delay to avoid confilict between flash cfg and instruction fetch by cache
    lega_flash_alg_f_delay(32);

    lega_flash_alg_abort_en();
    lega_flash_alg_check_abort_busy();

    FLASH->QSPI_CCR = 0x106;//IMODE=2'b01,INSTRUCTION=WREN
    lega_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    lega_flash_alg_polling_wel();

    FLASH->QSPI_CCR = cmd;
    if(CHIP_ERASE_CMD != cmd)
    {
        FLASH->QSPI_AR = adr;
    }
    lega_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    lega_flash_alg_clr_flg();
    lega_flash_alg_polling_wip();
    lega_flash_alg_f_delay(10);
    lega_flash_alg_clr_flg();
    lega_flash_alg_load_cfg();

    //flush cache after flash operation
    lega_flash_alg_cache_flush();
    /* Add your Code */
    return (0);                                  // Finished without Errors
}

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

FLASH_DRIVER_SEG int lega_flash_alg_programpage(unsigned long adr, unsigned long sz, unsigned char *buf) {
    unsigned long sz_temp;
    //volatile int cycle_count = 0;
    int var_rdata = 0;
    int fthres = 8;
    int cnt = 0;

    if((0 == sz) || (NULL == buf))
    {
        return 1;
    }
    sz_temp = sz - 1;

    sz = (sz + 3) & ~3;                           // Adjust size for Words

    //add delay to avoid confilict between flash cfg and instruction fetch by cache
    lega_flash_alg_f_delay(32);

    lega_flash_alg_abort_en();
    lega_flash_alg_check_abort_busy();

    FLASH->QSPI_CCR = 0x106;//IMODE=2'b01,INSTRUCTION=WREN
    lega_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    lega_flash_alg_polling_wel();

    FLASH->QSPI_CCR = 0x1002502;//fmode=2'b00,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b10,admode=2'b01,imode=2'b01,instruction=8'h02;
    FLASH->QSPI_DLR = sz_temp;
    FLASH->QSPI_AR = adr;
    while ((sz>0)&&(cnt<fthres)) {
        var_rdata = FLASH->QSPI_SR;
        var_rdata = var_rdata >>8 & 0x3F;
        if(var_rdata < fthres) {
            FLASH->QSPI_DR = *((unsigned long *)buf);        // Program Word
            cnt += 1;
            buf += 4;
            sz  -= 4;
        }
    }
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    FLASH->SBUS_START = 0x01; //WRITE
    lega_flash_alg_f_delay(10);
    while (sz) {
        var_rdata = FLASH->QSPI_SR;
        var_rdata = var_rdata >>8 & 0x3F;
        if(var_rdata < fthres) {
            FLASH->QSPI_DR = *((unsigned long *)buf);        // Program Word
            buf += 4;
            sz  -= 4;
        }
    }
    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    lega_flash_alg_polling_wip();
    lega_flash_alg_load_cfg ();

    //flush cache after flash operation
    lega_flash_alg_cache_flush();
    return (0);                                  // Finished without Errors
}

FLASH_DRIVER_SEG void lega_flash_alg_read_buf_clr(void)
{
#if 0
    unsigned int tmp = REG_RD(FLASH_BASE);
    REG_WR(FLASH_BASE, tmp & (~0x1));
    REG_WR(FLASH_BASE, tmp);
#else
    unsigned int tmp = FLASH->QSPI_CR;
    FLASH->QSPI_CR = tmp & (~0x1);
    FLASH->QSPI_CR = tmp;
#endif
}

#ifdef _SPI_FLASH_120MHz_
FLASH_DRIVER_SEG void lega_flash_alg_set_clk_120(unsigned char en_120m)
{
    if (en_120m)
        REG_WR(CLK_FLASH_SEL,FLASH_120_CLK);
    else
        REG_WR(CLK_FLASH_SEL,FLASH_52_CLK);
}
#endif

#ifdef _SPI_FLASH_240MHz_
FLASH_DRIVER_SEG int lega_flash_alg_read_id(void){
    int var_rdata;
    int dshift;
    lega_flash_alg_abort_en();
    lega_flash_alg_check_abort_busy();

    FLASH->QSPI_CCR = 0x500019F;//fmode=2'b01,dmode=2'b01,reserved=1'b0,dcyc=5'h0,absize=2'b00,abmode=2'b00,adsize=2'b00,admode=2'b00,imode=2'b01,instruction=8'h9F;
    FLASH->QSPI_DLR = 0x2;
    lega_flash_alg_clr_flg();
    FLASH->SBUS_START = 0x1;

    lega_flash_alg_f_delay(10);
    lega_flash_alg_check_busy();
    var_rdata = FLASH->QSPI_CR;
    dshift = (var_rdata>>13)&0x7;
    lega_flash_alg_f_delay(dshift);

    var_rdata = FLASH->QSPI_DR;

    lega_flash_alg_load_cfg();
    return(var_rdata);
}

FLASH_DRIVER_SEG void lega_flash_alg_calibrate_shift(int prescaler)
{
    volatile int delay_shift = 0;
    volatile int sshift = 0;
    unsigned int rdata;
    unsigned int wdata;
    unsigned int identi;
    volatile int id, mem_type, capacity;
    int i = 0;
    int sshift_max[8]={0};
    int sshift_min[8]={0};
    int match_flag[8]={0};
    int match_interval[8] = {0};

    //*(volatile int *)(0x40000808) = 0x2;
    REG_WR(CLK_FLASH_SEL,FLASH_240_CLK);

    for(delay_shift = 0; delay_shift < 8; delay_shift = delay_shift +1)
    {
        sshift_max[delay_shift]=0;
        sshift_min[delay_shift]=0;
        for(sshift = 0; sshift < prescaler; sshift = sshift +1)
        {
            rdata = FLASH->QSPI_CR;
            wdata = rdata&0x87FF1FFF;
            wdata |= (sshift&0xF) <<27;
            wdata |= (delay_shift&0x7) <<13;
            FLASH->QSPI_CR = wdata;
            //read_id
            identi = lega_flash_alg_read_id();

            id = identi&0xFF;
            mem_type = (identi&0xFF00)>>8;
            capacity = (identi&0xFF0000)>>16;

            if(id == 0x0B && mem_type == 0x40 && capacity == 0x15)
            {
                match_flag[delay_shift]=0x1;
                if(sshift_max[delay_shift] < sshift)
                    sshift_max[delay_shift] = sshift;
                if(sshift_min[delay_shift] > sshift)
                    sshift_min[delay_shift]=sshift;
            }
            else
            {
                sshift_min[delay_shift]=sshift + 1;
                if(sshift_min[delay_shift]==prescaler)
                {
                    sshift_min[delay_shift] = 0;
                }
            }
        }
    }

    for(i = 0;i< 8;i++)
    {
        if(match_flag[i] == 1)
            match_interval[i]= sshift_max[i]-sshift_min[i]+1;
        else{
            match_interval[i] = 0;
        }
    }
    //get the best delay_shift(means max mactch_interval
    delay_shift = 0;
    for(i = 1;i < 8;i = i+1)
    {
        if(match_interval[i] > match_interval[delay_shift])
        {
            delay_shift = i;
        }
    }
    sshift = (sshift_max[delay_shift] + sshift_min[delay_shift])/2 + (sshift_max[delay_shift] + sshift_min[delay_shift])%2;
    rdata = FLASH->QSPI_CR;
    wdata = rdata&0x87FF1FFF;
    wdata |= (sshift&0xF) <<27;
    wdata |= (delay_shift&0x7) <<13;
    FLASH->QSPI_CR = wdata;
}
#endif
#endif