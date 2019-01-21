#ifndef ATTRS_H
#define ATTRS_H

#if defined(M_BUILD_ROM) && (M_BUILD_ROM == 1)
#define ATTRIBUTE_SECTION_FAST
#else
// define P1.
#define ATTRIBUTE_SECTION_FBOOT             __attribute__((section(".fast_boot_code")))
#define ATTRIBUTE_SECTION_OTA_FBOOT         __attribute__((section(".ota_fast_boot_code")))
#define ATTRIBUTE_SECTION_OTA_FBOOT_DATA    __attribute__((section(".ota_fast_boot_data")))
// define P2.
#define ATTRIBUTE_SECTION_RBOOT             __attribute__((section(".rboot")))
// define P3.
#define ATTRIBUTE_SECTION_FAST_TEXT         __attribute__((section(".prog_in_sram")))
#define ATTRIBUTE_SECTION_FAST              __attribute__((section(".prog_in_sram")))
//#define ATTRIBUTE_SECTION_FAST_TEXT         
//#define ATTRIBUTE_SECTION_FAST              
// define P4.
#define ATTRIBUTE_SECTION_SLOW_TEXT
// define P5.
#define ATTRIBUTE_SECTION_DATA
// define P5.
#define ATTRIBUTE_SECTION_FAST_DATA
// define P6.
#define ATTRIBUTE_SECTION_BSS
// define P7.
#define ATTRIBUTE_SECTION_RSRAM             __attribute__((section(".rsram")))
#endif

#endif /* end of include guard: ATTRS_H */
