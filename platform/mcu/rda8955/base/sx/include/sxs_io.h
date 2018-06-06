/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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




#ifndef __SXS_IO_H__
#define __SXS_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sxs_type.h"
#include "sxr_cpu.h"
#include <stdarg.h>

#define PAL_DBG_PORT_IDX 0

#ifdef __DUALSIM__
/* replace __M2A and __STT by __MSSC and __RCO in dual SIM project*/
typedef enum {__SXR, __PAL, __L1A, __L1S, __LAP, __RLU, __RLD,
              __LLC, __MM,  __CC,  __SS,  __SMS,  __SM, __SND, __API,
              __MMI, __SIM, __AT , __MSSC, __RCO, __RRI, __RRD, __RLP,
              __HAL, __BCPU, __CSW, __EDRV, __MCI, __SVC1, __SVC2, __WCPU,
              SXS_NB_ID
             } sxs_TraceId_t;
#define __STT  SXS_NB_ID
#else
typedef enum {__SXR, __PAL, __L1A, __L1S, __LAP, __RLU, __RLD,
              __LLC, __MM,  __CC,  __SS,  __SMS,  __SM, __SND, __API,
              __MMI, __SIM, __AT , __M2A, __STT, __RRI, __RRD, __RLP,
              __HAL, __BCPU, __CSW, __EDRV, __MCI, __SVC1, __SVC2, __WCPU,
              SXS_NB_ID
             } sxs_TraceId_t;
#endif /* DUALSIM */

#define _SXR    TID(__SXR)
#define _PAL    TID(__PAL)
#define _L1A    TID(__L1A)
#define _L1S    TID(__L1S)
#define _LAP    TID(__LAP)
#define _RLU    TID(__RLU)
#define _RLD    TID(__RLD)
#define _LLC    TID(__LLC)
#define _MM     TID(__MM)
#define _CC     TID(__CC)
#define _SS     TID(__SS)
#define _SMS    TID(__SMS)
#define _SM     TID(__SM)
#define _SND    TID(__SND)
#define _API    TID(__API)
#define _MMI    TID(__MMI)
#define _SIM    TID(__SIM)
#define _AT     TID(__AT)
#define _M2A    TID(__M2A)
#define _STT    TID(__STT)
#define _RRI    TID(__RRI)
#define _RRD    TID(__RRD)
#define _RLP    TID(__RLP)
#define _HAL    TID(__HAL)
#define _BCPU   TID(__BCPU)
#define _CSW    TID(__CSW)
#define _EDRV   TID(__EDRV)
#define _MCI    TID(__MCI)
#define _SVC1   TID(__SVC1)
#define _SVC2   TID(__SVC2)
#define _WCPU   TID(__WCPU)

#define _INTERRAT   _WCPU

#ifdef __DUALSIM__
#define _MSSC   TID(__MSSC)
#define _RCO    TID(__RCO)
#endif //__DUALSIM__


#ifdef __SXS_IO_VAR__

#ifdef __DUALSIM__
const ascii * const sxs_TraceDesc [SXS_NB_ID] =
{
    "SXR", "PAL", "L1A", "L1S", "LAP", "RLU", "RLD",
    "LLC", "MM ", "CC ", "SS ", "SMS", "SM ", "SND", "API",
    "MMI", "SIM", "AT ", "MSSC", "RCO", "RRI", "RRD", "RLP",
    "HAL", "BCPU", "CSW", "EDRV", "MCI", "SV1", "SV2", "WCPU"
};
#else //DUALSIM
const ascii * const sxs_TraceDesc [SXS_NB_ID] =
{
    "SXR", "PAL", "L1A", "L1S", "LAP", "RLU", "RLD",
    "LLC", "MM ", "CC ", "SS ", "SMS", "SM ", "SND", "API",
    "MMI", "SIM", "AT ", "M2A", "STT", "RRI", "RRD", "RLP",
    "HAL", "BCPU", "CSW", "EDRV", "MCI", "SV1", "SV2", "WCPU"
};
#endif//DUALSIM

#else
extern const u8 * const sxs_TraceDesc [SXS_NB_ID];
#endif

void sxs_IoInit  (void);
void sxs_fprintf (u32 Id, const ascii *Fmt, ...);
void sxs_vprintf (u32 Id, const ascii *Fmt, va_list argPtr);
void sxs_Dump    (u32 Id, const ascii *Fmt, const void *Data, u16 Size);
void sxs_Raise   (u32 Id, const ascii *Fmt, ...);
void sxs_SetTraceLevel (u8 Id, u16 LevelBitMap);
void sxs_SetSpy  (u32 SpyEnableBitmap);
void sxs_BackTrace (u32 Ra, u32 *Sp);
void sxs_SetTraceEnabled(u8 enabled);

