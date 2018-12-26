/*
 * Copyright (c) 2008-2017 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxUtfCode.h
 * Description : Parse Utf Code head file
 * History :
 *
 */

#ifndef UTF_CODE_H
#define UTF_CODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "BaseUtils.h"

void CdxUtf16toUtf8(const cdx_uint16* src, size_t src_len, char* dst);
void CdxUtf32toUtf8(const cdx_uint32* src, size_t src_len, char* dst);

int32_t CdxUtf16toUtf8Length(const cdx_uint16 *src, size_t src_len);
int32_t CdxUtf32toUtf8Length(const cdx_uint32 *src, size_t src_len);

char* CdxAllocFromUTF8(const char* in, size_t len);
char* CdxAllocFromUTF16(const cdx_uint16* in, size_t len);
char* CdxAllocFromUTF32(const cdx_uint32* in, size_t len);

#endif
