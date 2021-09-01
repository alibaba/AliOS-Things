

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "v_typdef.h"

#ifndef _MED_AEC3_MAIN_H_
#define _MED_AEC3_MAIN_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

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
typedef VOS_VOID (*NS_HANDLER_T)(VOS_VOID *stNs, VOS_INT16 *shwBuf, VOS_INT32 swFrameLength);

/* AEC参数初始化接口 */
typedef struct
{
    VOS_BOOL                            enEAecEnable;                           /* AEC: 增强免提AEC使能开关 */
    VOS_BOOL                            enHpfEnable;                            /* HPF: 高通滤波开关 */
    VOS_BOOL                            enAfEnable;                             /* AF : 自适应滤波开关 */
    VOS_BOOL                            enNsEnable;                             /* NS : 噪声抑制开关 */
	VOS_INT16                           shwDelayLength;                         /* DELAY: 固定延时 */
} MED_AEC3_NV_STRU;

/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern VOS_UINT32  MED_AEC3_Main(
	                   VOS_VOID               *pAecInstance,
                       VOS_INT16              *pshwMicIn,
					   VOS_INT16              *pshwSpkIn,
                       VOS_INT16              *pshwLineOut);
extern VOS_VOID* MED_AEC3_Create(void);
extern VOS_UINT32  MED_AEC3_Destroy(VOS_VOID  **ppAecObj);
extern VOS_UINT32 MED_AEC3_SetPara (
                       VOS_VOID                        *pAecInstance,
					   MED_AEC3_NV_STRU                 *pstNv,
                       VOS_INT32                       enSampleRate);
extern VOS_UINT32  MED_AEC3_SetExtenalNsHandle(
                       VOS_VOID               *pAecInstance,
                       VOS_VOID               *pstNs,
                       NS_HANDLER_T           swNsHandler);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

