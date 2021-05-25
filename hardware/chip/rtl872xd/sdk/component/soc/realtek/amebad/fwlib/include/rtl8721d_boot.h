#ifndef _HAL_8710B_BOOT_
#define _HAL_8710B_BOOT_

#if defined ( __ICCARM__ )
extern u8* __image2_entry_func__;
extern u8* __image1_bss_start__;
extern u8* __image1_bss_end__;

extern u8* __bss_start__;
extern u8* __bss_end__;

extern u8* __cmd_table_start__;
extern u8* __cmd_table_end__;
extern u8* __psram_bss_start__;
extern u8* __psram_bss_end__;
extern u8* __ram_nocache_start__;
extern u8* __ram_nocache_end__;
extern u8* __image3_bss_start__;
extern u8* __image3_bss_end__;
#else
extern u8 __image1_validate_code__[];
extern u8 __image1_bss_start__[];
extern u8 __image1_bss_end__[];

extern u8 __image2_entry_func__[];
extern u8 __bss_start__[];
extern u8 __bss_end__[];

extern u8 __cmd_table_start__[];
extern u8 __cmd_table_end__[];
extern u8 __psram_bss_start__[];
extern u8 __psram_bss_end__[];
extern u8 __ram_nocache_start__[];
extern u8 __ram_nocache_end__[];
extern u8 __image3_bss_start__[];
extern u8 __image3_bss_end__[];
#endif
extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __rom_bss_start_s__[];
extern u8 __rom_bss_end_s__[];
extern u8 __rom_bss_start_ns__[];
extern u8 __rom_bss_end_ns__[];
extern u8 __ram_image3_start__[];
extern u8 __ram_image3_end__[];

extern u8 __flash_text_start__[];
extern u8 __flash_img2_end__[];
extern u8 __flash_sec_text_start__[];
extern u8 __ram_start_table_start__[];
extern u8 __rom_top_4k_start_[];

extern u8  __rom_entry_ns_start__[];

extern u8 __retention_entry_func__[];

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
	u32 boot_ram_end;
} BOOT_EXPORT_SYMB_TABLE;

/* security boot */
typedef struct {
	int (*ed25519_verify_signature)(const unsigned char sig[],
		const unsigned char *m, unsigned long long mlen,
		const unsigned char pk[]);
	void (*clear_ns_rom_bss)(void);
} ROM_SECURE_CALL_NS_ENTRY;

enum _CPU_PWRSEQ_ {
	CPU_PWRSEQ_CMD_READ = 0xFFFFFF00,
	CPU_PWRSEQ_CMD_WRITE  = 0xFFFFFF01,
	CPU_PWRSEQ_CMD_POLLING   = 0xFFFFFF02,
	CPU_PWRSEQ_CMD_DELAY   = 0xFFFFFF03,
	CPU_PWRSEQ_CMD_LOGE   = 0xFFFFFF08,
	CPU_PWRSEQ_CMD_END   = 0xFFFFFFFF,
};

typedef struct {
	u32 address;
	u32 cmd; /* read/write/polling/delay/end */
	u32 bitsc; /* bit mask clear or polling target */
	u32 bitss; /* bit mask set or polling mask */
} CPU_PWR_SEQ;

typedef enum {
	IMG_LS_BOOT = 0,
	IMG_BACKUP = 1,
	IMG_SYSDATA =  2,
	IMG_HS_BOOT = 3,
	IMG_LS_IMG2_OTA1 = 4, 
	IMG_HS_IMG_COMB_OTA1 = 5, 
	IMG_LS_IMG2_OTA2 = 6, 
	IMG_HS_IMG_COMB_OTA2 = 7,
} Image_Type;

typedef struct {
	u32 VAddrStart;
	u32 VAddrEnd;
	u32 PAddrStart;
	u32 PAddrEnd;
} MMU_ConfDef;

#define OTA_INDEX_1			0
#define OTA_INDEX_2			1

typedef struct {
	u32 MaskAddr; /*start address for RSIP Mask, should be 4KB aligned*/
	u16 MaskSize; /*size of the mask area, unit is 4KB */
} RSIP_MaskDef;

typedef u8 (*FuncPtr)(void);

extern void vPortSVCHandler( void );
//extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
#if defined (__ICCARM__)
extern void xPortPendSVHandler( void ); 
#elif defined (__GNUC__)
extern void xPortPendSVHandler( void ) __attribute__ (( naked ));
#endif

extern u32 BOOT_ROM_CM4PON(u32 pwr_cmd_addr);
extern void BOOT_FLASH_Image1(void);
extern void BOOT_FLASH_WakeFromPG(void);
extern void BOOT_RAM_FuncEnable(void);
extern u32 BOOT_RAM_FLASH_Calibration(u8 read_mode);
extern PRAM_START_FUNCTION BOOT_RAM_SectionInit(void);

extern u32 ROM_SIM_ENABLE;
extern u32 IS_FPGA_VERIF; /* set in boot flash, based on MACRO, can not be used in ROM code */
extern RAM_START_FUNCTION Img2EntryFun0; //RamWakeupFun
#endif   //_HAL_8710B_BOOT_
