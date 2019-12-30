/*------------------------------------------------------------------------------
* qxwz_typdes.h : type definitions
*          Copyright (C) 2015-2017 by QXSI, All rights reserved.
*/
#ifndef __QXWZ_TYPES_H__
#define __QXWZ_TYPES_H__

/*******************************************************************************
 * Type Definitions
 *******************************************************************************/

/** Signed 8bit integer. */
typedef /*signed*/ char                   qxwz_s8_t;

/** Unsigned 8bit integer. */
typedef unsigned char                     qxwz_u8_t;

/** Signed 16bit integer. */
typedef signed short                      qxwz_s16_t;

/** Unsigned 16bit integer. */
typedef unsigned short                    qxwz_u16_t;

/** Signed 32bit integer. */
typedef signed int                        qxwz_s32_t;

/** Unsigned 32bit integer. */
typedef unsigned int                      qxwz_u32_t;

/** Large signed integer. */
typedef signed long long                  qxwz_s64_t;

/** unsigned 64 bit integer */
typedef unsigned long long                qxwz_u64_t;

/** single precision float number */
typedef float                             qxwz_flt_t;

/** double precision float number */
typedef double                            qxwz_dbl_t;

/** double precision float number */
typedef void                              qxwz_void_t;


/* boolean representation */
typedef enum
{
    /* FALSE value */
    QXWZ_FALSE,
    /* TRUE value */
    QXWZ_TRUE
} qxwz_bool_t;

/* MISRA-C[pm098] */
#if !defined(NULL)
#define NULL    ((void*)0)
#endif

/*output data type*/
typedef enum{
    /*rtcm*/
    RTCM_TYPE_RAW,

    RTCM_TYPE_EPO,
    RTCM_TYPE_REF,
    RTCM_TYPE_EPH,
    RTCM_TYPE_GEPO,

    RTCM_TYPE_SSR1_ORBIT,
    RTCM_TYPE_SSR1_CLOCK,
    RTCM_TYPE_SSR1_ORBCLK,
    RTCM_TYPE_SSR1_CBIAS,
    RTCM_TYPE_SSR1_URA,

    RTCM_TYPE_SSR2_PBIAS,
    RTCM_TYPE_SSR2_VTEC,

    RTCM_TYPE_SSR2_COMBBIAS,    /*combined Code bias and Phase bias, just satellite's packet supports this type*/

    /*test*/
    SSR_TYPE_TEST,

    /*other*/
    /*...*/
}qxwz_data_type_e;

/*intput data type*/
typedef enum{
    UDATA_GGA,
    UDATA_IP,
    UDATA_SAT,
}qxwz_udata_type_e;

#endif /*__QXWZ_TYPES_H__*/

