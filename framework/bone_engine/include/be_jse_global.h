/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#ifndef BE_JSE_GLOBAL_H_
#define BE_JSE_GLOBAL_H_

#include "be_jse_feature.h"

#define BE_JSE_SYMBOL_DATA_STRING_LEN  8
#define BE_JSE_SYMBOL_DATA_STRING_MAX_LEN (BE_JSE_SYMBOL_DATA_STRING_LEN + sizeof(be_jse_node_t)*3)
#define BE_JSE_SYMBOL_STRING_OP_BUFFER_SIZE 256
#define MAX_TOKEN_LENGTH  64
#define BE_JSE_ERROR_BUF_SIZE 64
#define BE_JSE_ERROR_TOKEN_BUF_SIZE 16

#define BE_JSE_PARSE_MAX_SCOPES  32
#define BE_JSE_PARSE_MAX_LOOPS 8192

#define BE_JSE_PARSE_RETURN_VAR "return"
#define BE_JSE_PARSE_THIS_OBJ "this"
#define BE_JSE_PARSE_PROTOTYPE_CLASS "prototype"
#define BE_JSE_PARSE_FUNCTION_CODE_NAME "#code#"
#define BE_JSE_PARSE_FUNCTION_SCOPE_NAME "#scope#"

#define BE_JSE_SYMBOL_TABLE_SIZE 500
#define BE_JSE_SYMBOL_APPEND_STR_MAX_LEN (0x7FFFFFFF)

#define BE_JSE_FUNC_UNHANDLED ((be_jse_symbol_t*)-1)

#define BE_JSE_LOCK_SHIFT 8
#define BE_JSE_LOCK_MAX  15
#define BE_JSE_STRING_LEN_SHIFT 12
#define BE_JSE_STRING_LEN_MAX 15

#define PACKED_FLAGS    __attribute__ ((__packed__))
#define NO_INLINE       __attribute__ ((noinline))

//#define BE_JSE_SILENT

#ifdef __cplusplus
}
#endif

#endif /* BE_JSE_GLOBAL_H_ */
