ENTRY(Reset_Handler)

MEMORY
{
    IROM (rx) : 			ORIGIN = 0x10100000, LENGTH = 0x1010A000 - 0x10100000	/* ROM: 40k */
    IROM_NS (rx) : 			ORIGIN = 0x1010A000, LENGTH = 0x10140000 - 0x1010A000	/* ROM: 216k */
    DROM_NS (rx) : 			ORIGIN = 0x101C0000, LENGTH = 0x101D4000 - 0x101C0000	/* ROM: 80k */
    DROM (rx) : 			ORIGIN = 0x101D4000, LENGTH = 0x101D8000 - 0x101D4000	/* ROM: 16k */
    ROMBSS_RAM_COM (rw)  : 		ORIGIN = 0x10000000, LENGTH = 0x10001000 - 0x10000000	/* ROM BSS COMMON(S & NS both used) RAM: 4K */
    ROMBSS_RAM_NS (rw)  : 		ORIGIN = 0x10001000, LENGTH = 0x10002000 - 0x10001000	/* ROM BSS NS RAM: 4K */
    RSVD_RAM_NS (rw)  : 		ORIGIN = 0x10002000, LENGTH = 0x10004000 - 0x10002000	/* RSVD RAM: 8K */
    MSP_RAM_NS (rw)  : 		ORIGIN = 0x10004000, LENGTH = 0x10005000 - 0x10004000	/* MSP_NS RAM: 4K */

    BD_RAM_NS (rwx)  : 		ORIGIN = 0x10005000, LENGTH = 0x1007C000 - 0x10005000	/* MAIN RAM NS: 456K */
    ROMBSS_RAM_S (rwx)  : 		ORIGIN = 0x1007C000, LENGTH = 0x1007D000 - 0x1007C000	/* ROM BSS RAM S: 4K */
    BOOTLOADER_RAM_S (rwx)  : 	ORIGIN = 0x1007D000, LENGTH = 0x1007F000 - 0x1007D000	/* BOOT Loader RAM: 8K */
    MSP_RAM_S (rwx)  : 		ORIGIN = 0x1007F000, LENGTH = 0x10080000 - 0x1007F000	/* MSP_S RAM: 4k */
    EXTENTION_SRAM (rwx)  : 		ORIGIN = 0x100E0000, LENGTH = 0x10100000 - 0x100E0000	/* EXTENTION SRAM: 128k */

    PSRAM_NS (rwx)  : 		ORIGIN = 0x02000000 + 0x20, LENGTH = 0x02400000 - 0x02000000 - 0x20	/* PSRAM_NS: 4M */

    /* Flash */
    KM0_BOOT (rx)  :		ORIGIN = 0x08000000+0x20, LENGTH = 0x02000-0x20	/* XIPBOOT: 8k, 32 Bytes resvd for header*/
    BACKUP (r) :			ORIGIN = 0x08002000, LENGTH = 0x1000	/* BACKUP: 4K system data in flash */
    XIPSYS (r) :			ORIGIN = 0x08003000, LENGTH = 0x1000	/* XIPSYS: 4K system data in flash */
    KM4_BOOT (rx)  :		ORIGIN = 0x08004000+0x20, LENGTH = 0x02000-0x20	/* XIPBOOT: 8k, 32 Bytes resvd for header*/
    KM0_IMG2 (rx)  : 		ORIGIN = 0x0C000000+0x20, LENGTH = 0x02000000-0x20	/* KM0_IMG2: 32MB, 32 Bytes resvd for header, virtual address */
    KM4_IMG2 (rx)  : 		ORIGIN = 0x0E000000+0x20, LENGTH = 0x02000000-0x20	/* KM4_IMG2 OTA1: 32MB, 32 Bytes resvd for header,  virtual address */
    BTRACE (rx) :			ORIGIN = 0x00800000, LENGTH = 0x00C00000 -0x00800000	/* Bluetooth Trace */

    /* KM0 RAM*/
    KM0_SRAM (rwx)  : 		ORIGIN = 0x00080000, LENGTH = 0x00090000 - 0x00080000	/* KM0 SRAM: 64k */
    RETENTION_RAM (rwx)  : 		ORIGIN = 0x000C0000, LENGTH = 0x000C0400 - 0x000C0000	/* KM0 Retention SRAM: 1k */
}

