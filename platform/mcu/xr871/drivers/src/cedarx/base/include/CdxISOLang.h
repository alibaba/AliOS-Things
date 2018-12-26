/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxISOLang.h
 * Description : iso639 standard for language names
 * History :
 *
 */

#ifndef CDX_ISO_LANG_H
#define CDX_ISO_LANG_H

typedef struct Cdx_ISO_639_Lang_t {
    const char *eng_name; /* Description in English */
    const char iso639_1[3]; /* ISO-639-1 (2 characters) code */
    const char iso639_2T[4]; /* ISO-639-2/T (3 characters) English code */
    const char iso639_2B[4]; /* ISO-639-2/B (3 characters) native code */
} Cdx_ISO_639_Lang_t;

extern const Cdx_ISO_639_Lang_t CDX_ISO_639_LANGS[];

#endif
