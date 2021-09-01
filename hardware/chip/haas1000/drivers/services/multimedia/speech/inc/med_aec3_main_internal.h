

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "med_aec_comm.h"
#include "med_aec3_main.h"
#include "med_aec_hf_af.h"
#include "med_aec_hf_nlp.h"
#include "med_aec_hpf.h"
#include "med_emph_filter.h"
#include "med_aec_delay.h"

#ifndef _MED_AEC3_MAIN_INTERNAL_H_
#define _MED_AEC3_MAIN_INTERNAL_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* NS 内部结构体 */
typedef struct
{
    VOS_VOID                           *stNs;
    NS_HANDLER_T                        swHandler;
} MED_AEC3_HF_NS_STRU;

/* AEC内部状态结构 */
typedef struct
{
    MED_OBJ_HEADER
    VOS_BOOL                            shwIsEAecEnable;
    VOS_BOOL                            shwIsHpfEnable;
    VOS_BOOL                            shwIsPreEmpEnable;
    VOS_BOOL                            shwIsAfEnable;
    VOS_BOOL                            shwIsNsEnable;
	VOS_INT16                           shwFrameLength;
    VOS_INT16                           shwDelayLength;
    MED_AEC_DELAY_STRU                  stEAecDelay;
    MED_AEC_HF_AF_STRU                  stEAecHfAf;
    MED_AEC_HPF_STRU                    stEAecMicHpf;
    MED_AEC_HPF_STRU                    stEAecSpkHpf;
    MED_AEC3_HF_NS_STRU                  stEAecNs;
#ifdef MED_AEC_PREEMPH_ENABLE
    MED_PREEMPH_STRU                    stPreEmpMc;
    MED_PREEMPH_STRU                    stPreEmpFar;
    MED_DEEMPH_STRU                     stDeEmpMc;
#endif
} MED_AEC3_OBJ_STRU;



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

