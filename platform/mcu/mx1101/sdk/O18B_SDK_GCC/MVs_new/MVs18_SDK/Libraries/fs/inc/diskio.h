//maintainer lilu
/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file  R0.07   (C)ChaN, 2009
/-----------------------------------------------------------------------*/

#ifndef __DISKIO__
#define __DISKIO__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"


/* Status of Disk Functions */
typedef uint8_t	DSTATUS;

/* Results of Disk Functions */
typedef enum
{
    RES_OK = 0,		/* 0: Successful */
    RES_ERROR,		/* 1: R/W Error */
    RES_WRPRT,		/* 2: Write Protected */
    RES_NOTRDY,		/* 3: Not Ready */
    RES_PARERR,		/* 4: Invalid Parameter */
    RES_TIMEOUT		/* 5: Time Out */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize(uint8_t);
DSTATUS disk_status(uint8_t);
DRESULT disk_read(uint8_t, uint8_t*, uint32_t, uint8_t);
DRESULT disk_write(uint8_t, const uint8_t*, uint32_t, uint8_t);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
