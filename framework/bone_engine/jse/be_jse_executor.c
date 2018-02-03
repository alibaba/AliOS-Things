/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*
 * be_jse_executor.c
 *
 *  Ecmascript bytecode executor.
 *
 *
 */

#include "be_jse.h"

#ifdef USE_AOS
#include "aos/version.h"
#endif


//BoneEngine对象
//此处申明为静态全局变量，避免函数参数传递，而导致压栈浪费内存
static be_jse_vm_ctx_t vm;

static be_jse_symbol_t *handle_base_expr();
static be_jse_symbol_t *handle_block_syntax();
static be_jse_symbol_t *handle_statement_syntax();

/* VM 处理 */
void be_jse_execute_error()
{
    vm.execute = (vm.execute & (be_execflag_e)~EXEC_NORMAL) | EXEC_ERROR;
}

static inline void set_vm_idle()
{
    vm.execute = (vm.execute & (be_execflag_e)(int)~EXEC_RUN_MASK) | EXEC_IDLE;
}

static inline char be_vm_has_error()
{
    return (vm.execute&EXEC_ERROR_MASK)!=0;
}

static bool be_jse_vm_scope_insert(be_jse_node_t scope)
{
    if (vm.scopeCount >= BE_JSE_PARSE_MAX_SCOPES) {
        be_jse_error("Scopes is exceeded");
        be_jse_execute_error();
        return false;
    }
    vm.scopes[vm.scopeCount++] = INC_SYMBL_REF_BY_ID(scope);
    return true;
}

static void be_jse_vm_scope_remove()
{
    if (vm.scopeCount <= 0) {
        be_jse_error("Scopes is over removed ");
        be_jse_execute_error();
        return;
    }
    DEC_SYMBL_REF_BY_ID(vm.scopes[--vm.scopeCount]);
}


static be_jse_symbol_t *symbol_replace(be_jse_symbol_t *dst, be_jse_symbol_t *src)
{
    if (!symbol_is_name(dst)) {
        be_jse_error_at("Unable to assign value to non-reference", vm.lex, vm.lex->token_last_end);
        be_jse_execute_error();
        return dst;
    }

    return set_symbol_node_name(dst, src);
}

static be_jse_symbol_t *lookup_symbol_in_scopes(const char *name)
{
    int i;
    for (i=vm.scopeCount-1; i>=0; i--) {
        be_jse_symbol_t *ref = lookup_named_child_symbol(vm.scopes[i], name, false);
        if (ref) return ref;
    }
    return lookup_named_child_symbol(vm.executor->root, name, false);
}

static be_jse_symbol_t *lookup_symbol_on_top(const char *name, bool createIfNotFound)
{
    if (vm.scopeCount>0)
        return lookup_named_child_symbol(vm.scopes[vm.scopeCount-1], name, createIfNotFound);
    return lookup_named_child_symbol(vm.executor->root, name, createIfNotFound);
}

static be_jse_symbol_t *lookup_symbol_name_on_top( be_jse_symbol_t *childName, bool createIfNotFound)
{
    if (vm.scopeCount>0)
        return lookup_child_symbol(vm.scopes[vm.scopeCount-1], childName, createIfNotFound);
    return lookup_child_symbol(vm.executor->root, childName, createIfNotFound);
}

static be_jse_symbol_t *lookup_child_in_parent(be_jse_symbol_t *parent, const char *name)
{
    if (symbol_is_int(parent))
        return lookup_named_child_symbol(vm.executor->intClass, name, false);
    if (symbol_is_string(parent))
        return lookup_named_child_symbol(vm.executor->stringClass, name, false);
    if (symbol_is_array(parent))
        return lookup_named_child_symbol(vm.executor->arrayClass, name, false);

    if (symbol_is_object(parent)) {
        // If an object, look for prototypes
        be_jse_symbol_t *proto = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(parent), BE_JSE_PARSE_PROTOTYPE_CLASS, false));
        if (proto) {
            be_jse_symbol_t *child = lookup_named_child_symbol(get_symbol_node_id(proto), name, false);
            if (child) { // we found a child!
                symbol_unlock(proto);
                return child;
            }
            // else look for prototypes in THAT object
            child = lookup_child_in_parent(proto, name);
            symbol_unlock(proto);
            return child;
        } else {
            // look in the basic object class
            return lookup_named_child_symbol(vm.executor->objectClass, name, false);
        }
    }
    // no luck!
    return 0;
}

static be_jse_symbol_t *get_symbol_from_scopes()
{
    if (vm.scopeCount==0) return 0;

    int i;
    be_jse_symbol_t *arr = new_symbol(BE_SYM_ARRAY);

    for (i=0; i<vm.scopeCount; i++) {
        be_jse_symbol_t *scope = symbol_lock(vm.scopes[i]);
        be_jse_symbol_t *idx = new_named_symbol(new_int_symbol(i), scope);
        symbol_unlock(scope);
        if(!idx) {
            be_jse_execute_error();
            return arr;
        }
        add_symbol_node(arr, idx);
        symbol_unlock(idx);
    }
    return arr;
}

static void store_symbol_into_scopes(be_jse_symbol_t *arr)
{
    vm.scopeCount = 0;
    be_jse_node_t childref = arr->first_child;
    while (childref) {
        be_jse_symbol_t *child = symbol_lock(childref);
        vm.scopes[vm.scopeCount] = INC_SYMBL_REF_BY_ID(child->first_child);
        vm.scopeCount++;
        childref = child->next_sibling;
        symbol_unlock(child);
    }
}

static void be_jse_vm_init(be_jse_executor_ctx_t *executor, be_jse_lex_ctx_t *lex)
{
    vm.executor    = executor;
    vm.lex         = lex;
    vm.scopeCount  = 0;
    vm.execute     = EXEC_NORMAL;
}

static void be_jse_vm_deinit()
{
    vm.executor = 0;
    vm.lex = 0;
    be_assert(vm.scopeCount==0);
}

be_jse_vm_ctx_t* be_jse_get_vm()
{
    return &vm;
}

// Ecmascript 不带参数的函数处理
bool be_jse_is_none_arg_function()
{
    LEXER_MATCH(BE_TOKEN_ID);
    LEXER_MATCH('(');
    LEXER_MATCH(')');
    return true;
}

// Ecmascript 单参数函数处理
be_jse_symbol_t *be_jse_handle_single_arg_function()
{
    be_jse_symbol_t *v = 0;
    LEXER_MATCH(BE_TOKEN_ID);
    LEXER_MATCH('(');
    if (vm.lex->tk != ')')
        v = unlock_symbol_value(handle_base_expr());
    // throw away extra params
    while (vm.lex->tk != ')') {
        LEXER_MATCH(',');
        symbol_unlock(handle_base_expr());
    }
    LEXER_MATCH(')');
    return v;
}

// Ecmascript 函数参数处理
static bool be_jse_handle_function_arguments(be_jse_symbol_t *funcVar)
{
    LEXER_MATCH('(');
    while (vm.lex->tk!=')') {
        if (funcVar) {
            be_jse_symbol_t *param = add_symbol_node_name(funcVar, 0, lexer_get_token(vm.lex));
            if (!param) {
                // out of memory
                be_jse_execute_error();
                return false;
            }
            param->flags = BE_SYM_FUNCTION_ARGVS;
            symbol_unlock(param);
        }
        LEXER_MATCH(BE_TOKEN_ID);
        if (vm.lex->tk!=')') LEXER_MATCH(',');
    }
    LEXER_MATCH(')');
    return true;
}


// Ecmascript 函数定义处理
static be_jse_symbol_t *handle_function_definition()
{
    int funcBegin;

    be_jse_symbol_t *funcVar = 0;
    if (BE_VM_PARSE_SHOULD_EXECUTE)
        funcVar = new_symbol(BE_SYM_FUNCTION);

    if (!be_jse_handle_function_arguments(funcVar)) {
        return 0;
    }

    funcBegin = vm.lex->token_start;
    BE_SAVE_VM_EXE_STATE();
    set_vm_idle();
    symbol_unlock(handle_block_syntax());
    BE_RESTORE_VM_EXE_STATE();

    if (BE_VM_PARSE_SHOULD_EXECUTE) {
        be_jse_symbol_t *funcCodeVar = new_lexer_symbol(vm.lex, funcBegin, vm.lex->token_last_end+1);
        symbol_unlock(add_symbol_node_name(funcVar, funcCodeVar, BE_JSE_PARSE_FUNCTION_CODE_NAME));
        symbol_unlock(funcCodeVar);

        // scope var
        be_jse_symbol_t *funcScopeVar = get_symbol_from_scopes();
        if (funcScopeVar) {
            symbol_unlock(add_symbol_node_name(funcVar, funcScopeVar, BE_JSE_PARSE_FUNCTION_SCOPE_NAME));
            symbol_unlock(funcScopeVar);
        }
    }
    return funcVar;
}

