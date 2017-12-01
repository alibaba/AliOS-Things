/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "be_jse.h"
#include "be_bi_module.h"

//#define RAND_MAX (0x7FFFFFFF)

ALWAYS_INLINE be_jse_symbol_t* new_undefined_symbol()
{
    return new_symbol(BE_SYM_NULL); // TODO see about above - should really be 'undefined'
}

static BE_JSE_FUNCTION_EXECUTE_CB beJseHandleFunctionCallExt = 0;

void be_jse_register_function_cb(BE_JSE_FUNCTION_EXECUTE_CB func_cb)
{
    beJseHandleFunctionCallExt = func_cb;
}

#ifdef USE_STRING

static char get_char_in_str_symbol(be_jse_symbol_t *s, int idx)
{
    if (!symbol_is_string(s)) return 0;
    if (idx<0) idx += symbol_str_len(s); // <0 goes from end of string
    if (idx<0) return 0;

    s = symbol_relock(s);
    while (s && idx >= symbol_get_max_char_len(s)) {
        be_jse_node_t next;
        idx -= symbol_get_max_char_len(s);
        next = s->last_child;
        symbol_unlock(s);
        s = symbol_lock(next);
    }

    char c = 0;
    if (s) {
        c = s->data.str[idx];
        symbol_unlock(s);
    }
    return c;
}
#endif


