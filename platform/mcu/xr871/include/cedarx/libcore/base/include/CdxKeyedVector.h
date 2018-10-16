/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxKeyedVector.h
 * Description : KeyVector
 * History :
 *
 */

#ifndef __CDX_KEY_VECTOR_H
#define __CDX_KEY_VECTOR_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct CdxKeyedVectorS CdxKeyedVectorT;
typedef struct KeyValuePairS KeyValuePairT;

struct KeyValuePairS {
    char *key;
    char *val;
};

struct CdxKeyedVectorS {
    int size;
    int index;
    KeyValuePairT item[0];
};


/* return NULL on error */
CdxKeyedVectorT *CdxKeyedVectorCreate(int num);

void CdxKeyedVectorDestroy(CdxKeyedVectorT *keyedVector);

/* For the following four functions, keyedVector must not be NULL. */

/* return 0 on success, -1 on error */
int CdxKeyedVectorAdd(CdxKeyedVectorT *keyedVector,
        const char *key, const char *val);

int CdxKeyedVectorGetSize(CdxKeyedVectorT *keyedVector);

char *CdxKeyedVectorGetKey(CdxKeyedVectorT *keyedVector, int index);

char *CdxKeyedVectorGetValue(CdxKeyedVectorT *keyedVector, int index);

#ifdef __cplusplus
}
#endif

#endif
