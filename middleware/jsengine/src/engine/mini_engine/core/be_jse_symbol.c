/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "be_jse_symbol.h"
#include "be_jse_api.h"
#include "be_jse_executor.h"
#include "be_jse_lex.h"

#ifdef RESIZABLE_JSE_SYMBOL_TABLE
be_jse_symbol_t **gSymbolBlocks = 0;
unsigned int gSymbolTableSize   = 0;

#define BE_JSE_SYMBOL_TABLE_BLOCK_SIZE 512
#define BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT 9

#else
static be_jse_symbol_t gSymbolTable[BE_JSE_SYMBOL_TABLE_SIZE];
unsigned int gSymbolTableSize = BE_JSE_SYMBOL_TABLE_SIZE;

#endif
/* start from 1 */
static be_jse_node_t gSymbolTableFirstFreeSlotID;

#ifdef DUMP_SYMBL_USAGE
static int beJSESymbolMaxUsage = 0;
static int beJSESymbolUsage    = 0;
#endif

static be_jse_symbol_t *get_symbol_addr(be_jse_node_t ref)
{
#ifdef RESIZABLE_JSE_SYMBOL_TABLE
    be_jse_node_t t = ref - 1;
    return &gSymbolBlocks[t >> BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT]
                         [t & (BE_JSE_SYMBOL_TABLE_BLOCK_SIZE - 1)];
#else
    return &gSymbolTable[ref - 1];

#endif
}

static be_jse_node_t symbol_table_init(be_jse_node_t start, unsigned int count)
{
    be_jse_node_t i;
    for (i = start; i < start + count; i++) {
        be_jse_symbol_t *v = get_symbol_addr(i);
        v->refs            = SYM_TABLE_UNUSED_REF;
#ifdef LINUXHOST
        v->this = i;
#endif
        v->locks        = 0;
        v->next_sibling = (be_jse_node_t)(i + 1); /* link to next */
    }
    get_symbol_addr((be_jse_node_t)(start + count - 1))->next_sibling =
        (be_jse_node_t)0; /* set the final one to 0 */
    return start;
}

be_jse_node_t get_symbol_node_id(be_jse_symbol_t *s)
{
    if (!s) return 0;

#ifdef RESIZABLE_JSE_SYMBOL_TABLE
    unsigned int i, c = gSymbolTableSize >> BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT;
    for (i = 0; i < c; i++) {
        if (s >= gSymbolBlocks[i] &&
            s < &gSymbolBlocks[i][BE_JSE_SYMBOL_TABLE_BLOCK_SIZE]) {
            be_jse_node_t r =
                (be_jse_node_t)(1 + (i << BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT) +
                                (s - gSymbolBlocks[i]));
            return r;
        }
    }
    return 0;

#else

#ifdef LINUXHOST
    return s->node_id;
#else
    return (be_jse_node_t)(1 + (s - (be_jse_symbol_t *)get_symbol_first_ptr()));
#endif

#endif
}

void be_jse_symbol_table_init()
{
#ifdef RESIZABLE_JSE_SYMBOL_TABLE
    gSymbolTableSize = BE_JSE_SYMBOL_TABLE_BLOCK_SIZE;
    gSymbolBlocks    = calloc(1, sizeof(be_jse_symbol_t *));
    gSymbolBlocks[0] =
        calloc(1, sizeof(be_jse_symbol_t) * BE_JSE_SYMBOL_TABLE_BLOCK_SIZE);
#else
    memset(gSymbolTable, 0, sizeof(gSymbolTable));

#endif

    gSymbolTableFirstFreeSlotID =
        symbol_table_init(1 /* first */, gSymbolTableSize);

    printf("%s %d \n", __FUNCTION__, __LINE__);
    printf("gSymbolTableSize = %d gSymbolTableFirstFreeSlotID = %d \n",
           gSymbolTableSize, gSymbolTableFirstFreeSlotID);
#ifdef DUMP_SYMBL_USAGE
    printf("beJSESymbolUsage=%d\n", beJSESymbolUsage);
#endif
}

void be_jse_symbol_table_deinit()
{
#ifdef RESIZABLE_JSE_SYMBOL_TABLE
    unsigned int i;
    for (i = 0; i<gSymbolTableSize>> BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT; i++)
        free(gSymbolBlocks[i]);
    free(gSymbolBlocks);

    gSymbolTableSize = 0;
    gSymbolBlocks    = 0;
#endif
}

void be_jse_set_symbol_table_size(unsigned int newSize)
{
#ifdef RESIZABLE_JSE_SYMBOL_TABLE

    if (newSize <= gSymbolTableSize) return; /* never allow us to have less! */

    /* When resizing, we just allocate a bunch more */
    unsigned int oldSize = gSymbolTableSize;
    unsigned int oldBlockCount =
        gSymbolTableSize >> BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT;
    unsigned int newBlockCount =
        (newSize + BE_JSE_SYMBOL_TABLE_BLOCK_SIZE - 1) >>
        BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT;

    gSymbolTableSize = newBlockCount << BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT;

    /* resize block table */
    gSymbolBlocks =
        realloc(gSymbolBlocks, sizeof(be_jse_symbol_t *) * newBlockCount);
    printf("gSymbolBlocks = %p \n", gSymbolBlocks);

    /* allocate more blocks */
    unsigned int i;

    for (i = oldBlockCount; i < newBlockCount; i++) {
        gSymbolBlocks[i] =
            calloc(1, sizeof(be_jse_symbol_t) * BE_JSE_SYMBOL_TABLE_BLOCK_SIZE);
        printf("gSymbolBlocks[%d] = %p \n", i, gSymbolBlocks[i]);
    }

    /** and now reset all the newly allocated vars. We know
     * gSymbolTableFirstFreeSlotID is 0 (because jsiFreeMoreMemory returned 0)
     * so we can just assign it.  */
    be_assert(!gSymbolTableFirstFreeSlotID);
    gSymbolTableFirstFreeSlotID =
        symbol_table_init(oldSize + 1, gSymbolTableSize - oldSize);

    printf("Resized memory from %d blocks to %d\n", oldBlockCount,
           newBlockCount);
    printf("gSymbolTableFirstFreeSlotID = %d \n", gSymbolTableFirstFreeSlotID);
#else
    NOT_USED(newSize);
    be_assert(0);
#endif
}

/* alloc new emply symbol from table */
static be_jse_symbol_t *new_symbol_node()
{
    if (gSymbolTableFirstFreeSlotID != 0) {
        be_jse_symbol_t *s = symbol_lock(gSymbolTableFirstFreeSlotID);

        gSymbolTableFirstFreeSlotID = s->next_sibling;
        be_assert(s->refs == SYM_TABLE_UNUSED_REF);

        s->refs          = 0;
        s->locks         = 1;
        s->flags         = 0;
        s->data.callback = 0;
        s->first_child   = 0;
        s->last_child    = 0;
        s->prev_sibling  = 0;
        s->next_sibling  = 0;

#ifdef DUMP_SYMBL_USAGE

        beJSESymbolUsage++;
        if (beJSESymbolMaxUsage < beJSESymbolUsage) {
            beJSESymbolMaxUsage = beJSESymbolUsage;
        }
#endif

        return s;
    }

#ifdef RESIZABLE_JSE_SYMBOL_TABLE

    printf("gSymbolTableSize = %d \n", gSymbolTableSize);
#ifdef DUMP_SYMBL_USAGE
    printf("beJSESymbolUsage = %d \n", beJSESymbolUsage);
#endif

    be_jse_set_symbol_table_size(gSymbolTableSize +
                                 BE_JSE_SYMBOL_TABLE_BLOCK_SIZE);
    return new_symbol_node();
#endif

    be_jse_error("Out of Memory!");
    return 0;
}

