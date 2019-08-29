
/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "be_addon.h"
#include "be_module.h"
#include "jse_common.h"

#ifdef JSE_CORE_ADDON_TIMER
#include "module_timer.h"
#endif

#define BonePrefix "BoneEngine > "

be_jse_symbol_t* module_handle_cb_js_lite(be_jse_vm_ctx_t* execInfo,
                                          be_jse_symbol_t* var,
                                          const char* name)
{
    if (strcmp(name, "VERSION") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_str_symbol("0.0.1");
    }

    return BE_JSE_FUNC_UNHANDLED;
}

be_jse_symbol_t* module_handle_cb_js_lite_console(be_jse_vm_ctx_t* execInfo,
                                                  be_jse_symbol_t* var,
                                                  const char* name)
{
    if (strcmp(name, "log") == 0) {
        int len;
        be_jse_symbol_t* arg0 = be_jse_handle_single_arg_function();
        char* buf_header      = NULL;
        char* buf             = NULL;
        char* line            = NULL;

        len = symbol_str_len(arg0);
        if (len == 0) len = 32;

        buf_header = buf = (char*)jse_calloc(1, len + 1);
        symbol_to_str(arg0, buf, len);

        /* add prefix for every line */
        while (bone_console_get_log_flag() && buf) {
            line = strchr(buf, '\n');
            if (line == NULL) {
                jse_warn(BonePrefix "%s\n", buf);
                break;
            } else {
                *line = '\0';
                jse_warn(BonePrefix "%s\n", buf);
                buf = line + 1;
            }
        }

#ifdef JSE_IDE_DEBUG
        bone_websocket_send_frame("/ide/console", BE_LOG_LEVEL_INFO,
                                  buf_header); /* log output */
#endif
        jse_free(buf_header);

        symbol_unlock(arg0);
        return new_symbol(BE_SYM_NULL);
    }
    return BE_JSE_FUNC_UNHANDLED;
}

#ifdef USE_STRING

