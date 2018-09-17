// See LICENSE for license details.

#ifndef _SIFIVE_CLIC_H
#define _SIFIVE_CLIC_H

#define CLIC_HART0          0x00800000
#define CLIC_MSIP           0x0000
#define CLIC_MSIP_size      0x4
#define CLIC_MTIMECMP       0x4000
#define CLIC_MTIMECMP_size  0x8
#define CLIC_MTIME          0xBFF8
#define CLIC_MTIME_size     0x8

#define CLIC_INTIP          0x000
#define CLIC_INTIE          0x400
#define CLIC_INTCFG         0x800
#define CLIC_CFG            0xc00

// These interrupt IDs are consistent across old and new mtvec modes
#define SSIPID              1
#define MSIPID              3
#define STIPID              5
#define MTIPID              7
#define SEIPID              9
#define MEIPID              11
#define CSIPID              12
#define LOCALINTIDBASE      16


#endif /* _SIFIVE_CLIC_H */ 