bool be_jse_handle_function(be_jse_parse_skip_flag_e skipName, be_jse_symbol_t **arg0, be_jse_symbol_t **arg1, be_jse_symbol_t **arg2, be_jse_symbol_t **arg3)
{
    if (arg0) *arg0 = 0;
    if (arg1) *arg1 = 0;
    if (arg2) *arg2 = 0;
    if (arg3) *arg3 = 0;

    LEXER_MATCH(BE_TOKEN_ID);
    LEXER_MATCH('(');

    if (arg0 && vm.lex->tk != ')') {
        *arg0 = handle_base_expr();
        if (!(skipName&PARSE_NOSKIP_A)) *arg0 = unlock_symbol_value(*arg0);
    }
    if (arg1 && vm.lex->tk != ')') {
        LEXER_MATCH(',');
        *arg1 = handle_base_expr();
        if (!(skipName&PARSE_NOSKIP_B)) *arg1 = unlock_symbol_value(*arg1);
    }
    if (arg2 && vm.lex->tk != ')') {
        LEXER_MATCH(',');
        *arg2 = handle_base_expr();
        if (!(skipName&PARSE_NOSKIP_C)) *arg2 = unlock_symbol_value(*arg2);
    }
    if (arg3 && vm.lex->tk != ')') {
        LEXER_MATCH(',');
        *arg3 = handle_base_expr();
        if (!(skipName&PARSE_NOSKIP_D)) *arg3 = unlock_symbol_value(*arg3);
    }
    // throw away extra params
    while ( vm.lex->tk != ')') {
        LEXER_MATCH(',');
        symbol_unlock(handle_base_expr());
    }
    LEXER_MATCH(')');
    return true;
}

// ========================================================

//本地扩展对象函数
static bool be_jse_handle_native_function(be_jse_callback cb)
{
    char fn_name[MAX_TOKEN_LENGTH];
    be_jse_symbol_t *funcVar;
    be_jse_symbol_t *root = symbol_lock(vm.executor->root);

    LEXER_MATCH(BE_TOKEN_KW_FUNCTION);

    strncpy(fn_name, lexer_get_token(vm.lex), MAX_TOKEN_LENGTH);

    LEXER_MATCH(BE_TOKEN_ID);

    while (vm.lex->tk == '.') {
        be_jse_symbol_t *link;
        LEXER_MATCH('.');

        link = lookup_named_child_symbol(get_symbol_node_id(root), fn_name, false);
        if (!link) {
            be_jse_symbol_t *obj = new_symbol(BE_SYM_OBJECT);
            link = add_symbol_node_name(root, obj, fn_name);
            symbol_unlock(obj);
        }

        symbol_unlock(root);
        root = unlock_symbol_value(link);

        strncpy(fn_name, lexer_get_token(vm.lex), MAX_TOKEN_LENGTH);
        LEXER_MATCH(BE_TOKEN_ID);
    }

    funcVar = new_symbol(BE_SYM_FUNCTION | BE_SYM_NATIVE);
    if (!funcVar) {
        symbol_unlock(root);
        be_jse_execute_error();
        return false;
    }
    funcVar->data.callback = cb;
    be_jse_handle_function_arguments(funcVar);

    if (be_vm_has_error()) {
        symbol_unlock(root);
        symbol_unlock(funcVar);
        return false;
    }

    symbol_unlock(add_symbol_node_name(root, funcVar, fn_name));
    symbol_unlock(root);
    symbol_unlock(funcVar);
    return true;
}


bool be_jse_add_native_func(be_jse_executor_ctx_t *executor, const char *funcDesc, be_jse_callback callbackPtr)
{
    bool success;
    be_jse_lex_ctx_t lex;
    BE_SAVE_VM_EXE_STATE();
    be_jse_vm_ctx_t oldExecInfo = vm;

    be_jse_lexer_init(&lex, funcDesc, 0, -1);

    be_jse_vm_init(executor, &lex);

    success = be_jse_handle_native_function(callbackPtr);
    if (!success) {
        be_jse_error("Parsing Native Function failed!");
        be_jse_execute_error();
    }

    be_jse_vm_deinit();
    be_jse_lexer_deinit(&lex);
    BE_RESTORE_VM_EXE_STATE();
    oldExecInfo.execute = vm.execute;
    vm = oldExecInfo;

    return success;
}

// ========================================================

