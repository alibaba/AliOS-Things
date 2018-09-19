#ifndef __SOC_INIT_H
#define __SOC_INIT_H

#include "hal/hal.h"

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __SOC_INIT_H */
