# 本地扩展对象(Addon)开发介绍

使用头文件： `#include <be_jse_module.h>`, 参考例程 `module_timers.c` 、`module_mqtt.c` 等

`JavaScript`的本地扩展对象简称`Addon`，一般是用C语言实现的模块，在`JS`程序可以调用该对象的方法或属性；

当前__`Addon`__开发支持全局对象的扩展， 支持静态自定义对象扩展，__但不支持new一个自定义对象__

编写自定义扩展`Addon`需要了解`JSE`的词法分析、语法分析处理、符号表处理， 在`JSE`中每一个变量、函数都对应一个或多个`be_jse_symbol_t`符号表

举例说明： 处理执行`var num = 1 + 2`语句， 只有`num`会保存在符号表中， `1 + 2`在执行时会一边解析一边计算，将结果`3`保存在`num`的值中，也是保存在符号表中，这个语法块分析处理的结果占用两个符号表`be_jse_symbol_t`，一个保存`num`的名称，一个保存`num`的值`3`

## <span data-type="color" style="color:rgb(47, 84, 235)">Addon类型一， 全局的方法或属性的扩展</span>

以`module_timers.c` 为例作介绍

全局函数:  `setTimeout(callback, delay);`

在语法分析执行时，JS程序中遇到这种语法块`setTimeout(xxx)`，分析到`setTimeout`这个token时，会进入`timers_module_handle_cb`处理，并匹配到`setTimeout`关键词，并进入`setup_timer(0)`继续做语法分析处理，返回一个`be_jse_symbol_t*`对象.

```c
be_jse_symbol_t *timers_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name)
{
    if (!strcmp(name, "setTimeout"))
        return setup_timer(0);
    if (!strcmp(name, "clearTimeout"))
        return clear_timer();
    if (!strcmp(name, "setInterval"))
        return setup_timer(1);
    if (!strcmp(name, "clearInterval"))
        return clear_timer();

    return BE_JSE_FUNC_UNHANDLED;
}
```

`setTimeout`该全局对象方法的语法处理函数

```c
#define MAGIC  0x55ff0055
struct timer_wrap {
    int magic;
    be_jse_symbol_t *cbv;
    long ms;
    int repeat;
};
typedef struct timer_wrap timer_wrap_t;

static void timer_action(void *arg);

static void call_action(void *arg)
{
    be_jse_symbol_t *func = (be_jse_symbol_t*)arg;

    be_jse_execute_func(bone_engine_get_executor(), func, 0, NULL);

    DEC_SYMBL_REF(func);

}


static void timer_action(void *arg)
{
    LOGD("MODULE_TIMERS"," 1 aos_task_name=%s \r\n", aos_task_name());

    timer_wrap_t *t = (timer_wrap_t *)arg;

    LOGD("MODULE_TIMERS","JS handle =%d \n", t);

    if( t->magic == MAGIC ) {

        be_jse_symbol_t *func = get_symbol_value(t->cbv);
        if (symbol_is_function(func)) {
            INC_SYMBL_REF(func);
            // 需要考虑在callback中clearInterval/clearTimeout自身
            aos_schedule_call(call_action, func);
        }
        symbol_unlock(func);
        if (t->repeat) {
            aos_post_delayed_action(t->ms, timer_action, t);
        } else {
            symbol_unlock(t->cbv);
            t->magic = 0;
            aos_free(t);
            LOGI("MODULE_TIMERS","aos_free(%p) \n", t);
        }
    } else {
        LOGW("MODULE_TIMERS","alread free \n");
    }

    LOGD("MODULE_TIMERS"," 2 aos_task_name=%s \r\n", aos_task_name());

}

static be_jse_symbol_t *setup_timer(int repeat)
{
    be_jse_symbol_t *cbv;
    be_jse_symbol_t *msv;
    be_jse_handle_function(0, &cbv, &msv, NULL, NULL);

    be_jse_symbol_t *func = get_symbol_value(cbv);
    if( !func || !msv || (!symbol_is_function(func))) {
        LOGE("MODULE_TIMERS","Bad parameter \n");
        symbol_unlock(func);
        symbol_unlock(cbv);
        symbol_unlock(msv);
        return new_symbol(BE_SYM_NULL);
    }
    symbol_unlock(func);

    long ms = get_symbol_value_int(msv);
    symbol_unlock(msv);

    timer_wrap_t *t = aos_malloc(sizeof(*t));
    t->magic = MAGIC;
    t->cbv = cbv;
    t->ms = ms;
    t->repeat = repeat;
    aos_post_delayed_action(ms, timer_action, t);

    LOGD("MODULE_TIMERS","JS handle =%d \n", t);

    return new_int_symbol((be_jse_int_t)t);
}

```

