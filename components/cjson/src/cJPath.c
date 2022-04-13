#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "aos/list.h"
#include "aos/kernel.h"
// #include "aos/osal_debug.h"   //calon
#include "cJSON.h"

#define TAG "cjpath"

/*
$.data[?(@.service=="musicX")].intent.sematic[?(@.intent=="PLAY")].slots[?(@.name=="artist" | @.name=="song")].value
artist + name


$.data[?(@.service=="musicX")].intent.sematic[?(@.intent=="RANDOM_SEARCH")]


$.data[?(@.service=="musicX")].intent.sematic[?(@.intent=="INSTRUCTION")].slots[@.name=="insType"].value
volulme_plus
volulme_minus
mute
replay
pause
close

volume_select
$.data[?(@.service=="musicX")].intent.sematic[?(@.intent=="INSTRUCTION")].slots[@.name=="percent" | @.name=="series"].value


$.data[?(@.service=="musicX")].intent.sematic[?(@.intent=="INSTRUCTION")].slots[@.name=="insType"].value

$.data[?(@.service=="cmd")].intent.sematic[?(@.intent=="SET")].slots[@.name=="series"].value

--------
$  根节点开始
.  节点分割
[] 子节点操作
    ?节点查询条件开始?()
    @当前元素
---------
状态
1. 等待根节点$
2.
*/
typedef enum _cjpath_char_type_ {
    CJPATH_CHAR_TYPE_NONE,
    CJPATH_CHAR_TYPE_NORMAL,
    CJPATH_CHAR_TYPE_KEY, /* 关键字 */
    CJPATH_CHAR_TYPE_OPT  /* 操作符 */
} cjpath_char_type_t;

typedef struct _lexnode {
    slist_t next;
    char *  path;
    char *  query;
    char *  query_key;
    char *  query_value;
    char *  query_opt;
} lexnode_t;

static int is_space(char ch)
{
    return ch == ' ';
}

#if 0
static int is_json_value_char(char ch)
{
    return isalnum(ch); //TODO: utf8 support
}
#endif

static int is_opt_char(char ch)
{
    switch (ch) {
        case '<':
        case '>':
        case '=':
        case '!':
            return 1;
            break;
        default:;
    }
    return 0;
}

static int is_key_char(char ch)
{
    switch (ch) {
        case '$':
        case '[':
        case ']':
        case '?':
        case '(':
        case ')':
        case '@':
        case '.':
        case '\0':
            return 1;
            break;
        default:;
    }

    return 0;
}

static int is_json_char(char ch)
{
    return isalnum(ch) || ch == '"' || is_space(ch) || is_key_char(ch) || is_opt_char(ch);
}

static cjpath_char_type_t get_char_type(char ch)
{
    if (is_key_char(ch)) {
        return CJPATH_CHAR_TYPE_KEY;
    } else if (is_opt_char(ch)) {
        return CJPATH_CHAR_TYPE_OPT;
    } else if (isalnum(ch) || ch == '"') {
        return CJPATH_CHAR_TYPE_NORMAL;
    }
    return CJPATH_CHAR_TYPE_NONE;
}

static char *str_dup(const char *wordbegin, int wordlen)
{
    char *str = aos_zalloc(wordlen);
    memcpy(str, wordbegin, wordlen);
    str[wordlen] = '\0';
    return str;
}

static void free_lexnode(lexnode_t *node)
{
    if(node) {
        aos_free(node->path);
        aos_free(node->query);
        aos_free(node->query_key);
        aos_free(node->query_opt);
        aos_free(node->query_value);
        aos_free(node);
    }
}

static void free_lexnode_list(slist_t *lex_list)
{
    lexnode_t *node;
    slist_t *  tmp;

    slist_for_each_entry_safe(lex_list, tmp, node, lexnode_t, next)
    {
        slist_del(&node->next, lex_list);
        free_lexnode(node);
    }
}

