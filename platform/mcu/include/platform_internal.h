/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __PlatformInternal_h__
#define __PlatformInternal_h__

#include "common.h"

void init_clocks( void );
void init_memory( void );
void init_architecture( void) ;
void init_platform( void) ;
void init_platform_bootloader( void );
void startApplication( uint32_t app_addr );

#endif // __PlatformInternal_h__