SECTIONS
{
    .rom.text : { } > IROM_NS
    .rom.rodata : { } > DROM_NS
    .hal.rom.bss : { } > ROMBSS_RAM_COM
    .hal.ns_rom.bss : { } > ROMBSS_RAM_NS

    /* image2: normal image start */
    .ram_image2.entry :
    {
        __ram_image2_text_start__ = .;
        __image2_entry_func__ = .;
        KEEP(*(SORT(.image2.entry.data*)))

        __image2_validate_code__ = .;
        KEEP(*(.image2.validate.rodata*))

    } > BD_RAM_NS

    .ram_image2.text :
    {
        __ram_text_start__ = .;
        *(.image2.ram.text*)
        *(.image2.net.ram.text*)
        __ram_text_end__ = .;
    } > BD_RAM_NS

    .ram_image2.data :
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
        __ram_image2_text_end__ = .;
        . = ALIGN(16);
    } > BD_RAM_NS

    .ram_image2.bss :
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > BD_RAM_NS

    .ram_image2.nocache.data :
    {
        . = ALIGN (32);
        __ram_nocache_start__ = .;
        *(.bdsram.data*)
        . = ALIGN (32);
        __ram_nocache_end__ = .;
    } > BD_RAM_NS

    .ram_heap.data :
    {
        . = ALIGN (32);
        *(.bfsram.data*)
        end = .;
        *(.heap.stdlib*)
        . = . + 4096;
        __bfsram_end__ = .;
    } > BD_RAM_NS

    . = ALIGN(8);
    PROVIDE(heap_start        = .);
    PROVIDE(heap_end          = 0x1007A000);
    PROVIDE(heap_len          = heap_end - heap_start);

    .xip_image2.text :
    {
        __flash_text_start__ = .;

        *(.img2_custom_signature*)

        *(.text*)
    /*	*(.image2.net.ram.text*) */
        *(.rodata*)

        /* Add This for C++ support */
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP(*(.preinit_array))
        __preinit_array_end = .;
        . = ALIGN(4);
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
        . = ALIGN(4);
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
        /*-----------------*/

        . = ALIGN (4);
        __cmd_table_start__ = .;
        KEEP(*(.cmd.table.data*))
        __cmd_table_end__ = .;

    /************** added drivers **************/
    _cli_region_begin = .;
    KEEP(*(CliRegion))
    . = ALIGN(4);
    _cli_region_end = .;

    __core_driver_start__ = .;
    KEEP(*(.core_driver_entry))
    . = ALIGN(4);
    __core_driver_end__ = .;

    __bus_driver_start__ = .;
    KEEP(*(*.bus_driver_entry))
    __bus_driver_end__ = .;

    __early_driver_start__ = .;
    KEEP(*(*.early_driver_entry))
    __early_driver_end__ = .;

    __vfs_driver_start__ = .;
    KEEP(*(*.vfs_driver_entry))
    __vfs_driver_end__ = .;

    __level0_driver_start__ = .;
    KEEP(*(*.level0_driver_entry))
    __level0_driver_end__ = .;

    __level1_driver_start__ = .;
    KEEP(*(*.level1_driver_entry))
    __level1_driver_end__ = .;

    __level2_driver_start__ = .;
    KEEP(*(*.level2_driver_entry))
    __level2_driver_end__ = .;

    __level3_driver_start__ = .;
    KEEP(*(*.level3_driver_entry))
    __level3_driver_end__ = .;

    __post_driver_start__ = .;
    KEEP(*(*.post_driver_entry))
    __post_driver_end__ = .;
/************** end of drivers *********/

        __flash_text_end__ = .;

        . = ALIGN (16);
    } > KM4_IMG2

    /* Add This for C++ support */
    .ARM.extab  :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)

    } > KM4_IMG2

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > KM4_IMG2
    /*-----------------*/

    .bluetooth_trace.text :
    {
        __btrace_start__ = .;
        *(.BTTRACE)
        __btrace_end__ = .;
    } > BTRACE

    /* PSRAM_NS image start */
    .psram_image2.text :
    {
        __psram_image2_text_start__ = .;
        *(.psram.text*)
        __psram_image2_text_end__ = .;
    } > PSRAM_NS

    .psram_image2.data :
    {
        . = ALIGN (32);
        *(.psram.data*)
        . = ALIGN (32);
        *(.psram.rodata*)
    } > PSRAM_NS

    .psram_image2.bss :
    {
        . = ALIGN (32);
        __psram_bss_start__ = .;
        *(.psram.bss*)
        __psram_bss_end__ = .;
    } > PSRAM_NS

    .psram_heap.data :
    {
        . = ALIGN (32);
        *(.psram.heap*)
    } > PSRAM_NS
}

