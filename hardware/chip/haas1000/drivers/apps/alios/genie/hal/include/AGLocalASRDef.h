/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGLocalASRDef.h
 * @brief defininitions of local asr for SAL and APP
 * @author lifen.lf@alibaba-inc.com
 * @data 2019/02/14
 * @version 1.0
 */
#ifndef AG_LOCAL_ASR_DEFINITIONS_H
#define AG_LOCAL_ASR_DEFINITIONS_H

/**
 * \brief local asr result definition
 */
typedef struct AGLocalASRResult
{
    const char* pszASRText; /**< final asr text */
    double dConfidence;     /**< confidence */
    const char* pszNLUJson; /**< NLU result json string */
    bool fgFinalASR;           /**< is final asr text */
} AGLocalASRResult_T;

#endif