## <span data-type="color" style="color:rgb(47, 84, 235)">Addon类型二, 自定义对象的属性或方法的扩展</span>

以`module_mqtt.c`为例作介绍

`MQTT`对象扩展一个方法、一个属性, API如下

* `MQTT.VERSION`
* `MQTT.sign(obj)`

注册对象扩展函数，在语法分析时遇到`MQTT`这个token就进入该`module_handle_cb`函数做语法处理

```c
#define JS_MQTT_TAG  "MQTT"
be_jse_module_load(JS_MQTT_TAG, module_handle_cb);
```

`MQTT`对象的语法处理函数

```c
static be_jse_symbol_t* module_handle_cb(be_jse_vm_ctx_t* execInfo, be_jse_symbol_t* var, const char* name)
{
    int ret;
    be_jse_symbol_t* arg0 = NULL;
    be_jse_symbol_t* arg1 = NULL;

    char productKey[16];
    char deviceName[32];
    char deviceSecret[48];

    // 属性
    if (strcmp(name, "VERSION") == 0) {
        lexer_token_cmp(execInfo->lex, BE_TOKEN_ID);
        return new_str_symbol("0.0.1");
    }

    // 方法
    if(strcmp(name, "sign") == 0 ){
        be_jse_handle_function(0, &arg0, NULL, NULL, NULL);
        if (symbol_is_object(arg0)) {

                be_jse_symbol_t* productKeySymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "productKey", false));
                be_jse_symbol_t* deviceNameSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "deviceName", false));
                be_jse_symbol_t* deviceSecretSymbol = unlock_symbol_value(lookup_named_child_symbol(get_symbol_node_id(arg0), "deviceSecret", false));
                symbol_unlock(arg0);


                if (productKeySymbol == NULL
                    || deviceNameSymbol == NULL
                    || deviceSecretSymbol == NULL
                ) {

                    LOGW(JS_MQTT_TAG, "参数错误");
                    return new_str_symbol(NULL);
                }


                symbol_to_str(productKeySymbol, productKey, sizeof(productKey));
                symbol_to_str(deviceNameSymbol, deviceName, sizeof(deviceName));
                symbol_to_str(deviceSecretSymbol, deviceSecret, sizeof(deviceSecret));


                symbol_unlock(productKeySymbol);
                symbol_unlock(deviceNameSymbol);
                symbol_unlock(deviceSecretSymbol);

                unsigned char content[256] = {0};
                sprintf((char*)content, "clientId%sdeviceName%sproductKey%s", deviceName, deviceName, productKey);

                mbedtls_sha1_context sha1_ctx;
                unsigned char k_ipad[64] = {0};
                unsigned char k_opad[64] = {0};
                unsigned char out[20];

                memcpy(k_ipad, deviceSecret, strlen(deviceSecret));
                memcpy(k_opad, deviceSecret, strlen(deviceSecret));

                for (int i = 0; i < sizeof(k_ipad); i++) {
                    k_ipad[i] ^= 0x36;
                    k_opad[i] ^= 0x5c;
                }

                mbedtls_sha1_init(&sha1_ctx);
                mbedtls_sha1_starts(&sha1_ctx);
                mbedtls_sha1_update(&sha1_ctx, k_ipad, sizeof(k_ipad));
                mbedtls_sha1_update(&sha1_ctx, content, strlen((char*)content));
                mbedtls_sha1_finish(&sha1_ctx, out);

                mbedtls_sha1_init(&sha1_ctx);
                mbedtls_sha1_starts(&sha1_ctx);
                mbedtls_sha1_update(&sha1_ctx, k_opad, sizeof(k_opad));
                mbedtls_sha1_update(&sha1_ctx, out, sizeof(out));
                mbedtls_sha1_finish(&sha1_ctx, out);
                mbedtls_sha1_free(&sha1_ctx);

                char sign[41] = {0};
                for (int i=0; i<sizeof(out); ++i) {
                    unsigned char byte[2] = {0};
                    byte[0] = out[i] / 16;
                    byte[1] = out[i] % 16;

                    for(int j=0; j<2; ++j) {
                        if(byte[j] >= 0 && byte[j] <= 9)
                            sign[2 * i + j] = '0' + byte[j];
                        else
                            sign[2 * i + j] = 'a' + byte[j] - 10;
                    }
                }

                return new_str_symbol(sign);
            }
    }
    ... ...
}
```

