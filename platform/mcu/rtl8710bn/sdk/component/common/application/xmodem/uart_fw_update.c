/********************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "xmport_uart.h"
#include "xmport_loguart.h"
#include "rtl8195a.h"
#include "xmodem.h"
#include "xmport_uart.h"
#include "hal_spi_flash.h"
#include "rtl8195a_spi_flash.h"
#include <platform/platform_stdlib.h>

enum {
  XMODEM_UART_0     = 0,   
  XMODEM_UART_1     = 1,   
  XMODEM_UART_2     = 2,   
  XMODEM_LOG_UART   = 3   
};

FWU_DATA_SECTION char xMFrameBuf[XM_BUFFER_SIZE];
FWU_DATA_SECTION XMODEM_CTRL xMCtrl;

FWU_DATA_SECTION static u32 fw_img1_size;
FWU_DATA_SECTION static u32 fw_img2_size;
FWU_DATA_SECTION static u32 fw_img2_addr;
FWU_DATA_SECTION static u32 fw_img3_size;
FWU_DATA_SECTION static u32 fw_img3_addr;
FWU_DATA_SECTION static u32 flash_wr_offset;
FWU_DATA_SECTION static u32 flash_erased_addr;
FWU_DATA_SECTION static u8  start_with_img1;
FWU_DATA_SECTION static u32 flash_wr_err_cnt;

FWU_DATA_SECTION HAL_RUART_ADAPTER xmodem_uart_adp; // we can dynamic allocate memory for this object to save memory
static union { uint32_t u; unsigned char c[4]; } file_checksum;
static u32 updated_img2_size = 0;
static u32 default_img2_addr = 0;

FWU_RODATA_SECTION const char Img2Signature[8]="81958711";
extern u32 SpicCalibrationPattern[4];
extern const u8 ROM_IMG1_VALID_PATTEN[];
extern HAL_RUART_ADAPTER *pxmodem_uart_adp;

#ifdef CONFIG_GPIO_EN
extern HAL_GPIO_ADAPTER gBoot_Gpio_Adapter;
extern PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;
#endif

extern BOOLEAN SpicFlashInitRtl8195A(u8 SpicBitMode);
_LONG_CALL_
extern VOID SpicWaitBusyDoneRtl8195A(VOID);
extern VOID SpicWaitWipDoneRefinedRtl8195A(SPIC_INIT_PARA SpicInitPara);


FWU_TEXT_SECTION void FWU_WriteWord(u32 Addr, u32 FData)
{
    SPIC_INIT_PARA SpicInitPara;
    
    HAL_WRITE32(SPI_FLASH_BASE, Addr, FData);
    // Wait spic busy done
    SpicWaitBusyDoneRtl8195A();
    // Wait flash busy done (wip=0)
    SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);
}

FWU_TEXT_SECTION u32 xModem_MemCmp(const u32 *av, const u32 *bv, u32 len)
{
    const u32 *a = av;
    const u32 *b = (u32*)((u8*)bv+SPI_FLASH_BASE);
    u32 len4b = len >> 2;
    u32 i;
    
    for (i=0; i<len4b; i++) {
        if (a[i] != b[i]) {
            DBG_MISC_ERR("OTU: Flash write check error @ 0x%08x\r\n", (u32)(&b[i]));
            return ((u32)(&b[i])); 
        }
    }
    return 0;
}

FWU_TEXT_SECTION
u32 xModem_Frame_Img2(char *ptr,  unsigned int frame_num, unsigned int frame_size)    
{
    u32 address;
    u32 ImageIndex=0;
    u32 rx_len=0;
    u32 *chk_sr;
    u32 *chk_dr;
    u32 err_addr;
    
    if (frame_num == 1) {
        // Parse Image2 header
        memset(&file_checksum, 0, sizeof(file_checksum));
        flash_wr_offset = fw_img2_addr;
        fw_img2_size = rtk_le32_to_cpu(*((u32*)ptr)) + 0x14;
        if ((fw_img2_size & 0x03) != 0) {
            DBG_MISC_ERR("xModem_Frame_ImgAll Err#2: fw_img2_addr=0x%x fw_img2_size(%d) isn't 4-bytes aligned\r\n", fw_img2_addr, fw_img2_size);
            fw_img1_size = 0;
            fw_img2_size = 0;
            return rx_len;
        }
        
        if (fw_img2_size > (2*1024*1024)) {
            DBG_MISC_ERR("xModem_Frame_ImgAll Image2 to Big: fw_img2_addr=0x%x fw_img2_size(%d) \r\n", fw_img2_addr, fw_img2_size);
            fw_img1_size = 0;
            fw_img2_size = 0;
            return rx_len;
        }
        fw_img3_addr = fw_img2_addr + fw_img2_size;
        updated_img2_size = fw_img2_size;
        
        // erase Flash first
        address = fw_img2_addr & (~0xfff);     // 4k aligned, 4k is the page size of flash memory
        while ((address) < (fw_img2_addr+fw_img2_size)) {
            SpicSectorEraseFlashRtl8195A(SPI_FLASH_BASE + address);
            address += 0x1000;
        }
        flash_erased_addr = address;
    }

    if (fw_img2_size > 0) {
        // writing image2
        chk_sr = (u32*)((u8*)ptr+ImageIndex);
        chk_dr = (u32*)flash_wr_offset;
        while (ImageIndex < frame_size) {
            FWU_WriteWord(flash_wr_offset, (*((u32*)(ptr+ImageIndex))));
            ImageIndex += 4;
            flash_wr_offset += 4;
            rx_len += 4;
            fw_img2_size -= 4;
            if (fw_img2_size == 0) {
                // Image2 write done,
                break;
            }
        }
					        
        err_addr = xModem_MemCmp(chk_sr, chk_dr, (flash_wr_offset - (u32)chk_dr));
        if (err_addr) {
            flash_wr_err_cnt++;
        }
    }

    // checksum attached at file end
	file_checksum.c[0] = ptr[rx_len - 4];      
	file_checksum.c[1] = ptr[rx_len - 3];
	file_checksum.c[2] = ptr[rx_len - 2];
	file_checksum.c[3] = ptr[rx_len - 1];
	
    return rx_len;
}

FWU_TEXT_SECTION
s32
xModem_Init_UART_Port(u8 uart_idx, u8 pin_mux, u32 baud_rate)
{
    if (uart_idx <= XMODEM_UART_2) {
        // update firmware via generic UART
        pxmodem_uart_adp = &xmodem_uart_adp;    // we can use dynamic allocate to save memory
        xmodem_uart_init(uart_idx, pin_mux, baud_rate);
        xmodem_uart_func_hook(&(xMCtrl.ComPort));
    } else if(uart_idx == XMODEM_LOG_UART) {
        // update firmware via Log UART
        xmodem_loguart_init(baud_rate);
        xmodem_loguart_func_hook(&(xMCtrl.ComPort));    
    } else {
        // invalid UART port
		DBG_MISC_ERR("xModem_Init_UART_Port: Invaild UART port(%d)\n", uart_idx);
        return -1;
    }

    return 0;
}

FWU_TEXT_SECTION
VOID
xModem_DeInit_UART_Port(u8 uart_idx)
{
    if (uart_idx <= XMODEM_UART_2) {    
        xmodem_uart_deinit();
    } else if (uart_idx == XMODEM_LOG_UART) {
        xmodem_loguart_deinit();
    }
}

FWU_TEXT_SECTION
__weak s32
UpdatedImg2AddrValidate(
    u32 Image2Addr,
    u32 DefImage2Addr,
    u32 DefImage2Size
)
{
    if (Image2Addr == 0xffffffff) {
        // Upgraded Image2 isn't exist
        return 0;   // invalid address
    }

    if ((Image2Addr & 0xfff) != 0) {
        // Not 4K aligned
        return 0;   // invalid address
    }

    if (Image2Addr <= DefImage2Addr) {
        // Updated image2 address must bigger than the addrss of default image2
        return 0;   // invalid address
    }

    if (Image2Addr < (DefImage2Addr+DefImage2Size)) {
        // Updated image2 overlap with the default image2
        return 0;   // invalid address
    }

    return 1;   // this address is valid    
}

FWU_TEXT_SECTION
VOID
WriteImg2Sign(
    u32 Image2Addr
)
{
    u32 img2_sig[2];
    
    _memcpy((void*)img2_sig, (void*)Img2Signature, 8);    
    FWU_WriteWord((Image2Addr + 8), img2_sig[0]);
    FWU_WriteWord((Image2Addr + 12), img2_sig[1]);
    
    // set the default imag2's signature to old
    if(default_img2_addr != Image2Addr)
    {
        printf("set the signature of default img2 to old\n");
        FWU_WriteWord((default_img2_addr + 8), 0x35393130);
        FWU_WriteWord((default_img2_addr + 12), 0x31313738);
    }    

}

static void xmodem_write_ota_addr_to_system_data(u32 newImg2Addr)
{
    FWU_WriteWord(FLASH_SYSTEM_DATA_ADDR, newImg2Addr);
	return;
}

FWU_TEXT_SECTION
u32
SelectImg2ToUpdate(
    u32 *OldImg2Addr
)
{
    u32 DefImage2Addr=0xFFFFFFFF;  // the default Image2 addr.
    u32 SecImage2Addr=0xFFFFFFFF;  // the 2nd image2 addr.
    u32 ATSCAddr=0xFFFFFFFF; 
    u32 UpdImage2Addr;  // the addr of the image2 to be updated
    u32 DefImage2Len;
    
    *OldImg2Addr = 0;
    DefImage2Addr = (HAL_READ32(SPI_FLASH_BASE, 0x18)&0xFFFF) * 1024;
    if ((DefImage2Addr != 0) && ((DefImage2Addr < (16*1024*1024)))) {
        // Valid Default Image2 Addr: != 0 & located in 16M
        DefImage2Len = HAL_READ32(SPI_FLASH_BASE, DefImage2Addr);
        default_img2_addr = DefImage2Addr;

        // Get the pointer of the upgraded Image2
        SecImage2Addr = HAL_READ32(SPI_FLASH_BASE, FLASH_SYSTEM_DATA_ADDR);

        if (UpdatedImg2AddrValidate(SecImage2Addr, DefImage2Addr, DefImage2Len)) {
            UpdImage2Addr = SecImage2Addr; // Update the 2nd image2
        } else {
            // The upgraded image2 isn't exist or invalid so we can just update the default image2
            //UpdImage2Addr = DefImage2Addr; // Update the default image2
            UpdImage2Addr = 0x80000; // Update to a predefined address
       }
    } else {
        UpdImage2Addr = 0;
    }
    xmodem_write_ota_addr_to_system_data(UpdImage2Addr);

    return UpdImage2Addr;
}

static uint32_t xmodem_get_flash_checksum()
{
    uint32_t flash_checksum = 0;
    
    if(updated_img2_size == 0)
    {
        printf("img2 size is wrong\n");
        return 0;
    }

    for(int i = 0; i < updated_img2_size - 4; i++)
        flash_checksum += HAL_READ8(SPI_FLASH_BASE, fw_img2_addr + i); 
    	 
    return flash_checksum; 
}

FWU_TEXT_SECTION
void OTU_FW_Update(u8 uart_idx, u8 pin_mux, u32 baud_rate)
{
    u32 wr_len;
    u32 OldImage2Addr=0;  // the addr of the image2 will become old one
    SPIC_INIT_PARA SpicInitPara;

    fw_img1_size = 0;
    fw_img2_size = 0;
    fw_img2_addr = 0;
    fw_img3_size = 0;
    fw_img3_addr = 0;
    flash_wr_offset = 0;
    flash_erased_addr = 0;
    start_with_img1 = 0;;
    flash_wr_err_cnt = 0;
    u32 flash_checksum = 0;

    // Get the address of the image2 to be updated
	SPI_FLASH_PIN_FCTRL(ON);
	if (!SpicFlashInitRtl8195A(SpicOneBitMode)){
        SPI_FLASH_PIN_FCTRL(OFF);    
		DBG_MISC_ERR("OTU_FW_Update: SPI Init Fail!!!!!!\n");
        return;
	}
	SpicWaitWipDoneRefinedRtl8195A(SpicInitPara);

    printf("FW Update Over UART%d, PinMux=%d, Baud=%d\r\n", uart_idx, pin_mux, baud_rate);
    fw_img2_addr = SelectImg2ToUpdate(&OldImage2Addr);
    // Start to update the Image2 through xModem on peripheral device
    printf("FW Update Image2 @ 0x%x\r\n", fw_img2_addr);
    // We update the image via xModem on UART now, if we want to uase other peripheral device
    // to update the image then we need to redefine the API
    if (xModem_Init_UART_Port(uart_idx, pin_mux, baud_rate) < 0) {
        return;
    }

    xModemStart(&xMCtrl, xMFrameBuf, xModem_Frame_Img2);    // Support Image format: Image2 only
    wr_len = xModemRxBuffer(&xMCtrl, (2*1024*1024));
    xModemEnd(&xMCtrl);

    xModem_DeInit_UART_Port(uart_idx);
    
    // add checksum check
    
    flash_checksum = xmodem_get_flash_checksum();
    printf("flash_checksum: %x file_checksum: %x\n", flash_checksum, file_checksum.u);
    
    if(flash_checksum != file_checksum.u)
        printf("checksum error, please retry to update\n");
    else 
    {
        if ((wr_len > 0) && (flash_wr_err_cnt == 0)) {
            // Firmware update OK, now write the signature to active this image
            WriteImg2Sign(fw_img2_addr);        
        }
        else
            printf("error in writen to flash");
    }
        
    printf("OTU_FW_Update Done, Write Len=%d\n", wr_len);
    SPI_FLASH_PIN_FCTRL(OFF);    
}