be_jse_symbol_t *be_jse_do_function_call(be_jse_symbol_t *function, be_jse_symbol_t *parent, bool isParsing, int argCount, be_jse_symbol_t **argPtr)
{
    if (BE_VM_PARSE_SHOULD_EXECUTE && !function) {


        if(symbol_is_object(parent)){
            // 无法获取parent中获取object name
            trace_symbol_info(get_symbol_node_id(parent), 5);

#ifndef BE_JSE_SILENT
            // 仅仅用于调试
            printf("%s.%s not found \n", be_jse_get_tmp_token(),vm.lex->token);
#endif

            /*
            char buf[BE_JSE_SYMBOL_STRING_OP_BUFFER_SIZE];
            be_jse_symbol_t *ObjName = symbol_lock(parent->first_child);
            symbol_to_str(ObjName, buf, BE_JSE_SYMBOL_STRING_OP_BUFFER_SIZE);
            symbol_unlock(ObjName);
            printf("ObjName = %s \n", buf);
            */
        }
        be_jse_warn_at("FUNC:", vm.lex, vm.lex->token_last_end);
    }

    if (BE_VM_PARSE_SHOULD_EXECUTE && function) {
        be_jse_symbol_t *root;
        be_jse_symbol_t *returnVarName;
        be_jse_symbol_t *returnVar;
        be_jse_node_t s;

        if (!symbol_is_function(function)) {
            be_jse_error_at("Need a function to call", vm.lex, vm.lex->token_last_end);
            be_jse_execute_error();
            return 0;
        }
        if (isParsing) LEXER_MATCH('(');

        //从符号表中，分配一个新的符号项用于存储函数对象。
        root = new_symbol(BE_SYM_FUNCTION);
        if (!root) {
            be_jse_execute_error();
            return 0;
        }

        be_jse_symbol_t *this = 0;
        if (parent) {
            this = add_symbol_node_name(root, parent, BE_JSE_PARSE_THIS_OBJ);
        }
        if (isParsing) {
            s = function->first_child;
            while (!be_vm_has_error() && s) {
                //the functions has the args
                be_jse_symbol_t *argv = symbol_lock(s);
                if (symbol_is_function_argv(argv)) {
                    be_jse_symbol_t *valueName = 0;

                    if (vm.lex->tk!=')') valueName = handle_base_expr();

                    if (BE_VM_PARSE_SHOULD_EXECUTE) {
                        be_jse_symbol_t *value          = get_symbol_value(valueName);
                        be_jse_symbol_t *newValueName   = new_named_symbol(symbol_cp(argv), value);

                        if (newValueName)
                            add_symbol_node(root, newValueName);
                        else
                            be_jse_execute_error();

                        symbol_unlock(newValueName);
                        symbol_unlock(value);
                    }
                    symbol_unlock(valueName);
                    if (vm.lex->tk!=')') LEXER_MATCH(',');
                }

                s = argv->next_sibling;
                symbol_unlock(argv);
            }
            while (vm.lex->tk != ')') {
                LEXER_MATCH(',');
                symbol_unlock(handle_base_expr());
            }
            LEXER_MATCH(')');
        } else if( argCount > 0 && argPtr != NULL ) {
            // C call JavaScript 带参数
            s = function->first_child;
            int i=0;
            while (s && argCount > 0) {
                be_jse_symbol_t *param = symbol_lock(s);

                if (symbol_is_function_argv(param)) {

                    be_jse_symbol_t *arg0 = unlock_symbol_value(argPtr[i]);
                    i ++;
                    be_jse_symbol_t *newValueName = new_named_symbol(symbol_cp(param), arg0);
                    if (newValueName) {
                        add_symbol_node(root, newValueName);
                        symbol_unlock(newValueName);
                    }
                    symbol_unlock(arg0);
                }
                s = param->next_sibling;
                symbol_unlock(param);
            }
        }

        returnVarName = add_symbol_node_name(root, 0, BE_JSE_PARSE_RETURN_VAR);
        if (!returnVarName)
            be_jse_execute_error();

        if (!be_vm_has_error()) {
            if (symbol_is_native(function)) {
                be_assert(function->data.callback);
                function->data.callback(get_symbol_node_id(root));
            } else {
                // save old scopes
                be_jse_node_t oldScopes[BE_JSE_PARSE_MAX_SCOPES];
                int oldScopeCount=0;
                // if we have a scope var, load it up. We may not have one if there were no scopes apart from root


                int i;
                oldScopeCount = vm.scopeCount;
                for (i=0; i<vm.scopeCount; i++)
                    oldScopes[i] = vm.scopes[i];
                be_jse_symbol_t *functionScope = lookup_named_child_symbol(get_symbol_node_id(function), BE_JSE_PARSE_FUNCTION_SCOPE_NAME, false);

                if (functionScope) {
                    be_jse_symbol_t *functionScopeVar = symbol_lock(functionScope->first_child);

                    store_symbol_into_scopes(functionScopeVar);
                    symbol_unlock(functionScopeVar);
                    symbol_unlock(functionScope);
                } else {
                    vm.scopeCount = 0;
                }
                // add the function's execute space to the symbol table so we can recurse
                if(be_jse_vm_scope_insert(get_symbol_node_id(root))) {

                    be_jse_symbol_t *functionCode = lookup_named_child_symbol(get_symbol_node_id(function), BE_JSE_PARSE_FUNCTION_CODE_NAME, false);
                    if (functionCode) {
                        be_jse_lex_ctx_t *oldLex;
                        be_jse_lex_ctx_t newLex;
                        char* sourcePtr = NULL;

                        be_jse_symbol_t* functionCodeVar = unlock_symbol_value(functionCode);
                        int len = symbol_str_len(functionCodeVar);
                        if( len > 0 ) {
                            //printf("len=%d\n",len);
                            sourcePtr = aos_malloc(len+1);
                            symbol_to_str(functionCodeVar, sourcePtr, len);
                            be_jse_lexer_init(&newLex, sourcePtr, 0, -1);
                        }
                        symbol_unlock(functionCodeVar);

                        oldLex = vm.lex;
                        vm.lex = &newLex;
                        BE_SAVE_VM_EXE_STATE();
                        handle_block_syntax();
                        bool hasError = be_vm_has_error();
                        BE_RESTORE_VM_EXE_STATE(); // because return will probably have set execute to false
                        be_jse_lexer_deinit(&newLex);
                        if(sourcePtr)
                            aos_free(sourcePtr);
                        vm.lex = oldLex;
                        if (hasError) {
                            be_jse_error_at("executor function call error", vm.lex, vm.lex->token_last_end);
                            be_jse_execute_error();
                        }
                    }
                    be_jse_vm_scope_remove();
                }
                // Unref old scopes
                for (i=0; i<vm.scopeCount; i++)
                    DEC_SYMBL_REF_BY_ID(vm.scopes[i]);
                // restore function scopes
                for (i=0; i<oldScopeCount; i++)
                    vm.scopes[i] = oldScopes[i];
                vm.scopeCount = oldScopeCount;
            }
        }

        /* Now remove 'this' var */
        if (this) {
            remove_child_symbol(root, this);
            symbol_unlock(this);
            this = 0;
        }
        returnVar = unlock_symbol_value(returnVarName);
        if(returnVarName)
            set_symbol_node_name(returnVarName, 0); // remove return value (which helps stops circular references)
        symbol_unlock(root);
        if (returnVar)
            return returnVar;
        else
            return 0;
    } else if (isParsing) {
        LEXER_MATCH('(');
        while (vm.lex->tk != ')') {
            be_jse_symbol_t *value = handle_base_expr();
            symbol_unlock(value);
            if (vm.lex->tk!=')') LEXER_MATCH(',');
        }
        LEXER_MATCH(')');

        return function;
    }
    return 0;
}