/* release symbol */
void free_symbol_node(be_jse_symbol_t *s)
{
    be_assert(symbol_is_string_ext(s) ||
              (!s->next_sibling && !s->prev_sibling));

#ifdef DUMP_SYMBL_USAGE
    beJSESymbolUsage--;
#endif

    if (symbol_is_name(s)) {
        if (s->first_child) {
            be_jse_symbol_t *child = symbol_lock(s->first_child);
            DEC_SYMBL_REF(child);
            s->first_child = 0;   /* unlink the child */
            symbol_unlock(child); /* unlock should trigger a free */
        }
    }

    /* Now, unref children  */
    if (symbol_is_string(s) || symbol_is_string_ext(s)) {
        be_jse_node_t stringDataRef = s->last_child;
        s->last_child               = 0;
        if (stringDataRef) {
            be_jse_symbol_t *child = symbol_lock(stringDataRef);
            be_assert(symbol_is_string_ext(child));
            child->prev_sibling =
                0; /* these pointers may contain rubbish as StringEXT uses them
                      for extra characters */
            child->next_sibling = 0;

            if (child->refs > 1 || child->locks > 1) {
                /* printf("%d, string ext,  nodeid  %d \n", __LINE__,
                   get_symbol_node_id(child)); printf("%d, r %d, l %d, flag = %d
                   \n", __LINE__, child->refs, child->locks, child->flags); */
                child->locks = 1; /* bugfix */
                child->refs  = 1;
            }
            DEC_SYMBL_REF(child);
            symbol_unlock(child);
        }
    } else if (symbol_is_object(s) || symbol_is_function(s) ||
               symbol_is_array(s) || symbol_is_root(s)) {
        be_jse_node_t childref = s->first_child;
        s->first_child         = 0;
        s->last_child          = 0;
        while (childref) {
            be_jse_symbol_t *child = symbol_lock(childref);
            be_assert(symbol_is_name(child));
            childref            = child->next_sibling;
            child->prev_sibling = 0;
            child->next_sibling = 0;
            DEC_SYMBL_REF(child);
            symbol_unlock(child);
        }
    } else {
        be_assert(!s->first_child);
        be_assert(!s->last_child);
    }

    /* free! */
    s->refs = SYM_TABLE_UNUSED_REF;
    /* add this to our free list */
    s->next_sibling             = gSymbolTableFirstFreeSlotID;
    gSymbolTableFirstFreeSlotID = get_symbol_node_id(s);
}

/* new symbol */
be_jse_symbol_t *new_symbol(be_jse_symbol_type_e type)
{
    be_jse_symbol_t *var = new_symbol_node();
    if (!var) return 0;
    var->flags = type;
    return var;
}

/* int */
be_jse_symbol_t *new_int_symbol(be_jse_int_t value)
{
    be_jse_symbol_t *var = new_symbol_node();
    if (!var) return 0; /* no memory */
    var->flags        = BE_SYM_INTEGER;
    var->data.integer = value;
    return var;
}

/* bool */
be_jse_symbol_t *new_bool_symbol(bool value)
{
    be_jse_symbol_t *var = new_symbol_node();
    if (!var) return 0; /* no memory */
    var->flags        = BE_SYM_INTEGER;
    var->data.integer = value ? 1 : 0;
    return var;
}

/* float */
be_jse_symbol_t *new_float_symbol(be_jse_float_t value)
{
    be_jse_symbol_t *var = new_symbol_node();
    if (!var) return 0; /* no memory */
    var->flags         = BE_SYM_FLOAT;
    var->data.floating = value;
    return var;
}

be_jse_symbol_t *new_named_symbol(be_jse_symbol_t *s,
                                  be_jse_symbol_t *valueOrZero)
{
    if (!s) return 0;
    be_assert(s->refs == 0);
    s->flags |= BE_SYM_NAME;
    if (valueOrZero)
        s->first_child = get_symbol_node_id(INC_SYMBL_REF(valueOrZero));
    return s;
}

be_jse_symbol_t *new_json_symbol(char *json_str, size_t json_str_len)
{
    be_jse_symbol_t *ret;
    char *evalstr = (char *)calloc(1, json_str_len + 3);
    if (!json_str || strlen(json_str) == 0) json_str = "{}";

    char *s1 = strchr(json_str, '[');
    char *s2 = strchr(json_str, '{');
    if ((s1 && (s1 - json_str) == 0) || (s2 && (s2 - json_str) == 0)) {
        sprintf(evalstr, "(%.*s)", json_str_len, json_str);
    } else {
        sprintf(evalstr, "(%.*s)", (int)strlen("({})"), "({})");
    }

    ret = be_jse_eval_string(jsengine_get_executor(), evalstr);
    free(evalstr);

    return ret;
}

/* new symbol and save str */
be_jse_symbol_t *new_str_symbol(const char *str)
{
    be_jse_symbol_t *s;
    be_jse_symbol_t *first = new_symbol_node();
    if (!first) {
        be_jse_warn("Unable to create string ,Not enough memory");
        return 0;
    }
    s              = symbol_relock(first);
    s->flags       = BE_SYM_STRING;
    s->data.str[0] = 0;

    char *ptr;
    if (str != NULL) {
        while (*str) {
            int i;
            ptr = (char *)(&s->data);
            for (i = 0; i < (int)symbol_get_max_char_len(s); i++) {
                ptr[i] = *str;
                /* s->data.str[i] = *str; */
                if (*str) str++;
            }
            if (*str) {
                be_jse_symbol_t *next = new_symbol_node();
                if (!next) {
                    be_jse_warn("Truncating string as not enough memory");
                    symbol_unlock(s);
                    return first;
                }
                next          = INC_SYMBL_REF(next);
                next->flags   = BE_SYM_STRING_EXT;
                s->last_child = get_symbol_node_id(next);
                symbol_unlock(s);
                s = next;
            }
        }
    }
    symbol_unlock(s);

    return first;
}

static ALWAYS_INLINE bool is_space(char ch)
{
    return (ch == ' ') || (ch == '\t');
}

static ALWAYS_INLINE char change_state(char c, int *state, int *last)
{
    /* state ops */
    if (*state == 0 || *state == 9) { /* normal state */
        if (c == '/') {
            *state = 1;
        } else if (c == '"') {
            *state = 5;
            return (c);
        } else if (c == '\'') {
            *state = 6;
            return (c);
        } else if (is_space(c)) { /* space tab */
            if (*state == 0) {
                *state = 9;
                return c;
            }
            return 0;
        } else {
            *state = 0;
            return (c);
        }
    } else if (*state == 1) { /* detect a '/' */
        if (c == '/') {
            *state = 2;
        } else if (c == '*') {
            *state = 3;
        } else {
            *state = 0;
            *last  = 1;
            return (c);
        }
    } else if (*state == 2) { /* "//" */
        if (c == '\n') {
            *state = 0;
            return (c);
        } else {
            *state = 2;
        }
    } else if (*state == 3) { /* "/*" */
        if (c == '*') {
            *state = 4;
        } else {
            *state = 3;
        }
    } else if (*state == 4) {
        if (c == '/') {
            *state = 0;
        } else {
            *state = 3;
        }
    } else if (*state == 5) { /* in double quotes(") */
        if (c == '"') {
            *state = 0;
            return (c);
        } else if (c == '\\') {
            *state = 7;
            return (c);
        } else {
            *state = 5;
            return (c);
        }
    } else if (*state == 6) { /* in apostrophe(') */
        if (c == '\'') {
            *state = 0;
            return (c);
        } else if (c == '\\') {
            *state = 8;
            return (c);
        } else {
            *state = 6;
            return (c);
        }
    } else if (*state == 7) { /* backslash(\) in double quotes */
        *state = 5;
        return (c);
    } else if (*state == 8) { /* in backslash(\) in apostrophe */
        *state = 6;
        return (c);
    }

    return 0;
}

static ALWAYS_INLINE be_jse_symbol_t *_new_func_code_symbol(
    be_jse_lexer_ctx_t *lex, int charFrom, int charTo)
{
    be_jse_symbol_t *first = NULL;

    int len         = charTo - charFrom;
    char *buf       = malloc(len + 1); /* +1 */
    const char *ptr = lex->src + charFrom;
    char ch         = 0;
    char chout;
    int state = 0;
    int last;
    int i, j;

    /* remove comment/TAB/space, for decrease RAM alloc */
    for (i = 0, j = 0; i < len; i++) {
        last  = 0;
        chout = change_state(ptr[i], &state, &last);

        if (chout) {
            if (last) buf[j] = ch, j++;
            buf[j] = chout;
            j++;
        }

        ch = ptr[i];
    }

    buf[j] = 0;
    /*
        printf("function:%s\n", buf);
        ch = ptr[len+1];
        ptr[len+1] = 0;
        first = new_str_symbol(ptr);
        ptr[len+1] = ch;
    */
    first = new_str_symbol(buf);
    free(buf);
    return first;
}

