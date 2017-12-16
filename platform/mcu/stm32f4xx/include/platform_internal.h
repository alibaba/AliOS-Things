/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
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

