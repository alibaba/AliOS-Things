/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __LINK_SYM_ARMCLANG_H__
#define __LINK_SYM_ARMCLANG_H__

#ifdef __ARMCC_VERSION

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_addr_map.h"

#ifdef ROM_BUILD

#define __rom_got_info_start                Image$$rom_got_info$$Base
#define __audio_const_rom_start             Image$$rom_audio_const$$Base
#define __audio_const_rom_end               Image$$rom_audio_const$$Limit
#define __audio_const_rom_size              Image$$rom_audio_const$$Length
#define __rom_text0_end                     Load$$rom_ramx$$Base
#define __rom_ramx_start                    Image$$rom_ramx$$Base
#define __rom_ramx_end                      Image$$rom_ramx$$Limit
#define __rom_etext                         Load$$rom_data$$Base
#define __rom_data_start__                  Image$$rom_data$$Base
#define __rom_data_end__                    Image$$rom_data$$Limit
#define __rom_bss_start__                   Image$$rom_bss$$Base
#define __rom_bss_end__                     Image$$rom_bss$$ZI$$Limit
#define __rom_HeapBase                      Image$$ARM_LIB_HEAP$$Base
#define __rom_HeapLimit                     Image$$ARM_LIB_HEAP$$ZI$$Limit
#define __rom_StackTop                      Image$$ARM_LIB_STACK$$ZI$$Limit
#define __rom_stack                         __rom_StackTop
#define __rom_StackLimit                    Image$$ARM_LIB_STACK$$Base
#define __cp_ramx_last_dummy_start          Image$$cp_ramx_last_dummy$$Base
#define __cp_ram_last_dummy_start           Image$$cp_ram_last_dummy$$Base
#define __cp_stack_limit                    Image$$cp_stack$$Base
#define __cp_stack_top                      Image$$cp_stack$$ZI$$Limit
#define __export_fn_rom                     Image$$rom_export_fn$$Base

#define __boot_sram_start_flash__           __rom_HeapLimit
#define __boot_sram_start__                 __rom_HeapLimit
#define __boot_sram_end_flash__             __rom_HeapLimit
#define __boot_bss_sram_start__             __rom_HeapLimit
#define __boot_bss_sram_end__               __rom_HeapLimit
#define __fast_sram_text_data_start_flash__ __rom_HeapLimit
#define __fast_sram_text_data_start__       __rom_HeapLimit
#define __fast_sram_text_data_end__         __rom_HeapLimit
#define __fast_sram_text_data_end_flash__   __rom_HeapLimit
#define __sram_text_data_start_flash__      __rom_HeapLimit
#define __sram_text_data_start__            __rom_HeapLimit
#define __sram_text_data_end_flash__        __rom_HeapLimit
#define __sram_bss_start__                  __rom_HeapLimit
#define __sram_bss_end__                    __rom_HeapLimit

#ifndef NOSTD
#define __bss_start__                       __rom_bss_start__
#define __bss_end__                         __rom_bss_end__
#define __end__                             __rom_HeapBase
/*
#define end                                 __rom_HeapBase
*/
#define __StackTop                          __rom_StackTop
#define __stack                             __rom_StackTop
#endif

#elif defined(PROGRAMMER)

#define __exec_struct_start                 Image$$exec_struct$$Base
#define __got_info_start                    Image$$got_info$$Base
#define __cust_cmd_init_tbl_start           Image$$cust_cmd_init_tbl$$Base
#define __cust_cmd_init_tbl_end             Image$$cust_cmd_init_tbl$$Limit
#define __cust_cmd_hldr_tbl_start           Image$$cust_cmd_hdlr_tbl$$Base
#define __cust_cmd_hldr_tbl_end             Image$$cust_cmd_hdlr_tbl$$Limit
#define __etext                             Load$$data$$Base
#define __data_start__                      Image$$data$$Base
#define __data_end__                        Image$$data$$Limit
#define __bss_start__                       Image$$bss$$Base
#define __bss_end__                         Image$$bss$$ZI$$Limit
#define __HeapBase                          Image$$ARM_LIB_HEAP$$Base
#define __end__                             __HeapBase
/*
#define end                                 __HeapBase
*/
#define __HeapLimit                         Image$$ARM_LIB_HEAP$$ZI$$Limit
#define __StackTop                          Image$$ARM_LIB_STACK$$ZI$$Limit
#define __stack                             __StackTop
#define __StackLimit                        Image$$ARM_LIB_STACK$$Base

