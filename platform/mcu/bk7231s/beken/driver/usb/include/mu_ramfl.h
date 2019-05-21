/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * MUSB-MicroSW RAM-based "file" storage.
 * $Revision: 1.3 $
 */

#ifndef __MUSB_RAMFILE_H__
#define __MUSB_RAMFILE_H__

#include "mu_tools.h"

#define MUSB_RAMFILE_MAX_NAME	63

/**
* MUSB_RamFileInfo.
* Information about a file stored in RAM using this API.
* @field dwSize file size, in bytes
* @field szName NUL-terminated ASCII filename
* @field pData file data
*/
typedef struct
{
    uint32_t dwSize;
    const char szName[MUSB_RAMFILE_MAX_NAME + 1];
    uint8_t *pData;
} MUSB_RamFileInfo;

/**
* Create a file.
* Create a new file or silently replace an existing one of the same name.
* @param dwSize size for file
* @param pszName NUL-terminated ASCII name
* (will be silently truncated if necessary)
* @return file info (with allocated buffer) on success
* @return NULL on failure (out of memory)
*/
extern MUSB_RamFileInfo *MUSB_RamFileCreate(uint32_t dwSize, const char *pszName);

/**
* Delete a file.
* Delete the file with the given name.
* @param pszName NUL-terminated ASCII name
* (will be truncated if necessary).
* @return TRUE on success
* @return FALSE on failure (file not found)
*/
extern uint8_t MUSB_RamFileDelete(const char *pszName);

/**
* Count files.
* Return the number of files known.
* @return file count
*/
extern uint16_t MUSB_RamFileCount(void);

/**
* Get file info.
* Get information about the file at the given index.
* @param wIndex index (counting from 0) of file
* @return info on success
* @return NULL on failure (invalid index)
*/
extern const MUSB_RamFileInfo *MUSB_RamFileGetInfo(uint16_t wIndex);

#endif	/* multiple inclusion protection */
