/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/* For module features */
/* For linker scripts */
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
/* kernel feature conf */
/* kernel task conf */
/* kernel workqueue conf */
/* kernel mm_region conf */
/* kernel timer&tick conf */
/*must reserve enough stack size for timer cb will consume*/
/* kernel intrpt conf */
/* kernel dyn alloc conf */
/* kernel idle conf */
/* kernel hook conf */
/*task user info index start*/
/*task user info index end*/
/* cli panic dump */
/* TODO: Add FLASH_REGION_OFFSET if there are multiple levels of boot loaders */
/* Linker script to configure memory regions. */
/* See plat_addr_map.h and common.mk for the macro definitions */
MEMORY
{
 ROM (rx) : ORIGIN = 0x00000000, LENGTH = 0x00010000
 FLASH (r) : ORIGIN = (0x2C000000 + 0x2A000), LENGTH = (0x1000000 - ((0x2C000000 + 0x2A000) - 0x2C000000))
 FLASH_NC (r) : ORIGIN = (((0x2C000000 + 0x2A000)) - 0x2C000000 + 0x28000000), LENGTH = (0x1000000 - ((0x2C000000 + 0x2A000) - 0x2C000000))
 FLASHX (rx) : ORIGIN = (((0x2C000000 + 0x2A000)) - 0x2C000000 + 0x0C000000), LENGTH = (0x1000000 - ((0x2C000000 + 0x2A000) - 0x2C000000))
 RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x200000-0x00057000-0x00029000 - 0x16000
 RAMX (rx) : ORIGIN = 0x00200000, LENGTH = 0x200000-0x00057000-0x00029000 - 0x16000
 FRAMX (rwx) : ORIGIN = 0x00200000 + 0x200000-0x00057000-0x00029000 - 0x16000, LENGTH = 0x16000
 PSRAM (rwx) : ORIGIN = 0x34000000, LENGTH = 0x800000
 PSRAM_NC (rwx) : ORIGIN = 0x30000000, LENGTH = 0x800000
 PSRAMX (rx) : ORIGIN = 0x14000000, LENGTH = 0x800000
 PSRAMUHS (rwx) : ORIGIN = (0x3C000000 + 0x5D4000), LENGTH = 0x22C000
 PSRAMUHS_NC (rwx) : ORIGIN = (0x38000000 + 0x5D4000), LENGTH = 0x22C000
 PSRAMUHSX (rx) : ORIGIN = (0x1C000000 + 0x5D4000), LENGTH = 0x22C000
 RAMCP (rwx) : ORIGIN = 0x20000000+0x200000-0x00057000-0x00029000, LENGTH = 0x00057000
 RAMCPX (rwx) : ORIGIN = 0x00200000+0x200000-0x00057000-0x00029000+0x00057000, LENGTH = 0x00029000
 RAMEXT (rwx) : ORIGIN = 0x20040000, LENGTH = 0xc0000
}
/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)
/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __export_fn_rom
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   __free_ram
 *   __factory_start
 *   __factory_end
 *   __calib_start
 *   __calib_end
 *   __flash_start
 *   __flash_end
 *   __free_flash
 *   __boot_sram_start_flash__
 *   __boot_sram_end_flash__
 *   __boot_sram_start__
 *   __boot_bss_sram_start__
 *   __boot_bss_sram_end__
 *   __sram_text_data_start_flash__
 *   __sram_text_data_end_flash__
 *   __fast_sram_text_data_start__
 *   __fast_sram_text_data_end__
 *   __fast_sram_text_data_start_flash__
 *   __fast_sram_text_data_end_flash__
 *   __sram_text_data_start__
 *   __sram_bss_start__
 *   __sram_bss_end__
 *   __overlay_text_start__
 *   __overlay_text_exec_start__
 *   __overlay_data_start__
 */