/*
返回当前解析位置
*/
static const char *get_next_word(const char *exp_str, const char **wordbegin, int *wordlen)
{
    const char *       ch            = exp_str;
    int                wlen          = 0;
    cjpath_char_type_t pre_char_type = CJPATH_CHAR_TYPE_NONE;
    cjpath_char_type_t char_type     = CJPATH_CHAR_TYPE_NONE;

    if (exp_str == NULL || exp_str[0] == '\0') {
        *wordbegin = NULL;
        *wordlen   = 0;
        return NULL;
    }

    /* 去掉头部空格 */
    while (is_space(*ch)) {
        ch++;
    }
#if 1
    /* 引号开头的字符串处理 */
    if(*ch == '"') {
        *wordbegin = ch;
        ch++;
        for (;; ch++) {
            wlen++;
            if (*ch == '"') {
                *wordlen = wlen + 1;
                return ++ch;
            } else if (*ch == '\0') {
                *wordlen = wlen;
                return ch;
            }
        }
    }
#endif

    pre_char_type = get_char_type(*ch);

    *wordbegin = ch;
    wlen = 0;
    for (;; ch++) {
        wlen++;

        if (!is_json_char(*ch)) {
            /* 包含无效字符 */
            *wordbegin = NULL;
            *wordlen   = 0;
            return NULL;
        }

        char_type = get_char_type(*ch);

        /* 查到到关键字跳出, 对于一般的word关键字作为断句的依据 */
        if (is_key_char(*ch)) {
            break;
        }

        /* 普通字段和操作符字段切换的断句 */
        if (pre_char_type != char_type) {
            pre_char_type = char_type;
            break;
        }

        pre_char_type = char_type;
    }

    /* 当前word就是关键字，解析位置向下移动 */
    if (wlen == 1) {
        *wordlen = wlen;
        return ++ch;
    }

    /* 去掉尾部空格 */
    const char *word = ch - 1;
    wlen--;
    while (word > exp_str && is_space(*word)) {
        wlen--;
        word--;
    }

    *wordlen = wlen;
    return ch;
}

static int lex_process(const char *jpath, slist_t *lex_list)
{
    char *     sub_opt_status     = "[?(@*=*)]";
    int        sub_opt_status_idx = -1;
    lexnode_t *cur_node           = NULL;
    const char *wordbegin;
    int         wordlen;
    const char *cur_pos = jpath;
    int ret = 0;

    do {
        cur_pos = get_next_word(cur_pos, &wordbegin, &wordlen);
        if (wordbegin == NULL || wordlen <= 0) {
            break;
        }

        if (*wordbegin == '.') {
            continue;
        }

        if (sub_opt_status_idx >= 0) {
            /* 查询操作解析 */
            if ((sub_opt_status[sub_opt_status_idx + 1] == *wordbegin) ||
                (sub_opt_status[sub_opt_status_idx + 1] == '*') ||
                (sub_opt_status[sub_opt_status_idx + 1] == '=' && is_opt_char(*wordbegin))) {
                sub_opt_status_idx++;
            } else if (sub_opt_status_idx == 0) {
                if (isdigit(*wordbegin)) {
                    sub_opt_status_idx = strlen(sub_opt_status) - 1;
                    cur_node           = aos_zalloc(sizeof(lexnode_t));
                    cur_node->query    = str_dup(wordbegin, wordlen);
                    slist_add_tail(&cur_node->next, lex_list);
                    cur_node = NULL;
                } else {
                    ret = -1;
                    break;
                }
            } else {
                ret = -1;
                break;
            }

            switch (sub_opt_status_idx) {
                case 1: //query
                    cur_node        = aos_zalloc(sizeof(lexnode_t));
                    cur_node->query = str_dup(wordbegin, wordlen);
                    break;
                case 2:
                case 3:
                    break;
                case 4: //key
                    cur_node->query_key = str_dup(wordbegin, wordlen);
                    break;
                case 5:
                    cur_node->query_opt = str_dup(wordbegin, wordlen);
                    break;
                case 6: //value
                    if (*wordbegin)
                        cur_node->query_value = str_dup(wordbegin, wordlen);
                    break;
                case 7:
                    slist_add_tail(&cur_node->next, lex_list);
                    cur_node = NULL;
                    break;
                case 8:
                    sub_opt_status_idx = -1;
                    break;
                default:
                    free_lexnode(cur_node);
                    cur_node = NULL;
                    ret      = -1;
            }
        } else {
            /* 路径状态解析 */
            switch (*wordbegin) {
                case '[':
                    /* 准备进入查询操作 */
                    sub_opt_status_idx = 0;
                    break;
                default: {
                    cur_node       = aos_zalloc(sizeof(lexnode_t));
                    cur_node->path = str_dup(wordbegin, wordlen);
                    slist_add_tail(&cur_node->next, lex_list);
                    cur_node = NULL;
                }
            }
        }

    } while (cur_pos && cur_pos[0] && ret == 0);

    if (ret < 0) {
        free_lexnode_list(lex_list);
    }

    return ret;
}