/* store JS function code */

be_jse_symbol_t *new_func_code_symbol(be_jse_lexer_ctx_t *lex, int charFrom,
                                      int charTo)
{
    be_jse_symbol_t *first;

    first = _new_func_code_symbol(lex, charFrom, charTo);
    return first;

    /*
        first = new_symbol_node();
        if(!first) {
            return NULL;
        }

        be_jse_symbol_t *var;
        be_jse_lexer_ctx_t newLex;

        be_jse_lexer_init(&newLex, lex->src, charFrom, charTo);

        lexer_seekto_char(&newLex, newLex.start_pos);
        lexer_get_next_char(&newLex);
        lexer_get_next_char(&newLex);

        var                 = symbol_relock(first);
        var->flags          = BE_SYM_STRING;
        var->data.str[0] = 0;

        char* ptr;

        while (newLex.curr_char) {
            int i;
            ptr = (char*)(&var->data);
            for (i=0; i<(int)symbol_get_max_char_len(var); i++) {

                /* var->data.str[i] = newLex.curr_char;
                ptr[i] = newLex.curr_char;
                if (newLex.curr_char) lexer_get_next_char(&newLex);

            }

            if (newLex.curr_char) {
                be_jse_symbol_t *next   = new_symbol_node();
                if(!next) break; /* out of memory

                next                    = INC_SYMBL_REF(next);
                next->flags             = BE_SYM_STRING_EXT;
                var->last_child         = get_symbol_node_id(next);
                symbol_unlock(var);
                var = next;
            }
        }

        symbol_unlock(var);
        be_jse_lexer_deinit(&newLex);
    */
    /*
        int len = symbol_str_len(first);
        char* buf = malloc(len+1);
        symbol_to_str(first,buf, len);

        printf("==============\n");
        printf("len=%d\n", len);
        printf("function:%s\n", buf);
        free(buf);
    */
    return first;
}

be_jse_symbol_t *symbol_lock(be_jse_node_t id)
{
    /* printf("[%s][%d] id = %d ... \n", __FUNCTION__, __LINE__, id); */

    be_jse_symbol_t *s;
    be_assert(id);

    /* s = &gSymbolTable[id-1]; */
    s = get_symbol_addr(id);

    s->locks++;

    if (s->locks == 0) {
        /* printf("[%s][%d] id = %d ... \n", __FUNCTION__, __LINE__, id);
           printf("[%s][%d] locks = %d ... \n", __FUNCTION__, __LINE__,
           s->locks); printf("[%s][%d] refs = %d ... \n", __FUNCTION__,
           __LINE__, s->refs); */
        be_jse_error("Too many locks on the symbol!");
    }

    return s;
}

be_jse_symbol_t *symbol_relock(be_jse_symbol_t *s)
{
    s->locks++;
    if (s->locks == 0) be_jse_error("Too many locks on the symbol!");
    return s;
}

be_jse_node_t symbol_unlock(be_jse_symbol_t *s)
{
    be_jse_node_t ref;
    if (!s) return 0;
    ref = get_symbol_node_id(s);

    if (s->locks == 0) {
        be_error("JSE", "symbol_unlock error, ret = %d", ref);
    }
    be_assert(s->locks > 0);

    s->locks--;
    if (s->locks == 0 && s->refs == 0) free_symbol_node(s);
    return ref;
}

#ifndef RESIZABLE_JSE_SYMBOL_TABLE
void *get_symbol_first_ptr()
{
    return &gSymbolTable[0];
}
#endif

/* get symbol table root */
be_jse_symbol_t *get_root_node()
{
    int i;
    for (i = 1; i < gSymbolTableSize; i++) {
        if (get_symbol_addr(i)->flags == BE_SYM_ROOT) {
            return symbol_lock(i);
        }
    }

    return INC_SYMBL_REF(new_symbol(BE_SYM_ROOT));
}

/* === */
static bool symbol_type_equal(be_jse_symbol_t *a, be_jse_symbol_t *b)
{
    if (a == b) return true;
    if (!a || !b) return false;

    if (symbol_is_number(a) && symbol_is_number(b)) {
        if (symbol_is_int(a)) {
            if (symbol_is_int(b)) {
                return a->data.integer == b->data.integer;
            } else {
                be_assert(symbol_is_float(b));
                return a->data.integer == b->data.floating;
            }
        } else {
            be_assert(symbol_is_float(a));
            if (symbol_is_int(b)) {
                return a->data.floating == b->data.integer;
            } else {
                be_assert(symbol_is_float(b));
                return a->data.floating == b->data.floating;
            }
        }
    } else if (symbol_is_string(a) && symbol_is_string(b)) {
        int i;
        be_jse_symbol_t *va = a;
        be_jse_symbol_t *vb = b;
        char *ptra;
        char *ptrb;
        while (true) {
            be_jse_node_t var, vbr;
            ptra = (char *)(&va->data);
            ptrb = (char *)(&vb->data);
            for (i = 0; i < (int)symbol_get_max_char_len(va); i++) {
                if (ptra[i] != ptrb[i]) return false;
                if (!ptra[i]) return true;

                /* if (va->data.str[i] != vb->data.str[i]) return false;
                   if (!va->data.str[i]) return true; /* equal, but end of
                   string */
            }

            var = a->last_child;
            vbr = b->last_child;
            if ((var == 0) && (vbr == 0)) return true;
            if ((var == 0) != (vbr == 0)) return false;
            if (va != a) symbol_unlock(va);
            if (vb != b) symbol_unlock(vb);
            va = symbol_lock(var);
            vb = symbol_lock(vbr);
        }
        return true; /* but we never get here */
    } else {
        return false;
    }
}

void symbol_to_str(be_jse_symbol_t *s, char *str, size_t len)
{
    int cnt = 0;
    if (symbol_is_undefined(s)) {
        strncpy(str, "undefined", len - 1);
    } else if (symbol_is_int(s)) {
        /* itostr(v->data.integer, str, 10); */
        itoa((int)s->data.integer, str, 10);
    } else if (symbol_is_float(s)) {
        /* ftoa_bounded(v->data.floating, str, len); */
        ftoa(s->data.floating, str);
    } else if (symbol_is_null(s)) {
        strncpy(str, "null", len);
    } else if (symbol_is_string(s) || symbol_is_string_ext(s) ||
               symbol_is_name(s) || symbol_is_function_argv(s)) {
        be_jse_symbol_t *var = symbol_relock(s);
        be_jse_node_t ref    = 0;
        char *ptr;
        if (symbol_is_string_ext(s))
            be_jse_warn("INTERNAL: Calling symbol_to_str on a JSV_STRING_EXT");
        while (var) {
            be_jse_node_t refNext;
            int i;
            ptr = (char *)(&var->data);
            for (i = 0; i < (int)symbol_get_max_char_len(var); i++) {
                /* if(var->data.str[i]==0) */
                if (ptr[i] == 0) {
                    *str = 0;
                    break;
                }

                if (len-- <= 0) {
                    *str = 0;
                    be_jse_warn("symbol_to_str overflowed\n");
                    symbol_unlock(var);
                    return;
                }
                /* *(str++) = var->data.str[i]; */
                *(str++) = ptr[i];
                cnt++;
            }

            refNext = var->last_child;
            symbol_unlock(var);
            ref = refNext;
            var = ref ? symbol_lock(ref) : 0;
        }
        symbol_unlock(var);
        /* if (str[-1]) *str = 0; */
        if (cnt) *str = 0;
    } else if (symbol_is_function(s)) {
        strncpy(str, "function", len);
    } else if (symbol_is_array(s)) {
        strncpy(str, "array", len);
    } else {
        be_warn("JSE", "s->flags = 0x%x\n", s->flags);
        str[0] = ' ';
        str[1] = 0;
    }
}

