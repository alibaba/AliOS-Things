#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H

#if defined(ARMCM0)
#define CMSIS_device_header "ARMCM0.h"
#elif defined(ARMCM0P)
#define CMSIS_device_header "ARMCM0plus.h"
#elif defined(ARMCM0P_MPU)
#define CMSIS_device_header "ARMCM0plus_MPU.h"
#elif defined(ARMCM3)
#define CMSIS_device_header "ARMCM3.h"
#elif defined(ARMCM4)
#define CMSIS_device_header "ARMCM4.h"
#elif defined(ARMCM4_FP)
#define CMSIS_device_header "ARMCM4_FP.h"
#elif defined(ARMCM7)
#define CMSIS_device_header "ARMCM7.h"
#elif defined(ARMCM7_SP)
#define CMSIS_device_header "ARMCM7_SP.h"
#elif defined(ARMCM7_DP)
#define CMSIS_device_header "ARMCM7_DP.h"
#elif defined(ARMCM23)
#define CMSIS_device_header "ARMCM23.h"
#elif defined(ARMCM23_TZ)
#define CMSIS_device_header "ARMCM23_TZ.h"
#elif defined(ARMCM33)
#define CMSIS_device_header "ARMCM33.h"
#elif defined(ARMCM33_DSP_FP)
#define CMSIS_device_header "ARMCM33_DSP_FP.h"
#elif defined(ARMCM33_DSP_FP_TZ)
#define CMSIS_device_header "ARMCM33_DSP_FP_TZ.h"
#elif defined(ARMCM35P)
#define CMSIS_device_header "ARMCM35P.h"
#elif defined(ARMCM35P_DSP_FP_TZ)
#define CMSIS_device_header "ARMCM35P_DSP_FP_TZ.h"
#elif defined(ARMCA5)
#define CMSIS_device_header "ARMCA5.h"
#elif defined(ARMCA7)
#define CMSIS_device_header "ARMCA7.h"
#elif defined(ARMCA9)
#define CMSIS_device_header "ARMCA9.h"
#else
#error "Unknown device selection!"
#endif

#ifdef __UVISION_VERSION
#define RTE_Compiler_IO_STDOUT          /* Compiler I/O: STDOUT */
#if defined(ARMCM0) || defined(ARMCM0plus) 
#define RTE_Compiler_IO_STDOUT_EVR      /* Compiler I/O: STDOUT EVR */
#ifndef RTE_Compiler_EventRecorder
#define RTE_Compiler_EventRecorder
#endif
#else
#define RTE_Compiler_IO_STDOUT_ITM      /* Compiler I/O: STDOUT ITM */
#endif
#else
#define RTE_Compiler_IO_SEMIHOST
#endif

#endif
