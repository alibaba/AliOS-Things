/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Board configuration and pin definitions.
 */
#ifndef __OMV_BOARDCONFIG_H__
#define __OMV_BOARDCONFIG_H__
#include <esp_attr.h>
#include <esp_heap_caps.h>
#include "arm_compat.h"

//#define omv_malloc(size)  heap_caps_malloc(size, MALLOC_CAP_SPIRAM|MALLOC_CAP_32BIT|MALLOC_CAP_DMA)

// Architecture info
#define OMV_ARCH_STR            "OMV4 esp32s 1024" // 33 chars max
#define OMV_BOARD_TYPE          "esp32s3_ksdiy"
#define OMV_UNIQUE_ID_ADDR      &unique_id[0]
#define OMV_UNIQUE_ID_SIZE      3 // 3 words

extern char unique_id[OMV_UNIQUE_ID_SIZE*4];

// Needed by the SWD JTAG testrig - located at the bottom of the frame buffer stack.
#define OMV_SELF_TEST_SWD_ADDR  MAIN_FB()->bpp

// Flash sectors for the bootloader.
// Flash FS sector, main FW sector, max sector.
#define OMV_FLASH_LAYOUT        {1, 2, 15}

#define OMV_XCLK_MCO            (0U)
#define OMV_XCLK_TIM            (1U)
#define OMV_XCLK_OSC            (2U)

// Sensor external clock source.
#define OMV_XCLK_SOURCE         (OMV_XCLK_OSC)

// Sensor external clock timer frequency.
#define OMV_XCLK_FREQUENCY      (24000000)

// RAW buffer size
#define OMV_RAW_BUF_SIZE        (640*480*3)

// Enable hardware JPEG
#define OMV_HARDWARE_JPEG       (0)

// Enable WiFi debug
#define OMV_ENABLE_WIFIDBG      (1)

// Enable self-tests on first boot
#define OMV_ENABLE_SELFTEST     (1)

// If buffer size is bigger than this threshold, the quality is reduced.
// This is only used for JPEG images sent to the IDE not normal compression.
#define JPEG_QUALITY_THRESH     (640*480*3)

// Low and high JPEG QS.
#define JPEG_QUALITY_LOW        50
#define JPEG_QUALITY_HIGH       90

// FB Heap Block Size
#define OMV_UMM_BLOCK_SIZE      16

// Core VBAT for selftests
#define OMV_CORE_VBAT           "3.3"

// USB IRQn.
//#define OMV_USB_IRQN            (OTG_FS_IRQn)


#define OMV_FB_SIZE             (OMV_RAW_BUF_SIZE)      // FB memory: header + VGA/GS image
#define OMV_FB_ALLOC_SIZE       (384*1024)      // minimum fb alloc size
#ifdef OMV_FB_OVERLAY_MEMORY
#define OMV_FB_ALLOC_OVERLAY_SIZE (384*1024)
#endif
#define OMV_STACK_SIZE            (384*1024)
//#define OMV_HEAP_SIZE           (236*1024)

//#define OMV_LINE_BUF_SIZE       (3 * 1024)    // Image line buffer round(640 * 2BPP * 2 buffers).
//#define OMV_MSC_BUF_SIZE        (2*1024)      // USB MSC bot data
//#define OMV_VFS_BUF_SIZE        (1*1024)      // VFS sturct + FATFS file buffer (624 bytes)
#define OMV_JPEG_BUF_SIZE         (384 * 1024)   // IDE JPEG buffer (header + data).
#define OMV_HEAP_SIZE             (384*1024)

#endif //__OMV_BOARDCONFIG_H__