int symbol_str_len(be_jse_symbol_t *v)
{
    if (!symbol_is_string(v)) return 0;

    int strLength        = 0;
    be_jse_symbol_t *var = v;
    be_jse_node_t ref    = 0;

    char *ptr;
    while (var) {
        be_jse_node_t refNext = var->last_child;

        if (refNext != 0) {
            strLength += symbol_get_max_char_len(var);
        } else {
            int i;
            ptr = (char *)(&var->data);
            for (i = 0; i < symbol_get_max_char_len(var); i++) {
                /* if (var->data.str[i]) */
                if (ptr[i])
                    strLength++;
                else
                    break;
            }
        }

        if (ref) symbol_unlock(var);
        ref = refNext;
        var = ref ? symbol_lock(ref) : 0;
    }
    if (ref) symbol_unlock(var);
    return strLength;
}

void symbol_str_append(be_jse_symbol_t *s, const char *str)
{
    int blockChars;
    char *ptr;
    be_assert(symbol_is_string(s));
    be_jse_symbol_t *block = symbol_relock(s);

    while (block->last_child) {
        be_jse_node_t next = block->last_child;
        symbol_unlock(block);
        block = symbol_lock(next);
    }

    blockChars = 0;
    ptr        = (char *)(&block->data);
    /* while (blockChars<symbol_get_max_char_len(block) &&
       block->data.str[blockChars]) */
    while (blockChars < symbol_get_max_char_len(block) && ptr[blockChars]) {
        blockChars++;
    }

    while (*str) {
        int i;
        ptr = (char *)(&block->data);
        for (i = blockChars; i < symbol_get_max_char_len(block); i++) {
            ptr[i] = *str;
            /* block->data.str[i] = *str; */
            if (*str) str++;
        }

        if (*str) {
            be_jse_symbol_t *next = new_symbol_node();
            if (!next) break;
            next              = INC_SYMBL_REF(next);
            next->flags       = BE_SYM_STRING_EXT;
            block->last_child = get_symbol_node_id(next);

            symbol_unlock(block);
            block      = next;
            blockChars = 0;
        }
    }
    symbol_unlock(block);
}

be_jse_symbol_t *str_to_symbol(be_jse_symbol_t *s, bool bUnLock)
{
    if (symbol_is_string(s) || symbol_is_name(s)) {
        if (bUnLock) return s;
        return symbol_relock(s);
    }

    char buf[BE_JSE_SYMBOL_STRING_OP_BUFFER_SIZE];
    symbol_to_str(s, buf, BE_JSE_SYMBOL_STRING_OP_BUFFER_SIZE - 1);
    if (bUnLock) symbol_unlock(s);
    return new_str_symbol(buf);
}

void symbol_str_value_append(be_jse_symbol_t *s, be_jse_symbol_t *str,
                             int stridx, int maxLength)
{
    be_assert(symbol_is_string(str) || symbol_is_name(str));
    str = symbol_relock(str);

    be_jse_symbol_t *block = symbol_relock(s);
    int blockChars;
    be_assert(symbol_is_string(s));

    while (block->last_child) {
        be_jse_node_t next = block->last_child;
        symbol_unlock(block);
        block = symbol_lock(next);
    }

    blockChars = 0;
    char *ptr;
    ptr = (char *)(&block->data);
    /* while (blockChars<symbol_get_max_char_len(block) &&
       block->data.str[blockChars]) */
    while (blockChars < symbol_get_max_char_len(block) && ptr[blockChars])
        blockChars++;

    if (stridx < 0) stridx += (int)symbol_str_len(str);

    while (stridx >= (int)symbol_get_max_char_len(str)) {
        be_jse_node_t n = str->last_child;
        stridx -= (int)symbol_get_max_char_len(str);
        symbol_unlock(str);
        str = n ? symbol_lock(n) : 0;
    }

    int i;
    while (str) {
        for (i = blockChars; i < symbol_get_max_char_len(block); i++) {
            char ch = 0;

            if (str) {
                /* ch = str->data.str[stridx]; */
                ptr = (char *)(&str->data);
                ch  = ptr[stridx];
                if (ch && --maxLength > 0) {
                    stridx++;
                    if (stridx >= (int)symbol_get_max_char_len(str)) {
                        be_jse_node_t n = str->last_child;
                        stridx          = 0;
                        symbol_unlock(str);
                        str = n ? symbol_lock(n) : 0;
                    }
                } else {
                    symbol_unlock(str);
                    str = 0;
                }
            }
            ptr    = (char *)(&block->data);
            ptr[i] = ch;
            /* block->data.str[i] = ch; */
        }

        if (str) {
            be_jse_symbol_t *next = new_symbol_node();
            if (!next) break;
            next              = INC_SYMBL_REF(next);
            next->flags       = BE_SYM_STRING_EXT;
            block->last_child = get_symbol_node_id(next);

            symbol_unlock(block);

            block      = next;
            blockChars = 0;
        }
    }
    symbol_unlock(block);
}

/* convert to int */
ALWAYS_INLINE be_jse_int_t get_symbol_int(be_jse_symbol_t *v)
{
    if (!v) return 0;

    if (symbol_is_int(v)) return v->data.integer;
    if (symbol_is_null(v)) return 0;
    if (symbol_is_undefined(v)) return 0;
    if (symbol_is_float(v)) return (be_jse_int_t)v->data.floating;

    if (symbol_is_string(v) && symbol_str_len(v)) return 1;

    if (symbol_is_object(v) && v->first_child) {
        return 1;
    }

    if (symbol_is_array(v) && get_symbol_array_length(v)) return 1;

    if (symbol_is_function(v)) return 1;

    return 0;
}

/* convert to boolean */
ALWAYS_INLINE be_jse_bool_t get_symbol_bool(be_jse_symbol_t *v)
{
    return get_symbol_int(v) != 0;
}

/* convert to double */
ALWAYS_INLINE be_jse_float_t get_symbol_double(be_jse_symbol_t *s)
{
    if (!s) return 0;
    if (symbol_is_float(s)) return s->data.floating;
    if (symbol_is_int(s)) return (be_jse_float_t)s->data.integer;
    if (symbol_is_null(s)) return 0;
    if (symbol_is_undefined(s)) return 0;
    return 0;
}

/* get (key/value) from symbol and convert to int */
be_jse_int_t get_symbol_value_int(be_jse_symbol_t *s)
{
    be_jse_symbol_t *a = get_symbol_value(s);
    be_jse_int_t l     = get_symbol_int(a);
    symbol_unlock(a);
    return l;
}

/* get (key/value) from symbol and convert to bool */
bool get_symbol_value_bool(be_jse_symbol_t *v)
{
    return get_symbol_value_int(v) != 0;
}

bool symbol_str_equal(be_jse_symbol_t *var, const char *str)
{
    be_jse_symbol_t *v;
    be_jse_node_t next;

    /* be_assert(symbol_is_basic(var) || symbol_has_characterdata(var)); */

    if (!symbol_has_characterdata(var)) return 0;

    v = symbol_relock(var);

    while (true) {
        int i;
        char *ptr = (char *)(&v->data);

        for (i = 0; i < symbol_get_max_char_len(v); i++) {
            if (ptr[i] != *str)
            /* if (v->data.str[i] != *str) */
            {
                symbol_unlock(v);
                return false;
            }
            if (*str == 0) {
                symbol_unlock(v);
                return true;
            }
            str++;
        }

        next = v->last_child;
        if (*str == 0) {
            symbol_unlock(v);
            return next == 0;
        }

        symbol_unlock(v);
        if (!next) return false;
        v = symbol_lock(next);
    }
    return true;
}
int symbol_str_cmp(be_jse_symbol_t *va, be_jse_symbol_t *vb, int starta,
                   int startb, bool equalAtEndOfString)
{
    int idxa = starta;
    int idxb = startb;
    be_assert(symbol_is_string(va) ||
              symbol_is_name(va)); /* we hope! Might just want to return 0? */
    be_assert(symbol_is_string(vb) ||
              symbol_is_name(vb)); /* we hope! Might just want to return 0? */
    va = symbol_relock(va);
    vb = symbol_relock(vb);

    while (true) {
        while (va && idxa >= (int)symbol_get_max_char_len(va)) {
            be_jse_node_t n = va->last_child;
            idxa -= (int)symbol_get_max_char_len(va);
            symbol_unlock(va);
            va = n ? symbol_lock(n) : 0;
        }
        while (vb && idxb >= (int)symbol_get_max_char_len(vb)) {
            be_jse_node_t n = vb->last_child;
            idxb -= (int)symbol_get_max_char_len(vb);
            symbol_unlock(vb);
            vb = n ? symbol_lock(n) : 0;
        }

        /* char ca = (char) (va ? va->data.str[idxa] : 0); */
        /* char cb = (char) (vb ? vb->data.str[idxb] : 0); */
        char ca = (char)(va ? ((char *)(&va->data))[idxa] : 0);
        char cb = (char)(vb ? ((char *)(&vb->data))[idxb] : 0);

        if (ca != cb) {
            symbol_unlock(va);
            symbol_unlock(vb);
            if ((ca == 0 || cb == 0) && equalAtEndOfString) return 0;
            return ca - cb;
        }

        if (ca == 0) {
            symbol_unlock(va);
            symbol_unlock(vb);
            return 0;
        }

        idxa++;
        idxb++;
    }

    return true;
}