ENTRY(Boot_Loader)
SECTIONS
{
 __export_fn_rom = (ORIGIN(ROM) + LENGTH(ROM) - 40 - 128);
 .boot_struct (ORIGIN(FLASH)) :
 {
  __flash_start = .;
  KEEP(*(.boot_struct))
  . = ALIGN(4);
 } > FLASH
 . = ((.) - 0x2C000000 + 0x0C000000);
 .boot_text_flash (.) : AT (((ADDR(.boot_text_flash)) - 0x0C000000 + 0x2C000000))
 {
  *(.boot_loader)
  *(.boot_text_flash*)
  *(.boot_rodata*)
  . = ALIGN(4);
 } > FLASHX
 __boot_sram_start_flash__ = ((.) - 0x0C000000 + 0x2C000000);
 .vector_table (ORIGIN(RAM)) (NOLOAD) :
 {
  KEEP(*(.vector_table))
  . = 360;
  . = ALIGN(4);
 } > RAM
 .reboot_param (.) (NOLOAD) :
 {
  KEEP(*(.reboot_param))
  . = 64;
  . = ALIGN(4);
 } > RAM
 .userdata_pool (.) (NOLOAD) :
 {
  *(.userdata_pool)
  . = ALIGN(4);
  __userdata_pool_end__ = .;
 } > RAM
 __boot_sram_start__ = __userdata_pool_end__;
 .boot_text_sram (((__boot_sram_start__) - 0x20000000 + 0x00200000)) : AT (__boot_sram_start_flash__)
 {
  __boot_text_sram_start = .;
  /* memcpy.o or libc_nano.a(lib_a-memcpy.o/lib_a-memcpy-stub.o) */
  *:memcpy.o(.text*)
  *:lib_a-memcpy*.o(.text*)
  *:libc_rom.o(.text*)
  /* memset.o or libc_nano.a(lib_a-memset.o) */
  *:memset.o(.text*)
  *:lib_a-memset*.o(.text*)
  *:hal_norflash*.o(.text*)
  *:norflash_*.o(EXCLUDE_FILE(*:norflash_api.o) .text*)
  *psram*.o(.text*)
  *(.boot_text_sram*)
  . = ALIGN(4);
  __boot_text_sram_end = .;
 } > RAMX
 .boot_data_sram (((ADDR(.boot_text_sram) + SIZEOF(.boot_text_sram)) - 0x00200000 + 0x20000000)) :
     AT (__boot_sram_start_flash__ + SIZEOF(.boot_text_sram))
 {
  *:hal_norflash*.o(.data* .rodata*)
  *:norflash_*.o(EXCLUDE_FILE(*:norflash_api.o) .data* EXCLUDE_FILE(*:norflash_api.o) .rodata*)
  *psram*.o(.data* .rodata*)
  *(.boot_data*)
  . = ALIGN(4);
  __boot_data_sram_end = .;
 } > RAM
 __boot_sram_end_flash__ = __boot_sram_start_flash__ +
        SIZEOF(.boot_text_sram) + SIZEOF(.boot_data_sram);
 .boot_bss_sram (.) (NOLOAD) : AT (ADDR(.boot_bss_sram))
 {
  __boot_bss_sram_start__ = .;
  *:hal_norflash*.o(.bss*)
  *:norflash_*.o(EXCLUDE_FILE(*:norflash_api.o) .bss*)
  *psram*.o(.bss*)
  *(.boot_bss*)
  . = ALIGN(4);
  __boot_bss_sram_end__ = .;
 } > RAM
 __sram_text_data_start_flash__ = __boot_sram_end_flash__;
 __sram_text_data_start__ = __boot_bss_sram_end__;
 .sram_text (((__sram_text_data_start__) - 0x20000000 + 0x00200000)) : AT (__sram_text_data_start_flash__)
 {
  __sram_text_start__ = .;
  *:hal_memsc.o(.text*)
  *:hal_trace.o(.text*)
  *:mpu.o(.text*)
  *:codec_sbc.o(.text*)
  *:multi_heap.o(.text*)
  *:pool_api.o(.text*)
  *libgcc.a:(.text*)
  /* for LIBC_ROM=0 */
  *libc_nano.a:(.text*)
  *:norflash_api.o(.text*)
  *:tcp.o(.text*)
  *:tcp_in.o(.text*)
  . = ALIGN(0x2000); /*align for dubug hook*/
  *:tcp_out.o(.text*)
  /* for SCO_CP_ACCEL=1 */
  *:cmsis_dsp_lib.o(.text*)
  *libm.a:(.text*)
  *:tcp*.o(.text*)
  *:net*.o(.text*)
  *:agg*.o(.text*)
  *:michael.o(.text*)
  *:rx.o(.text*)
  *:event.o(.text*)
  *:util.o(.text*)
  *:rate.o(.text*)
  *:rc80211*.o(.text*)
  *:tx.o(.text*)
  *:status.o(.text*)
  *:hw_crc.o(.text*)
  *:queue.o(.text*)
  *:hwio.o(.text*)
  *:fwio.o(.text*)
  *:bh.o(.text*)
  *:txrx.o(.text*)
  *:skbuff.o(.text*)
  *:eth*.o(.text*)
  *:ip*.o(.text*)
  *:def.o(.text*)
  *:pbuf.o(.text*)
  *:raw.o(.text*)
  *:sockets.o(.text*)
  *:cmsis*.o(.text*)
  *:list.o(.text*)
  *:memp*.o(.text*)
  *:mcu_slave_init.o(.text*)
  *:dsp_code_decom.o(.text*)
  *:lzma_decode.o(.text*)
  *:ftp_client.o(.text*)
  *:port_s.o(.text*)
  *:k_*.o(.text*)
  *:flash.o(.text*)
/*
		*:k_buf_queue.o(.text*)
		*:k_dyn_mem_proc.o(.text*)
		*:k_err.o(.text*)
		*:k_event.o(.text*)
		*:k_idle.o(.text*)
		*:k_mm.o(.text*)
		*:k_mm_blk.o(.text*)
		*:k_mm_debug.o(.text*)
		*:k_mutex.o(.text*)
		*:k_obj.o(.text*)
		*:k_pend.o(.text*)
		*:k_queue.o(.text*)
		*:k_ringbuf.o(.text*)
		*:k_sched.o(.text*)
		*:k_sem.o(.text*)
		*:k_spin_lock.o(.text*)
		*:k_stats.o(.text*)
		*:k_sys.o(.text*)
		*:k_task.o(.text*)
		*:k_task_sem.o(.text*)
		*:k_tick.o(.text*)
		*:k_time.o(.text*)
		*:k_timer.o(.text*)
		*:k_workqueue.o(.text*)
*/
  *(.sram_text*)
  . = ALIGN(4);
  __sram_text_end__ = .;
 } > RAMX
 .sram_data (((ADDR(.sram_text) + SIZEOF(.sram_text)) - 0x00200000 + 0x20000000)) :
    AT (__sram_text_data_start_flash__ + SIZEOF(.sram_text))
 {
  *:rtos_lib.o(.data* .rodata*)
  . = ALIGN(0x500);
        *(.nv_extension_pool)
        . = ALIGN(0x500);
  *:utils.o(.data* .rodata*)
  *:cqueue.o(.data* .rodata*)
  *:iir_process.o(.data* .rodata*)
  *:sbcplc.o(.data* .rodata*)
  *:sbc.o(.data* .rodata*)
  *:sbc_math.o(.data* .rodata*)
  *:crossover_filter.o(.data* .rodata*)
  *:exp_func_tables.o(.data* .rodata*)
  *:hal_memsc.o(.data* .rodata*)
  *:hal_trace.o(.data* .rodata*)
  *:mpu.o(.data* .rodata*)
  *:multi_heap.o(.data* .rodata*)
  *:pool_api.o(.data* .rodata*)
  *:a2dp_decoder_cp.o(.data* .rodata*)
  *libgcc.a:(.data* .rodata*)
  /* for LIBC_ROM=0 */
  *libc_nano.a:(.data* .rodata*)
  /* for SCO_CP_ACCEL=1 */
  *:cmsis_dsp_lib.o(.data* .rodata*)
  *libm.a:(.data* .rodata*)
  *:norflash_api.o(.rodata*)
        *:tcp*.o(.data*)
        *:net*.o(.data*)
        *:agg*.o(.data*)
        *:rx.o(.data*)
        *:event.o(.data*)
        *:util.o(.data*)
        *:rate.o(.data*)
        *:rc80211*.o(.data*)
        *:tx.o(.data*)
        *:status.o(.data*)
        *:hw_crc.o(.data*)
        *:queue.o(.data*)
        *:hwio.o(.data*)
        *:fwio.o(.data*)
        *:bh.o(.data*)
        *:txrx.o(.data*)
        *:skbuff.o(.data*)
        *:eth*.o(.data*)
        *:ip*.o(.data*)
        *:def.o(.data*)
        *:pbuf.o(.data*)
        *:raw.o(.data*)
        *:sockets.o(.data*)
        *:apps.o(.data*)
        *:cmsis*.o(.data*)
        *:list.o(.data*)
  *:k_*.o(.data*)
  *:mcu_slave_init.o(.data*)
  *:dsp_code_decom.o(.data*)
  *:lzma_decode.o(.data*)
  *:ftp_client.o(.data*)
/*
		*:k_buf_queue.o(.data*)
		*:k_dyn_mem_proc.o(.data*)
		*:k_err.o(.data*)
		*:k_event.o(.data*)
		*:k_idle.o(.data*)
		*:k_mm.o(.data*)
		*:k_mm_blk.o(.data*)
		*:k_mm_debug.o(.data*)
		*:k_mutex.o(.data*)
		*:k_obj.o(.data*)
		*:k_pend.o(.data*)
		*:k_queue.o(.data*)
		*:k_ringbuf.o(.data*)
		*:k_sched.o(.data*)
		*:k_sem.o(.data*)
		*:k_spin_lock.o(.data*)
		*:k_stats.o(.data*)
		*:k_sys.o(.data*)
		*:k_task.o(.data*)
		*:k_task_sem.o(.data*)
		*:k_tick.o(.data*)
		*:k_time.o(.data*)
		*:k_timer.o(.data*)
		*:k_workqueue.o(.data*)
*/
  *genie_json_c.a:(.data.random_seed*)
  *(.sram_data*)
  . = ALIGN(4);
 } > RAM

 .core_driver_entry (.):
 {
  __core_driver_start__ = .;
  KEEP(*(.core_driver_entry))
  . = ALIGN(4);
  __core_driver_end__ = .;
 } > RAM

 .bus_driver_entry (.):
 {
  __bus_driver_start__ = .;
  KEEP(*(*.bus_driver_entry))
  __bus_driver_end__ = .;
 } > RAM

 .early_driver_entry (.):
  {
   __early_driver_start__ = .;
   KEEP(*(*.early_driver_entry))
   __early_driver_end__ = .;
  } > RAM

 .vfs_driver_entry (.):
  {
   __vfs_driver_start__ = .;
   KEEP(*(*.vfs_driver_entry))
   __vfs_driver_end__ = .;
  } > RAM


 .level0_driver_entry (.):
  {
   __level0_driver_start__ = .;
   KEEP(*(*.level0_driver_entry))
   __level0_driver_end__ = .;
  } > RAM

 .level1_driver_entry (.):
  {
   __level1_driver_start__ = .;
   KEEP(*(*.level1_driver_entry))
   __level1_driver_end__ = .;
  } > RAM

 .level2_driver_entry (.):
  {
   __level2_driver_start__ = .;
   KEEP(*(*.level2_driver_entry))
   __level2_driver_end__ = .;
  } > RAM

 .level3_driver_entry (.):
  {
   __level3_driver_start__ = .;
   KEEP(*(*.level3_driver_entry))
   __level3_driver_end__ = .;
  } > RAM

 .post_driver_entry (.):
  {
   __post_driver_start__ = .;
   KEEP(*(*.post_driver_entry))
   __post_driver_end__ = .;
  } > RAM

 __sram_text_data_end_flash__ = __sram_text_data_start_flash__ +
        SIZEOF(.sram_text) + SIZEOF(.sram_data) +
        SIZEOF(.core_driver_entry) + SIZEOF(.bus_driver_entry) +
        SIZEOF(.early_driver_entry) + SIZEOF(.vfs_driver_entry) +
        SIZEOF(.level0_driver_entry) + SIZEOF(.level1_driver_entry) +
        SIZEOF(.level2_driver_entry) + SIZEOF(.level3_driver_entry) +
        SIZEOF(.post_driver_entry);
 .sram_bss (.) (NOLOAD) : AT (ADDR(.sram_bss))
 {
  __sram_bss_start__ = .;
  *:utils.o(.bss*)
  *:tcp*.o(.bss*)
  *:net*.o(.bss*)
  *:agg*.o(.bss*)
  *:rx.o(.bss*)
  *:event.o(.bss*)
  *:util.o(.bss*)
  *:rate.o(.bss*)
  *:rc80211*.o(.bss*)
  *:tx.o(.bss*)
  *:status.o(.bss*)
  *:hw_crc.o(.bss*)
  *:queue.o(.bss*)
  *:hwio.o(.bss*)
  *:fwio.o(.bss*)
  *:bh.o(.bss*)
  *:txrx.o(.bss*)
  *:skbuff.o(.bss*)
  *:eth*.o(.bss*)
  *:ip*.o(.bss*)
  *:def.o(.bss*)
  *:pbuf.o(.bss*)
  *:raw.o(.bss*)
  *:sockets.o(.bss*)
  *:apps.o(.bss*)
  *:cmsis*.o(.bss*)
  *:list.o(.bss*)
  *:memp*.o(.bss*)
  *:k_*.o(.bss*)
  *:mcu_slave_init.o(.bss*)
  *:dsp_code_decom.o(.bss*)
  *:lzma_decode.o(.bss*)
  *:ftp_client.o(.bss*)
/*
		*:k_buf_queue.o(.bss*)
		*:k_dyn_mem_proc.o(.bss*)
		*:k_err.o(.bss*)
		*:k_event.o(.bss*)
		*:k_idle.o(.bss*)
		*:k_mm.o(.bss*)
		*:k_mm_blk.o(.bss*)
		*:k_mm_debug.o(.bss*)
		*:k_mutex.o(.bss*)
		*:k_obj.o(.bss*)
		*:k_pend.o(.bss*)
		*:k_queue.o(.bss*)
		*:k_ringbuf.o(.bss*)
		*:k_sched.o(.bss*)
		*:k_sem.o(.bss*)
		*:k_spin_lock.o(.bss*)
		*:k_stats.o(.bss*)
		*:k_sys.o(.bss*)
		*:k_task.o(.bss*)
		*:k_task_sem.o(.bss*)
		*:k_tick.o(.bss*)
		*:k_time.o(.bss*)
		*:k_timer.o(.bss*)
		*:k_workqueue.o(.bss*)
*/
  *(.sram_bss*)
  . = ALIGN(4);
  __sram_bss_end__ = .;
 } > RAM
 __fast_sram_text_data_start_flash__ = __sram_text_data_end_flash__;
 .fast_text_sram (ORIGIN(FRAMX)) : AT (__fast_sram_text_data_start_flash__)
 {
  __fast_sram_text_exec_start__ = .;
  *:rtos_lib.o(.text*)
  /* memmove.o or libc_nano.a(lib_a-memmove.o) */
  *:memmove.o(.text*)
  *:lib_a-memmove.o(.text*)
  *:utils.o(.text*)
  *:cqueue.o(.text*)
  *:audio_resample_ex.o(.text*)
  *:sbc.o(.text*)
  *:sbc_math.o(.text*)
  *:fixpoint_math.o(.text*)
  *:FDK_bitbuffer.o(.text*)
  *:FDK_tools_rom.o(.text*FDKgetWindowSlope*)
  *:FDK_tools_rom.o(.text*getBitstreamElementList*)
  *:dct.o(.text*dct_IV*)
  *:fft.o(.text*._Z3fftiPlPi)
  *:fft_rad2.o(.text*dit_fft*)
  *:dct.o(.text*getTables*)
  *:crossover_filter.o(.text*)
  *:data_compressor.o(.text*)
  *:drc.o(.text*)
  *:limiter.o(.text*)
  :audio_prompt_sbc.o(.text*)
  *(.fast_text_sram*)
  . = ALIGN(4);
  __fast_sram_text_exec_end__ = .;
 } > FRAMX
 __fast_sram_text_data_end_flash__ = __fast_sram_text_data_start_flash__ + SIZEOF(.fast_text_sram);
 __fast_sram_text_data_start__ = ((__fast_sram_text_exec_start__) - 0x00200000 + 0x20000000);
 __fast_sram_text_data_end__ = ((__fast_sram_text_exec_end__) - 0x00200000 + 0x20000000);
 __cp_text_sram_start_flash__ = __fast_sram_text_data_end_flash__;
 .cp_text_sram (ORIGIN(RAMCPX)) : AT (__cp_text_sram_start_flash__)
 {
  __cp_text_sram_exec_start__ = .;
  *(.cp_text_sram*)
  /* opus */
  *:opus_decoder.o(.text*)
  *:bands.o(.text*)
  *:celt_decoder.o(.text*)
  *:entenc.o(.text*)
  *:entdec.o(.text*)
  *:vq.o(.text*)
  *:decode_core.o(.text*)
  *:opus.o(.text*)
  *:dec_API.o(.text*)
  *:init_decoder.o(.text*)
  *:CNG.o(.text*)
  *:PLC.o(.text*)
  *:decoder_set_fs.o(.text*)
  *:decode_frame.o(.text*)
  *:decode_indices.o(.text*)
  *:decode_pulses.o(.text*)
  *:decode_parameters.o(.text*)
  *:decode_pitch.o(.text*)
  *:control_audio_bandwidth.o(.text*)
  *:gain_quant.o(.text*)
  *:NLSF_decode.o(.text*)
  *:NLSF_stabilize.o(.text*)
  *:NLSF2A.o(.text*)
  *:stereo_MS_to_LR.o(.text*)
  *:LPC_fit.o(.text*)
  *:bwexpander_32.o(.text*)
  *:LPC_inv_pred_gain.o(.text*)
  *:resampler_private_IIR_FIR.o(.text*)
  *:resampler_private_AR2.o(.text*)
  *:stereo_decode_pred.o(.text*)
  *:resampler_private_down_FIR.o(.text*)
  *:resampler_private_up2_HQ.o(.text*)
  *:bwexpander.o(.text*)
  *:quant_bands.o(.text*)
  *:opus_kiss_fft.o(.text*)
  *:rate.o(.text*.compute_allocation)
  *:cwrs.o(.text*)
  *:mathops.o(.text*)
  *:LPC_analysis_filter.o(.text*)
  *:burg_modified_FIX.o(.text*)
  *:vector_ops_FIX.o(.text*)
  *:NLSF_del_dec_quant.o(.text*)
  *:encode_frame_FIX.o(.text*)
  *:pitch.o(.text*)
  *:resampler.o(.text*)
  *:VAD.o(.text*)
  *:encode_indices.o(.text*)
  *:control_codec.o(.text*)
  *:celt_lpc.o(.text*)
  *:shell_coder.o(.text*)
  *:init_encoder.o(.text*)
  *:log2lin.o(.text*)
  *:biquad_alt.o(.text*)
  *:stereo_encode_pred.o(.text*)
  *:NLSF_unpack.o(.text*)
  *:code_signs.o(.text*)
  *:lin2log.o(.text*)
  *:HP_variable_cutoff.o(.text*)
  *:sum_sqr_shift.o(.text*)
  *:inner_prod_aligned.o(.text*)
  *:stereo_quant_pred.o(.text*)
  *:entcode.o(.text*)
  *:celt.o(.text*)
  *:laplace.o(.text*)
  *:NSQ.o(.text*) /*complexity 1*/
  *:NSQ_del_dec.o(.text*) /*complexity 2*/
  /* /opus/src/src */
  *:analysis.o(.text*)
  *:mlp.o(.text*)
  *:mlp_data.o(.text*)
  *:opus.o(.text*)
  /*:opus_decoder.o(.text*) */
  *:opus_encoder.o(.text*)
  *:opus_memory.o(.text*)
  *:opus_multistream.o(.text*)
  /*:opus_multistream_decoder.o(.text*) */
  *:opus_multistream_encoder.o(.text*)
  *:repacketizer.o(.text*)
  /* /opus/src/silk */
  *:A2NLSF.o(.text*)
  *:ana_filt_bank_1.o(.text*)
  *:biquad_alt.o(.text*)
  *:bwexpander_32.o(.text*)
  *:bwexpander.o(.text*)
  *:check_control_input.o(.text*)
  *:CNG.o(.text*)
  *:code_signs.o(.text*)
  *:control_audio_bandwidth.o(.text*)
  *:control_codec.o(.text*)
  *:control_SNR.o(.text*)
  *:debug.o(.text*)
  /*:dec_API.o(.text*) */
  /*:decode_core.o(.text*) */
  /*:decode_frame.o(.text*) */
  /*:decode_indices.o(.text*) */
  /*:decode_parameters.o(.text*) */
  /*:decode_pitch.o(.text*) */
  *:decode_pulses.o(.text*)
  /*:decoder_set_fs.o(.text*) */
  *:enc_API.o(.text*)
  *:encode_indices.o(.text*)
  *:encode_pulses.o(.text*)
  *:gain_quant.o(.text*)
  *:HP_variable_cutoff.o(.text*)
  /*:init_decoder.o(.text*) */
  *:init_encoder.o(.text*)
  *:inner_prod_aligned.o(.text*)
  *:interpolate.o(.text*)
  *:lin2log.o(.text*)
  *:log2lin.o(.text*)
  *:LPC_analysis_filter.o(.text*)
  *:LPC_fit.o(.text*)
  *:LPC_inv_pred_gain.o(.text*)
  *:LP_variable_cutoff.o(.text*)
  *:NLSF2A.o(.text*)
  *:NLSF_decode.o(.text*)
  *:NLSF_del_dec_quant.o(.text*)
  *:NLSF_encode.o(.text*)
  *:NLSF_stabilize.o(.text*)
  *:NLSF_unpack.o(.text*)
  *:NLSF_VQ.o(.text*)
  *:NLSF_VQ_weights_laroia.o(.text*)
  *:NSQ.o(.text*)
  *:NSQ_del_dec.o(.text*)
  *:pitch_est_tables.o(.text*)
  *:PLC.o(.text*)
  *:process_NLSFs.o(.text*)
  *:quant_LTP_gains.o(.text*)
  *:resampler.o(.text*)
  *:resampler_down2_3.o(.text*)
  *:resampler_down2.o(.text*)
  *:resampler_private_AR2.o(.text*)
  *:resampler_private_down_FIR.o(.text*)
  *:resampler_private_IIR_FIR.o(.text*)
  *:resampler_private_up2_HQ.o(.text*)
  *:resampler_rom.o(.text*)
  *:shell_coder.o(.text*)
  *:sigm_Q15.o(.text*)
  *:sort.o(.text*)
  /*:stereo_decode_pred.o(.text*) */
  *:stereo_encode_pred.o(.text*)
  *:stereo_find_predictor.o(.text*)
  *:stereo_LR_to_MS.o(.text*)
  *:stereo_MS_to_LR.o(.text*)
  *:stereo_quant_pred.o(.text*)
  *:sum_sqr_shift.o(.text*)
  *:table_LSF_cos.o(.text*)
  *:tables_gain.o(.text*)
  *:tables_LTP.o(.text*)
  *:tables_NLSF_CB_NB_MB.o(.text*)
  *:tables_NLSF_CB_WB.o(.text*)
  *:tables_other.o(.text*)
  *:tables_pitch_lag.o(.text*)
  *:tables_pulses_per_block.o(.text*)
  *:VAD.o(.text*)
  *:VQ_WMat_EC.o(.text*)
  /* /opus/src/silk/fixed */
  *:apply_sine_window_FIX.o(.text*)
  *:autocorr_FIX.o(.text*)
  *:burg_modified_FIX.o(.text*)
  *:corrMatrix_FIX.o(.text*)
  *:encode_frame_FIX.o(.text*)
  *:find_LPC_FIX.o(.text*)
  *:find_LTP_FIX.o(.text*)
  *:find_pitch_lags_FIX.o(.text*)
  *:find_pred_coefs_FIX.o(.text*)
  *:k2a_FIX.o(.text*)
  *:k2a_Q16_FIX.o(.text*)
  *:LTP_analysis_filter_FIX.o(.text*)
  *:LTP_scale_ctrl_FIX.o(.text*)
  *:noise_shape_analysis_FIX.o(.text*)
  *:pitch_analysis_core_FIX.o(.text*)
  *:process_gains_FIX.o(.text*)
  *:regularize_correlations_FIX.o(.text*)
  *:residual_energy16_FIX.o(.text*)
  *:residual_energy_FIX.o(.text*)
  *:schur64_FIX.o(.text*)
  *:schur_FIX.o(.text*)
  *:vector_ops_FIX.o(.text*)
  *:warped_autocorrelation_FIX.o(.text*)
  /* /opus/src/celt */
  *:bands.o(.text*)
  *:celt.o(.text*)
  /*:celt_decoder.o(.text*) */
  *:celt_encoder.o(.text*)
  *:celt_lpc.o(.text*)
  *:cwrs.o(.text*)
  *:entcode.o(.text*)
  *:entdec.o(.text*)
  *:entenc.o(.text*)
  *:laplace.o(.text*)
  *:mathops.o(.text*)
  *:mdct.o(.text*)
  *:modes.o(.text*)
  *:opus_kiss_fft.o(.text*)
  *:pitch.o(.text*)
  *:quant_bands.o(.text*)
  *:rate.o(.text*.compute_allocation)
  *:vq.o(.text*)
  /* eq */
  *:iir_process.o(.text*)
  /* drc */
  *:drc.o(*text)
  *:data_compressor.o(*text)
  /* limiter */
  *:limiter.o(.text*)
  *:floatlimiter.o(.text*)
  *:audio_resample_ex.o(.text*)
  *:resample_coef.o(.text*)
  . = ALIGN(4);
  __cp_text_sram_exec_end__ = .;
 } > RAMCPX
 __cp_text_sram_start = ((__cp_text_sram_exec_start__) - 0x00200000 + 0x20000000);
 __cp_text_sram_end = ((__cp_text_sram_exec_end__) - 0x00200000 + 0x20000000);
 .vector_table_cp (ORIGIN(RAMCP)) (NOLOAD) :
 {
  KEEP(*(.vector_table_cp))
  . = 360;
  . = ALIGN(4);
 } > RAMCP
 __cp_data_sram_start_flash__ = __cp_text_sram_start_flash__ + SIZEOF(.cp_text_sram);
 .cp_data_sram (.) : AT (__cp_data_sram_start_flash__)
 {
  __cp_data_sram_start = .;
  *(.cp_data*)
  . = ALIGN(4);
  *:cp_server.o(.data* .rodata*)
  /* opus */
  *:resampler.o(.data* .rodata*)
  *:tables_other.o(.data* .rodata*)
  *:tables_gain.o(.data* .rodata*)
  *:tables_LTP.o(.data* .rodata*)
  *:tables_NLSF_CB_NB_MB.o(.data* .rodata*)
  *:NLSF2A.o(.data* .rodata*)
  *:table_LSF_cos.o(.data* .rodata*)
  *:tables_pulses_per_block.o(.data* .rodata*)
  *:PLC.o(.data* .rodata*)
  *:resampler_rom.o(.data* .rodata*)
  *:pitch_est_tables.o(.data* .rodata*)
  *:tables_pitch_lag.o(.data* .rodata*)
  *:celt_decoder.o(.data* .rodata*)
  *:bands.o(.data* .rodata*)
  *:vq.o(.data* .rodata*)
  *:cwrs.o(.data* .rodata*)
  *:entcode.o(.data* .rodata*)
  *:rate.o(.rodata.LOG2_FRAC_TABLE)
  *:modes.o(.data* .rodata*)
  *:quant_bands.o(.data* .rodata*)
  *:opus_decoder.o(.data* .rodata*)
  *:dec_API.o(.data* .rodata*)
  *:tables_NLSF_CB_WB.o(.data* .rodata*)
  *:celt.o(.data* .rodata*)
  /* /opus/src/src */
  *:analysis.o(.data* .rodata*)
  *:mlp.o(.data* .rodata*)
  *:mlp_data.o(.data* .rodata*)
  *:opus.o(.data* .rodata*)
  /*:opus_decoder.o(.data* .rodata*) */
  *:opus_encoder.o(.data* .rodata*)
  *:opus_memory.o(.data* .rodata*)
  *:opus_multistream.o(.data* .rodata*)
  /*:opus_multistream_decoder.o(.data* .rodata*) */
  *:opus_multistream_encoder.o(.data* .rodata*)
  *:repacketizer.o(.data* .rodata*)
  /* /opus/src/silk */
  *:A2NLSF.o(.data* .rodata*)
  *:ana_filt_bank_1.o(.data* .rodata*)
  *:biquad_alt.o(.data* .rodata*)
  *:bwexpander_32.o(.data* .rodata*)
  *:bwexpander.o(.data* .rodata*)
  *:check_control_input.o(.data* .rodata*)
  *:CNG.o(.data* .rodata*)
  *:code_signs.o(.data* .rodata*)
  *:control_audio_bandwidth.o(.data* .rodata*)
  *:control_codec.o(.data* .rodata*)
  *:control_SNR.o(.data* .rodata*)
  *:debug.o(.data* .rodata*)
  /*:dec_API.o(.data* .rodata*) */
  /*:decode_core.o(.data* .rodata*) */
  /*:decode_frame.o(.data* .rodata*) */
  /*:decode_indices.o(.data* .rodata*) */
  /*:decode_parameters.o(.data* .rodata*) */
  /*:decode_pitch.o(.data* .rodata*) */
  *:decode_pulses.o(.data* .rodata*)
  /*:decoder_set_fs.o(.data* .rodata*) */
  *:enc_API.o(.data* .rodata*)
  *:encode_indices.o(.data* .rodata*)
  *:encode_pulses.o(.data* .rodata*)
  *:gain_quant.o(.data* .rodata*)
  *:HP_variable_cutoff.o(.data* .rodata*)
  /*:init_decoder.o(.data* .rodata*) */
  *:init_encoder.o(.data* .rodata*)
  *:inner_prod_aligned.o(.data* .rodata*)
  *:interpolate.o(.data* .rodata*)
  *:lin2log.o(.data* .rodata*)
  *:log2lin.o(.data* .rodata*)
  *:LPC_analysis_filter.o(.data* .rodata*)
  *:LPC_fit.o(.data* .rodata*)
  *:LPC_inv_pred_gain.o(.data* .rodata*)
  *:LP_variable_cutoff.o(.data* .rodata*)
  *:NLSF2A.o(.data* .rodata*)
  *:NLSF_decode.o(.data* .rodata*)
  *:NLSF_del_dec_quant.o(.data* .rodata*)
  *:NLSF_encode.o(.data* .rodata*)
  *:NLSF_stabilize.o(.data* .rodata*)
  *:NLSF_unpack.o(.data* .rodata*)
  *:NLSF_VQ.o(.data* .rodata*)
  *:NLSF_VQ_weights_laroia.o(.data* .rodata*)
  *:NSQ.o(.data* .rodata*)
  *:NSQ_del_dec.o(.data* .rodata*)
  *:pitch_est_tables.o(.data* .rodata*)
  *:PLC.o(.data* .rodata*)
  *:process_NLSFs.o(.data* .rodata*)
  *:quant_LTP_gains.o(.data* .rodata*)
  *:resampler.o(.data* .rodata*)
  *:resampler_down2_3.o(.data* .rodata*)
  *:resampler_down2.o(.data* .rodata*)
  *:resampler_private_AR2.o(.data* .rodata*)
  *:resampler_private_down_FIR.o(.data* .rodata*)
  *:resampler_private_IIR_FIR.o(.data* .rodata*)
  *:resampler_private_up2_HQ.o(.data* .rodata*)
  *:resampler_rom.o(.data* .rodata*)
  *:shell_coder.o(.data* .rodata*)
  *:sigm_Q15.o(.data* .rodata*)
  *:sort.o(.data* .rodata*)
  /*:stereo_decode_pred.o(.data* .rodata*) */
  *:stereo_encode_pred.o(.data* .rodata*)
  *:stereo_find_predictor.o(.data* .rodata*)
  *:stereo_LR_to_MS.o(.data* .rodata*)
  *:stereo_MS_to_LR.o(.data* .rodata*)
  *:stereo_quant_pred.o(.data* .rodata*)
  *:sum_sqr_shift.o(.data* .rodata*)
  *:table_LSF_cos.o(.data* .rodata*)
  *:tables_gain.o(.data* .rodata*)
  *:tables_LTP.o(.data* .rodata*)
  *:tables_NLSF_CB_NB_MB.o(.data* .rodata*)
  *:tables_NLSF_CB_WB.o(.data* .rodata*)
  *:tables_other.o(.data* .rodata*)
  *:tables_pitch_lag.o(.data* .rodata*)
  *:tables_pulses_per_block.o(.data* .rodata*)
  *:VAD.o(.data* .rodata*)
  *:VQ_WMat_EC.o(.data* .rodata*)
  /* /opus/src/silk/fixed */
  *:apply_sine_window_FIX.o(.data* .rodata*)
  *:autocorr_FIX.o(.data* .rodata*)
  *:burg_modified_FIX.o(.data* .rodata*)
  *:corrMatrix_FIX.o(.data* .rodata*)
  *:encode_frame_FIX.o(.data* .rodata*)
  *:find_LPC_FIX.o(.data* .rodata*)
  *:find_LTP_FIX.o(.data* .rodata*)
  *:find_pitch_lags_FIX.o(.data* .rodata*)
  *:find_pred_coefs_FIX.o(.data* .rodata*)
  *:k2a_FIX.o(.data* .rodata*)
  *:k2a_Q16_FIX.o(.data* .rodata*)
  *:LTP_analysis_filter_FIX.o(.data* .rodata*)
  *:LTP_scale_ctrl_FIX.o(.data* .rodata*)
  *:noise_shape_analysis_FIX.o(.data* .rodata*)
  *:pitch_analysis_core_FIX.o(.data* .rodata*)
  *:process_gains_FIX.o(.data* .rodata*)
  *:regularize_correlations_FIX.o(.data* .rodata*)
  *:residual_energy16_FIX.o(.data* .rodata*)
  *:residual_energy_FIX.o(.data* .rodata*)
  *:schur64_FIX.o(.data* .rodata*)
  *:schur_FIX.o(.data* .rodata*)
  *:vector_ops_FIX.o(.data* .rodata*)
  *:warped_autocorrelation_FIX.o(.data* .rodata*)
  /* /opus/src/celt */
  *:bands.o(.data* .rodata*)
  *:celt.o(.data* .rodata*)
  /*:celt_decoder.o(.data* .rodata*) */
  *:celt_encoder.o(.data* .rodata*)
  *:celt_lpc.o(.data* .rodata*)
  *:cwrs.o(.data* .rodata*)
  *:entcode.o(.data* .rodata*)
  *:entdec.o(.data* .rodata*)
  *:entenc.o(.data* .rodata*)
  *:laplace.o(.data* .rodata*)
  *:mathops.o(.data* .rodata*)
  *:mdct.o(.data* .rodata*)
  *:modes.o(.data* .rodata*)
  *:opus_kiss_fft.o(.data* .rodata*)
  *:pitch.o(.data* .rodata*)
  *:quant_bands.o(.data* .rodata*)
  *:rate.o(.rodata.LOG2_FRAC_TABLE)
  *:vq.o(.data* .rodata*)
  /* drc */
  *:drc.o(.data* .rodata*)
  *:data_compressor.o(.data* .rodata*)
  *:floatlimiter.o(.data* .rodata*)
  *:audio_resample_ex.o(.data* .rodata*)
  *:resample_coef.o(.data* .rodata*)
  . = ALIGN(4);
  __cp_data_sram_end = .;
 } > RAMCP
 __cp_sram_end_flash__ = __cp_data_sram_start_flash__ + SIZEOF(.cp_data_sram);
 .cp_bss_sram (.) (NOLOAD) : AT (ADDR(.cp_bss_sram))
 {
  __cp_bss_sram_start = .;
  *(.cp_bss*)
  . = ALIGN(4);
  /* opus */
  *:celt.o(.bss*)
  /* /opus/src/src */
  *:analysis.o(.bss*)
  *:mlp.o(.bss*)
  *:mlp_data.o(.bss*)
  *:opus.o(.bss*)
  /*:opus_decoder.o(.bss*) */
  *:opus_encoder.o(.bss*)
  *:opus_memory.o(.bss*)
  *:opus_multistream.o(.bss*)
  /*:opus_multistream_decoder.o(.bss*) */
  *:opus_multistream_encoder.o(.bss*)
  *:repacketizer.o(.bss*)
  /* /opus/src/silk */
  *:A2NLSF.o(.bss*)
  *:ana_filt_bank_1.o(.bss*)
  *:biquad_alt.o(.bss*)
  *:bwexpander_32.o(.bss*)
  *:bwexpander.o(.bss*)
  *:check_control_input.o(.bss*)
  *:CNG.o(.bss*)
  *:code_signs.o(.bss*)
  *:control_audio_bandwidth.o(.bss*)
  *:control_codec.o(.bss*)
  *:control_SNR.o(.bss*)
  *:debug.o(.bss*)
  /*:dec_API.o(.bss*) */
  /*:decode_core.o(.bss*) */
  /*:decode_frame.o(.bss*) */
  /*:decode_indices.o(.bss*) */
  /*:decode_parameters.o(.bss*) */
  /*:decode_pitch.o(.bss*) */
  *:decode_pulses.o(.bss*)
  /*:decoder_set_fs.o(.bss*) */
  *:enc_API.o(.bss*)
  *:encode_indices.o(.bss*)
  *:encode_pulses.o(.bss*)
  *:gain_quant.o(.bss*)
  *:HP_variable_cutoff.o(.bss*)
  /*:init_decoder.o(.bss*) */
  *:init_encoder.o(.bss*)
  *:inner_prod_aligned.o(.bss*)
  *:interpolate.o(.bss*)
  *:lin2log.o(.bss*)
  *:log2lin.o(.bss*)
  *:LPC_analysis_filter.o(.bss*)
  *:LPC_fit.o(.bss*)
  *:LPC_inv_pred_gain.o(.bss*)
  *:LP_variable_cutoff.o(.bss*)
  *:NLSF2A.o(.bss*)
  *:NLSF_decode.o(.bss*)
  *:NLSF_del_dec_quant.o(.bss*)
  *:NLSF_encode.o(.bss*)
  *:NLSF_stabilize.o(.bss*)
  *:NLSF_unpack.o(.bss*)
  *:NLSF_VQ.o(.bss*)
  *:NLSF_VQ_weights_laroia.o(.bss*)
  *:NSQ.o(.bss*)
  *:NSQ_del_dec.o(.bss*)
  *:pitch_est_tables.o(.bss*)
  *:PLC.o(.bss*)
  *:process_NLSFs.o(.bss*)
  *:quant_LTP_gains.o(.bss*)
  *:resampler.o(.bss*)
  *:resampler_down2_3.o(.bss*)
  *:resampler_down2.o(.bss*)
  *:resampler_private_AR2.o(.bss*)
  *:resampler_private_down_FIR.o(.bss*)
  *:resampler_private_IIR_FIR.o(.bss*)
  *:resampler_private_up2_HQ.o(.bss*)
  *:resampler_rom.o(.bss*)
  *:shell_coder.o(.bss*)
  *:sigm_Q15.o(.bss*)
  *:sort.o(.bss*)
  /*:stereo_decode_pred.o(.bss*) */
  *:stereo_encode_pred.o(.bss*)
  *:stereo_find_predictor.o(.bss*)
  *:stereo_LR_to_MS.o(.bss*)
  *:stereo_MS_to_LR.o(.bss*)
  *:stereo_quant_pred.o(.bss*)
  *:sum_sqr_shift.o(.bss*)
  *:table_LSF_cos.o(.bss*)
  *:tables_gain.o(.bss*)
  *:tables_LTP.o(.bss*)
  *:tables_NLSF_CB_NB_MB.o(.bss*)
  *:tables_NLSF_CB_WB.o(.bss*)
  *:tables_other.o(.bss*)
  *:tables_pitch_lag.o(.bss*)
  *:tables_pulses_per_block.o(.bss*)
  *:VAD.o(.bss*)
  *:VQ_WMat_EC.o(.bss*)
  /* /opus/src/silk/fixed */
  *:apply_sine_window_FIX.o(.bss*)
  *:autocorr_FIX.o(.bss*)
  *:burg_modified_FIX.o(.bss*)
  *:corrMatrix_FIX.o(.bss*)
  *:encode_frame_FIX.o(.bss*)
  *:find_LPC_FIX.o(.bss*)
  *:find_LTP_FIX.o(.bss*)
  *:find_pitch_lags_FIX.o(.bss*)
  *:find_pred_coefs_FIX.o(.bss*)
  *:k2a_FIX.o(.bss*)
  *:k2a_Q16_FIX.o(.bss*)
  *:LTP_analysis_filter_FIX.o(.bss*)
  *:LTP_scale_ctrl_FIX.o(.bss*)
  *:noise_shape_analysis_FIX.o(.bss*)
  *:pitch_analysis_core_FIX.o(.bss*)
  *:process_gains_FIX.o(.bss*)
  *:regularize_correlations_FIX.o(.bss*)
  *:residual_energy16_FIX.o(.bss*)
  *:residual_energy_FIX.o(.bss*)
  *:schur64_FIX.o(.bss*)
  *:schur_FIX.o(.bss*)
  *:vector_ops_FIX.o(.bss*)
  *:warped_autocorrelation_FIX.o(.bss*)
  /* /opus/src/celt */
  *:bands.o(.bss*)
  *:celt.o(.bss*)
  /*:celt_decoder.o(.bss*) */
  *:celt_encoder.o(.bss*)
  *:celt_lpc.o(.bss*)
  *:cwrs.o(.bss*)
  *:entcode.o(.bss*)
  *:entdec.o(.bss*)
  *:entenc.o(.bss*)
  *:laplace.o(.bss*)
  *:mathops.o(.bss*)
  *:mdct.o(.bss*)
  *:modes.o(.bss*)
  *:opus_kiss_fft.o(.bss*)
  *:pitch.o(.bss*)
  *:quant_bands.o(.bss*)
  /*:rate.o(.bss*)*/
  *:vq.o(.bss*)
  *:data_compressor.o(.bss*)
  *:floatlimiter.o(.bss*)
  *:audio_resample_ex.o(.bss*)
  *:resample_coef.o(.bss*)
  . = ALIGN(4);
  __cp_bss_sram_end = .;
 } > RAMCP
 __overlay_text_start_flash__ = __cp_sram_end_flash__;
 __overlay_text_start__ = __cp_text_sram_end;
 __overlay_text_exec_start__ = ((__overlay_text_start__) - 0x20000000 + 0x00200000);
 OVERLAY (__overlay_text_exec_start__) : NOCROSSREFS AT (__overlay_text_start_flash__)
 {
  .overlay_text_last
  {
   LONG(0);
   . = ALIGN(4);
  }
 } > RAMCPX
 .dummy_overlay_text_end (.) (NOLOAD) : AT (ADDR(.dummy_overlay_text_end))
 {
  __overlay_text_exec_end__ = .;
 } > RAMCPX
 __fast_sram_end__ = __fast_sram_text_data_end__;
 ASSERT(__fast_sram_end__ - __fast_sram_text_data_start__ <= 0x16000, "fast xram sections too large")
 __free_fram = 0x16000 - (__fast_sram_end__ - __fast_sram_text_data_start__);
 __overlay_data_start__ = __cp_bss_sram_end;
 OVERLAY (__overlay_data_start__) : NOCROSSREFS AT (__load_stop_overlay_text_last)
 {
  .overlay_data_last
  {
   LONG(0);
   . = ALIGN(4);
  }
 } > RAMCP
 .dummy_overlay_data_end (.) (NOLOAD) : AT (ADDR(.dummy_overlay_data_end))
 {
  __overlay_data_end__ = .;
 } > RAMCP
 ASSERT(__overlay_data_end__ - __overlay_data_start__ <= 0x10000, "overlay data sections too large")
 .cp_ramx_last_dummy (NOLOAD) : AT (ADDR(.cp_ramx_last_dummy))
 {
  __cp_ramx_last_dummy_start = .;
 } > RAMCPX
 .cp_ram_last_dummy (NOLOAD) : AT (ADDR(.cp_ram_last_dummy))
 {
  . = ALIGN(8);
  __cp_ram_last_dummy_start = .;
 } > RAMCP
 __cp_stack_top = ORIGIN(RAMCP) + LENGTH(RAMCP);
 __cp_stack_limit = __cp_stack_top - 0x6000;
 /* LD v2.24 (GCC 4.9) cannot handle address conversion correctly if involving immediate value.
	   E.g., <absolute_addr> + <immediate_val> + <relative_addr> */
 __free_ramcpx = ORIGIN(RAMCPX) + LENGTH(RAMCPX) - ABSOLUTE(__cp_ramx_last_dummy_start);
 ASSERT(__cp_stack_limit >= __cp_ram_last_dummy_start, "region RAMCP overflowed with stack")
 __free_ramcp = __cp_stack_limit - __cp_ram_last_dummy_start;
 __flash_text_data_start__ = __load_stop_overlay_data_last;
 __psram_text_data_start__ = ORIGIN(PSRAM);
 __psram_text_data_start_flash__ = __flash_text_data_start__;
 .psram_text (((__psram_text_data_start__) - 0x34000000 + 0x14000000)) : AT (__psram_text_data_start_flash__)
 {
/*
		__psramx_text_start__ .;

		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o  *bpabi.o *div*.o *_dvmd_tls.o *best*.o) .text*)
		*(.psram_text*)
		. = ALIGN(4);
		__psramx_text_end__ = .;
*/
 } > PSRAMX
 .psram_data (__psram_text_data_start__ + SIZEOF(.psram_text)) :
    AT (__psram_text_data_start_flash__ + SIZEOF(.psram_text))
 {
  *(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o ) .data*)
  *(.psram_data*)
  . = ALIGN(4);
 } > PSRAM
 __psram_text_data_end_flash__ = __psram_text_data_start_flash__ + SIZEOF(.psram_text)+ SIZEOF(.psram_data);
 .psram_bss (.) (NOLOAD) : AT (ADDR(.psram_bss))
 {
  __psram_bss_start__ = .;
  *(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o ) .bss*)
  *(.psram_bss*)
  . = ALIGN(4);
  __psram_bss_end__ = .;
 } > PSRAM
 __flash_text_data_start__ = __psram_text_data_end_flash__;
 __psramuhs_text_data_start__ = ORIGIN(PSRAMUHS);
 __psramuhs_text_data_start_flash__ = __flash_text_data_start__;
 .psramuhs_text (((__psramuhs_text_data_start__) - 0x3C000000 + 0x1C000000)) : AT (__psramuhs_text_data_start_flash__)
 {
  __psramux_text_start__ = .;
  *(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o wifi_drv*.o) .text*)
  *(.psramuhs_text*)
  . = ALIGN(4);
  __psramux_text_end__ = .;
 } > PSRAMUHSX
 .psramuhs_data (__psramuhs_text_data_start__ + SIZEOF(.psramuhs_text)) :
    AT (__psramuhs_text_data_start_flash__ + SIZEOF(.psramuhs_text))
 {
/*
		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o wifi_drv*.o) .data*)
*/
  *(.psramuhs_data*)
  . = ALIGN(4);
 } > PSRAMUHS
 __psramuhs_text_data_end_flash__ = __psramuhs_text_data_start_flash__ + SIZEOF(.psramuhs_text)+ SIZEOF(.psramuhs_data);
 .psramuhs_bss (.) (NOLOAD) : AT (ADDR(.psramuhs_bss))
 {
  __psramuhs_bss_start__ = .;
/*
		*(EXCLUDE_FILE(*hal_*.o *cmsis_nvic.o *system_ARMCM4.o *system_utils.o *system_cp.o *cp_accel.o *bpabi.o *div*.o *_dvmd_tls.o *best*.o wifi_drv*.o) .bss*)
*/
  *(.psramuhs_bss*)
  . = ALIGN(4);
  __psramuhs_bss_end__ = .;
 } > PSRAMUHS
 __flash_text_data_start__ = __psramuhs_text_data_end_flash__;
 .text (((__flash_text_data_start__) - 0x2C000000 + 0x0C000000)) : AT (((ADDR(.text)) - 0x0C000000 + 0x2C000000))
 {
  __flashx_text_start__ = .;
  *(.text*)
  *(.flash_text*)
  KEEP(*(.init))
  KEEP(*(.fini))
  /* .ctors */
  *crtbegin.o(.ctors)
  *crtbegin?.o(.ctors)
  *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
  *(SORT(.ctors.*))
  *(.ctors)
  /* .dtors */
  *crtbegin.o(.dtors)
  *crtbegin?.o(.dtors)
  *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
  *(SORT(.dtors.*))
  *(.dtors)
 } > FLASHX
 .ARM.extab (.) : AT (((ADDR(.ARM.extab)) - 0x0C000000 + 0x2C000000))
 {
  *(.ARM.extab* .gnu.linkonce.armextab.*)
 } > FLASHX
 __exidx_start = .;
 /* .ARM.exidx contains R_ARM_PREL31 (+-0x40000000) offset to functions, which means
	 * the session location cannot be too far away from the function addresses */
 .ARM.exidx (.) : AT (((ADDR(.ARM.exidx)) - 0x0C000000 + 0x2C000000))
 {
  *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  __flashx_text_end__ = .;
 } > FLASHX
 __exidx_end = .;
 __flash_text_end__ = .;
 . = ((.) - 0x0C000000 + 0x2C000000);
 .rodata (.) :
 {
  *(.rodata*)
  *(.flash_rodata*)
        . = ALIGN(4);
        __cust_cmd_hldr_tbl_start = .;
        KEEP(*(.cust_cmd_hldr_tbl))
        __cust_cmd_hldr_tbl_end = .;
  . = ALIGN(4);
  __ai_gatt_server_table_start = .;
  KEEP(*(.ai_gatt_server_table))
  __ai_gatt_server_table_end = .;
  . = ALIGN(4);
  __ai_ble_handler_table_start = .;
  KEEP(*(.ai_ble_handler_table))
  __ai_ble_handler_table_end = .;
  . = ALIGN(4);
  __ai_spp_register_table_start = .;
  KEEP(*(.ai_spp_register_table))
  __ai_spp_register_table_end = .;
  . = ALIGN(4);
  __tota_handler_table_start = .;
  KEEP(*(.tota_handler_table))
  __tota_handler_table_end = .;
  . = ALIGN(4);
  __ai_handler_function_table_start = .;
  KEEP(*(.ai_handler_function_table))
  __ai_handler_function_table_end = .;
  . = ALIGN(4);
  __custom_handler_table_start = .;
  KEEP(*(.custom_handler_table))
  __custom_handler_table_end = .;
  . = ALIGN(4);
  __thirdparty_handler_table_start = .;
  KEEP(*(.thirdparty_handler_table))
  __thirdparty_handler_table_end = .;
                . = ALIGN(4);
                __mixprompt_property_table_start = .;
                KEEP(*(.mixprompt_property_table))
                __mixprompt_property_table_end = .;
   KEEP(*(.eh_frame*))
  *(.note.gnu.build-id)
 } > FLASH
 .CliRegion (.) :
 {
  _cli_region_begin = .;
  KEEP(*(CliRegion))
  . = ALIGN(4);
  _cli_region_end = .;
 } > FLASH

  .UsrCmdSymTab (.) :
 {
  _usrcmd_table_begin = .;
  KEEP(*(UsrCmdSymTab))
  . = ALIGN(4);
  _usrcmd_table_end = .;
 } > FLASH
 /* To copy multiple FLASH to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
 /*
	.copy.table (.) :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG (__data_end__ - __data_start__)
		LONG (__etext2)
		LONG (__data2_start__)
		LONG (__data2_end__ - __data2_start__)
		__copy_table_end__ = .;
	} > FLASH
	*/
 /* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
 /*
	.zero.table (.) :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG (__bss_end__ - __bss_start__)
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
		__zero_table_end__ = .;
	} > FLASH
	*/
 /* Location counter can end up 2byte aligned with narrow Thumb code but
	   __etext is assumed by startup code to be the LMA of a section in RAM
	   which must be 4byte aligned */
 __etext = ALIGN(4);
 /* The VMA is either the end of overlay_data or the end of sram bss */
 . = __overlay_data_end__;
 .data : AT (__etext)
 {
  __data_start__ = .;
  _sdata = .;
  *(vtable)
  . = ALIGN(4);
  *(.data*)
  . = ALIGN(4);
  /* preinit data */
  PROVIDE_HIDDEN (__preinit_array_start = .);
  KEEP(*(.preinit_array))
  PROVIDE_HIDDEN (__preinit_array_end = .);
  . = ALIGN(4);
  /* init data */
  PROVIDE(__ctors_start__ = .);
  PROVIDE_HIDDEN (__init_array_start = .);
  KEEP(*(SORT(.init_array.*)))
  KEEP(*(.init_array))
  PROVIDE_HIDDEN (__init_array_end = .);
  PROVIDE(__ctors_end__ = .);
  . = ALIGN(4);
  /* finit data */
  PROVIDE_HIDDEN (__fini_array_start = .);
  KEEP(*(SORT(.fini_array.*)))
  KEEP(*(.fini_array))
  PROVIDE_HIDDEN (__fini_array_end = .);
  KEEP(*(.jcr*))
  /* All data end */
  . = ALIGN(4);
  *(*.got*)
  __data_end__ = .;
  _edata = .;
 } > RAM
