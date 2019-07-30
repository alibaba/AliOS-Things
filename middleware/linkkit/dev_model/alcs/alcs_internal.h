#ifndef _ALCS_INTERNAL_H_
#define _ALCS_INTERNAL_H_

#include "linkkit/infra/infra_config.h"
#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"
#include "linkkit/infra/infra_md5.h"
#include "linkkit/infra/infra_sha1.h"
#include "linkkit/infra/infra_json_parser.h"
#include "alcs_base64.h"
#include "linkkit/wrappers/wrappers.h"

#ifdef INFRA_MEM_STATS
    #include "linkkit/infra/infra_mem_stats.h"
    #define ALCS_malloc(size)            LITE_malloc(size, MEM_MAGIC, "alcs")
    #define ALCS_ADAPTER_malloc(size)    LITE_malloc(size, MEM_MAGIC, "alcs_adapter")
    #define ALCS_free(ptr)               LITE_free(ptr)
#else
    #define ALCS_malloc(size)            HAL_Malloc(size)
    #define ALCS_ADAPTER_malloc(size)    HAL_Malloc(size)
    #define ALCS_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#endif