/* symbol copy */
be_jse_symbol_t *symbol_cp(be_jse_symbol_t *src)
{
    be_jse_symbol_t *dst = new_symbol(src->flags);
    if (!dst) return 0; /* out of memory */
    if (!symbol_is_string_ext(src)) {
        memcpy(&dst->data, &src->data, sizeof(be_symbol_data_t));

        dst->last_child   = 0;
        dst->first_child  = 0;
        dst->prev_sibling = 0;
        dst->next_sibling = 0;
    } else {
        memcpy(&dst->data, &src->data,
               sizeof(be_symbol_data_t) + sizeof(be_jse_node_t) * 3);
        dst->last_child = 0;
    }

    /* Copy what names point to */
    if (symbol_is_name(src)) {
        if (src->first_child) {
            be_jse_symbol_t *child = symbol_lock(src->first_child);
            dst->first_child = symbol_unlock(INC_SYMBL_REF(symbol_cp(child)));
            symbol_unlock(child);
        }
    }

    if (symbol_is_string(src) || symbol_is_string_ext(src)) {
        /* copy extra bits of string if there were any */
        if (src->last_child) {
            be_jse_symbol_t *child     = symbol_lock(src->last_child);
            be_jse_symbol_t *childCopy = symbol_cp(child);
            if (childCopy)
                dst->last_child = symbol_unlock(INC_SYMBL_REF(childCopy));
            symbol_unlock(child);
        }
    } else if (symbol_is_object(src) || symbol_is_function(src)) {
        /* Copy children.. */
        be_jse_node_t vr;
        vr = src->first_child;
        while (vr) {
            be_jse_symbol_t *name = symbol_lock(vr);
            be_jse_symbol_t *child =
                symbol_name_cp(name, true, true); /* NO DEEP COPY! */
            if (child) {
                add_symbol_node(dst, child);
                symbol_unlock(child);
            }
            vr = name->next_sibling;
            symbol_unlock(name);
        }
    } else
        be_assert(symbol_is_basic(src)); /* in case we missed something! */

    return dst;
}

/* copy symbol name */
be_jse_symbol_t *symbol_name_cp(be_jse_symbol_t *src, bool linkChildren,
                                bool keepAsName)
{
    be_jse_symbol_type_e flags = src->flags;
    if (!keepAsName) flags &= (be_jse_symbol_type_e)~BE_SYM_NAME;
    be_jse_symbol_t *dst = new_symbol(flags);

    if (!dst) return 0;
    memcpy(&dst->data, &src->data, sizeof(be_symbol_data_t));

    dst->last_child   = 0;
    dst->first_child  = 0;
    dst->prev_sibling = 0;
    dst->next_sibling = 0;

    if (linkChildren && src->first_child)
        dst->first_child = INC_SYMBL_REF_BY_ID(src->first_child);

    if (symbol_is_string(src)) {
        if (src->last_child) {
            be_jse_symbol_t *child     = symbol_lock(src->last_child);
            be_jse_symbol_t *childCopy = symbol_cp(child);
            if (childCopy)
                dst->last_child = symbol_unlock(INC_SYMBL_REF(childCopy));
            symbol_unlock(child);
        }
    } else
        be_assert(symbol_is_basic(src));

    return dst;
}

/* added new child node to the parent */
void add_symbol_node(be_jse_symbol_t *parent, be_jse_symbol_t *namedChild)
{
    namedChild = INC_SYMBL_REF(namedChild);
    be_assert(symbol_is_name(namedChild));
    if (parent->last_child) {
        be_jse_symbol_t *last_child = symbol_lock(parent->last_child);
        last_child->next_sibling    = get_symbol_node_id(namedChild);
        symbol_unlock(last_child);

        namedChild->prev_sibling = parent->last_child;

        parent->last_child = get_symbol_node_id(namedChild);
    } else {
        parent->first_child = parent->last_child =
            get_symbol_node_id(namedChild);
    }
}

/* add symbol node name */
be_jse_symbol_t *add_symbol_node_name(be_jse_symbol_t *parent,
                                      be_jse_symbol_t *child, const char *name)
{
    be_jse_symbol_t *namedChild = new_named_symbol(new_str_symbol(name), child);
    if (!namedChild) return 0;
    add_symbol_node(parent, namedChild);
    return namedChild;
}

/* set symbol node name */
be_jse_symbol_t *set_symbol_node_name(be_jse_symbol_t *name,
                                      be_jse_symbol_t *src)
{
    be_assert(name && symbol_is_name(name));
    be_assert(name != src);
    if (name->first_child) DEC_SYMBL_REF_BY_ID(name->first_child);
    if (src) {
        name->first_child = get_symbol_node_id(INC_SYMBL_REF(src));
    } else
        name->first_child = 0;
    return name;
}

/* search child node from symbol table */
be_jse_symbol_t *lookup_named_child_symbol(be_jse_node_t parent_id,
                                           const char *name,
                                           bool createIfNotFound)
{
    be_jse_symbol_t *parent = symbol_lock(parent_id);
    be_jse_symbol_t *child;
    be_jse_node_t childref = parent->first_child;
    while (childref) {
        child = symbol_lock(childref);
        if (symbol_str_equal(child, name)) {
            symbol_unlock(parent);
            return child;
        }
        childref = child->next_sibling;
        symbol_unlock(child);
    }

    child = 0;
    if (createIfNotFound) {
        child = new_named_symbol(new_str_symbol(name), 0);
        if (child) add_symbol_node(parent, child);
    }
    symbol_unlock(parent);
    return child;
}

/* search child node from symbol table */
be_jse_symbol_t *lookup_child_symbol(be_jse_node_t parent_id,
                                     be_jse_symbol_t *childName,
                                     bool addIfNotFound)
{
    be_jse_symbol_t *parent = symbol_lock(parent_id);
    be_jse_symbol_t *child;
    be_jse_node_t childref = parent->first_child;

    while (childref) {
        child = symbol_lock(childref);
        if (symbol_type_equal(child, childName)) {
            symbol_unlock(parent);
            return child;
        }
        childref = child->next_sibling;
        symbol_unlock(child);
    }

    child = 0;
    if (addIfNotFound && childName) {
        if (childName->refs == 0) {
            if (!symbol_is_name(childName))
                childName = new_named_symbol(childName, 0);

            add_symbol_node(parent, childName);
            child = symbol_relock(childName);
        } else {
            child = new_named_symbol(symbol_cp(childName), 0);
            add_symbol_node(parent, child);
        }
    }
    symbol_unlock(parent);
    return child;
}

/* remove child node from symbol table */
void remove_child_symbol(be_jse_symbol_t *parent, be_jse_symbol_t *child)
{
    be_assert(symbol_is_array(parent) || symbol_is_object(parent) ||
              symbol_is_function(parent));
    be_jse_node_t childref = get_symbol_node_id(child);
    /* unlink from parent */
    if (parent->first_child == childref)
        parent->first_child = child->next_sibling;
    if (parent->last_child == childref)
        parent->last_child = child->prev_sibling;
    /* unlink from child list */
    if (child->prev_sibling) {
        be_jse_symbol_t *v = symbol_lock(child->prev_sibling);
        v->next_sibling    = child->next_sibling;
        symbol_unlock(v);
    }
    if (child->next_sibling) {
        be_jse_symbol_t *v = symbol_lock(child->next_sibling);
        v->prev_sibling    = child->prev_sibling;
        symbol_unlock(v);
    }
    child->prev_sibling = 0;
    child->next_sibling = 0;

    DEC_SYMBL_REF(child);
}

#ifdef USE_ARRAY
/* array ops */

