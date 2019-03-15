/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxUrl.h
 * Description : Url
 * History :
 *
 */

#ifndef CDX_URL
#define CDX_URL

typedef struct AW_URL
{
    char* url;
    char* protocol;
    char* hostname;
    char* file;
    unsigned int port;
    char* username;
    char* password;
    char* noauth_url;
}CdxUrlT;

CdxUrlT* CdxUrlNew(char* url);
void CdxUrlPrintf(CdxUrlT* url);
void CdxUrlFree(CdxUrlT* curUrl);

void CdxUrlEscapeString(char *outbuf, const char *inbuf);

void CdxUrlUnescapeString(char *outbuf, const char *inbuf);

void CdxUrlEscapeStringPart(char *outbuf, const char *inbuf);

#endif