#ifndef SXS_NO_PRINTF
#define SXS_TRACE(ID, format, ...)  sxs_fprintf(ID,format, ##__VA_ARGS__)
#else
#define SXS_TRACE(ID, format, ...)
#endif

#ifndef SXS_NO_DUMP
#define SXS_DUMP(ID, format, ...)  sxs_Dump(ID,format, ##__VA_ARGS__)
#else
#define SXS_DUMP(ID, format, ...)
#endif

#define __SXS_TRACE_ENABLE__

#ifdef __SXS_TRACE_ENABLE__
#define SXS_FPRINTF(a)  SXS_TRACE   a
#define SXS_RAISE(a)    sxs_Raise   a
#else
#define SXS_FPRINTF(a)  {}
#define SXS_RAISE(a)    {}
#endif


#define TID_POS         0
#define TID_MSK         (0x1F << TID_POS)               /* 5 bits for user Id.  */
#define TID(Id)         ((u8)((Id << TID_POS) & TID_MSK))
#define TGET_ID(Id)     (u8)((Id & TID_MSK) >> TID_POS)
#define TLEVEL_POS      5
#define TLEVEL_MSK      (0x0F << TLEVEL_POS)            /* 4 bits for level.    */
#define TLEVEL(Lev)     ((((Lev)-1) << TLEVEL_POS) & TLEVEL_MSK)
#define TGET_LEVEL(Id)  (u8)((Id & TLEVEL_MSK) >> TLEVEL_POS)
#define TIDU_POS        9
#define TIDU            (1 << TIDU_POS)                 /* Do not display id.*/
#define TABORT_POS      10
#define TABORT          (1 << TABORT_POS)               /* Abort indicator for abort exception. */
#define TRAISE_POS      11
#define TRAISE          (1 << TRAISE_POS)               /* Raise indicator for catched exception. */
#define TDB_POS         12
#ifdef __SXS_STR_TO_ID__
#define TDB             (1 << TDB_POS)                  /* Data base indicator: format string to be found in data base. */
#else
#define TDB             0
#endif

#define TSMAP_POS       15
#define TSMAP_MSK       (0x3F << TSMAP_POS)             /* 6 bits for string parameter bit map. */
#define TSMAP(SMap)     ((SMap << TSMAP_POS) & TSMAP_MSK)
#define TGET_SMAP(Id)   (u8)((Id & TSMAP_MSK) >> TSMAP_POS)
#define TSTDOUT_POS     21
#define TSTDOUT         (1 << TSTDOUT_POS)              /* Stdout flow, trace setting independent. */
#define TARG_POS        22
#define TARG            (1 << TARG_POS)                 /* Argument number indicator.*/
#define TNB_ARG_POS     23
#define TNB_ARG_MSK     (0x07 << TNB_ARG_POS)           /* 3 bits for number of aguments. */
#define TNB_ARG(Nb)     (((Nb << TNB_ARG_POS) & TNB_ARG_MSK) | TARG)
#define TGET_NB_ARG(Id) (u8)((Id & TNB_ARG_MSK) >> TNB_ARG_POS)

#define SXS_DEFAULT_NB_ARG 6

#ifdef __SXS_STR_TO_ID__
#define TSTR(Str,Id) (void *)Id
#else
#define TSTR(Str,Id) Str
#endif


#define SXS_BOOT_PATTERN            0xCAFEBABE
#define SXS_RMT_BUFFER_SAVED_FLAG   0xA81D40E2

typedef struct
{
    u32 Pattern;
    u8  ExceptionCount;
    u8  DwlReq;
    u8  WDog;
    u8  ExceptionExpected;
    u8  ExceptionCountChk;
    u32 RmtBufferSaved;
    void *DbgBuffer;
    u32 Lr;
    u32 Sp;
    u32 Spsr;
} sxs_Boot_t;

#if defined (__SXS_IO_VAR__) && defined (__TARGET__)
sxs_Boot_t *const sxs_Boot = (sxs_Boot_t *)SXS_BOOT_DIAG_AREA;
#else
extern sxs_Boot_t *const sxs_Boot;
#endif

typedef struct
{
    u32 SpyEnableBitmap;
    u16 TraceBitMap [SXS_NB_ID];
} sxs_IoCtx_t;

#ifdef __SXS_IO_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