__sram_end__ = ORIGIN(BD_RAM_NS) + LENGTH(BD_RAM_NS);
SECTIONS
{
    __rom_bss_start__ = 0x10000000;
    ConfigDebugClose = 0x10000000;
    ConfigDebugBuffer = 0x10000004;
    ConfigDebugBufferGet = 0x10000008;
    ConfigDebug = 0x1000000c;
    rand_first = 0x1000001c;
    rand_seed = 0x10000020;
    RBSS_UDELAY_DIV = 0x10000034;
    flash_init_para = 0x10000038;
    GDMA_Reg = 0x10000098;
    PortA_IrqHandler = 0x1000009c;
    PortA_IrqData = 0x1000011c;
    PortB_IrqHandler = 0x1000019c;
    PortB_IrqData = 0x1000021c;
    IPC_IrqHandler = 0x1000029c;
    IPC_IrqData = 0x1000031c;
    IS_FPGA_VERIF = 0x1000039c;
    crypto_engine = 0x100003a0;
    __rom_bss_end__ = 0x10000564;
    __rom_bss_start_ns__ = 0x10001000;
    NewVectorTable = 0x10001000;
    UserIrqFunTable = 0x10001140;
    UserIrqDataTable = 0x10001240;
    mpu_entry_register = 0x10001340;
    p_rom_ssl_ram_map = 0x10001348;
    rom_ssl_ram_map = 0x1000134c;
    __rom_bss_end_ns__ = 0x1000138c;
    __rom_bss_start_s__ = 0x1007c000;
    __ram_start_table_start__ = 0x1007d000;
    STACK_TOP = 0x1007effc;
    __vectors_table = 0x1010a000;
    Reset_Handler = 0x1010a101;
    NMI_Handler = 0x1010a111;
    SecureFault_Handler = 0x1010a125;
    DiagVSprintf = 0x1010a12d;
    DiagPrintf = 0x1010a3f5;
    DiagPrintfD = 0x1010a445;
    DiagSPrintf = 0x1010a471;
    DiagSnPrintf = 0x1010a49d;
    Rand = 0x1010a765;
    Rand_Arc4 = 0x1010a7f1;
    RandBytes_Get = 0x1010a825;
    io_assert_failed = 0x1010a871;
    BKUP_Write = 0x1010a891;
    BKUP_Read = 0x1010a8b5;
    BKUP_Set = 0x1010a8d5;
    BKUP_Clear = 0x1010a8fd;
    BOOT_Reason = 0x1010a929;
    DelayNop = 0x1010a939;
    DelayUs = 0x1010a949;
    DelayMs = 0x1010a995;
    EFUSEPowerSwitch = 0x1010a9a9;
    EFUSERead8 = 0x1010aa31;
    EFUSEWrite8 = 0x1010ab1d;
    EFUSE_PG_Packet = 0x1010ac21;
    EFUSE_LogicalMap_Read = 0x1010ae99;
    EFUSE_LogicalMap_Write = 0x1010afad;
    FLASH_RxData = 0x1010b195;
    FLASH_TxCmd = 0x1010b29d;
    FLASH_SW_CS_Control = 0x1010b329;
    FLASH_SetSpiMode = 0x1010b37d;
    FLASH_RxCmd = 0x1010b415;
    FLASH_WaitBusy = 0x1010b479;
    FLASH_WriteEn = 0x1010b4f9;
    FLASH_TxData256B = 0x1010b51d;
    FLASH_TxData12B = 0x1010b60d;
    FLASH_SetStatus = 0x1010b6ed;
    FLASH_Erase = 0x1010b70d;
    FLASH_DeepPowerDown = 0x1010b7b9;
    FLASH_SetStatusBits = 0x1010b809;
    FLASH_StructInit_Micron = 0x1010b8dd;
    FLASH_StructInit_MXIC = 0x1010b98d;
    FLASH_StructInit_GD = 0x1010ba39;
    FLASH_StructInit = 0x1010baed;
    FLASH_Init = 0x1010bba1;
    GDMA_StructInit = 0x1010bc25;
    GDMA_SetLLP = 0x1010bc45;
    GDMA_ClearINTPendingBit = 0x1010bcd9;
    GDMA_ClearINT = 0x1010bda5;
    GDMA_INTConfig = 0x1010be75;
    GDMA_Cmd = 0x1010bf91;
    GDMA_Init = 0x1010bff9;
    GDMA_ChCleanAutoReload = 0x1010c14d;
    GDMA_SetSrcAddr = 0x1010c1d1;
    GDMA_GetSrcAddr = 0x1010c221;
    GDMA_GetDstAddr = 0x1010c26d;
    GDMA_SetDstAddr = 0x1010c2b9;
    GDMA_SetBlkSize = 0x1010c309;
    GDMA_GetBlkSize = 0x1010c369;
    GDMA_ChnlRegister = 0x1010c3b9;
    GDMA_ChnlUnRegister = 0x1010c401;
    GDMA_ChnlAlloc = 0x1010c43d;
    GDMA_ChnlFree = 0x1010c4d9;
    GDMA_GetIrqNum = 0x1010c565;
    GPIO_INTMode = 0x1010c5a9;
    GPIO_INTConfig = 0x1010c641;
    GPIO_INTHandler = 0x1010c679;
    GPIO_Direction = 0x1010c721;
    GPIO_Init = 0x1010c755;
    GPIO_DeInit = 0x1010c7e1;
    GPIO_ReadDataBit = 0x1010c84d;
    GPIO_WriteBit = 0x1010c871;
    GPIO_PortDirection = 0x1010c8a1;
    GPIO_PortRead = 0x1010c8c5;
    GPIO_PortWrite = 0x1010c8d5;
    GPIO_UserRegIrq = 0x1010c8ed;
    IPC_INTConfig = 0x1010c925;
    IPC_IERSet = 0x1010c939;
    IPC_IERGet = 0x1010c93d;
    IPC_INTRequest = 0x1010c941;
    IPC_INTClear = 0x1010c94d;
    IPC_INTGet = 0x1010c959;
    IPC_CPUID = 0x1010c95d;
    IPC_SEMGet = 0x1010c969;
    IPC_SEMFree = 0x1010c9b9;
    IPC_INTHandler = 0x1010ca21;
    IPC_INTUserHandler = 0x1010ca61;
    LOGUART_StructInit = 0x1010ca9d;
    LOGUART_Init = 0x1010cab5;
    LOGUART_PutChar = 0x1010caf5;
    LOGUART_GetChar = 0x1010cb29;
    LOGUART_Readable = 0x1010cb45;
    LOGUART_GetIMR = 0x1010cb59;
    LOGUART_SetIMR = 0x1010cb65;
    LOGUART_WaitBusy = 0x1010cb71;
    LOGUART_SetBaud = 0x1010cb91;
    mpu_enable = 0x1010cbd5;
    mpu_disable = 0x1010cbed;
    mpu_init = 0x1010cc01;
    mpu_set_mem_attr = 0x1010cc3d;
    mpu_region_cfg = 0x1010ccb1;
    mpu_entry_free = 0x1010cdb5;
    mpu_entry_alloc = 0x1010cdc1;
    RSIP_Cmd = 0x1010cde9;
    RSIP_OTF_init = 0x1010ce05;
    RSIP_OTF_Cmd = 0x1010ce6d;
    RSIP_OTF_Mask = 0x1010ce81;
    RSIP_KEY_Request = 0x1010cec9;
    RSIP_MMU_Config = 0x1010cf0d;
    RSIP_MMU_Cmd = 0x1010cf31;
    PAD_DrvStrength = 0x1010cf51;
    PAD_PullCtrl = 0x1010cf6d;
    PAD_CMD = 0x1010cf99;
    Pinmux_Config = 0x1010cfb9;
    Pinmux_ConfigGet = 0x1010cfd9;
    Pinmux_UartLogCtrl = 0x1010cfe9;
    Pinmux_SpicCtrl = 0x1010d03d;
    simulation_bit_index = 0x1010d181;
    simulation_stage_set = 0x1010d199;
    SYSTIMER_Init = 0x1010d1c5;
    SYSTIMER_TickGet = 0x1010d215;
    SYSTIMER_GetPassTime = 0x1010d225;
    RTIM_TimeBaseStructInit = 0x1010d255;
    RTIM_Cmd = 0x1010d26d;
    RTIM_GetCount = 0x1010d2f5;
    RTIM_INTConfig = 0x1010d361;
    RTIM_INTClear = 0x1010d3f1;
    RTIM_TimeBaseInit = 0x1010d45d;
    RTIM_DeInit = 0x1010d595;
    RTIM_INTClearPendingBit = 0x1010d611;
    RTIM_GetFlagStatus = 0x1010d685;
    RTIM_GetINTStatus = 0x1010d75d;
    UART_DeInit = 0x1010d83d;
    UART_StructInit = 0x1010d845;
    UART_BaudParaGetFull = 0x1010d861;
    UART_BaudParaGet = 0x1010d899;
    UART_SetBaud = 0x1010d8bd;
    UART_SetBaudExt = 0x1010d955;
    UART_SetRxLevel = 0x1010d9cd;
    UART_RxCmd = 0x1010d9f5;
    UART_Writable = 0x1010da09;
    UART_Readable = 0x1010da11;
    UART_CharPut = 0x1010da19;
    UART_CharGet = 0x1010da1d;
    UART_ReceiveData = 0x1010da25;
    UART_SendData = 0x1010da4d;
    UART_ReceiveDataTO = 0x1010da75;
    UART_SendDataTO = 0x1010dab5;
    UART_RxByteCntClear = 0x1010daf5;
    UART_RxByteCntGet = 0x1010db01;
    UART_BreakCtl = 0x1010db09;
    UART_ClearRxFifo = 0x1010db1d;
    UART_Init = 0x1010db3d;
    UART_ClearTxFifo = 0x1010dbe5;
    UART_INTConfig = 0x1010dbf1;
    UART_IntStatus = 0x1010dc01;
    UART_ModemStatusGet = 0x1010dc05;
    UART_LineStatusGet = 0x1010dc09;
    UART_WaitBusy = 0x1010dc0d;
    BOOT_ROM_SignatureCheck = 0x1010e1a9;
    BOOT_ROM_FromFlash = 0x1010e281;
    BOOT_ROM_InitDebugFlg = 0x1010e38d;
    BOOT_ROM_ResetVsr = 0x1010e3a9;
    EXT32K_Cmd = 0x1010e4c9;
    XTAL_ClkGet = 0x1010e4e9;
    CPU_ClkSet = 0x1010e501;
    CPU_ClkGet = 0x1010e515;
    clear_ns_rom_bss = 0x10110021;
    RCC_PeriphClockCmd = 0x10110039;
    RCC_PeriphClockSource_RTC = 0x101100d1;
    RCC_PeriphClockSource_I2C = 0x1011011d;
    RCC_PeriphClockSource_QDEC = 0x1011013d;
    RCC_PeriphClockSource_UART = 0x1011015d;
    SYSCFG_GetChipInfo = 0x101101d1;
    INT_HardFault = 0x101101dd;
    INT_MemManage = 0x101101fd;
    INT_BusFault = 0x1011021d;
    INT_UsageFault = 0x1011023d;
    INT_SecureFault = 0x1011025d;
    INT_HardFault_C = 0x1011027d;
    INT_NMI = 0x1011064d;
    irq_table_init = 0x10110795;
    irq_enable = 0x101109d1;
    irq_disable = 0x101109ed;
    irq_set_priority = 0x10110a11;
    irq_get_priority = 0x10110a3d;
    irq_set_pending = 0x10110a65;
    irq_get_pending = 0x10110a81;
    irq_clear_pending = 0x10110aa5;
    irq_register = 0x10110ac1;
    irq_unregister = 0x10110b0d;
    _char2num = 0x10110b2d;
    _2char2dec = 0x10110b5d;
    _2char2hex = 0x10110bd1;
    _memchr = 0x10110c39;
    _memcmp = 0x10110cc9;
    _memcpy = 0x10110d2d;
    _memmove = 0x10110dd9;
    _memset = 0x10110ea1;
    _vsscanf = 0x10110ffd;
    _sscanf = 0x101115c1;
    _stratoi = 0x101115e1;
    _strcat = 0x10111635;
    _strchr = 0x10111675;
    _strcmp = 0x10111745;
    _strcpy = 0x101117b9;
    _stricmp = 0x10111805;
    _strlen = 0x10111839;
    _strncat = 0x1011189d;
    _strncmp = 0x101118f9;
    _strncpy = 0x1011199d;
    _strnlen = 0x10111a05;
    _strpbrk = 0x10111a39;
    _strsep = 0x10111a65;
    _strstr = 0x10111d25;
    _strtoull = 0x10111f3d;
    _strtoll = 0x10111ffd;
    _strtok = 0x1011201d;
    __strtok_r = 0x10112029;
    _strtok_r = 0x1011208d;
    _strtol_r = 0x10112095;
    _strtol = 0x101121b1;
    _strtoul_r = 0x101121b5;
    _strtoul = 0x101122e9;
    _strupr = 0x101122ed;
    CRYPTO_chacha_20 = 0x10112309;
    rom_ed25519_gen_keypair = 0x101125c5;
    rom_ed25519_gen_signature = 0x101125c9;
    rom_ed25519_verify_signature = 0x101125df;
    rom_ed25519_ge_double_scalarmult_vartime = 0x10113e89;
    rom_ed25519_ge_frombytes_negate_vartime = 0x10114195;
    rom_ed25519_ge_p3_tobytes = 0x10114695;
    rom_ed25519_ge_scalarmult_base = 0x101146df;
    rom_ed25519_ge_tobytes = 0x10114807;
    rom_ed25519_crypto_sign_seed_keypair = 0x10114851;
    rom_ed25519_crypto_sign_verify_detached = 0x101148a9;
    rom_ed25519_sc_muladd = 0x10114ad5;
    rom_ed25519_sc_reduce = 0x1011b265;
    rom_ed25519_crypto_sign_detached = 0x1011c6dd;
    CRYPTO_poly1305_init = 0x1011ca99;
    CRYPTO_poly1305_update = 0x1011cb31;
    CRYPTO_poly1305_finish = 0x1011cba7;
    rom_sha512_starts = 0x1011cd4d;
    rom_sha512_update = 0x1011cd51;
    rom_sha512_finish = 0x1011cd55;
    rom_sha512 = 0x1011cd59;
    rom_sha512_hmac_starts = 0x1011cd5d;
    rom_sha512_hmac_update = 0x1011cdf1;
    rom_sha512_hmac_finish = 0x1011cdf5;
    rom_sha512_hmac_reset = 0x1011ce49;
    rom_sha512_hmac = 0x1011ce65;
    rom_sha512_hkdf = 0x1011cea1;
    curve25519_donna = 0x1011dce5;
    __rom_entry_ns_start__ = 0x101c0000;
    rom_sec_call_ns_entry = 0x101c0000;
    __rom_entry_ns_end__ = 0x101c0008;
    SPIC_CALIB_PATTERN = 0x101c0008;
    PORT_AB = 0x101c0024;
    armBitRevIndexTable1024 = 0x101c004c;
    twiddleCoef_1024 = 0x101c0e5c;
    __rom_bss_end_s__ = 0x200f9000;
}