//处理数字，关键词，括号
static be_jse_symbol_t *handle_factor_syntax()
{
    // debug处理
    #ifndef BE_JSE_SILENT
    char objectName[MAX_TOKEN_LENGTH];
    char errMsg[MAX_TOKEN_LENGTH*3];
    errMsg[0] = 0;
    #endif

    // 处理 parentheses
    if (vm.lex->tk=='(') {
        be_jse_symbol_t *a;

        LEXER_MATCH('(');

        a = handle_base_expr();

        if (!be_vm_has_error()) LEXER_MATCH_WITH_RETURN(')',a);

        return a;
    }
    // true
    if (vm.lex->tk==BE_TOKEN_KW_TRUE) {
        LEXER_MATCH(BE_TOKEN_KW_TRUE);
        return BE_VM_PARSE_SHOULD_EXECUTE ? new_bool_symbol(true) : 0;
    }
    //false
    if (vm.lex->tk==BE_TOKEN_KW_FALSE) {
        LEXER_MATCH(BE_TOKEN_KW_FALSE);
        return BE_VM_PARSE_SHOULD_EXECUTE ? new_bool_symbol(false) : 0;
    }
    //NULL
    if (vm.lex->tk==BE_TOKEN_KW_NULL) {
        LEXER_MATCH(BE_TOKEN_KW_NULL);
        return BE_VM_PARSE_SHOULD_EXECUTE ? new_symbol(BE_SYM_NULL) : 0;
    }
    //undefined
    if (vm.lex->tk==BE_TOKEN_KW_UNDEFINED) {
        LEXER_MATCH(BE_TOKEN_KW_UNDEFINED);
        return 0;
    }
    // 标识符：identifier
    if (vm.lex->tk==BE_TOKEN_ID) {
        be_jse_symbol_t *parent = 0;

        be_jse_symbol_t *a = BE_VM_PARSE_SHOULD_EXECUTE ? lookup_symbol_in_scopes(lexer_get_token(vm.lex)) : 0;

        if (BE_VM_PARSE_SHOULD_EXECUTE && !a) {
            // 特殊case, 查找builtins函数...
            a = handle_function_call(&vm, 0, lexer_get_token(vm.lex));
            if (a==BE_JSE_FUNC_UNHANDLED) {
                a = new_named_symbol(new_str_symbol(lexer_get_token(vm.lex)), 0);
                LEXER_MATCH(BE_TOKEN_ID);
            }
        } else {
            LEXER_MATCH(BE_TOKEN_ID);
        }


#ifndef BE_JSE_SILENT
        // 仅仅用于调试
        //printf("[%s][%s][%d] token = %s \n", __FILE__, __FUNCTION__, __LINE__, vm.lex->token);
        be_jse_save_tmp_token(vm.lex->token);
#endif
        while (vm.lex->tk=='(' || vm.lex->tk=='.' || vm.lex->tk=='[') {
            if (vm.lex->tk=='(') {
                be_jse_symbol_t *func = 0;
                func = unlock_symbol_value(a);
                a = be_jse_do_function_call(func, parent, true, 0, NULL);
                symbol_unlock(func);
            } else if (vm.lex->tk == '.') {
                strcpy(objectName, vm.lex->token);
                LEXER_MATCH('.');
                if (BE_VM_PARSE_SHOULD_EXECUTE) {
                    const char *name = lexer_get_token(vm.lex);

                    be_jse_symbol_t *aVar = get_symbol_value(a);
                    be_jse_symbol_t *child = 0;
                    if (aVar && (symbol_is_object(aVar) || symbol_is_string(aVar) || symbol_is_array(aVar))) {
                        child = lookup_named_child_symbol(get_symbol_node_id(aVar), name, false);
                        if (!child)
                            child = lookup_child_in_parent(aVar, name);

                        if (child) {
                            LEXER_MATCH_WITH_CLEAN_AND_RETURN(BE_TOKEN_ID, symbol_unlock(parent); symbol_unlock(a);, child);
                        } else {
                            // 特殊case, 查找builtins函数...
                            child = handle_function_call(&vm, aVar, name);
                            if (child == BE_JSE_FUNC_UNHANDLED) {
                                child = 0;
                                // It wasn't handled... We already know this is an object so just add a new child
                                if (symbol_is_object(aVar)) {
                                    child = add_symbol_node_name(aVar, 0, name);
                                } else {
                                    // could have been a string...
                                    //strcat(errMsg, objectName);
                                    //strcat(errMsg, ".");
                                    //strcat(errMsg, vm.lex->token);
                                    //strcat(errMsg, " not found ");
									sprintf(errMsg,"OBJ:%s",objectName);
                                    be_jse_error_at(errMsg, vm.lex, vm.lex->token_last_end);
                                }
                                LEXER_MATCH_WITH_CLEAN_AND_RETURN(BE_TOKEN_ID, symbol_unlock(parent); symbol_unlock(a);, child);
                            }
                        }
                    } else {
                        sprintf(errMsg,"OBJ:%s",objectName);
                        //strcat(errMsg, objectName);
                        //strcat(errMsg, " is not defined");
                        be_jse_error_at(errMsg, vm.lex, vm.lex->token_last_end);
                    }
                    symbol_unlock(parent);
                    parent = aVar;
                    symbol_unlock(a);
                    a = child;
                } else {
                    // Not executing, just match
                    LEXER_MATCH_WITH_RETURN(BE_TOKEN_ID, a);
                }
            } else if (vm.lex->tk == '[') {
                be_jse_symbol_t *index;
                LEXER_MATCH('[');
                index = handle_base_expr();
                LEXER_MATCH_WITH_CLEAN_AND_RETURN(']', symbol_unlock(parent); symbol_unlock(index);, a);
                if (BE_VM_PARSE_SHOULD_EXECUTE) {
                    be_jse_symbol_t *aVar = get_symbol_value(a);
                    if (aVar && (symbol_is_array(aVar) || symbol_is_object(aVar))) {
                        // TODO: If we set to undefined, maybe we should remove the name?
                        be_jse_symbol_t *indexValue = get_symbol_value(index);
                        be_jse_symbol_t *child = lookup_child_symbol(get_symbol_node_id(aVar), indexValue, true);
                        symbol_unlock(indexValue);

                        symbol_unlock(parent);
                        parent = aVar;
                        symbol_unlock(a);
                        a = child;
                    } else {
                        be_jse_warn_at("Variable is not an Array or Object", vm.lex, vm.lex->token_last_end);
                        symbol_unlock(parent);
                        parent = 0;
                        symbol_unlock(a);
                        a = 0;
                    }
                }
                symbol_unlock(index);
            } else be_assert(0);
        }
        symbol_unlock(parent);
        return a;
    }
    if (vm.lex->tk==BE_TOKEN_INT) {
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_int_t v = (be_jse_int_t)string_to_int(lexer_get_token(vm.lex)); // atol
            LEXER_MATCH(BE_TOKEN_INT);
            return new_int_symbol(v);
        } else {
            LEXER_MATCH(BE_TOKEN_INT);
            return 0;
        }
    }
    if (vm.lex->tk==BE_TOKEN_FLOAT) {
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_float_t v = string_to_float(lexer_get_token(vm.lex)); // atof
            LEXER_MATCH(BE_TOKEN_FLOAT);
            return new_float_symbol(v);
        } else {
            LEXER_MATCH(BE_TOKEN_FLOAT);
            return 0;
        }
    }
    if (vm.lex->tk==BE_TOKEN_STR) {
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *a = new_str_symbol(lexer_get_token(vm.lex));
            LEXER_MATCH_WITH_RETURN(BE_TOKEN_STR, a);
            return a;
        } else {
            LEXER_MATCH(BE_TOKEN_STR);
            return 0;
        }
    }
    if (vm.lex->tk=='{') {
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *contents = new_symbol(BE_SYM_OBJECT);
            if (!contents) {
                be_jse_execute_error();
                return 0;
            }
            LEXER_MATCH_WITH_RETURN('{', contents);
            while (!be_vm_has_error() && vm.lex->tk != '}') {
                be_jse_symbol_t *varName = 0;
                if (BE_VM_PARSE_SHOULD_EXECUTE) {
                    varName = new_str_symbol(lexer_get_token(vm.lex));
                    if (!varName) {
                        be_jse_execute_error();
                        return contents;
                    }
                }
                if (vm.lex->tk==BE_TOKEN_STR) {
                    LEXER_MATCH_WITH_CLEAN_AND_RETURN(BE_TOKEN_STR, symbol_unlock(varName), contents);
                } else {
                    LEXER_MATCH_WITH_CLEAN_AND_RETURN(BE_TOKEN_ID, symbol_unlock(varName), contents);
                }
                LEXER_MATCH_WITH_CLEAN_AND_RETURN(':', symbol_unlock(varName), contents);
                if (BE_VM_PARSE_SHOULD_EXECUTE) {
                    be_jse_symbol_t *valueVar;
                    be_jse_symbol_t *value = handle_base_expr();
                    if (!value) {
                        be_jse_execute_error();
                        symbol_unlock(varName);
                        return contents;
                    }
                    valueVar = unlock_symbol_value(value);
                    varName = new_named_symbol(varName, valueVar);
                    add_symbol_node(contents, varName);
                    symbol_unlock(valueVar);
                }
                symbol_unlock(varName);
                if (vm.lex->tk != '}') LEXER_MATCH_WITH_RETURN(',', contents);
            }
            LEXER_MATCH_WITH_RETURN('}', contents);
            return contents;
        } else {
            return handle_block_syntax();
        }
    }
    if (vm.lex->tk=='[') {
        int idx = 0;
        be_jse_symbol_t *contents = 0;
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            contents = new_symbol(BE_SYM_ARRAY);
            if (!contents) {
                be_jse_execute_error();
                return 0;
            }
        }
        LEXER_MATCH_WITH_RETURN('[', contents);
        while (!be_vm_has_error() && vm.lex->tk != ']') {
            if (BE_VM_PARSE_SHOULD_EXECUTE) {
                be_jse_symbol_t *a;
                be_jse_symbol_t *aVar;
                be_jse_symbol_t *indexName;
                a = handle_base_expr();
                aVar = unlock_symbol_value(a);
                indexName = new_named_symbol(new_int_symbol(idx),  aVar);

                if (!indexName)
                    be_jse_execute_error();
                else {
                    add_symbol_node(contents, indexName);
                    symbol_unlock(indexName);
                }
                symbol_unlock(aVar);
            }
            if (vm.lex->tk != ']') LEXER_MATCH_WITH_RETURN(',', contents);
            idx++;
        }
        LEXER_MATCH_WITH_RETURN(']', contents);
        return contents;
    }
    if (vm.lex->tk==BE_TOKEN_KW_FUNCTION) {
        LEXER_MATCH(BE_TOKEN_KW_FUNCTION);
        return handle_function_definition();
    }
    if (vm.lex->tk==BE_TOKEN_KW_NEW) {
        // new -> create a new object
        LEXER_MATCH(BE_TOKEN_KW_NEW);

        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *obj;
            be_jse_symbol_t *objClassOrFunc = unlock_symbol_value(lookup_symbol_in_scopes(lexer_get_token(vm.lex)));
            if (!objClassOrFunc) {
                be_jse_warn_at("Prototype used in NEW is not defined", vm.lex, vm.lex->token_start);
            }
            LEXER_MATCH(BE_TOKEN_ID);
            obj = new_symbol(BE_SYM_OBJECT);
            if (symbol_is_function(objClassOrFunc)) {
                symbol_unlock(be_jse_do_function_call(objClassOrFunc, obj, true, 0, NULL));
            } else {
                symbol_unlock(add_symbol_node_name(obj, objClassOrFunc, BE_JSE_PARSE_PROTOTYPE_CLASS));
                if (vm.lex->tk == '(') {
                    LEXER_MATCH('(');
                    LEXER_MATCH(')');
                }
            }
            symbol_unlock(objClassOrFunc);
            return obj;
        } else {
            LEXER_MATCH(BE_TOKEN_ID);
            if (vm.lex->tk == '(') {
                LEXER_MATCH('(');
                LEXER_MATCH(')');
            }
        }
    }
    LEXER_MATCH(BE_TOKEN_END);
    return 0;
}