#define __boot_sram_start_flash__           __HeapLimit
#define __boot_sram_start__                 __HeapLimit
#define __boot_sram_end_flash__             __HeapLimit
#define __boot_bss_sram_start__             __HeapLimit
#define __boot_bss_sram_end__               __HeapLimit
#define __fast_sram_text_data_start_flash__ __HeapLimit
#define __fast_sram_text_data_start__       __HeapLimit
#define __fast_sram_text_data_end__         __HeapLimit
#define __fast_sram_text_data_end_flash__   __HeapLimit
#define __sram_text_data_start_flash__      __HeapLimit
#define __sram_text_data_start__            __HeapLimit
#define __sram_text_data_end_flash__        __HeapLimit
#define __sram_bss_start__                  __HeapLimit
#define __sram_bss_end__                    __HeapLimit
#define __cp_stack_limit                    __HeapLimit
#define __cp_stack_top                      __HeapLimit

#else

#define Boot_Loader                         __main
#define __flash_start                       Image$$boot_struct$$Base
#define __userdata_pool_end__               Image$$userdata_pool$$Base
#define __boot_sram_start_flash__           Load$$boot_text_sram$$Base
#define __boot_sram_start__                 Image$$boot_text_sram$$Base
#define __boot_sram_end_flash__             Load$$boot_data_sram$$Limit
/*
 * CAUTION:
 * If the section name of BSS variables has no ".bss." prefix, they will be considered as DATA, not BSS (ZI) !
 */
#define __boot_bss_sram_start__             Image$$boot_bss_sram$$Base
#define __boot_bss_sram_end__               Image$$boot_bss_sram$$ZI$$Limit
#define __fast_sram_text_data_start_flash__ Load$$fast_text_sram$$Base
#define __fast_sram_text_data_start__       Image$$fast_text_sram$$Base
#define __fast_sram_text_data_end__         Image$$fast_text_sram$$Limit
#define __fast_sram_text_data_end_flash__   Load$$fast_text_sram$$Limit

#define __cp_text_sram_start_flash__        Load$$cp_text_sram$$Base
#define __cp_text_sram_exec_start__         Image$$cp_text_sram$$Base
#define __cp_text_sram_exec_end__           Image$$cp_text_sram$$Limit
#define __cp_text_sram_start                Image$$cp_text_sram_start$$Base
#define __cp_text_sram_end                  Image$$cp_text_sram_start$$Limit
#define __cp_data_sram_start_flash__        Load$$cp_data_sram$$Base
#define __cp_data_sram_start                Image$$cp_data_sram$$Base
#define __cp_data_sram_end                  Image$$cp_data_sram$$Limit
#define __cp_sram_end_flash__               Load$$cp_data_sram$$Limit
#define __cp_bss_sram_start                 Image$$cp_bss_sram$$Base
#define __cp_bss_sram_end                   Image$$cp_bss_sram$$ZI$$Limit
#define __cp_stack_limit                    Image$$cp_stack$$Base
#define __cp_stack_top                      Image$$cp_stack$$ZI$$Limit

#define __overlay_text_start__              Image$$overlay_start$$Base
#define __overlay_text_exec_start__         Image$$overlay_text0$$Base