### <span data-type="color" style="color:rgb(47, 84, 235)">回调或事件输出介绍</span>

#### <span data-type="color" style="color:rgb(47, 84, 235)">名词解释</span>

* __回调函数(callback)与事件处理(event)__
    回调和事件处理程序本质上并无区别，只是在不同情况下的不同叫法。
    
    回调函数
    
    * 程序（JS）执行某个API方法操作时不阻塞，在另一个task中执行具体工作，JSE顺序分析执行接下的语法块，在具体工作完成之后或指定时间到达之后再来执行该回调函数，典型应用如下:
        * MQTT.start(config, callback),  MQTT.start函数会立即返回，不会阻塞主任务，MQTT的启动、连接等实际工作在另一个task中执行，执行完成之后再来执行callback函数
        * setTimeout(callback, delay),  在定时器到期后执行callback函数
    
    事件处理：
    * 在JS主任务中，事件循环处理中收到其它task的发来的事件请求后执行指定的事件回调函数
    * MQTT.subscribe(topic, callback), MQTT.subscribe函数会立即返回，不会阻塞主任务，在MQTT任务中收到订阅的topic之后会发事件请求给主任务，主任务收到该事件之后调用callback函数
    
* <strong>符号表(</strong><strong><code>be_jse_symbol_t</code></strong><strong>)</strong>
    JSE解析JavaScript时把所有JS对象都会记录保持在符号表中，每个符号表的两中重要标识，一个是`refs`引用计数，与作用域有关，还有一个是`locks`锁定计数，一般与函数使用有关，在函数中使用时+1，不再使用时-1，只有当引用计数和锁定计数都为`0`时，回收该对象，释放该符号表。
    符号表的基本类型主要有： `NULL`，`字符串`，`数组`，`对象`，`函数`,`整数`，`浮点数`，`对象名称`，`本地扩展对象`
    举例`var a=5`说明， 在符号表数组中，`a`是对象名称，`5`一个整数类型的值。

#### <span data-type="color" style="color:rgb(47, 84, 235)">开发详解 </span>

<span data-type="color" style="color:rgb(38, 38, 38)">（具体实现可以参考 module_mqtt.c、module_timers.c 等）</span>

* #### __<span data-type="color" style="color:rgb(47, 84, 235)">如何产生异步调用（异步编程模型）？</span>__

耗时操作应该在一个new task中运行，结果通过aos\_schedule\_call函数在`action`中输出， 通过JS callback函数输出，（C调用javascript函数）在JavaScript主任务上下文中执行

