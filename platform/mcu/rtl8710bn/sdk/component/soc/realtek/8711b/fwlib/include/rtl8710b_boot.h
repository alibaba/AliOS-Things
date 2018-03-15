#ifndef _HAL_8710B_BOOT_
#define _HAL_8710B_BOOT_

#if defined ( __ICCARM__ )
extern u8* __image2_entry_func__;
extern u8* __image2_validate_code__;
extern u8* __image1_bss_start__;
extern u8* __image1_bss_end__;
#else
extern u8 __image1_validate_code__[];
extern u8 __image1_bss_start__[];
extern u8 __image1_bss_end__[];

extern u8 __image2_entry_func__[];
extern u8 __image2_validate_code__[];
extern u8 __bss_start__[];
extern u8 __bss_end__[];
extern u8 heap_start[];
extern u8 heap_end[];
extern u8 heap_len[];

#endif
extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __flash_text_start__[];
extern u8 __ram_start_table_start__[];
extern u8 __rom_top_4k_start_[];

enum _BOOT_TYPE_ {
	BOOT_FROM_FLASH = 0,
	BOOT_FROM_SDIO  = 1,        
	BOOT_FROM_USB   = 2,
	BOOT_FROM_UART0   = 3,
	BOOT_FROM_UART1   = 4,
	BOOT_FROM_SPI   = 5,
	BOOT_FROM_RSVD  = 6,    
};

typedef struct {
	u32 (*rdp_valid)(void);
	int (*swd_off)(void);
	int (*boot_system_init1)(void);
	int (*boot_system_init2)(void);
	u32 boot_ram_end;
} BOOT_EXPORT_SYMB_TABLE;

_LONG_CALL_ VOID SPI_Boot_ROM(VOID);
_LONG_CALL_ VOID UARTIMG_Download(u8 uart_idx);
_LONG_CALL_ VOID USB_Boot_ROM(VOID);
_LONG_CALL_ VOID SDIO_Boot_Up(VOID);
_LONG_CALL_ VOID BOOT_ROM_ShowBuildInfo(u32 Complete);

#ifdef CONFIG_KERNEL
extern void vPortSVCHandler( void );
//extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
#if defined (__ICCARM__)
extern void xPortPendSVHandler( void ); 
#elif defined (__GNUC__)
extern void xPortPendSVHandler( void ) __attribute__ (( naked ));
#endif
#endif

extern void BOOT_FLASH_Image1(void);
extern u32 BOOT_FLASH_RDP_VALID(void);
extern BOOT_EXPORT_SYMB_TABLE boot_export_symbol;
#endif   //_HAL_8710B_BOOT_