DefExtern sxs_IoCtx_t sxs_IoCtx;
DefExtern s8 sxs_StdErr;

#undef DefExtern

#ifdef __SXS_IO_VAR__
s16 sxs_IoCtxHandle = -1;
#else
extern s16 sxs_IoCtxHandle;
#endif

#ifndef __SXS_STD_FILE_LIB__

#define SXS_O_APPEND      (1 << 0)   // Moves file pointer to end of file before every write operation.
#define SXS_O_BINARY      0          // Opens file in binary mode. No Effect as file are assumed binary.
#define SXS_O_CREAT       (1 << 1)   // Creates and opens new file for writing. If file specified by filename exists, existing file is opened for writing.
#define SXS_O_RDONLY      (1 << 2)   // Opens file for reading only.
#define SXS_O_WRONLY      (1 << 3)   // Opens file for writing only.
#define SXS_O_RDWR        (SXS_O_RDONLY|SXS_O_WRONLY) // Opens file for both reading and writing.
#define SXS_O_TRUNC       (1 << 5)   // Opens file and truncates it to zero length. Used with, _O_CREAT opens an existing file or creates a new file.
#define SXS_O_TEMPORARY   (1 << 6)   // Create file as temporary. File is created in stacked bank and is deleted when stacked bank is full.
#define SXS_O_SHORT_LIVED (1 << 7)   // Create file as temporary. File is created in dynamic bank

#define SXS_S_IREAD       (1 << 0)   // Reading only permitted
#define SXS_S_IWRITE      (1 << 1)   // Writing permitted (effectively permits reading and writing)

#define SXS_SEEK_SET 0               // Beginning of file
#define SXS_SEEK_CUR 1               // Current position of file pointer
#define SXS_SEEK_END 2               // End of file

s16  sxs_Open  (const ascii *FileName, s16 OFlag, s16 PMode);
s16  sxs_Close (s16 Handle);
s16  sxs_Read  (s16 Handle, void *Buffer, u16 Count);
s16  sxs_Write (s16 Handle, const void *Buffer, u16 Count);
s32  sxs_LSeek (s16 Handle, u32 Offset, u16 Origin);

#else

#include "io.h"
#include "fcntl.h"
#include <sys/stat.h>

#define SXS_O_APPEND      O_APPEND
#define SXS_O_BINARY      O_BINARY
#define SXS_O_CREAT       O_CREAT
#define SXS_O_RDONLY      O_RDONLY
#define SXS_O_WRONLY      O_WRONLY
#define SXS_O_RDWR        O_RDWR
#define SXS_O_TRUNC       O_TRUNC
#define SXS_O_TEMPORARY   O_TEMPORARY
#define SXS_O_SHORT_LIVED O_SHORT_LIVED

#define SXS_S_IREAD       S_IREAD
#define SXS_S_IWRITE      S_IWRITE

#define SXS_SEEK_SET      SEEK_SET
#define SXS_SEEK_CUR      SEEK_CUR
#define SXS_SEEK_END      SEEK_END

#define sxs_Open     open
#define sxs_Close    close
#define sxs_Write    write
#define sxs_Read     read
#define sxs_LSeek    lseek

#endif

#define SXS_FILE_NAME_MAX_LEN 12

typedef /* __packed TBC */ struct
{
    ascii Name [SXS_FILE_NAME_MAX_LEN];
    u16   OFlag;
    u16   Offset;
    u16   Size;
} sxs_FileDesc_t;

void sxs_DelFile (s16 Handle);
void sxs_CheckFiles (void);


#define SPY_FRAME_END                   0x7E
#define SPY_CRC_16_SEED                 0xFFFF
#define SPY_MAX_ESCAPE_DATA_SIZE        2048
#define SPY_SEND_BUF_NUM                       2

#define UTS_TIME_STAMP_LENGTH         6
#define UTS_WCDMA_MAX_SINGNAL_BYTES 2048

typedef struct
{
    u8                  bCmd;       /* 0xab : 171 */
    u8                  abTime[UTS_TIME_STAMP_LENGTH];
    u32                 lLogicalChannel;
    u32                 lTransportChannel;
    u8                  bMsgId;
    u16                 wLength;
    u8                  abMessage[UTS_WCDMA_MAX_SINGNAL_BYTES];
#ifdef CSPY_ADD_ABSOLUTE_TIME
    u32                 wAbsoluteTime;
#endif
} PACKED UtsCmdWcdmaSignalingEvent_t;

#ifdef __cplusplus
}
#endif

#endif