```c
static void call_action(void *arg)
{
    be_jse_symbol_t *func = (be_jse_symbol_t*)arg;
    be_jse_execute_func(bone_engine_get_executor(), func, 0, NULL);
    DEC_SYMBL_REF(func);
}
```

#### 
* #### <strong><span data-type="color" style="color:rgb(47, 84, 235)">C 如何调用</span></strong><strong><span data-type="color" style="color:rgb(47, 84, 235)"><code>JavaScript</code></span></strong><strong><span data-type="color" style="color:rgb(47, 84, 235)">的函数</span></strong>

`C`调用`Javascript`函数，一般采用 <strong><span data-type="color" style="color:rgb(47, 84, 235)"><code>be_jse_execute_func</code></span></strong><strong> </strong>API函数，<strong><span data-type="color" style="color:rgb(47, 84, 235)"><code>be_jse_execute_func</code></span></strong><strong><span data-type="color" style="color:rgb(47, 84, 235)"> </span></strong><span data-type="color" style="color:rgb(38, 38, 38)">函数原始如下：</span>

```c
bool be_jse_execute_func(be_jse_executor_ctx_t *executor, be_jse_symbol_t *func, int argCount, be_jse_symbol_t **argPtr);
```

__注意事件__
1.事件通知:
如何需要多次事件通知， func不能释放，可以参考module\_timers.c
2.`BE_ASYNC_S* async` 使用完之后需要释放

### <span data-type="color" style="color:rgb(47, 84, 235)">如何构造数组？</span>

示例： `['abcd', 'abcd', 'abcd','abcd','abcd']`

```c
int i;
be_jse_symbol_t *arr = new_symbol(BE_SYM_ARRAY);
for (i=0; i<5; i++) {
    be_jse_symbol_t *val = new_str_symbol("abcd");
    be_jse_symbol_t *idx = new_named_symbol(new_int_symbol(i), val);
    symbol_unlock(val);
    add_symbol_node(arr, idx);
    symbol_unlock(idx);
}
```

### <span data-type="color" style="color:rgb(47, 84, 235)">如何构造对象？</span>

示例：`{"name": "IoT"}`

```c
be_jse_symbol_t *obj = new_symbol(BE_SYM_OBJECT);
be_jse_symbol_t *name = new_str_symbol("IoT");
be_jse_symbol_t *val = add_symbol_node_name(obj, name, "name");
symbol_unlock(name);
symbol_unlock(val);
```

* __根据JSON字符串构造对象或数组__
    `be_jse_symbol_t *new_json_symbol(char* json_str, size_t json_str_len)`

### <span data-type="color" style="color:rgb(47, 84, 235)">JSE重点API介绍</span>

* __符号相关函数__
    JSE中符号表类型`be_jse_symbol_type_e`