be_jse_symbol_t *handle_function_call(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *a, const char *name)
{
    // 外部扩展
    if (beJseHandleFunctionCallExt) {
        be_jse_symbol_t *v = beJseHandleFunctionCallExt(execInfo, a, name);
        if (v!=BE_JSE_FUNC_UNHANDLED)
            return v;
    }

#ifdef USE_GLOBAL

    if (a==0) { // Special cases for where we're just a basic function
        if (strcmp(name,"eval")==0) {
            be_jse_symbol_t *v = be_jse_handle_single_arg_function();
            be_jse_symbol_t *result = 0;
            if (v) {
                int len = symbol_str_len(v);
                if(len) {
                    char *sourcePtr = aos_malloc(len+1);
                    symbol_to_str(v, sourcePtr, len);
                    result = be_jse_eval_string(execInfo->executor, sourcePtr);
                    aos_free(sourcePtr);
                }
                symbol_unlock(v);
            }
            //if (!result) result = new_undefined_symbol();
            return result;
        }
#ifdef TRACE_JSE_INFO
        if(strcmp(name, "trace")==0) {
            be_jse_handle_function(0, 0, 0, 0, 0);
            trace_symbol_info(execInfo->executor->root, 0);
            return new_symbol(BE_SYM_NULL);
        }
#endif
        // unhandled
        return BE_JSE_FUNC_UNHANDLED;
    }
#endif

    // Is actually a method on some variable
    if (strcmp(name,"length")==0) {
        if (symbol_is_array(a)) {
            lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
            return new_int_symbol(get_symbol_array_length(a));
        }
        if (symbol_is_string(a)) {
            lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
            return new_int_symbol((be_jse_int_t)symbol_str_len(a));
        }
    }
#ifdef USE_PROCESS
    // process 扩展
    if (get_symbol_node_id(a) == execInfo->executor->processClass) {
#ifdef USE_AOS
        if (strcmp(name,"version")==0) {
            lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
            return new_str_symbol("0.0.1");
        }

        if (strcmp(name,"platform")==0) {
            lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
            return new_str_symbol(aos_get_product_model());
        }
#endif
    }
#endif


#ifdef USE_INTEGER
    // --------------------------------- built-in Integer
    if (get_symbol_node_id(a) == execInfo->executor->intClass) {
        if (strcmp(name,"parseInt")==0) {
            char buffer[16];
            be_jse_symbol_t *v = be_jse_handle_single_arg_function();
            symbol_to_str(v, buffer, 16);
            symbol_unlock(v);
            return new_int_symbol((be_jse_int_t)string_to_int(buffer));
        }
        if (strcmp(name,"valueOf")==0) {
            // value of a single character
            int c;
            be_jse_symbol_t *v = be_jse_handle_single_arg_function(execInfo);
            if (!symbol_is_string(v) || symbol_str_len(v)!=1) {
                symbol_unlock(v);
                return new_int_symbol(0);
            }
            c = (int)v->data.str[0];
            symbol_unlock(v);
            return new_int_symbol(c);
        }
    }

#endif

#ifdef USE_MATH

// --------------------------------- built-in Math
    if (get_symbol_node_id(a) == execInfo->executor->mathClass) {
        if (strcmp(name,"random")==0) {
            if (be_jse_is_none_arg_function())
                return new_float_symbol((be_jse_float_t)rand() / (be_jse_float_t)RAND_MAX);
        }
    }
#endif

#ifdef USE_JSON

// --------------------------------- built-in JSON
    if (get_symbol_node_id(a) == execInfo->executor->jsonClass) {
        if (strcmp(name,"stringify")==0) {
            be_jse_symbol_t *v = be_jse_handle_single_arg_function();
            be_jse_symbol_t *result = new_str_symbol("");
            if(result)
                symbol_to_json(v, result);
            symbol_unlock(v);
            return result;
        }
        if (strcmp(name,"parse")==0) {
            be_jse_symbol_t *v = be_jse_handle_single_arg_function();
            be_jse_symbol_t *res=0;

            int len = symbol_str_len(v);
            if(len) {
                char *sourcePtr = aos_malloc(len+3);
                sourcePtr[0] = '(';
                symbol_to_str(v, sourcePtr+1, len);
                sourcePtr[len+1] = ')';
                sourcePtr[len+2] = 0;
                res = be_jse_eval_string(execInfo->executor, sourcePtr);
                printf("sourcePtr=%s\n",sourcePtr);
                aos_free(sourcePtr);
            }
            symbol_unlock(v);

            return res;
        }
    }
#endif

#ifdef USE_STRING
// ------------------------------------------ Built-in String
    if (symbol_is_string(a)) {
        if (strcmp(name,"charAt")==0) {
            char buffer[2];
            int idx = 0;
            be_jse_symbol_t *v = be_jse_handle_single_arg_function();
            idx = (int)get_symbol_int(v);
            buffer[0] = get_char_in_str_symbol(a, idx);
            buffer[1] = 0;
            symbol_unlock(v);
            return new_str_symbol(buffer);
        }
        if (strcmp(name,"substring")==0) {
            be_jse_symbol_t *vStart, *vEnd, *res;
            int pStart, pEnd;
            //bepParseDoubleFunction(&vStart, &vEnd);
            be_jse_handle_function(0,&vStart, &vEnd, 0, 0);
            pStart = (int)get_symbol_int(vStart);
            pEnd = symbol_is_undefined(vEnd) ? BE_JSE_SYMBOL_APPEND_STR_MAX_LEN : (int)get_symbol_int(vEnd);
            symbol_unlock(vStart);
            symbol_unlock(vEnd);
            if (pStart<0) pStart=0;
            if (pEnd<0) pEnd=0;
            if (pEnd<pStart) {
                int l = pStart;
                pStart = pEnd;
                pEnd = l;
            }
            res = new_symbol(BE_SYM_STRING);
            if (!res) return 0; // out of memory
            symbol_str_value_append(res, a, pStart, pEnd-pStart);
            return res;
        }
        if (strcmp(name,"substr")==0) {
            be_jse_symbol_t *vStart, *vLen, *res;
            int pStart, pLen;
            //bepParseDoubleFunction(&vStart, &vLen);
            be_jse_handle_function(0,&vStart, &vLen, 0, 0);
            pStart = (int)get_symbol_int(vStart);
            pLen = symbol_is_undefined(vLen) ? BE_JSE_SYMBOL_APPEND_STR_MAX_LEN : (int)get_symbol_int(vLen);
            symbol_unlock(vStart);
            symbol_unlock(vLen);
            if (pLen<0) pLen=0;
            res = new_symbol(BE_SYM_STRING);
            if (!res) return 0; // out of memory
            symbol_str_value_append(res, a, pStart, pLen);
            return res;
        }
        if (strcmp(name,"indexOf")==0) {
            be_jse_symbol_t *v = str_to_symbol(be_jse_handle_single_arg_function(), true);
            if (!v) return 0; // out of memory
            int idx = -1;
            int l = (int)symbol_str_len(a) - (int)symbol_str_len(v);
            for (idx=0; idx<l; idx++) {
                if (symbol_str_cmp(a, v, idx, 0, true)==0) {
                    symbol_unlock(v);
                    return new_int_symbol(idx);
                }
            }
            symbol_unlock(v);
            return new_int_symbol(-1);
        }
    }

#endif

    if (symbol_is_string(a) || symbol_is_object(a)) {
        if (strcmp(name,"clone")==0) {
            if (be_jse_is_none_arg_function())
                return symbol_cp(a);
        }
    }

#ifdef USE_STRING
// ------------------------------------------ Built-in Array
    if (symbol_is_array(a)) {
        if (strcmp(name,"contains")==0) {
            be_jse_symbol_t *childValue = be_jse_handle_single_arg_function();
            be_jse_node_t found = symbol_unlock(get_symbol_array_index(a, childValue));
            symbol_unlock(childValue);
            return new_bool_symbol(found!=0);
        }
        if (strcmp(name,"indexOf")==0) {
            be_jse_symbol_t *childValue = be_jse_handle_single_arg_function();
            be_jse_symbol_t *idxName = get_symbol_array_index(a, childValue);
            symbol_unlock(childValue);
            if (idxName==0) return new_undefined_symbol();
            be_jse_symbol_t *idx = symbol_name_cp(idxName, false, false);
            symbol_unlock(idxName);
            return idx;
        }
        if (strcmp(name,"push")==0) {
            be_jse_symbol_t *childValue = be_jse_handle_single_arg_function();
            be_jse_int_t newSize = symbol_array_push(a, childValue);
            symbol_unlock(childValue);
            return new_int_symbol(newSize);
        }
        if (strcmp(name,"pop")==0) {
            be_jse_handle_function(0,0, 0, 0, 0);
            be_jse_symbol_t *item = symbol_array_pop(a);
            item = unlock_symbol_value(item);
            return item;
        }
    }
#endif

// unhandled
    return BE_JSE_FUNC_UNHANDLED;
}



