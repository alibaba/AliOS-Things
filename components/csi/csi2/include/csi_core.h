/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     csi_core.h
 * @brief    CSI Core Layer Header File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#ifndef _CORE_H_
#define _CORE_H_

#include <stdint.h>

#if defined(__CK801__) || defined(__E801__)
#include <core/core_801.h>
#elif defined(__CK802__) || defined(__E802__) || defined(__E802T__) || defined(__S802__) || defined(__S802T__)
#include <core/core_802.h>
#elif defined(__CK804__) || defined(__E804D__) || defined(__E804DT__) || defined(__E804F__)  || defined(__E804FT__) || defined (__E804DF__) || defined(__E804DFT__)
#include <core/core_804.h>
#elif defined(__CK803__) || defined(__E803__) || defined(__E803T__) || defined(__S803__) || defined(__S803T__)
#include <core/core_803.h>
#elif defined(__CK805__) || defined(__I805__) || defined(__I805F__)
#include <core/core_805.h>
#elif defined(__CK610__)
#include <core/core_ck610.h>
#elif defined(__CK810__) || defined(__C810__) || defined(__C810T__) || defined(__C810V__) || defined(__C810VT__)
#include <core/core_810.h>
#elif defined(__CK807__) || defined(__C807__) || defined(__C807F__) || defined(__C807FV__) || defined(__R807__)
#include <core/core_807.h>
#elif defined(__riscv) && defined(CONFIG_CSKY_CORETIM)
#include <core/core_rv32_old.h>
#elif defined(__riscv) && (__riscv_xlen == 32)
#include <core/core_rv32.h>
#elif defined(__riscv) && (__riscv_xlen == 64)
#include <core/core_rv64.h>
#endif

#if defined(__riscv) && (__riscv_xlen == 32)
#include <core/csi_rv32_gcc.h>
#elif defined(__riscv) && (__riscv_xlen == 64)
#include <core/csi_rv64_gcc.h>
#elif defined(__csky__)
#include <core/csi_gcc.h>
#endif

#ifdef __arm__
#include <csi_core_cmsis.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CORE_H_ */