```js
// 符号表类型判断
bool symbol_is_root(const be_jse_symbol_t *v);
bool symbol_is_int(const be_jse_symbol_t *s);
bool symbol_is_float(const be_jse_symbol_t *s);
bool symbol_is_string(const be_jse_symbol_t *s);
bool symbol_is_string_ext(const be_jse_symbol_t *s);
bool symbol_is_number(const be_jse_symbol_t *s);
bool symbol_is_function(const be_jse_symbol_t *s);
bool symbol_is_function_argv(const be_jse_symbol_t *s);
bool symbol_is_object(const be_jse_symbol_t *s);
bool symbol_is_array(const be_jse_symbol_t *s);
bool symbol_is_native(const be_jse_symbol_t *s);
bool symbol_is_undefined(const be_jse_symbol_t *s);
bool symbol_is_null(const be_jse_symbol_t *s);
bool symbol_is_basic(const be_jse_symbol_t *s);
bool symbol_is_name(const be_jse_symbol_t *s);
// 新建符号表对象
be_jse_symbol_t *new_symbol(be_jse_symbol_type_e type)
be_jse_symbol_t *new_str_symbol(const char *str)
be_jse_symbol_t *new_int_symbol(be_jse_int_t value)
be_jse_symbol_t *new_bool_symbol(bool value)
be_jse_symbol_t *new_float_symbol(be_jse_float_t value)
be_jse_symbol_t *new_json_symbol(char* json_str, size_t json_str_len)
// 锁定或解锁
be_jse_symbol_t *symbol_lock(be_jse_node_t id);
be_jse_symbol_t *symbol_relock(be_jse_symbol_t *s);
be_jse_node_t    symbol_unlock(be_jse_symbol_t *s);
// 获取符号表的值
be_jse_int_t     get_symbol_int(be_jse_symbol_t *s);
be_jse_bool_t    get_symbol_bool(be_jse_symbol_t *s);
be_jse_symbol_t *get_symbol_value(be_jse_symbol_t *s);
be_jse_bool_t    get_symbol_value_bool(be_jse_symbol_t *s);
be_jse_float_t   get_symbol_double(be_jse_symbol_t *s);
be_jse_int_t     get_symbol_value_int(be_jse_symbol_t *s);
// 获取v的值并解锁v
be_jse_symbol_t *unlock_symbol_value(be_jse_symbol_t *v);
// 字符串相关函数
void symbol_to_str(be_jse_symbol_t *v, char *str, size_t len);
int  symbol_str_len(be_jse_symbol_t *v);
bool symbol_str_equal(be_jse_symbol_t *var, const char *str);
int  symbol_str_cmp(be_jse_symbol_t *va, be_jse_symbol_t *vb, int starta, int startb, bool equalAtEndOfString); 
void symbol_str_append(be_jse_symbol_t *var, const char *str);
void symbol_str_value_append(be_jse_symbol_t *var, be_jse_symbol_t *str, int stridx, int maxLength);
// 运算比较函数
be_jse_symbol_t *symbol_maths_op(be_jse_symbol_t *a, be_jse_symbol_t *b, int op);
be_jse_symbol_t *symbol_value_maths_op(be_jse_symbol_t *a, be_jse_symbol_t *b, int op);
// 拷贝相关函数
be_jse_symbol_t *symbol_cp(be_jse_symbol_t *src);
be_jse_symbol_t *symbol_name_cp(be_jse_symbol_t *src, bool linkChildren, bool keepAsName);
// 子节点相关处理，对象，数组
be_jse_symbol_t *add_symbol_node_name(be_jse_symbol_t* parent, be_jse_symbol_t* child, const char *name);
be_jse_symbol_t *set_symbol_node_name(be_jse_symbol_t *name, be_jse_symbol_t *src);
be_jse_symbol_t *lookup_named_child_symbol(be_jse_node_t parentref, const char *name, bool createIfNotFound);
be_jse_symbol_t *lookup_child_symbol(be_jse_node_t parentref, be_jse_symbol_t *childName, bool addIfNotFound);
void             remove_child_symbol(be_jse_symbol_t *parent, be_jse_symbol_t *child);
// 数组相关函数
be_jse_int_t     get_symbol_array_length(be_jse_symbol_t *v);
be_jse_symbol_t *get_symbol_array_item(be_jse_symbol_t *arr, int index); 
be_jse_symbol_t *get_symbol_array_index(be_jse_symbol_t *arr, be_jse_symbol_t *value); 
be_jse_int_t     symbol_array_push(be_jse_symbol_t *arr, be_jse_symbol_t *value);
be_jse_symbol_t *symbol_array_pop(be_jse_symbol_t *arr);
```

* __词法分析__
    JSE支持的词法类型`be_lexer_token_type_e`

