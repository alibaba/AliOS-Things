/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : RecoderWriter.h
 * Description : RecoderWriter
 * History :
 *
 */


#ifndef __RECODER_CBWRITER_H__
#define __RECODER_CBWRITER_H__

#include "CdxWriter.h"

typedef uint32_t (*CdxWriterCb)(void *buf, uint32_t size);

typedef struct RecoderCbWriter RecoderCbWriterT;
struct RecoderCbWriter
{
    CdxWriterT cdx_writer;
    int        file_mode;
    int        fd_file;
	CdxWriterCb cb;
};

typedef struct WriterDataDest {
	char *pUrl;
	void *pArg;
} WriterDataDest;

CdxWriterT *CdxCbWriterCreat(WriterDataDest *dataDest);
void CdxCbWriterDestroy(CdxWriterT *writer);

#endif