/* get array length */
be_jse_int_t get_symbol_array_length(be_jse_symbol_t *v)
{
    be_jse_int_t lastIdx   = 0;
    be_jse_node_t childref = v->first_child;
    while (childref) {
        be_jse_int_t childIndex;
        be_jse_symbol_t *child = symbol_lock(childref);

        be_assert(symbol_is_int(child));
        childIndex = get_symbol_int(child) + 1;
        if (childIndex > lastIdx) lastIdx = childIndex;
        childref = child->next_sibling;
        symbol_unlock(child);
    }
    return lastIdx;
}

/* get item from array */
be_jse_symbol_t *get_symbol_array_item(be_jse_symbol_t *arr, int index)
{
    be_jse_node_t childref = arr->first_child;
    while (childref) {
        be_jse_int_t childIndex;
        be_jse_symbol_t *child = symbol_lock(childref);

        be_assert(symbol_is_int(child));
        childIndex = get_symbol_int(child);
        if (childIndex == index) {
            be_jse_symbol_t *item = symbol_lock(child->first_child);
            symbol_unlock(child);
            return item;
        }
        childref = child->next_sibling;
        symbol_unlock(child);
    }
    return 0; /* undefined */
}

/* get index from array */
be_jse_symbol_t *get_symbol_array_index(be_jse_symbol_t *arr,
                                        be_jse_symbol_t *value)
{
    be_jse_node_t indexref;
    be_assert(symbol_is_array(arr) || symbol_is_object(arr));
    indexref = arr->first_child;
    while (indexref) {
        be_jse_symbol_t *childIndex = symbol_lock(indexref);
        be_assert(symbol_is_name(childIndex)) be_jse_symbol_t *childValue =
            symbol_lock(childIndex->first_child);
        if (symbol_type_equal(childValue, value)) {
            symbol_unlock(childValue);
            return childIndex;
        }
        symbol_unlock(childValue);
        indexref = childIndex->next_sibling;
        symbol_unlock(childIndex);
    }
    return 0; /* undefined */
}

/* push to array */
be_jse_int_t symbol_array_push(be_jse_symbol_t *arr, be_jse_symbol_t *value)
{
    be_assert(symbol_is_array(arr));
    be_jse_int_t index   = get_symbol_array_length(arr);
    be_jse_symbol_t *idx = new_named_symbol(new_int_symbol(index), value);
    if (!idx) {
        be_jse_warn("Out of memory while appending to array");
        return 0;
    }
    add_symbol_node(arr, idx);
    symbol_unlock(idx);
    return index + 1; /* new size */
}

/* pop from array */
be_jse_symbol_t *symbol_array_pop(be_jse_symbol_t *arr)
{
    be_assert(symbol_is_array(arr));
    if (arr->last_child) {
        be_jse_symbol_t *child = symbol_lock(arr->last_child);

        if (arr->first_child == arr->last_child) arr->first_child = 0;

        arr->last_child = child->prev_sibling;
        DEC_SYMBL_REF(child);

        if (child->prev_sibling) {
            be_jse_symbol_t *v = symbol_lock(child->prev_sibling);
            v->next_sibling    = 0;
            symbol_unlock(v);
        }
        child->prev_sibling = 0;
        return child;
    } else {
        return 0;
    }
}

#endif

be_jse_symbol_t *get_symbol_value(be_jse_symbol_t *a)
{
    be_jse_symbol_t *pa = a;
    if (!a) return a;

    while (symbol_is_name(pa)) {
        be_jse_node_t n = pa->first_child;
        if (pa != a) symbol_unlock(pa);
        if (!n) return 0;
        pa = symbol_lock(n);
    }
    if (pa == a) symbol_relock(pa);
    return pa;
}

/* get symbol reference count */
int get_symbol_ref_count(be_jse_symbol_t *toCount, be_jse_symbol_t *var)
{
    int refCount = 0;

    if (symbol_is_name(var)) {
        be_jse_node_t child = var->first_child;
        if (child) {
            be_jse_symbol_t *childVar = symbol_lock(child);
            if (childVar == toCount)
                refCount += 1;
            else
                refCount += get_symbol_ref_count(toCount, childVar);
            child = childVar->first_child;
            symbol_unlock(childVar);
        }
    } else if (symbol_has_child(var)) {
        be_jse_node_t child = var->first_child;
        while (child) {
            be_jse_symbol_t *childVar;
            childVar = symbol_lock(child);
            if (childVar == toCount)
                refCount += 1;
            else
                refCount += get_symbol_ref_count(toCount, childVar);
            child = childVar->next_sibling;
            symbol_unlock(childVar);
        }
    }
    return refCount;
}

be_jse_symbol_t *unlock_symbol_value(be_jse_symbol_t *a)
{
    be_jse_symbol_t *b = get_symbol_value(a);
    symbol_unlock(a);
    return b;
}

be_jse_symbol_t *symbol_maths_op(be_jse_symbol_t *a, be_jse_symbol_t *b, int op)
{
    if (op == BE_TOKEN_OP_STRICT_EQUAL || op == BE_TOKEN_OP_STRICT_NEQUAL) {
        bool eql = (a == 0) == (b == 0);
        if (a && b)
            eql = ((a->flags & BE_SYM_TYPE_MASK) ==
                   (b->flags & BE_SYM_TYPE_MASK));
        if (eql) {
            be_jse_symbol_t *contents =
                symbol_maths_op(a, b, BE_TOKEN_OP_EQUAL);
            if (!get_symbol_bool(contents)) eql = false;
            symbol_unlock(contents);
        }
        if (op == BE_TOKEN_OP_STRICT_EQUAL)
            return new_bool_symbol(eql);
        else
            return new_bool_symbol(!eql);
    }

    if ((symbol_is_undefined(a) || symbol_is_null(a)) &&
        (symbol_is_undefined(b) || symbol_is_null(b))) {
        if (op == BE_TOKEN_OP_EQUAL)
            return new_bool_symbol(true);
        else if (op == BE_TOKEN_OP_NEQUAL)
            return new_bool_symbol(false);
        else
            return 0;
    } else if ((symbol_is_number(a) || symbol_is_undefined(a) ||
                symbol_is_null(a)) &&
               (symbol_is_number(b) || symbol_is_undefined(b))) {
        if (!symbol_is_float(a) && !symbol_is_float(b)) {
            /* use ints */
            be_jse_int_t da = get_symbol_int(a);
            be_jse_int_t db = get_symbol_int(b);
            switch (op) {
                case '+':
                    return new_int_symbol(da + db);
                case '-':
                    return new_int_symbol(da - db);
                case '*':
                    return new_int_symbol(da * db);
                case '/':
                    return new_int_symbol(da / db);
                case '&':
                    return new_int_symbol(da & db);
                case '|':
                    return new_int_symbol(da | db);
                case '^':
                    return new_int_symbol(da ^ db);
                case '%':
                    return new_int_symbol(da % db);
                case BE_TOKEN_OP_LEFT_SHIFT:
                    return new_int_symbol(da << db);
                case BE_TOKEN_OP_RIGHT_SHIFT:
                    return new_int_symbol(da >> db);
                case BE_TOKEN_OP_RIGHT_SHIFT_UNSIGN:
                    return new_int_symbol(((be_jse_uint_t)da) >> db);
                case BE_TOKEN_OP_EQUAL:
                    return new_bool_symbol(da == db);
                case BE_TOKEN_OP_NEQUAL:
                    return new_bool_symbol(da != db);
                case '<':
                    return new_bool_symbol(da < db);
                case BE_TOKEN_OP_LESS_EQUAL:
                    return new_bool_symbol(da <= db);
                case '>':
                    return new_bool_symbol(da > db);
                case BE_TOKEN_OP_MORE_EQUAL:
                    return new_bool_symbol(da >= db);
                default:
                    return symbol_maths_op_error(op, "Integer");
            }
        } else {
            be_jse_float_t da = get_symbol_double(a);
            be_jse_float_t db = get_symbol_double(b);
            switch (op) {
                case '+':
                    return new_float_symbol(da + db);
                case '-':
                    return new_float_symbol(da - db);
                case '*':
                    return new_float_symbol(da * db);
                case '/':
                    return new_float_symbol(da / db);
                case BE_TOKEN_OP_EQUAL:
                    return new_bool_symbol(da == db);
                case BE_TOKEN_OP_NEQUAL:
                    return new_bool_symbol(da != db);
                case '<':
                    return new_bool_symbol(da < db);
                case BE_TOKEN_OP_LESS_EQUAL:
                    return new_bool_symbol(da <= db);
                case '>':
                    return new_bool_symbol(da > db);
                case BE_TOKEN_OP_MORE_EQUAL:
                    return new_bool_symbol(da >= db);
                default:
                    return symbol_maths_op_error(op, "Double");
            }
        }
    } else if (symbol_is_array(a) || symbol_is_object(a) ||
               symbol_is_array(b) || symbol_is_object(b)) {
        bool isArray = symbol_is_array(a);

        switch (op) {
            case BE_TOKEN_OP_EQUAL:
                return new_bool_symbol(a == b);
            case BE_TOKEN_OP_NEQUAL:
                return new_bool_symbol(a != b);
            default:
                return symbol_maths_op_error(op, isArray ? "Array" : "Object");
        }
    } else {
        be_jse_symbol_t *da = str_to_symbol(a, false);
        be_jse_symbol_t *db = str_to_symbol(b, false);
        if (!da || !db) { /* out of memory */
            symbol_unlock(da);
            symbol_unlock(db);
            return 0;
        }
        if (op == '+') {
            be_jse_symbol_t *v = symbol_cp(da);
            /* TODO: can we be fancy and not copy da if we know it isn't reffed?
               what about locks? */
            if (v)
                symbol_str_value_append(v, db, 0,
                                        BE_JSE_SYMBOL_APPEND_STR_MAX_LEN);
            symbol_unlock(da);
            symbol_unlock(db);
            return v;
        }

        int cmp = symbol_str_cmp(da, db, 0, 0, false);
        symbol_unlock(da);
        symbol_unlock(db);
        /* use strings */
        switch (op) {
            case BE_TOKEN_OP_EQUAL:
                return new_bool_symbol(cmp == 0);
            case BE_TOKEN_OP_NEQUAL:
                return new_bool_symbol(cmp != 0);
            case '<':
                return new_bool_symbol(cmp < 0);
            case BE_TOKEN_OP_LESS_EQUAL:
                return new_bool_symbol(cmp <= 0);
            case '>':
                return new_bool_symbol(cmp > 0);
            case BE_TOKEN_OP_MORE_EQUAL:
                return new_bool_symbol(cmp >= 0);
            default:
                return symbol_maths_op_error(op, "String");
        }
    }
    be_assert(0);
    return 0;
}