/*
	__got_start_flash__ = __etext + SIZEOF(.data);
	.got_info : AT (__got_start_flash__)
	{
		__got_info_start = .;
		__got_start = .;
		*(.got)
		. = ALIGN(4);
		__got_end = .;
		__got_plt_start = .;
		*(.got.plt)
		. = ALIGN(4);
		__igot_plt_start = .;
		*(.igot.plt)
		. = ALIGN(4);
		__dynamic_start = .;
		*(.dynamic)
		. = ALIGN(4);
		__got_info_end = .;
	} > RAM
	__got_end_flash__ = __got_start_flash__ + SIZEOF(.got_info);
*/
 .bss (.) (NOLOAD) : AT (ADDR(.bss))
 {
  . = ALIGN(4);
  __bss_start__ = .;
  *(.bss*)
  *(COMMON)
  . = ALIGN(4);
  __bss_end__ = .;
 } > RAM
 .reboot_custom_param (.) (NOLOAD) :
 {
  KEEP(*(.reboot_custom_param))
  . = ALIGN(4);
  __reboot_custom_param_end__ = .;
 } > RAM
 .heap (.) (NOLOAD) :
 {
  . = ALIGN(8);
  __HeapBase = .;
  __end__ = .;
  end = __end__;
  . += 0x1000;
  . = ALIGN(8);
  __HeapLimit = .;
 } > RAM
 /* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
 .stack_dummy (.) (COPY) :
 {
  . = 0x8000;
  . = ALIGN(8);
 } > RAM
 /* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
 __StackTop = ORIGIN(RAM) + LENGTH(RAM) - 0x4000 - 0x4000;
 __StackLimit = __StackTop - SIZEOF(.stack_dummy);
 PROVIDE(__stack = __StackTop);
 .a7_lastword (ORIGIN(RAM) + LENGTH(RAM) - 0x4000 - 0x4000) (NOLOAD) :
 {
  __a7_lastword_start = .;
  . = 0x4000;
  __a7_lastword_end = .;
  . = ALIGN(4);
 } > RAM
 .aos_lastword (ORIGIN(RAM) + LENGTH(RAM) - 0x4000) (NOLOAD) :
 {
  __aos_lastword_start = .;
  . = 0x4000;
  __aos_lastword_end = .;
  . = ALIGN(4);
 } > RAM
 ASSERT((0x20000000 +0x200000-0x00057000-0x00029000 -0x16000-0x4000) == __aos_lastword_start,
  "Adjust M33 and A7 DEBUG_CONFIG_LASTWORD_RAM_ADDR in k_config.h, if you change SRAM map")
 /* Check if data + heap + stack exceeds RAM limit */
 ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
 __free_ram = __StackLimit - __HeapLimit;
 /*. = __etext + SIZEOF(.data) + SIZEOF(.got_info);*/
 . = __etext + SIZEOF(.data);
 .system_info (.) :
 {
  KEEP(*(.system_info_list_size))
  KEEP(*(.system_info_list))
  . = ALIGN(4);
 } > FLASH
 .build_info (.) :
 {
  KEEP(*(.build_info))
  . = ALIGN(4);
 } > FLASH = 0x00000000
 /* The following section MUST be the last loaded section */
 .code_start_addr (.) :
 {
  LONG(0xBE57341D);
  LONG(ABSOLUTE(__flash_start));
 } > FLASH
 __flash_end = .;
 /* file system fixed to 0x4B8000 space, start addr:0xB38000 */
 .filesystem (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC)- 0x4C8000) :
 {
  __file_system_flash_start = .;
  . = 0x4B8000;
  __file_system_flash_stop = .;
 } > FLASH_NC
 .coredump_section (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000 -
  0 - 0x1000*2 - 0x1000 - 0x1000 -
  0 - 0 - 0 - 0) (NOLOAD) :
 {
  __coredump_section_start = .;
  . = 0;
  __coredump_section_end = .;
 } > FLASH_NC
 .ota_upgrade_log (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000 -
  0 - 0x1000*2 - 0x1000 - 0x1000 -
  0 - 0 - 0) (NOLOAD) :
 {
  __ota_upgrade_log_start = .;
  . = 0;
  __ota_upgrade_log_end = .;
 } > FLASH_NC
 .log_dump (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000 -
  0 - 0x1000*2 - 0x1000 - 0x1000 -
  0 - 0) (NOLOAD) :
 {
  __log_dump_start = .;
  . = 0;
  __log_dump_end = .;
 } > FLASH_NC
 .crash_dump (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000 -
  0 - 0x1000*2 - 0x1000 - 0x1000 -
  0) (NOLOAD) :
 {
  __crash_dump_start = .;
  . = 0;
  __crash_dump_end = .;
 } > FLASH_NC
 .custom_parameter (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000 -
  0 - 0x1000*2 - 0x1000 - 0x1000) (NOLOAD) :
 {
  __custom_parameter_start = .;
  . = 0x1000;
  __custom_parameter_end = .;
 } > FLASH_NC
 .persist_data (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000 -
  0 - 0x1000*2 - 0x1000) (NOLOAD) :
 {
  __persist_data_start = .;
  . = 0x1000;
  __persist_data_end = .;
 } > FLASH_NC
 .userdata (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000 -
  0 - 0x1000*2) (NOLOAD) :
 {
  __userdata_start = .;
  . = 0x1000*2;
  __userdata_end = .;
 } > FLASH_NC
 .audio (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000 -
  0) (NOLOAD) :
 {
  __aud_start = .;
  . = 0;
  __aud_end = .;
 } > FLASH_NC
 .reserved (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000 - 0x1000) (NOLOAD) :
 {
  __reserved_start = .;
  . = 0x1000;
  __reserved_end = .;
 } > FLASH_NC
 .factory (ORIGIN(FLASH_NC) + LENGTH(FLASH_NC) - 0x1000) (NOLOAD) :
 {
  __factory_start = .;
  . = 0x1000;
  __factory_end = .;
 } > FLASH_NC
 __tail_section_start = __ota_upgrade_log_start;
 ASSERT(((__tail_section_start) - 0x28000000 + 0x2C000000) >= __flash_end, "region FLASH overflowed")
 __free_flash = ((__tail_section_start) - 0x28000000 + 0x2C000000) - __flash_end;
}
