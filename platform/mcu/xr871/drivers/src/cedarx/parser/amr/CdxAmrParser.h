/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : CdxAmrParser.h
* Description : Amr Parser
* History :
*
*/
#ifndef CDX_AMR_PARSER_H
#define CDX_AMR_PARSER_H

#define SEEK_TABLE_LEN 10	/* i think it's useless when we don't need parser seekto,
							   but i'm not sure. origin is 300 */
#define CODEC_ID_AMR_NB 1
#define CODEC_ID_AMR_WB 2

typedef struct AmrParserImplS
{
     //audio common
     CdxParserT base;
     CdxStreamT *stream;
     cdx_int64 ulDuration;//ms
     pthread_cond_t cond;
     cdx_int64 fileSize;//total file length
     cdx_int64 file_offset; //now read location
     cdx_int32 mErrno; //Parser Status
     cdx_uint32 flags; //cmd
     //amr base
     cdx_int64  pts;
     cdx_int32  head_len;
     cdx_int32  framecounts;//samples
     cdx_int32  framepcms;
     cdx_int32  chans;
     cdx_int32  sample_rate;
     cdx_int32  is_wide;
     cdx_int32  seek_table_len;
     cdx_int64  seek_table[SEEK_TABLE_LEN];

}AmrParserImplS;

static const char AMR_header [] = "#!AMR\n";
static const char AMRWB_header [] = "#!AMR-WB\n";

static const cdx_uint8 block_size_NB[16] = { 12, 13, 15, 17, 19, 20, 26,
                                             31, 5, 0, 0, 0, 0, 0, 0, 0 };
static const cdx_uint8 block_size_WB[16] = {18, 24, 33, 37, 41, 47, 51,
                                            59, 61, 6, 6, 0, 0, 0, 1, 1};

#endif
