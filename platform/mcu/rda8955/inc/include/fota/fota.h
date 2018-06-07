/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _FOTA_H_
#define _FOTA_H_

#include <stdint.h>
#include <stdbool.h>

#define FOTA_PACK_FILENAME_LEN_MAX (32)   // include trailing \0
#define FOTA_INDEX_FILE_NAME_LEN_MAX (31) // include trailing \0
#define FOTA_PACK_DEFAULT_FILENAME "fota.pack"
#define FOTA_INDEX_DEFAULT_FILENAME "fota.idx"

struct FOTA_CONTEXT_T;
typedef struct FOTA_CONTEXT_T FOTA_CONTEXT_T;
typedef void (*FOTA_PROGRESS_CALLBACK_T)(uint16_t total, uint16_t current);

typedef enum {
    FOTA_AREA_EMPTY,       // nothing in FOTA area
    FOTA_AREA_DOWNLOADING, // download in progress
    FOTA_AREA_DOWNLOADED,  // download finished and upgrade not started
    FOTA_AREA_UPGRADING,   // upgrade has started
    FOTA_AREA_UPGRADED     // upgrade done
} FOTA_AREA_STATUS_T;

#undef FLASH_SECTOR_SIZE
#define FLASH_SECTOR_SIZE (4 * 1024)

typedef struct fota_env
{
    const uint8_t *packFname;
    const uint8_t *idxFname;

    // The followings are only needed at upgrade.
    // When not needed, set to zero to pass check.
    uint32_t backupStart; // must be 64K aligned
    uint32_t backupSize;  // must be 64K (or larger)
    uint8_t *upgradeMemAddr;
    uint32_t upgradeMemSize;
} FOTA_ENV_T;

// =============================================================================
// External provided API, also memcpy/memset/strlen will be used
// =============================================================================
#define FOTA_FILE_INVALID (-1)
void *fotaFlashUncacheMap(unsigned address);
void fotaFlashErase(unsigned address, unsigned length);
void fotaFlashProgram(unsigned address, void *data, unsigned length);
void fotaFlashEraseProgram(unsigned address, void *data, unsigned length);
int fotaFileOpen(const char *fn, bool new_wr);
void fotaFileClose(int fp);
int fotaFileWrite(int fp, uint32_t offset, const void *data, uint32_t length);
int fotaFileRead(int fp, uint32_t offset, void *data, uint32_t buflen);
int fotaFileRenameAtomic(const char *oldname, const char *newname);
int fotaFileRemove(const char *path);

// =============================================================================
// fotaInit (download/upgrade)
// -----------------------------------------------------------------------------
/// Initialize FOTA engine.
///
/// @param fc           Pointer to the FOTA context
/// @param env          Fota basic environment
/// @return     false when parameter is invalid
// =============================================================================
unsigned fotaContextSize(void);
bool fotaInit(FOTA_CONTEXT_T *fc, FOTA_ENV_T *env);

// =============================================================================
// fotaGetStatus (download/upgrade)
// -----------------------------------------------------------------------------
/// Get status of fota area.
// =============================================================================
FOTA_AREA_STATUS_T fotaGetStatus(FOTA_CONTEXT_T *fc);

// =============================================================================
// fotaUpgrade (upgrade)
// -----------------------------------------------------------------------------
/// It should be called in boot to start FOTA upgrade or continue interrupted
/// FOTA upgrade. After upgrade, caller should check status again. If status is
/// UPGRADING, caller shouldn't try normal boot. The original data may be
/// destroyed, and caller can try other recovery method if available.
///
/// progress callback will be called periodic during upgrade.
// =============================================================================
bool fotaUpgrade(FOTA_CONTEXT_T *fc, FOTA_PROGRESS_CALLBACK_T progress);

// =============================================================================
// fotaDownloadFinished (download)
// -----------------------------------------------------------------------------
/// Update FOTA internal status to indicate upgrade available. There are sanity
/// check in it. And will return false if sanity check failed.
///
/// It is recommended to call fotaInvalidate at fail.
// =============================================================================
bool fotaDownloadFinished(FOTA_CONTEXT_T *fc);

// =============================================================================
// fotaInvalidate (download)
// -----------------------------------------------------------------------------
/// Discard information in FOTA area.
// =============================================================================
void fotaInvalidate(FOTA_CONTEXT_T *fc);

#endif