static be_jse_symbol_t *handle_postfix_syntax()
{
    be_jse_symbol_t *a;
    bool invert = false;

    /* 取反处理 '!'操作 */
    if (vm.lex->tk=='!') {
        LEXER_MATCH('!');
        invert = true;
    }

    a = handle_factor_syntax();

    // ++/-- 运算
    while (MEET_POSTFIX_OP) {
        int op = vm.lex->tk;
        LEXER_MATCH(vm.lex->tk);

        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *one = symbol_lock(vm.executor->oneInt);                                       // a++ 或者 a--
            be_jse_symbol_t *res = symbol_value_maths_op(a, one, op==BE_TOKEN_OP_PLUS_PLUS ? '+' : '-');    // ++ 或者 --

            be_jse_symbol_t *origin;

            symbol_unlock(one);
            origin = get_symbol_value(a); // 保存

            symbol_replace(a, res);

            symbol_unlock(res);

            symbol_unlock(a);
            a = origin;             // 后加后减使用的是原数
        }
    }

    if (invert&&BE_VM_PARSE_SHOULD_EXECUTE) {
        be_jse_symbol_t *zero = symbol_lock(vm.executor->zeroInt);
        be_jse_symbol_t *res = symbol_value_maths_op(a, zero, BE_TOKEN_OP_EQUAL);  // a = !a;
        symbol_unlock(zero);
        symbol_unlock(a);
        a = res;
    }

    return a;
}


/* 处理加/减/乘/除/余 算术运算*/
static be_jse_symbol_t *handle_precedent_expr()
{
    bool negative = false;
    if (vm.lex->tk=='-') {
        LEXER_MATCH('-');
        negative = true;
    }

    /*取第一个操作数*/
    be_jse_symbol_t *first_ops = handle_postfix_syntax();

    /*根据运算操作的规则，*,/,% 优先级最高，先处理这些操作
    * 一直处理，直到不是 *，/，% 为止
    *   a = a*b*c +e*f+g;
    */
    while (vm.lex->tk=='*' || vm.lex->tk=='/' || vm.lex->tk=='%') {         // *，/，% 操作
        be_jse_symbol_t *b;

        //取操作符号
        int op = vm.lex->tk;
        LEXER_MATCH(vm.lex->tk);

        /*取第二个操作数*/
        b = handle_postfix_syntax();

        //计算结果，赋值给第一个操作数;
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *res = symbol_value_maths_op(first_ops, b, op);     // first =a*b;
            symbol_unlock(first_ops);
            first_ops = res;
        }
        symbol_unlock(b);
    }

    //如果是负值，则反号

    if (negative) {
        be_jse_symbol_t *zero   = symbol_lock(vm.executor->zeroInt);
        be_jse_symbol_t *res    = symbol_value_maths_op(zero, first_ops, '-');
        symbol_unlock(zero);
        symbol_unlock(first_ops);
        first_ops               = res;
    }

    // 如果 当前的运算为： +/-
    while (vm.lex->tk=='+' || vm.lex->tk=='-') {
        int op      = vm.lex->tk;
        LEXER_MATCH(vm.lex->tk);

        be_jse_symbol_t *second_ops = handle_postfix_syntax();

        /*根据运算操作的规则，*,/,% 优先级最高，先处理这些操作 */
        while (vm.lex->tk=='*' || vm.lex->tk=='/' || vm.lex->tk=='%') {
            be_jse_symbol_t *c;
            int op = vm.lex->tk;
            LEXER_MATCH(vm.lex->tk);

            c = handle_postfix_syntax();

            if (BE_VM_PARSE_SHOULD_EXECUTE) {
                be_jse_symbol_t *res = symbol_value_maths_op(second_ops, c, op);
                symbol_unlock(second_ops);
                second_ops = res;
            }
            symbol_unlock(c);
        }

        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *res = symbol_value_maths_op(first_ops, second_ops, op);
            symbol_unlock(first_ops);
            first_ops = res;
        }
        symbol_unlock(second_ops);
    }

    /* 移位操作 */
    if (MEET_SHIFT_OP) {
        be_jse_symbol_t *b;

        int op = vm.lex->tk;
        LEXER_MATCH(op);

        b = handle_base_expr();

        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *res = symbol_value_maths_op(first_ops, b, op);
            symbol_unlock(first_ops);
            first_ops = res;
        }
        symbol_unlock(b);

    }
    return first_ops;
}


/*  */
static be_jse_symbol_t *handle_base_expr()
{
    be_jse_symbol_t *right_ops;
    be_jse_symbol_t *second_ops;

    //获取第一个操作数
    be_jse_symbol_t *left_ops = handle_precedent_expr();

    while (MEET_RELATIONAL_OP) {
        int op = vm.lex->tk;
        LEXER_MATCH(vm.lex->tk);

        second_ops = handle_precedent_expr();

        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *res = symbol_value_maths_op(left_ops, second_ops, op);
            symbol_unlock(left_ops);
            left_ops = res;
        }
        symbol_unlock(second_ops);
    }

    /* 处理 &, | ,^,&&,|| */
    while ( MEET_BIT_ARITHMATICAL_OP || MEET_LOGICAL_OP) {
        bool short_circuit  = false;            /* 逻辑判断短路原则：if (0&&(a>b)&&a>0)) 或者   if(1||(a>0)) */
        bool boolean        = false;
        int op              = vm.lex->tk;

        be_jse_symbol_t *b;

        LEXER_MATCH(vm.lex->tk);
        if (op==BE_TOKEN_OP_LOGIC_AND) {
            op = '&';
            short_circuit = !get_symbol_value_bool(left_ops);
            boolean = true;
        } else if (op==BE_TOKEN_OP_LOGIC_OR) {
            op = '|';
            short_circuit = get_symbol_value_bool(left_ops);
            boolean = true;
        }

        BE_SAVE_VM_EXE_STATE();
        if (short_circuit) set_vm_idle();

        //获取第二个操作数
        b  = handle_precedent_expr();

        while (MEET_RELATIONAL_OP) {
            int op = vm.lex->tk;
            LEXER_MATCH(vm.lex->tk);

            second_ops = handle_precedent_expr();

            if (BE_VM_PARSE_SHOULD_EXECUTE) {
                be_jse_symbol_t *res = symbol_value_maths_op(b, second_ops, op);
                symbol_unlock(b);
                b = res;
            }
            symbol_unlock(second_ops);
        }

        if (short_circuit) BE_RESTORE_VM_EXE_STATE();

        if (BE_VM_PARSE_SHOULD_EXECUTE && !short_circuit) {
            be_jse_symbol_t *res;
            if (boolean) {
                be_jse_symbol_t *newa = new_bool_symbol(get_symbol_value_bool(left_ops));
                be_jse_symbol_t *newb = new_bool_symbol(get_symbol_value_bool(b));
                symbol_unlock(left_ops);

                left_ops = newa;

                symbol_unlock(b);
                b = newb;
            }
            res = symbol_value_maths_op(left_ops, b, op);
            symbol_unlock(left_ops);
            left_ops = res;
        }
        symbol_unlock(b);
    }

    //三元运算符处理
    if (vm.lex->tk=='?') {
        LEXER_MATCH('?');
        if (!BE_VM_PARSE_SHOULD_EXECUTE) {
            symbol_unlock(handle_base_expr());
            LEXER_MATCH(':');
            symbol_unlock(handle_base_expr());
        } else {
            bool first = get_symbol_value_bool(left_ops);
            symbol_unlock(left_ops);

            if (first) {
                left_ops = handle_base_expr();          //嵌套调用
                LEXER_MATCH(':');
                BE_SAVE_VM_EXE_STATE();
                set_vm_idle();
                symbol_unlock(handle_base_expr());
                BE_RESTORE_VM_EXE_STATE();
            } else {
                BE_SAVE_VM_EXE_STATE();

                set_vm_idle();
                symbol_unlock(handle_base_expr());
                BE_RESTORE_VM_EXE_STATE();
                LEXER_MATCH(':');
                left_ops = handle_base_expr();
            }
        }
    }

    // 赋值处理：+=/-=
    if (MEET_ASSIGNMENT_OP) {
        int op;

        if (BE_VM_PARSE_SHOULD_EXECUTE && left_ops && !left_ops->refs) {
            if (symbol_is_name(left_ops)) {
                be_jse_symbol_t* root = symbol_lock(vm.executor->root);
                add_symbol_node(root, left_ops);
                symbol_unlock(root);
            } else {
                be_jse_warn_at("Trying to assign to an un-named type\n", vm.lex, vm.lex->token_last_end);
            }
        }

        op = vm.lex->tk;

        LEXER_MATCH(vm.lex->tk);

        right_ops = handle_base_expr();
        right_ops = unlock_symbol_value(right_ops);

        if (BE_VM_PARSE_SHOULD_EXECUTE && left_ops) {
            if (op=='=') {
                symbol_replace(left_ops, right_ops);
            } else if (op==BE_TOKEN_OP_PLUS_EQUAL) {
                be_jse_symbol_t *res = symbol_value_maths_op(left_ops,right_ops, '+');
                symbol_replace(left_ops, res);
                symbol_unlock(res);
            } else if (op==BE_TOKEN_OP_MINUS_EQUAL) {
                be_jse_symbol_t *res = symbol_value_maths_op(left_ops,right_ops, '-');
                symbol_replace(left_ops, res);
                symbol_unlock(res);
            } else {
                be_assert(0);
            }
        }

        symbol_unlock(right_ops);
    }
    return left_ops;
}

