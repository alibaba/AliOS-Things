#ifndef _REG_TDM_H_
#define _REG_TDM_H_

#include "plat_types.h"
#include "reg_i2sip.h"
// 0  enable offset
#define TDM_ENABLE_SHIFT                  0x0
#define TDM_ENABLE                            0x1
#define TDM_DISABLE                           0x0
// 1  FS_ASSERTED
#define TDM_MODE_FS_ASSERTED_SHIFT        0x1
#define TDM_MODE_FS_ASSERTED_AT_FIRST         0x0
#define TDM_MODE_FS_ASSERTED_AT_LAST          0x1
// 2  FS_EDGE
#define TDM_FS_EDGE_SHIFT                 0x2
#define TDM_FS_EDGE_POSEDGE                   0x0
#define TDM_FS_EDGE_NEGEDGE                   0x1
// 3  FRAME_WIDTH
#define TDM_FRAME_WIDTH_SHIFT             0x3
#define TDM_FRAME_WIDTH_16_CYCLES             0x1
#define TDM_FRAME_WIDTH_32_CYCLES             0x2
#define TDM_FRAME_WIDTH_64_CYCLES             0x3
#define TDM_FRAME_WIDTH_128_CYCLES            0x4
#define TDM_FRAME_WIDTH_256_CYCLES            0x5
// 6 FRAME_WIDTH
#define TDM_FS_WIDTH_SHIFT                0x6
#define TDM_FS_WIDTH_1_CYCLE                  0x0
#define TDM_FS_WIDTH_8_CYCLES                 0x1
#define TDM_FS_WIDTH_16_CYCLES                0x2
#define TDM_FS_WIDTH_32_CYCLES                0x3
#define TDM_FS_WIDTH_64_CYCLES                0x4
#define TDM_FS_WIDTH_128_CYCLES               0x5
#define TDM_FS_WIDTH_FRAME_LENGTH_1_CYCLES    0x7
// 9 SLOT_WIDTH
#define TDM_SLOT_WIDTH_SHIFT              0x9
#define TDM_SLOT_WIDTH_32_BIT                 0x0
#define TDM_SLOT_WIDTH_16_BIT                 0x1

#define TDM_DATA_OFFSET_SHIT              0xa
#define TDM_DATA_OFFSET_MIN                   0x0
#define TDM_DATA_OFFSET_SIZE                  0x0
#define TDM_DATA_OFFSET_MAX                   0x7

#endif