static char get_char_in_str_symbol(be_jse_symbol_t* s, int idx)
{
    if (!symbol_is_string(s)) return 0;
    if (idx < 0) idx += symbol_str_len(s); /* <0 goes from end of string */
    if (idx < 0) return 0;

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

be_jse_symbol_t* module_handle_cb_js_lite_string(be_jse_vm_ctx_t* execInfo,
                                                 be_jse_symbol_t* var,
                                                 const char* name)
{
    /* Don't need get_symbol_value(var) */
    if (strcmp(name, "length") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol((be_jse_int_t)symbol_str_len(var));
    }

    if (strcmp(name, "charAt") == 0) {
        char buffer[2];
        int idx            = 0;
        be_jse_symbol_t* v = be_jse_handle_single_arg_function();
        idx                = (int)get_symbol_int(v);
        buffer[0]          = get_char_in_str_symbol(var, idx);
        buffer[1]          = 0;
        symbol_unlock(v);
        return new_str_symbol(buffer);
    }
    if (strcmp(name, "substring") == 0) {
        be_jse_symbol_t *vStart, *vEnd, *res;
        int pStart, pEnd;

        /* bepParseDoubleFunction(&vStart, &vEnd); */
        be_jse_handle_function(0, &vStart, &vEnd, 0, 0);
        pStart = (int)get_symbol_int(vStart);
        pEnd   = symbol_is_undefined(vEnd) ? BE_JSE_SYMBOL_APPEND_STR_MAX_LEN
                                         : (int)get_symbol_int(vEnd);
        symbol_unlock(vStart);
        symbol_unlock(vEnd);
        if (pStart < 0) pStart = 0;
        if (pEnd < 0) pEnd = 0;
        if (pEnd < pStart) {
            int l  = pStart;
            pStart = pEnd;
            pEnd   = l;
        }
        res = new_symbol(BE_SYM_STRING);
        if (!res) return 0; /* out of memory */
        symbol_str_value_append(res, var, pStart, pEnd - pStart);
        return res;
    }
    if (strcmp(name, "substr") == 0) {
        be_jse_symbol_t *vStart, *vLen, *res;
        int pStart, pLen;
        be_jse_handle_function(0, &vStart, &vLen, 0, 0);
        pStart = (int)get_symbol_int(vStart);
        pLen   = symbol_is_undefined(vLen) ? BE_JSE_SYMBOL_APPEND_STR_MAX_LEN
                                         : (int)get_symbol_int(vLen);
        symbol_unlock(vStart);
        symbol_unlock(vLen);
        if (pLen < 0) pLen = 0;
        res = new_symbol(BE_SYM_STRING);
        if (!res) return 0; /* out of memory */
        symbol_str_value_append(res, var, pStart, pLen);
        return res;
    }
    if (strcmp(name, "indexOf") == 0) {
        be_jse_symbol_t* v =
            str_to_symbol(be_jse_handle_single_arg_function(), true);
        if (!v) return 0; /* out of memory */
        int idx = -1;
        int l   = (int)symbol_str_len(var) - (int)symbol_str_len(v);
        for (idx = 0; idx < l; idx++) {
            if (symbol_str_cmp(var, v, idx, 0, true) == 0) {
                symbol_unlock(v);
                return new_int_symbol(idx);
            }
        }
        symbol_unlock(v);
        return new_int_symbol(-1);
    }

    return BE_JSE_FUNC_UNHANDLED;
}

#endif

static be_jse_symbol_t* module_handle_cb_js_lite_global(
    be_jse_vm_ctx_t* execInfo, be_jse_symbol_t* var, const char* name)
{
    be_jse_symbol_t* ret;

    /* if (strcmp(name, "getip") == 0) {
        be_jse_handle_function(0, 0, 0, 0, 0);
        jse_debug("getip ...");
        return new_symbol(BE_SYM_NULL);
    } */

    if (symbol_is_string(var) || symbol_is_object(var)) {
        if (strcmp(name, "clone") == 0) {
            if (be_jse_is_none_arg_function()) return symbol_cp(var);
        }
    }

    if (strcmp(name, "eval") == 0) {
        be_jse_symbol_t* v      = be_jse_handle_single_arg_function();
        be_jse_symbol_t* result = 0;
        if (v) {
            int len = symbol_str_len(v);
            if (len) {
                char* sourcePtr = jse_malloc(len + 1);
                symbol_to_str(v, sourcePtr, len);
                result = be_jse_eval_string(execInfo->executor, sourcePtr);
                jse_free(sourcePtr);
            }
            symbol_unlock(v);
        }
        return result;
    }

    /* TIMER handler */
#ifdef JSE_CORE_ADDON_TIMER
    ret = timers_module_handle_cb(execInfo, var, name);
    if (ret != BE_JSE_FUNC_UNHANDLED) return ret;
#endif

#ifdef USE_MODULES
    if (strcmp(name, "require") == 0) {
        be_jse_symbol_t* v      = be_jse_handle_single_arg_function();
        be_jse_symbol_t* result = 0;
        if (v) {
            int len = symbol_str_len(v);
            if (len) {
                char* sourcePtr = jse_malloc(len + 1);
                symbol_to_str(v, sourcePtr, len);
                jse_warn("");
                jse_warn("require(%s)", sourcePtr);

#ifdef SUPPORT_NODE_MODELES
                /* get current sourcePtr (./1.js) and convert to absolute path
                 */

                /* read header */
                /* struct list_head* headPtr = bone_engine_dirname_get_head();
                 */
                BE_JSE_DIRNAME_s* firstDir;

                firstDir = bone_engine_dirname_get();

                /* jse_warn("firstDir = %p ", firstDir);
                jse_warn("firstDir->dirname = %s ",
                firstDir->dirname); */

                /* add new path */

                int fullPathSize;
                char* fullPath;
                char* clearPath;

                if (sourcePtr[0] != '.') {
/* node_modules */
#define NODE_MODULES "node_modules/"

                    fullPathSize = strlen(firstDir->dirname) +
                                   strlen(sourcePtr) + 1 + strlen(NODE_MODULES);
                    fullPath = jse_calloc(1, fullPathSize);
                    snprintf(fullPath, fullPathSize, "%s%s%s",
                             firstDir->dirname, NODE_MODULES, sourcePtr);

                    jse_warn("fullPath = %s ", fullPath);

                    /* node_modules/gpio */
                    fullPath = getNodeModulePath(fullPath);
                } else {
                    fullPathSize =
                        strlen(firstDir->dirname) + strlen(sourcePtr) + 1;
                    fullPath = jse_calloc(1, fullPathSize);
                    snprintf(fullPath, fullPathSize, "%s%s", firstDir->dirname,
                             sourcePtr);
                }

                clearPath = getClearPath(fullPath);

                /* jse_warn("fullPath = %s ", fullPath); */
                /* jse_warn("clearPath = %s ", clearPath); */

                jse_free(fullPath);

                /* save current dirname */
                firstDir          = jse_calloc(1, sizeof(BE_JSE_DIRNAME_s));
                firstDir->dirname = getFilePath(clearPath);
                bone_engine_dirname_push(firstDir);

                /* module loading */
                jse_warn("be_jse_require  %s ", clearPath);
                result = be_jse_require(execInfo->executor, clearPath);
#else
                result = be_jse_require(execInfo->executor, sourcePtr);
#endif
                if (result == NULL) {
                    char* buf = jse_calloc(1, 256);
                    snprintf(buf, 255, "%sCannot find module '%s' ", BonePrefix,
                             sourcePtr);
                    jse_error("%s\n", buf);
                    fflush(stdout);
                    /* report JSEngine error message via websocket */
                    /* bone_websocket_send_frame("/ide/console",
                                              BE_LOG_LEVEL_ERROR, buf); */
                    jse_free(buf);
                }

#ifdef SUPPORT_NODE_MODELES
                /* pop dirname */
                bone_engine_dirname_pop_del();
                jse_free(clearPath);
#endif
                jse_free(sourcePtr);
            }
            symbol_unlock(v);
        }
        if (!result) result = new_symbol(BE_SYM_NULL);

        return result;
    }
#endif

    return BE_JSE_FUNC_UNHANDLED;
}

#ifdef USE_JSON

be_jse_symbol_t* module_handle_cb_js_lite_json(be_jse_vm_ctx_t* execInfo,
                                               be_jse_symbol_t* var,
                                               const char* name)
{
    if (strcmp(name, "stringify") == 0) {
        be_jse_symbol_t* v      = be_jse_handle_single_arg_function();
        be_jse_symbol_t* result = new_str_symbol("");
        symbol_to_json(v, result);
        symbol_unlock(v);
        return result;
    }

    if (strcmp(name, "parse") == 0) {
        be_jse_symbol_t* text =
            str_to_symbol(be_jse_handle_single_arg_function(), true);
        char* buf = (char*)jse_calloc(1, symbol_str_len(text) + 1);
        symbol_to_str(text, buf, symbol_str_len(text));
        symbol_unlock(text);
        return new_json_symbol(buf, (int)strlen(buf));
    }

    return BE_JSE_FUNC_UNHANDLED;
}

#endif

#ifdef USE_ARRAY

be_jse_symbol_t* module_handle_cb_js_lite_array(be_jse_vm_ctx_t* execInfo,
                                                be_jse_symbol_t* var,
                                                const char* name)
{
    if (strcmp(name, "contains") == 0) {
        be_jse_symbol_t* childValue = be_jse_handle_single_arg_function();
        be_jse_node_t found =
            symbol_unlock(get_symbol_array_index(var, childValue));
        symbol_unlock(childValue);
        return new_bool_symbol(found != 0);
    }
    if (strcmp(name, "indexOf") == 0) {
        be_jse_symbol_t* childValue = be_jse_handle_single_arg_function();
        be_jse_symbol_t* idxName    = get_symbol_array_index(var, childValue);
        symbol_unlock(childValue);
        if (idxName == 0) return new_symbol(BE_SYM_NULL);
        be_jse_symbol_t* idx = symbol_name_cp(idxName, false, false);
        symbol_unlock(idxName);
        return idx;
    }
    if (strcmp(name, "push") == 0) {
        be_jse_symbol_t* childValue = be_jse_handle_single_arg_function();
        be_jse_int_t newSize        = symbol_array_push(var, childValue);
        symbol_unlock(childValue);
        return new_int_symbol(newSize);
    }
    if (strcmp(name, "pop") == 0) {
        be_jse_handle_function(0, 0, 0, 0, 0);
        be_jse_symbol_t* item = symbol_array_pop(var);
        item                  = unlock_symbol_value(item);
        return item;
    }

    if (strcmp(name, "length") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_int_symbol(get_symbol_array_length(var));
    }

    return BE_JSE_FUNC_UNHANDLED;
}

#endif

#ifdef USE_MODULES
be_jse_symbol_t* module_handle_cb_js_lite_modules(be_jse_vm_ctx_t* execInfo,
                                                  be_jse_symbol_t* var,
                                                  const char* name)
{
    /* Modules addons reference Espruino API */
    /* Modules.addCached("a","module.exports.foo=42;");
       var ta = require("a").foo == 42; */
    if (strcmp(name, "addCached") == 0) {
        jse_warn(" Modules.%s\n", name);
        be_jse_symbol_t *id, *code;
        be_jse_symbol_t* result = 0;
        be_jse_handle_function(0, &id, &code, 0, 0);
        result = be_jse_add_module(execInfo->executor, id, code);
        symbol_unlock(id);
        symbol_unlock(code);
        return result;
    }
    return BE_JSE_FUNC_UNHANDLED;
}
#endif

void module_builtin_load(void)
{
    be_jse_module_load("JS", module_handle_cb_js_lite);
    be_jse_module_load("console", module_handle_cb_js_lite_console);

#ifdef USE_STRING
    be_jse_module_load("String", module_handle_cb_js_lite_string);
#endif

#ifdef USE_JSON
    be_jse_module_load("JSON", module_handle_cb_js_lite_json);
#endif

#ifdef USE_ARRAY
    be_jse_module_load("Array", module_handle_cb_js_lite_array);
#endif

#ifdef USE_MODULES
    be_jse_module_load(BE_JSE_PARSE_MODULE_NAME,
                       module_handle_cb_js_lite_modules);
#endif

    jsengine_native_function_register(module_handle_cb_js_lite_global);
}
