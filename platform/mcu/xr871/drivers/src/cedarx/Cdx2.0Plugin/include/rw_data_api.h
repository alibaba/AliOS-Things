#ifndef RW_DATA_API_H
#define RW_DATA_API_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ad_cedarlib_com.h"
#include "AudioDec_Decode.h"

#ifdef __cplusplus
extern "C" {
#endif 

int ShowAbsBits(void *buf, int Len, Ac320FileRead *FileInfo);

void FlushAbsBits(int Len, Ac320FileRead *FileInfo);

int FREAD_AC320(void *buf, int Len, Ac320FileRead *FileInfo);

int FSeek_AC320(int Len, Ac320FileRead *FileInfo);

int FSeek_AC320_Origin(int Len, Ac320FileRead *FileInfo, int origin);

void FlushPtsAtOnce(Ac320FileRead *FileInfo);

int BigLitEndianCheck(Ac320FileRead *FileInfo, short data);

#ifdef __cplusplus
}
#endif

#endif
