/**
 ****************************************************************************************
 *
 * @file version.h
 *
 * @brief Definition of version macros.
 *
 * This file should be included if any version related macro is needed.
 * It is modified automatically by build tool, and should seldom be
 * touched manually.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */
#ifndef _VERSION_H_
#define _VERSION_H_

/**
 ****************************************************************************************
 * @addtogroup MACSW
 * @brief Versioning
 * @{
 ****************************************************************************************
 */

// Version has the form Major.minor.release.patch
// The version string is "vMM.mm.rr.pp"
// Each number should fit in a 8 bits unsigned (0 to 255)
/// Major version number
#define NX_VERSION_MAJ      5
/// Minor version number
#define NX_VERSION_MIN      0
/// Release number
#define NX_VERSION_REL      4
/// Patch number
#define NX_VERSION_PAT      0

/// Version word
#define NX_VERSION        ((NX_VERSION_MAJ << 24) | (NX_VERSION_MIN << 16) |             \
                           (NX_VERSION_REL << 8) | NX_VERSION_PAT)

/// Version string (global variable defined in src/build/build_version.c)
extern const char nx_version_str[];
/// Macro for previous variable. Do not use!
#define NX_VERSION_STR      "v5.0.4.0"

/// Date at which the system is built (defined in build_version.c)
extern const char nx_build_date[];

/// @}

#endif // _VERSION_H_