static be_jse_symbol_t *handle_block_syntax()
{
    LEXER_MATCH('{');
    if (BE_VM_PARSE_SHOULD_EXECUTE) {
        while (vm.lex->tk && vm.lex->tk!='}') {
            symbol_unlock(handle_statement_syntax());
            if (be_vm_has_error()) return 0;
        }
        LEXER_MATCH('}');
    } else {
        int brackets = 1;
        while (vm.lex->tk && brackets) {
            if (vm.lex->tk == '{') brackets++;
            if (vm.lex->tk == '}') brackets--;
            LEXER_MATCH(vm.lex->tk);
        }
    }
    return 0;
}

be_jse_symbol_t *handle_block_or_statement()
{
    if (vm.lex->tk=='{') { //块操作语句
        return handle_block_syntax();
    } else { // statement
        be_jse_symbol_t *v = handle_statement_syntax();
        if (vm.lex->tk==';') LEXER_MATCH(';');
        return v;
    }
}

/*语句处理部分*/
static be_jse_symbol_t *handle_statement_syntax()
{
    if (vm.lex->tk==BE_TOKEN_ID ||
        vm.lex->tk==BE_TOKEN_INT ||
        vm.lex->tk==BE_TOKEN_FLOAT ||
        vm.lex->tk==BE_TOKEN_STR ||
        vm.lex->tk=='-'     ||
        vm.lex->tk=='(') {
        be_jse_symbol_t *res = handle_base_expr();
        return res;
    } else if (vm.lex->tk=='{') { //程序块 { ... }
        handle_block_syntax();
    } else if (vm.lex->tk==';') { //语法结束符,新的语法开始;
        LEXER_MATCH(';');
    } else if (vm.lex->tk==BE_TOKEN_KW_VAR) { // var 申明语法：var xxx;
        be_jse_symbol_t *last_var = 0;

        LEXER_MATCH(BE_TOKEN_KW_VAR);

        while (vm.lex->tk == BE_TOKEN_ID) {
            be_jse_symbol_t *a = 0;
            if (BE_VM_PARSE_SHOULD_EXECUTE) {
                a = lookup_symbol_on_top(lexer_get_token(vm.lex), true);
                if (!a) {
                    be_jse_execute_error();
                    return last_var;
                }
            }
            LEXER_MATCH_WITH_CLEAN_AND_RETURN(BE_TOKEN_ID, symbol_unlock(a), last_var);

            while (vm.lex->tk == '.') {
                LEXER_MATCH_WITH_CLEAN_AND_RETURN('.', symbol_unlock(a), last_var);
                if (BE_VM_PARSE_SHOULD_EXECUTE) {
                    be_jse_symbol_t *lastA = a;
                    a = lookup_named_child_symbol(get_symbol_node_id(lastA), lexer_get_token(vm.lex), true);
                    symbol_unlock(lastA);
                }
                LEXER_MATCH_WITH_CLEAN_AND_RETURN(BE_TOKEN_ID, symbol_unlock(a), last_var);
            }

            if (vm.lex->tk == '=') { //赋值语句:var a =b ;
                be_jse_symbol_t *b;
                LEXER_MATCH_WITH_CLEAN_AND_RETURN('=', symbol_unlock(a), last_var);

                b = handle_base_expr();

                if (BE_VM_PARSE_SHOULD_EXECUTE) {
                    symbol_replace(a, b);
                }
                symbol_unlock(b);
            }

            symbol_unlock(last_var);
            last_var = a;

            if (vm.lex->tk != ';' && vm.lex->tk != BE_TOKEN_KW_IN) { //如果不以 “;" 结束，或者   a in array
                LEXER_MATCH_WITH_RETURN(',', last_var);     //逗号运算符
            }
        }
        return last_var;
    } else if (vm.lex->tk==BE_TOKEN_KW_IF) { // 'if' 判断条件语句
        bool cond_result;
        be_jse_symbol_t *var;

        // 处理 'if' 语句的判断条件
        LEXER_MATCH(BE_TOKEN_KW_IF);
        LEXER_MATCH('(');

        var = handle_base_expr();

        LEXER_MATCH(')');

        // 最终结果是 TRUE or FALSE
        cond_result = BE_VM_PARSE_SHOULD_EXECUTE && get_symbol_value_bool(var);
        symbol_unlock(var);

        BE_SAVE_VM_EXE_STATE();

        //'if' 条件为 FALSE 时,把 VM 设置成 IDLE 状态，后续的语法块处理时，不处理逻辑，只是对'{}'对进行遍历，找到{}语句结束块
        if (!cond_result) set_vm_idle();

        symbol_unlock(handle_block_or_statement());

        if (!cond_result) BE_RESTORE_VM_EXE_STATE();

        //处理'else' 条件处理逻辑

        if (vm.lex->tk==BE_TOKEN_KW_ELSE) {
            LEXER_MATCH(BE_TOKEN_KW_ELSE);

            BE_SAVE_VM_EXE_STATE();

            if (cond_result) set_vm_idle();

            symbol_unlock(handle_block_or_statement());

            if (cond_result) BE_RESTORE_VM_EXE_STATE();
        }
    } else if (vm.lex->tk==BE_TOKEN_KW_WHILE) {     // 'while' 循环语句
        int loopCount = BE_JSE_PARSE_MAX_LOOPS;     //循环次数，避免死循环

        int whileCondStart;     //循环条件起始位置
        int whileBodyStart;     //循环体的起始位置
        bool loopCond;          // 循环条件

        be_jse_lex_ctx_t whileCond;  //循环条件
        be_jse_lex_ctx_t whileBody;  //循环体

        be_jse_lex_ctx_t *oldLex;
        be_jse_symbol_t *cond;

        bool hasHadBreak = false;

        // 处理 while()

        LEXER_MATCH(BE_TOKEN_KW_WHILE);
        LEXER_MATCH('(');

        whileCondStart = vm.lex->token_start;

        cond = handle_base_expr();      //处理条件部分的语句，到 ')'结束

        loopCond = BE_VM_PARSE_SHOULD_EXECUTE && get_symbol_value_bool(cond);

        symbol_unlock(cond);

        be_jse_lexer_init2(&whileCond, vm.lex, whileCondStart); //初始化到开始处理位置

        LEXER_MATCH(')');

        //记住当前的循环起始位置，做为下一次循环的开始；

        whileBodyStart = vm.lex->token_start;

        BE_SAVE_VM_EXE_STATE();

        if (!loopCond) set_vm_idle();

        symbol_unlock(handle_block_or_statement());

        if (vm.execute == EXEC_CONTINUE) vm.execute = EXEC_NORMAL;

        if (vm.execute == EXEC_BREAK) {
            vm.execute  = EXEC_NORMAL;
            hasHadBreak = true;         // 退出循环
        }

        if (!loopCond) BE_RESTORE_VM_EXE_STATE();

        be_jse_lexer_init2(&whileBody, vm.lex, whileBodyStart);
        oldLex = vm.lex;

        //处理主循环
        while (!hasHadBreak && loopCond && loopCount-->0) {
            be_jse_lexer_reset(&whileCond);
            vm.lex      = &whileCond;
            cond        = handle_base_expr();
            loopCond    = BE_VM_PARSE_SHOULD_EXECUTE && get_symbol_value_bool(cond);
            symbol_unlock(cond);

            if (loopCond) {
                be_jse_lexer_reset(&whileBody);
                vm.lex = &whileBody;
                symbol_unlock(handle_block_or_statement());

                if (vm.execute == EXEC_CONTINUE) vm.execute = EXEC_NORMAL;

                if (vm.execute == EXEC_BREAK) {
                    vm.execute = EXEC_NORMAL;
                    hasHadBreak = true;
                }
            }
        }

        //循环处理结束
        vm.lex = oldLex;
        be_jse_lexer_deinit(&whileCond);
        be_jse_lexer_deinit(&whileBody);

        if (loopCount<=0) {
            be_jse_error_at("WHILE Loop exceeded the maximum number of iterations", vm.lex, vm.lex->token_last_end);
            be_jse_execute_error();
        }
    } else if (vm.lex->tk==BE_TOKEN_KW_FOR) {
        LEXER_MATCH(BE_TOKEN_KW_FOR);
        LEXER_MATCH('(');

        be_jse_symbol_t *forStatement = handle_statement_syntax();

        if (vm.lex->tk == BE_TOKEN_KW_IN) {
            // for (i in array)
            // where i = symbol_unlock(forStatement);

            if (!symbol_is_name(forStatement)) {
                symbol_unlock(forStatement);
                be_jse_error_at("FOR a IN b - 'a' must be a variable name", vm.lex, vm.lex->token_last_end);
                be_jse_execute_error();
                return 0;
            }
            bool addedIteratorToScope = false;
            if (BE_VM_PARSE_SHOULD_EXECUTE && !forStatement->refs) {
                // if the variable did not exist, add it to the scope
                addedIteratorToScope = true;
                be_jse_symbol_t *root = symbol_lock(vm.executor->root);
                add_symbol_node(root, forStatement);
                symbol_unlock(root);
            }
            LEXER_MATCH(BE_TOKEN_KW_IN);
            be_jse_symbol_t *array = unlock_symbol_value(handle_precedent_expr());
            LEXER_MATCH(')');

            int forBodyStart = vm.lex->token_start;
            BE_SAVE_VM_EXE_STATE();
            set_vm_idle();

            symbol_unlock(handle_block_or_statement());

            BE_RESTORE_VM_EXE_STATE();
            be_jse_lex_ctx_t forBody;
            be_jse_lexer_init2(&forBody, vm.lex, forBodyStart);
            be_jse_lex_ctx_t *oldLex = vm.lex;

            be_jse_node_t loopIndex = 0;
            if (symbol_is_array(array) || symbol_is_object(array)) {
                loopIndex = array->first_child;
            } else {
                be_jse_error_at("FOR loop can only iterate over Arrays or Objects", vm.lex, vm.lex->token_last_end);
                be_jse_execute_error();
            }

            bool hasHadBreak = false;

            while (BE_VM_PARSE_SHOULD_EXECUTE && loopIndex && !hasHadBreak) {
                be_jse_symbol_t *loopIndexVar   = symbol_lock(loopIndex);
                be_jse_symbol_t *indexValue     = symbol_name_cp(loopIndexVar, false, true);

                be_assert(symbol_is_name(indexValue) && indexValue->refs==0);
                indexValue->flags &= ~BE_SYM_NAME;                  // make sure this is NOT a name
                set_symbol_node_name(forStatement, indexValue);
                symbol_unlock(indexValue);
                loopIndex = loopIndexVar->next_sibling;
                symbol_unlock(loopIndexVar);

                be_jse_lexer_reset(&forBody);
                vm.lex = &forBody;
                symbol_unlock(handle_block_or_statement());

                if (vm.execute == EXEC_CONTINUE) vm.execute = EXEC_NORMAL;

                if (vm.execute == EXEC_BREAK) {
                    vm.execute = EXEC_NORMAL;
                    hasHadBreak = true;
                }
            }
            vm.lex = oldLex;
            be_jse_lexer_deinit(&forBody);

            if (addedIteratorToScope) {
                be_jse_symbol_t *rootScope = symbol_lock(vm.executor->root);
                remove_child_symbol(rootScope, forStatement);
                symbol_unlock(rootScope);
            }
            symbol_unlock(forStatement);
            symbol_unlock(array);

        } else { //  for (;;)
            int loopCount = BE_JSE_PARSE_MAX_LOOPS;
            be_jse_symbol_t *cond;
            bool loopCond;
            be_jse_lex_ctx_t forCond;
            be_jse_lex_ctx_t forIter;
            bool hasHadBreak = false;

            symbol_unlock(forStatement);
            LEXER_MATCH(';');
            int forCondStart = vm.lex->token_start;
            cond = handle_base_expr();
            loopCond = BE_VM_PARSE_SHOULD_EXECUTE && get_symbol_value_bool(cond);
            symbol_unlock(cond);
            be_jse_lexer_init2(&forCond, vm.lex, forCondStart);
            LEXER_MATCH(';');
            int forIterStart = vm.lex->token_start;
            {
                BE_SAVE_VM_EXE_STATE();
                set_vm_idle();
                symbol_unlock(handle_base_expr()); // iterator
                BE_RESTORE_VM_EXE_STATE();
            }
            be_jse_lexer_init2(&forIter, vm.lex, forIterStart);
            LEXER_MATCH(')');
            int forBodyStart = vm.lex->token_start;
            BE_SAVE_VM_EXE_STATE();
            if (!loopCond) set_vm_idle();
            symbol_unlock(handle_block_or_statement());

            if (vm.execute == EXEC_CONTINUE) vm.execute = EXEC_NORMAL;

            if (vm.execute == EXEC_BREAK) {
                vm.execute = EXEC_NORMAL;
                hasHadBreak = true;
            }
            if (!loopCond) BE_RESTORE_VM_EXE_STATE();
            be_jse_lex_ctx_t forBody;
            be_jse_lexer_init2(&forBody, vm.lex, forBodyStart);
            be_jse_lex_ctx_t* oldLex = vm.lex;

            if (loopCond) {
                be_jse_lexer_reset(&forIter);
                vm.lex = &forIter;
                symbol_unlock(handle_base_expr());
            }
            while (!hasHadBreak && BE_VM_PARSE_SHOULD_EXECUTE && loopCond && loopCount-->0) {
                be_jse_lexer_reset(&forCond);
                vm.lex      = &forCond;
                cond        = handle_base_expr();
                loopCond    = get_symbol_value_bool(cond);
                symbol_unlock(cond);

                if (BE_VM_PARSE_SHOULD_EXECUTE && loopCond) {
                    be_jse_lexer_reset(&forBody);
                    vm.lex = &forBody;
                    symbol_unlock(handle_block_or_statement());
                    if (vm.execute == EXEC_CONTINUE) vm.execute = EXEC_NORMAL;

                    if (vm.execute == EXEC_BREAK) {
                        vm.execute = EXEC_NORMAL;
                        hasHadBreak = true;
                    }
                }
                if (BE_VM_PARSE_SHOULD_EXECUTE && loopCond) {
                    be_jse_lexer_reset(&forIter);
                    vm.lex = &forIter;
                    symbol_unlock(handle_base_expr());
                }
            }
            vm.lex = oldLex;
            be_jse_lexer_deinit(&forCond);
            be_jse_lexer_deinit(&forIter);
            be_jse_lexer_deinit(&forBody);
            if (loopCount<=0) {
                be_jse_error_at("FOR Loop exceeded the maximum number of iterations", vm.lex, vm.lex->token_last_end);
                be_jse_execute_error();
            }
        }
    } else if (vm.lex->tk==BE_TOKEN_KW_RETURN) {
        be_jse_symbol_t *result = 0;
        LEXER_MATCH(BE_TOKEN_KW_RETURN);

        if (vm.lex->tk != ';') { // return 返回带有表达式的处理
            result = unlock_symbol_value(handle_base_expr());
        }
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *resultVar = lookup_symbol_on_top(BE_JSE_PARSE_RETURN_VAR, false);
            if (resultVar) {
                symbol_replace(resultVar, result);
                symbol_unlock(resultVar);
            } else {
                be_jse_error_at("RETURN statement, but not in a function.\n", vm.lex, vm.lex->token_last_end);
                be_jse_execute_error();
            }
            set_vm_idle();
        }
        symbol_unlock(result);
    } else if (vm.lex->tk==BE_TOKEN_KW_FUNCTION) { /* 函数 */
        be_jse_symbol_t *funcName = 0;
        be_jse_symbol_t *funcVar;

        LEXER_MATCH(BE_TOKEN_KW_FUNCTION);
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            funcName = new_named_symbol(new_str_symbol(lexer_get_token(vm.lex)), 0);
        }

        if (!funcName) {
            be_jse_execute_error();
            return 0;
        }
        LEXER_MATCH(BE_TOKEN_ID);
        funcVar = handle_function_definition();
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            be_jse_symbol_t *existingFunc = lookup_symbol_name_on_top(funcName, true);
            symbol_replace(existingFunc, funcVar);
            symbol_unlock(funcName);
            funcName = existingFunc;
        }
        symbol_unlock(funcVar);
        return funcName;
    } else if (vm.lex->tk==BE_TOKEN_KW_CONTINUE) {
        LEXER_MATCH(BE_TOKEN_KW_CONTINUE);
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            vm.execute = EXEC_CONTINUE;
        }
    } else if (vm.lex->tk==BE_TOKEN_KW_BREAK) {
        LEXER_MATCH(BE_TOKEN_KW_BREAK);
        if (BE_VM_PARSE_SHOULD_EXECUTE) {
            vm.execute = EXEC_BREAK;
        }
    } else if (vm.lex->tk==BE_TOKEN_KW_SWITCH) {
        /*
            处理switch 语法

            switch (a)
            {
                case b:
                ...
                break;
                case 1:
                ...
                break;

                default:
                ...
            }
        */

        LEXER_MATCH(BE_TOKEN_KW_SWITCH);

        //分析switch 中的基本语句

        LEXER_MATCH('(');

        be_jse_symbol_t *switchOn = handle_base_expr();

        LEXER_MATCH_WITH_CLEAN_AND_RETURN(')', symbol_unlock(switchOn), 0);

        /*
            {...}
        */

        LEXER_MATCH_WITH_CLEAN_AND_RETURN('{', symbol_unlock(switchOn), 0);

        BE_SAVE_VM_EXE_STATE();
        bool execute        = BE_VM_PARSE_SHOULD_EXECUTE;
        bool hasExecuted    = false;
        if (execute) vm.execute=EXEC_IDLE;

        /*处理 case 语句*/

        while (vm.lex->tk==BE_TOKEN_KW_CASE) {
            LEXER_MATCH_WITH_CLEAN_AND_RETURN(BE_TOKEN_KW_CASE, symbol_unlock(switchOn), 0);
            be_execflag_e oldFlags  = vm.execute;
            if (execute) vm.execute = EXEC_NORMAL;
            be_jse_symbol_t *test   = handle_base_expr();       /* 处理基本逻辑*/
            vm.execute              = oldFlags;

            LEXER_MATCH_WITH_CLEAN_AND_RETURN(':', symbol_unlock(switchOn); symbol_unlock(test), 0);
            bool cond = false;
            if (execute)
                cond = get_bool_symbol_and_unlock(symbol_value_maths_op(switchOn, test, BE_TOKEN_OP_EQUAL));

            if (cond) hasExecuted = true;
            symbol_unlock(test);
            if (cond && vm.execute==EXEC_IDLE) vm.execute=EXEC_NORMAL;

            while (vm.lex->tk!=BE_TOKEN_END && vm.lex->tk!=BE_TOKEN_KW_CASE && vm.lex->tk!=BE_TOKEN_KW_DEFAULT && vm.lex->tk!='}')
                symbol_unlock(handle_block_or_statement());
        }
        symbol_unlock(switchOn);

        if (execute && vm.execute==EXEC_BREAK) vm.execute=EXEC_NORMAL;

        BE_RESTORE_VM_EXE_STATE();

        /* default 处理; */

        if (vm.lex->tk==BE_TOKEN_KW_DEFAULT) {
            LEXER_MATCH(BE_TOKEN_KW_DEFAULT);
            LEXER_MATCH(':');

            BE_SAVE_VM_EXE_STATE();

            if (hasExecuted) set_vm_idle();

            while (vm.lex->tk!=BE_TOKEN_END && vm.lex->tk!='}') {
                symbol_unlock(handle_block_or_statement());
            }

            BE_RESTORE_VM_EXE_STATE();
        }
        LEXER_MATCH('}');
    } else
        LEXER_MATCH(BE_TOKEN_END);

    return 0;
}

