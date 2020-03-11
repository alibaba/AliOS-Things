/****************************************************************************
 *
 * @file     tle_device.h
 *           Infineon TLE Device Series
 * @version  V1.0.0
 * @date     16 Feb 2015
 *
   Copyright (C) 2015 Infineon Technologies AG. All rights reserved.
 *
 *
 * @par
 * Infineon Technologies AG (Infineon) is supplying this software for use with 
 * Infineon's microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such microcontrollers.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED AS IS.  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

#ifndef _TLE_DEVICE_H
#define _TLE_DEVICE_H

/* Family definitions  */
#define    TLE98      (98)

/* Series definitions  */
#define    TLE986     (986)
#define    TLE987     (987)

/* Device definitions  */
#define    TLE9861    (9861)
#define    TLE9867    (9867)
#define    TLE9869    (9869)
#define    TLE9871    (9871)
#define    TLE9873    (9873)
#define    TLE9877    (9877)
#define    TLE9879    (9879)

/* Package definitions */
#define    BGA144     (1)
#define    LQFP144    (2)
#define    LQFP100    (3)
#define    BGA64      (4)
#define    LQFP64     (5)
#define    VQFN48     (6)
#define    TSSOP38    (7)
#define    TSSOP28    (8)
#define    TSSOP16    (9) 
#define    VQFN24     (10)
#define    VQFN40     (11)


#if defined(TLE9861QXA20)	
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE986
#define UC_DEVICE       TLE9861
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (36UL)
#define ProgFlashSize   (0x8000U)
#define RAMSize         (0xC00U)

#elif defined(TLE9867QXA20)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE986
#define UC_DEVICE       TLE9867
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (64UL)
#define ProgFlashSize   (0xF000U)
#define RAMSize         (0x1800U)

#elif defined(TLE9867QXA40)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE986
#define UC_DEVICE       TLE9867
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (64UL)
#define ProgFlashSize   (0xF000U)
#define RAMSize         (0x1800U)

#elif defined(TLE9869QXA20)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE986
#define UC_DEVICE       TLE9869
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (128UL)
#define ProgFlashSize   (0x1F000U)
#define RAMSize         (0x1800U)

#elif defined(TLE9871QXA20)	
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE987
#define UC_DEVICE       TLE9871
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (36UL)
#define ProgFlashSize   (0x8000U)
#define RAMSize         (0xC00U)

#elif defined(TLE9873QXW40)	
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE987
#define UC_DEVICE       TLE9873
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (48UL)
#define ProgFlashSize   (0xB000U)
#define RAMSize         (0xC00U)

#elif defined(TLE9877QXA20)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE987
#define UC_DEVICE       TLE9877
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (64UL)
#define ProgFlashSize   (0xF000U)
#define RAMSize         (0x1800U)

#elif defined(TLE9877QXA40)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE987
#define UC_DEVICE       TLE9877
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (64UL)
#define ProgFlashSize   (0xF000U)
#define RAMSize         (0x1800U)

#elif defined(TLE9877QXW40)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE987
#define UC_DEVICE       TLE9877
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (64UL)
#define ProgFlashSize   (0xF000U)
#define RAMSize         (0x1800U)

#elif defined(TLE9879QXA20)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE987
#define UC_DEVICE       TLE9879
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (128UL)
#define ProgFlashSize   (0x1F000U)
#define RAMSize         (0x1800U)

#elif defined(TLE9879QXA40)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE987
#define UC_DEVICE       TLE9879
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (128UL)
#define ProgFlashSize   (0x1F000U)
#define RAMSize         (0x1800U)

#elif defined(TLE9879QXW40)
#define UC_FAMILY       TLE98
#define UC_SERIES       TLE987
#define UC_DEVICE       TLE9879
#define UC_PACKAGE      VQFN48
#define UC_FLASH        (128UL)
#define ProgFlashSize   (0x1F000U)
#define RAMSize         (0x1800U)

#else
#error "tle_device.h: device not supported"
#endif 	    

#if UC_SERIES == TLE986
#include <TLE986x.h>
#include <Types.h>

#elif UC_SERIES == TLE987
#include <TLE987x.h>
#include <Types.h>
#endif

#endif