#define __load_start_overlay_text0          Load$$overlay_text0$$Base
#define __load_stop_overlay_text0           Load$$overlay_text0$$Limit
#define __load_start_overlay_text1          Load$$overlay_text1$$Base
#define __load_stop_overlay_text1           Load$$overlay_text1$$Limit
#define __load_start_overlay_text2          Load$$overlay_text2$$Base
#define __load_stop_overlay_text2           Load$$overlay_text2$$Limit
#define __load_start_overlay_text3          Load$$overlay_text3$$Base
#define __load_stop_overlay_text3           Load$$overlay_text3$$Limit
#define __load_start_overlay_text4          Load$$overlay_text4$$Base
#define __load_stop_overlay_text4           Load$$overlay_text4$$Limit
#define __load_start_overlay_text5          Load$$overlay_text5$$Base
#define __load_stop_overlay_text5           Load$$overlay_text5$$Limit
#define __load_start_overlay_text6          Load$$overlay_text6$$Base
#define __load_stop_overlay_text6           Load$$overlay_text6$$Limit
#define __load_start_overlay_text7          Load$$overlay_text7$$Base
#define __load_stop_overlay_text7           Load$$overlay_text7$$Limit

#define __overlay_text_exec_end__           Image$$overlay_text_end$$Base
#define __overlay_data_start__              Image$$overlay_data0$$Base

#define __load_start_overlay_data0          Load$$overlay_data0$$Base
#define __load_stop_overlay_data0           Load$$overlay_data0$$Limit
#define __load_start_overlay_data1          Load$$overlay_data1$$Base
#define __load_stop_overlay_data1           Load$$overlay_data1$$Limit
#define __load_start_overlay_data2          Load$$overlay_data2$$Base
#define __load_stop_overlay_data2           Load$$overlay_data2$$Limit
#define __load_start_overlay_data3          Load$$overlay_data3$$Base
#define __load_stop_overlay_data3           Load$$overlay_data3$$Limit
#define __load_start_overlay_data4          Load$$overlay_data4$$Base
#define __load_stop_overlay_data4           Load$$overlay_data4$$Limit
#define __load_start_overlay_data5          Load$$overlay_data5$$Base
#define __load_stop_overlay_data5           Load$$overlay_data5$$Limit
#define __load_start_overlay_data6          Load$$overlay_data6$$Base
#define __load_stop_overlay_data6           Load$$overlay_data6$$Limit
#define __load_start_overlay_data7          Load$$overlay_data7$$Base
#define __load_stop_overlay_data7           Load$$overlay_data7$$Limit

#define __sram_text_data_start_flash__      Load$$sram_text$$Base
#define __sram_text_data_start__            Image$$sram_text$$Base
#define __sram_text_data_end_flash__        Load$$sram_data$$Limit
#define __sram_bss_start__                  Image$$sram_bss$$Base
#define __sram_bss_end__                    Image$$sram_bss$$ZI$$Limit
#define __etext                             Load$$data$$Base
#define __data_start__                      Image$$data$$Base
#define __data_end__                        Image$$data$$Limit
#define __bss_start__                       Image$$bss$$Base
#define __bss_end__                         Image$$bss$$ZI$$Limit
#define __HeapBase                          Image$$ARM_LIB_HEAP$$Base
#define __end__                             __HeapBase
/*
#define end                                 __HeapBase
*/
#define __HeapLimit                         Image$$ARM_LIB_HEAP$$ZI$$Limit
#define __StackTop                          Image$$ARM_LIB_STACK$$ZI$$Limit
#define __stack                             __StackTop
#define __StackLimit                        Image$$ARM_LIB_STACK$$Base
#define __flash_end                         Image$$code_start_addr$$Limit
#define __custom_parameter_start            Image$$custom_parameter$$Base
#define __custom_parameter_end              Image$$custom_parameter$$ZI$$Limit
#define __userdata_start                    Image$$userdata$$Base
#define __userdata_end                      Image$$userdata$$ZI$$Limit
#define __aud_start                         Image$$audio$$Base
#define __aud_end                           Image$$audio$$ZI$$Limit
#define __factory_start                     Image$$factory$$Base
#define __factory_end                       Image$$factory$$ZI$$Limit

#endif

#ifdef __cplusplus
}
#endif

#endif

#endif

