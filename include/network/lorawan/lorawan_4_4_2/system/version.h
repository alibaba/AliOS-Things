/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __VERSION_H__
#define __VERSION_H__

#define LORA_MAC_VERSION   (uint32_t) 0x44020000
#define TEST_VERSION (uint32_t) 0x00000000  /*1 lsb is always 0 in releases   */
#define LRWAN_VERSION  (uint32_t) 0x00001110  /*3 next hex is i_cube release*/
#define VERSION   (uint32_t) (LORA_MAC_VERSION | LRWAN_VERSION | TEST_VERSION)
#define AT_VERSION_STRING   "3.1.3"
#define AT_VERSION_LORAWAN_STRING  "1.0.2"

#endif /*__VERSION_H__*/