#if 0
static void lex_debug(const char *jpath)
{
    const char *wordbegin;
    int         wordlen;

    const char *cur_pos = jpath;

    //int    ret    = 0;
    //cJSON *cur_js = NULL;

    do {
        cur_pos = get_next_word(cur_pos, &wordbegin, &wordlen);
        if (wordbegin == NULL) {
            break;
        }
        /* debug */
        if (wordbegin) {
            char *ch = malloc(wordlen + 1);
            memcpy(ch, wordbegin, wordlen);
            ch[wordlen] = '\0';

            printf("<%s>\n", ch);
            free(ch);

        } else {
            printf("end\n");
            break;
        }
    } while (cur_pos && cur_pos[0]);
}
#endif

cJSON *cJSON_Path(cJSON *jsroot, const char *jpath)
{
    /* lex debug */
    //lex_debug(jpath);

    slist_t lex_list = {NULL};

    lex_process(jpath, &lex_list);

    cJSON *jsquery = NULL;

    lexnode_t *node;
    slist_for_each_entry(&lex_list, node, lexnode_t, next)
    {
        /* lex list debug */
        printf("%16s,%16s,%16s,%16s,%16s\n", node->path, node->query, node->query_key,
               node->query_opt, node->query_value);
        /*
        查询目前支持的几种状态
           path    query    key    opt    value
        ----------------------------------------
        1. $        null    null   null   null
        2. xxx      null    null   null   null
        3. null     1       null   null   null       xxx[1]
        4. null     ?       name    ==    "insType"  xxx[?(name=="insType"))]
        */

        if (node->path != NULL) {
            if (strcmp(node->path, "$") == 0) {
                jsquery = jsroot;
            } else if (node->path[0] != ']') {
                jsquery = cJSON_GetObjectItem(jsquery, node->path);
                if (jsquery == NULL) {
                    break;
                }
            }
        } else if (node->query != NULL) {
            if (cJSON_IsArray(jsquery)) {
                if (strcmp(node->query, "?") == 0) {
                    /*子对象查找*/
                    int arr_size = cJSON_GetArraySize(jsquery);
                    int bfound   = 0;
                    for (int i = 0; i < arr_size; i++) {
                        cJSON *obj  = cJSON_GetArrayItem(jsquery, i);
                        cJSON *qval = cJSON_GetObjectItem(obj, node->query_key);
                        if (cJSON_IsString(qval)) {
                            int slen = strlen(node->query_value);
                            if (slen > 2 && node->query_value[0] == '"') {
                                if (strncmp(qval->valuestring, &node->query_value[1], slen - 2) ==
                                    0) {
                                    bfound  = 1;
                                    jsquery = obj;
                                    break;
                                }
                            }
                        }
                    }

                    if (!bfound) {
                        jsquery = NULL;
                    }
                } else {
                    /*索引下标*/
                    jsquery = cJSON_GetArrayItem(jsquery, atoi(node->query));
                }
            }
        } /* query */
    }     /* for each */

    return jsquery;
}