void be_jse_executor_init(be_jse_executor_ctx_t *executor)
{
    executor->root = symbol_unlock(get_root_node());

    be_jse_symbol_t *name;

    executor->zeroInt = symbol_unlock(INC_SYMBL_REF(new_int_symbol(0)));
    executor->oneInt = symbol_unlock(INC_SYMBL_REF(new_int_symbol(1)));

    name = lookup_named_child_symbol(executor->root, "String", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->stringClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

    name = lookup_named_child_symbol(executor->root, "Object", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->objectClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

    name = lookup_named_child_symbol(executor->root, "Array", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->arrayClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

    name = lookup_named_child_symbol(executor->root, "Integer", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->intClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

    name = lookup_named_child_symbol(executor->root, "Math", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->mathClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);

    name = lookup_named_child_symbol(executor->root, "JSON", true);
    name->flags |= BE_SYM_NATIVE;
    if (!name->first_child) name->first_child = symbol_unlock(INC_SYMBL_REF(new_symbol(BE_SYM_OBJECT)));
    executor->jsonClass = INC_SYMBL_REF_BY_ID(name->first_child);
    symbol_unlock(name);


}

void be_jse_executor_deinit(be_jse_executor_ctx_t *executor)
{
    DEC_SYMBL_REF_BY_ID(executor->zeroInt);
    DEC_SYMBL_REF_BY_ID(executor->oneInt);
    DEC_SYMBL_REF_BY_ID(executor->stringClass);
    DEC_SYMBL_REF_BY_ID(executor->objectClass);
    DEC_SYMBL_REF_BY_ID(executor->arrayClass);
    DEC_SYMBL_REF_BY_ID(executor->intClass);
    DEC_SYMBL_REF_BY_ID(executor->mathClass);
    DEC_SYMBL_REF_BY_ID(executor->jsonClass);

#ifdef USE_ALINK
    DEC_SYMBL_REF_BY_ID(executor->alinkClass);
#endif

#ifdef USE_PROCESS
    DEC_SYMBL_REF_BY_ID(executor->processClass);
#endif

#ifdef USE_GPIO
    DEC_SYMBL_REF_BY_ID(executor->gpioClass);
#endif

#ifdef USE_UART
    DEC_SYMBL_REF_BY_ID(executor->uartClass);
#endif

    DEC_SYMBL_REF_BY_ID(executor->root);
}

/* execute a JS string */
be_jse_symbol_t *be_jse_eval_string(be_jse_executor_ctx_t *executor, const char *str)
{
    be_execflag_e execute   = EXEC_NORMAL;
    be_jse_lex_ctx_t        lex;
    be_jse_symbol_t *s      = 0;

    BE_SAVE_VM_EXE_STATE();

    be_jse_vm_ctx_t oldExecInfo = vm;

    be_jse_lexer_init(&lex, str, 0, -1);
    be_jse_vm_init(executor, &lex);

    while (!be_vm_has_error() && vm.lex->tk != BE_TOKEN_END) {
        symbol_unlock(s);
        s = handle_block_or_statement(&execute);
    }
    // clean up
    be_jse_vm_deinit();
    be_jse_lexer_deinit(&lex);
    // restore state
    BE_RESTORE_VM_EXE_STATE();
    oldExecInfo.execute = vm.execute; // BE_JSP_RESTORE_EXECUTE has made this ok.
    vm = oldExecInfo;

    // It may have returned a reference, but we just want the value...
    if (s) {
        return unlock_symbol_value(s);
    }

    return 0;
}

bool be_jse_execute_func(be_jse_executor_ctx_t *executor, be_jse_symbol_t *func, int argCount, be_jse_symbol_t **argPtr)
{
    BE_SAVE_VM_EXE_STATE();
    be_jse_vm_ctx_t oldExecInfo = vm;
    be_jse_vm_init(executor, 0);

    be_jse_symbol_t *resultVar = be_jse_do_function_call(func, 0, false, argCount, argPtr);

    bool result = get_symbol_bool(resultVar);
    symbol_unlock(resultVar);
    // clean up
    be_jse_vm_deinit();

    // restore state
    BE_RESTORE_VM_EXE_STATE();

    oldExecInfo.execute = vm.execute; // BE_JSP_RESTORE_EXECUTE has made this ok.
    vm = oldExecInfo;

    return result;
}
