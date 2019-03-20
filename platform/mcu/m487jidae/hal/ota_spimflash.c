#if defined( USE_OTA_SPIM_FLASH)

#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "hal/hal.h"

#define USE_4_BYTES_MODE            0            /* W25Q20 does not support 4-bytes address mode. */

#define TEST_BLOCK_ADDR             0x00000000   /* Test block address on SPI flash. */
#define ROUND_DOWN(a,b)     (((a) / (b)) * (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define FLASH_PAGE_SIZE         4096

static void spim_init ( void )
{
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Init SPIM multi-function pins, MOSI(PC.0), MISO(PC.1), CLK(PC.2), SS(PC.3), D3(PC.4), and D2(PC.5) */
    SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk | SYS_GPC_MFPL_PC2MFP_Msk |
                       SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC4MFP_Msk | SYS_GPC_MFPL_PC5MFP_Msk);
    SYS->GPC_MFPL |= SYS_GPC_MFPL_PC0MFP_SPIM_MOSI | SYS_GPC_MFPL_PC1MFP_SPIM_MISO |
                     SYS_GPC_MFPL_PC2MFP_SPIM_CLK | SYS_GPC_MFPL_PC3MFP_SPIM_SS |
                     SYS_GPC_MFPL_PC4MFP_SPIM_D3 | SYS_GPC_MFPL_PC5MFP_SPIM_D2;
    PC->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;

    /* Set SPIM I/O pins as high slew rate up to 80 MHz. */
    PC->SLEWCTL = (PC->SLEWCTL & 0xFFFFF000) |
                  (0x1<<GPIO_SLEWCTL_HSREN0_Pos) | (0x1<<GPIO_SLEWCTL_HSREN1_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN2_Pos) | (0x1<<GPIO_SLEWCTL_HSREN3_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN4_Pos) | (0x1<<GPIO_SLEWCTL_HSREN5_Pos);
}

void spim_flash_init (void)
{
        uint8_t     idBuf[3];
        spim_init ( );
    
        SYS_UnlockReg();                   /* Unlock register lock protect */

    SPIM_SET_CLOCK_DIVIDER(1);        /* Set SPIM clock as HCLK divided by 2 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(0);    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
    SPIM_SET_RXCLKDLY_RDEDGE();       /* Use SPI input clock rising edge to sample received data. */

    SPIM_SET_DCNUM(8);                /* Set 8 dummy cycle. */
    
    if (SPIM_InitFlash(1) != 0)        /* Initialized SPI flash */
    {
        printf("SPIM flash initialize failed!\n");
        goto lexit;
    }
        
    SPIM_ReadJedecId(idBuf, sizeof (idBuf), 1);
    printf("SPIM get JEDEC ID=0x%02X, 0x%02X, 0x%02X\n", idBuf[0], idBuf[1], idBuf[2]);

    SPIM_Enable_4Bytes_Mode(USE_4_BYTES_MODE, 1);


lexit:
    SYS_LockReg();                     /* Lock protected registers */
    return;
}

static int spim_flash_update(uint32_t dst_addr, const void *data, uint32_t size)
{
    int ret = 0;
    int remaining = size;
    uint8_t *src_addr = (uint8_t *) data;
    uint32_t fl_addr = 0;
    int fl_offset = 0;
    int len = 0;
    uint64_t *page_cache = NULL;
    
        //printf("[%s] dst_addr=%x, buf at %x, length is %d\n", __func__, dst_addr, data ,size );

        page_cache = (uint64_t *)krhino_mm_alloc(FLASH_PAGE_SIZE);
        if (page_cache == NULL)
                return -1;

        memset(page_cache, 0xFF, FLASH_PAGE_SIZE);

    do {
        fl_addr = ROUND_DOWN(dst_addr, FLASH_PAGE_SIZE);
        fl_offset = dst_addr - fl_addr;
        len = MIN(FLASH_PAGE_SIZE - fl_offset, remaining);

                /* Load from the flash into the cache */
                //printf("Read %x to %x\n", fl_addr, page_cache);
                SPIM_IO_Read (fl_addr, USE_4_BYTES_MODE, FLASH_PAGE_SIZE, (uint8_t*)&page_cache[0], OPCODE_FAST_READ, 1, 1, 1, 1);
                /* Erase the page, and write the cache */

                //printf("Erasing %x\n", fl_addr);
                SPIM_EraseBlock(fl_addr, USE_4_BYTES_MODE, OPCODE_SE_4K, 1, 1);

        /* Update the cache from the source */
        memcpy((uint8_t *)page_cache + fl_offset, src_addr, len);

                SPIM_IO_Write   (fl_addr, USE_4_BYTES_MODE, FLASH_PAGE_SIZE, (uint8_t*)&page_cache[0], OPCODE_PP, 1, 1, 1);
                
        dst_addr += len;
        src_addr += len;
        remaining -= len;
                
    } while ( (remaining > 0));

    krhino_mm_free(page_cache);

    return ret;
}

int spim_flash_erase_write (volatile uint32_t* off_set, uint8_t* in_buf ,uint32_t in_buf_len)
{
    int ret=0;
    //printf("[%s] offset=%x, buf at %x, length is %d\n", __func__, *off_set, in_buf ,in_buf_len );
    if ( off_set && in_buf )
    {
        ret = spim_flash_update(*off_set, in_buf, in_buf_len);
        *off_set += in_buf_len;
    }
    return ret;
}

int spim_flash_read (volatile uint32_t* off_set, uint8_t* out_buf, uint32_t out_buf_len)
{
    if ( off_set && out_buf )
    {
        SPIM_IO_Read (*off_set, USE_4_BYTES_MODE, out_buf_len, out_buf, OPCODE_FAST_READ, 1, 1, 1, 1);
        *off_set += out_buf_len;
    }
    return 0;
}

#endif