/* symbol value ops */
be_jse_symbol_t *symbol_value_maths_op(be_jse_symbol_t *a, be_jse_symbol_t *b,
                                       int op)
{
    be_jse_symbol_t *pa  = get_symbol_value(a);
    be_jse_symbol_t *pb  = get_symbol_value(b);
    be_jse_symbol_t *res = symbol_maths_op(pa, pb, op);
    symbol_unlock(pa);
    symbol_unlock(pb);
    return res;
}

be_jse_symbol_t *symbol_maths_op_error(int op, const char *datatype)
{
#ifndef BE_JSE_SILENT
    char buf[BE_JSE_ERROR_BUF_SIZE];
    size_t bufpos = 0;
    strncpy(&buf[bufpos], "Operation ", BE_JSE_ERROR_BUF_SIZE - bufpos);
    bufpos = strlen(buf);
    lexer_token_to_str(op, &buf[bufpos], BE_JSE_ERROR_BUF_SIZE - bufpos);
    bufpos = strlen(buf);
    strncat(&buf[bufpos], " not supported on the  ",
            BE_JSE_ERROR_BUF_SIZE - bufpos);
    bufpos = strlen(buf);
    strncat(&buf[bufpos], datatype, BE_JSE_ERROR_BUF_SIZE - bufpos);
    bufpos = strlen(buf);
    strncat(&buf[bufpos], " datatype", BE_JSE_ERROR_BUF_SIZE - bufpos);
    be_jse_error(buf);
#else
    NOT_USED(op);
    NOT_USED(datatype);
    be_jse_error("symbol_maths_op_error");
#endif
    return 0;
}

#ifdef USE_JSON

typedef void (*get_JSON_cb_string)(void *data, const char *string);
typedef void (*get_JSON_cb_var)(void *data, be_jse_symbol_t *var);

void get_JSON_with_cb(be_jse_symbol_t *var, get_JSON_cb_string callbackString,
                      get_JSON_cb_var callbackVar, void *callbackData)
{
    if (symbol_is_undefined(var)) {
        callbackString(callbackData, "undefined");
#ifdef USE_ARRAY
    } else if (symbol_is_array(var)) {
        int length = (int)get_symbol_array_length(var);
        int i;
        callbackString(callbackData, "[");
        for (i = 0; i < length; i++) {
            be_jse_symbol_t *item = get_symbol_array_item(var, i);
            get_JSON_with_cb(item, callbackString, callbackVar, callbackData);
            symbol_unlock(item);
            if (i < length - 1) callbackString(callbackData, ",");
        }
        callbackString(callbackData, "]");
#endif
    } else if (symbol_is_object(var)) {
        be_jse_node_t childref = var->first_child;
        callbackString(callbackData, "{");
        while (childref) {
            be_jse_symbol_t *child = symbol_lock(childref);
            be_jse_symbol_t *childVar;
            callbackString(callbackData, "\"");
            callbackVar(callbackData, child); /* FIXME: escape the string */
            callbackString(callbackData, "\":");
            childVar = symbol_lock(child->first_child);
            childref = child->next_sibling;
            symbol_unlock(child);
            get_JSON_with_cb(childVar, callbackString, callbackVar,
                             callbackData);
            symbol_unlock(childVar);
            if (childref) callbackString(callbackData, ",");
        }
        callbackString(callbackData, "}");
    } else if (symbol_is_function(var)) {
        be_jse_node_t coderef  = 0;
        be_jse_node_t childref = var->first_child;
        bool firstParm         = true;
        callbackString(callbackData, "function (");
        while (childref) {
            be_jse_symbol_t *child = symbol_lock(childref);
            childref               = child->next_sibling;
            if (symbol_is_function_argv(child)) {
                if (firstParm)
                    firstParm = false;
                else
                    callbackString(callbackData, ",");
                callbackVar(callbackData, child); /* FIXME: escape the string */
            } else if (symbol_is_string(child) &&
                       symbol_str_equal(child,
                                        BE_JSE_PARSE_FUNCTION_CODE_NAME)) {
                coderef = child->first_child;
            }
            symbol_unlock(child);
        }
        callbackString(callbackData, ") ");
        if (coderef) {
            be_jse_symbol_t *codeVar = symbol_lock(coderef);
            callbackVar(callbackData, codeVar);
            symbol_unlock(codeVar);
        } else
            callbackString(callbackData, "{}");
    } else {
        char buf[MAX_TOKEN_LENGTH];
        symbol_to_str(var, buf, MAX_TOKEN_LENGTH - 1);
        if (symbol_is_string(var)) callbackString(callbackData, "\"");
        callbackString(callbackData, buf);
        if (symbol_is_string(var)) callbackString(callbackData, "\"");
    }
    /* TODO: functions */
}

void append_str_var_symbol(be_jse_symbol_t *var, be_jse_symbol_t *str)
{
    symbol_str_value_append(var, str, 0, BE_JSE_SYMBOL_APPEND_STR_MAX_LEN);
}

void symbol_to_json_new(be_jse_symbol_t *var, be_jse_symbol_t *result)
{
    be_assert(symbol_is_string(result));
    get_JSON_with_cb(var, (get_JSON_cb_string)symbol_str_append,
                     (get_JSON_cb_var)append_str_var_symbol, result);
}

