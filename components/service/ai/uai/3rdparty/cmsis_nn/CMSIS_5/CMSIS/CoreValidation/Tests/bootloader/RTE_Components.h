#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H

#if defined(ARMCM23_TZ)
#define CMSIS_device_header "ARMCM23_TZ.h"
#elif defined(ARMCM33_DSP_FP_TZ)
#define CMSIS_device_header "ARMCM33_DSP_FP_TZ.h"
#elif defined(ARMCM35P_DSP_FP_TZ)
#define CMSIS_device_header "ARMCM35P_DSP_FP_TZ.h"
#else
#error "Unknown device selection!"
#endif

#endif