```js
// 从字符串输入中做词法分析
void be_jse_lexer_init(be_jse_lexer_ctx_t *lex, const char *src, int startPos, int endPos);
// 用于for/while循环中分析条件变量
void be_jse_lexer_init2(be_jse_lexer_ctx_t *lex, be_jse_lexer_ctx_t *initFrom, int startPos);
void be_jse_lexer_deinit(be_jse_lexer_ctx_t *lex);
void be_jse_lexer_reset(be_jse_lexer_ctx_t *lex);
// token相关函数
bool lexer_token_cmp(be_jse_lexer_ctx_t *lex, int expected_token);
char *lexer_get_token(be_jse_lexer_ctx_t *lex);
```

* __语法分析__

```js
void be_jse_executor_init(be_jse_executor_ctx_t *executor);
void be_jse_executor_deinit(be_jse_executor_ctx_t *executor);
bool be_jse_add_native_func(be_jse_executor_ctx_t *executor, const char *funcDesc, be_jse_callback callbackPtr);
be_jse_symbol_t *be_jse_eval_string(be_jse_executor_ctx_t *executor, const char *str);
bool be_jse_execute_func(be_jse_executor_ctx_t *executor, be_jse_symbol_t *func, int argCount, be_jse_symbol_t **argPtr);
bool be_jse_handle_function(be_jse_parse_skip_flag_e skipName, be_jse_symbol_t **a, be_jse_symbol_t **b, be_jse_symbol_t **c, be_jse_symbol_t **d);
bool be_jse_is_none_arg_function();
be_jse_symbol_t *be_jse_handle_single_arg_function();
```

### <span data-type="color" style="color:rgb(47, 84, 235)">如何输出日志？</span>

建议使用AOS系统中的LOG输出系统

```c
定义在.c文件的第一个.h文件之前
#define CONFIG_LOGMACRO_DETAILS

推荐使用
LOGD LOGW LOGE

```

# 关于自测

#### __完整性自测要求__

* 测试程序放到apps/js/index.js，模组启动之后会自动执行
* 测试API完整覆盖，回调或事件通知需要做稳定性测试，避免内存泄露

#### __程序crash的调试方法__

```bash
kernel panic,err 1200!
assertion "0" failed: file "platform/mcu/esp32/aos/soc_impl.c", line 35, function: soc_err_proc
abort() was called at PC 0x4013826b on core 0

Backtrace: 0x40088af0:0x3fffe4a0 0x40088bef:0x3fffe4c0 0x4013826b:0x3fffe4e0 0x4008b2c1:0x3fffe510 0x400859bd:0x3fffe530 0x40084706:0x3fffe550 0x4008b985:0x3fffe570 0x4008b962:0x3fffe590 0x400862cc:0x3fffe5b0 0x4008675f:0x3fffe5d0 0x40086bc8:0x3fffe5f0 0x400d3bee:0x3fffe610 0x400d3e82:0x3fffe640 0x400d26fb:0x3fffe660 0x40089f4e:0x3fffe6b0 0x401152a9:0x3fffe6e0 0x4011deec:0x3fffe700
```

```plain

1. gdb加载对应的elf文件
./xtensa-esp32-elf-gdb.exe   ./gravity_lite@esp32devkitc.elf

2. 查看Backtrace中地址对应的函数
(gdb) info symbol 0x4008675f
krhino_sem_take + 135 in section .iram0.text
(gdb) info symbol 0x40086bc8
espos_sem_take + 12 in section .iram0.text
(gdb) info symbol 0x400d3bee
xRingbufferReceiveGeneric + 26 in section .flash.text
(gdb) info symbol 0x400d3e82
xRingbufferReceive + 14 in section .flash.text
(gdb) info symbol 0x400d26fb
uart_read_bytes + 175 in section .flash.text
(gdb) info symbol 0x40089f4e
hal_uart_recv_II + 18 in section .iram0.text
(gdb) info symbol 0x401152a9
aos_uart_recv + 17 in section .flash.text
(gdb) info symbol 0x4011deec
task_uart_recv + 100 in section .flash.text
```

 