void symbol_to_json(be_jse_symbol_t *var, be_jse_symbol_t *result)
{
    be_assert(symbol_is_string(result));
    if (symbol_is_undefined(var)) {
        symbol_str_append(result, "undefined");
#ifdef USE_ARRAY
    } else if (symbol_is_array(var)) {
        int length = (int)get_symbol_array_length(var);
        int i;
        symbol_str_append(result, "[");
        for (i = 0; i < length; i++) {
            be_jse_symbol_t *item = get_symbol_array_item(var, i);
            symbol_to_json(item, result);
            symbol_unlock(item);
            if (i < length - 1) symbol_str_append(result, ",");
        }
        symbol_str_append(result, "]");
#endif
    } else if (symbol_is_object(var)) {
        be_jse_node_t childref = var->first_child;
        symbol_str_append(result, "{");
        while (childref) {
            be_jse_symbol_t *child = symbol_lock(childref);
            be_jse_symbol_t *childVar;
            /* TODO: ability to append one string to another */
            symbol_str_append(result, "\"");
            symbol_str_value_append(
                result, child, 0,
                BE_JSE_SYMBOL_APPEND_STR_MAX_LEN); /* FIXME: escape the string
                                                    */
            symbol_str_append(result, "\":");
            childVar = symbol_lock(child->first_child);
            childref = child->next_sibling;
            symbol_unlock(child);
            symbol_to_json(childVar, result);
            symbol_unlock(childVar);
            if (childref) symbol_str_append(result, ",");
        }
        symbol_str_append(result, "}");
    } else if (symbol_is_function(var)) {
        be_jse_node_t coderef  = 0;
        be_jse_node_t childref = var->first_child;
        bool firstParm         = true;
        symbol_str_append(result, "function (");
        while (childref) {
            be_jse_symbol_t *child = symbol_lock(childref);
            childref               = child->next_sibling;
            if (symbol_is_function_argv(child)) {
                if (firstParm)
                    firstParm = false;
                else
                    symbol_str_append(result, ",");
                symbol_str_value_append(
                    result, child, 0,
                    BE_JSE_SYMBOL_APPEND_STR_MAX_LEN); /* FIXME: escape the
                                                          string */
            } else if (symbol_is_string(child) &&
                       symbol_str_equal(child,
                                        BE_JSE_PARSE_FUNCTION_CODE_NAME)) {
                coderef = child->first_child;
            }
            symbol_unlock(child);
        }
        symbol_str_append(result, ") ");
        if (coderef) {
            be_jse_symbol_t *codeVar = symbol_lock(coderef);
            symbol_str_value_append(result, codeVar, 0,
                                    BE_JSE_SYMBOL_APPEND_STR_MAX_LEN);
            symbol_unlock(codeVar);
        } else
            symbol_str_append(result, "{}");
        /*  } else {
               char buf[MAX_TOKEN_LENGTH];
               symbol_to_str(var, buf, MAX_TOKEN_LENGTH-1);
               if( symbol_is_string(var) )
                   symbol_str_append(result, "\"");
               symbol_str_append(result, buf);
               if( symbol_is_string(var) )
                   symbol_str_append(result, "\"");
           } */
    } else if (symbol_is_string(var)) {
        symbol_str_append(result, "\"");
        symbol_str_value_append(result, var, 0,
                                BE_JSE_SYMBOL_APPEND_STR_MAX_LEN);
        symbol_str_append(result, "\"");
    } else {
        char buf[MAX_TOKEN_LENGTH];
        symbol_to_str(var, buf, MAX_TOKEN_LENGTH - 1);
        symbol_str_append(result, buf);
    }
    /* TODO: functions */
}

#endif

/* dump symbol usage */

#ifdef DUMP_SYMBL_USAGE

/* get symbol usage */
/*
int be_jse_get_memory_usage()
{
    int usage = 0;
    int i;
    for (i=1; i<BE_JSE_SYMBOL_TABLE_SIZE; i++)
    {
        if (gSymbolTable[i].refs != SYM_TABLE_UNUSED_REF)
            usage++;
    }
    return usage;
}
beJSESymbolUsage == be_jse_get_memory_usage() + 1
*/

int be_jse_get_memory_usage()
{
    return beJSESymbolUsage;
}

int be_jse_get_memory_max_usage()
{
    return beJSESymbolMaxUsage;
}

int be_jse_get_memory_count()
{
    return gSymbolTableSize;
}

void be_jse_show_symbol_table_used()
{
    int i;
    for (i = 1; i < gSymbolTableSize; i++) {
        if (get_symbol_addr(i)->refs != SYM_TABLE_UNUSED_REF) {
            printf("USED VAR #%d:", i);
            trace_symbol_info(i, 2);
        }
    }
}

#endif

#ifdef TRACE_JSE_INFO

void dump_symbol_node_id(int id)
{
    be_jse_symbol_t *s;
    s = get_symbol_addr(id);
    if (s) {
        printf("nodeid: %d \n", id);
        printf("r %d, l %d, flag = %d \n", s->refs, s->locks, s->flags);
        char buf[64];
        symbol_to_str(s, buf, 63);
        printf("to str: %s \n", buf);
    }
}

ALWAYS_INLINE void trace_symbol_lock_info(be_jse_symbol_t *v)
{
    printf("#%d [r%d,l%d] ", get_symbol_node_id(v), v->refs, v->locks - 1);
}

void trace_symbol_info(be_jse_node_t ref, int indent)
{
#ifdef BE_JSE_SILENT
    NOT_USED(ref);
    NOT_USED(indent);
#else
    int i;
    char buf[BE_JSE_ERROR_BUF_SIZE];
    be_jse_symbol_t *var;

    for (i = 0; i < indent; i++) printf(" ");

    if (!ref) {
        printf("undefined\n");
        return;
    }
    var = symbol_lock(ref);
    printf("#%d[r%d,l%d] ", ref, var->refs, var->locks - 1);

    if (symbol_is_name(var)) {
        if (symbol_is_function_argv(var)) printf("Param ");
        symbol_to_str(var, buf, BE_JSE_ERROR_BUF_SIZE - 1);
        if (symbol_is_int(var))
            printf("Name: int %s  ", buf);
        else if (symbol_is_float(var))
            printf("Name: flt %s  ", buf);
        else if (symbol_is_string(var) || symbol_is_function_argv(var))
            printf("Name: '%s'  ", buf);
        else
            be_assert(0);

        ref = var->first_child;
        symbol_unlock(var);
        if (ref) {
            var = symbol_lock(ref);
            trace_symbol_lock_info(var);
        } else {
            printf("undefined\n");
            return;
        }
    }
    if (symbol_is_name(var)) {
        printf("\n");
        trace_symbol_info(get_symbol_node_id(var), indent + 1);
        symbol_unlock(var);
        return;
    }
    if (symbol_is_object(var))
        printf("Object {\n");
    else if (symbol_is_array(var))
        printf("Array [\n");
    else if (symbol_is_int(var))
        printf("Integer ");
    else if (symbol_is_float(var))
        printf("Double ");
    else if (symbol_is_string(var))
        printf("String ");
    else if (symbol_is_function(var))
        printf("Function {\n");
    else
        printf("Flags %d\n", var->flags);

    if (!symbol_is_object(var) && !symbol_is_array(var) &&
        !symbol_is_function(var)) {
        symbol_to_str(var, buf, BE_JSE_ERROR_BUF_SIZE - 1);
        printf("%s", buf);
    }

    if (symbol_is_string(var) || symbol_is_string_ext(var)) {
        if (!symbol_is_string_ext(var) && var->first_child) {
            printf("( Multi-block string ");
            be_jse_node_t child = var->first_child;
            while (child) {
                be_jse_symbol_t *childVar = symbol_lock(child);
                trace_symbol_lock_info(childVar);
                child = childVar->first_child;
                symbol_unlock(childVar);
            }
            printf(")\n");
        } else
            printf("\n");
    } else if (!(var->flags & BE_SYM_RECURSING)) {
        var->flags |= BE_SYM_RECURSING;
        be_jse_node_t child = var->first_child;
        printf("\n");
        /* dump children */
        while (child) {
            be_jse_symbol_t *childVar;
            trace_symbol_info(child, indent + 1);
            childVar = symbol_lock(child);
            child    = childVar->next_sibling;
            symbol_unlock(childVar);
        }
        var->flags &= (be_jse_symbol_type_e)~BE_SYM_RECURSING;
    } else {
        printf(" ... ");
    }

    if (symbol_is_object(var) || symbol_is_function(var)) {
        for (i = 0; i < indent; i++) printf(" ");
        printf("}\n");
    } else if (symbol_is_array(var)) {
        for (i = 0; i < indent; i++) printf(" ");
        printf("]\n");
    }

    symbol_unlock(var);
#endif
}

#endif
