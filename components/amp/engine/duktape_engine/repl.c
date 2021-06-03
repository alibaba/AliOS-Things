/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "amp_config.h"
#include "aos_system.h"
#include "amp_task.h"
#include "amp_defines.h"
#include "aos_hal_uart.h"
#include "duktape.h"
#include "be_inl.h"

#include "repl.h"

#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define MOD_STR "REPL"

#define RET_CHAR '\n'
#define END_CHAR '\r'

struct repl_status
{
    int32_t inited;
    uint32_t num;
    int32_t echo_disabled;
    uint32_t bp; /* Buffer pointer */

    char inbuf[REPL_INBUF_SIZE];
    char *outbuf;

    int32_t his_idx;
    int32_t his_cur;
    char history[REPL_INBUF_SIZE];
};

static struct repl_status *g_repl = NULL;
static uart_dev_t g_repl_uart;
static char g_repl_tag[64] = {0};
static uint8_t g_repl_tag_len = 0;
static int execute_finish = 0;
static char repl_message[REPL_OUTBUF_SIZE] = {0};

static int32_t repl_getchar(char *inbuf)
{
    int32_t ret = REPL_OK;
    uint32_t recv_size = 0;

    ret = aos_hal_uart_recv_II(&g_repl_uart, inbuf, 1, &recv_size, AOS_WAIT_FOREVER);
    if ((ret == 0) && (recv_size == 1))
    {
        return recv_size;
    }
    else
    {
        return 0;
    }
}

static int32_t repl_putstr(char *msg)
{
    if (msg[0] != 0)
    {
        aos_hal_uart_send(&g_repl_uart, (void *)msg, strlen(msg), 0xFFFFFFFFU);
    }

    return REPL_OK;
}

int32_t repl_printf(const char *buffer, ...)
{
    va_list ap;

    int32_t sz, len;

    char *pos = NULL;

    memset(repl_message, 0, REPL_OUTBUF_SIZE);

    sz = 0;
    if (g_repl_tag_len > 0)
    {
        len = strlen(g_repl_tag);
        strncpy(repl_message, g_repl_tag, len);
        sz = len;
    }

    pos = repl_message + sz;

    va_start(ap, buffer);
    len = vsnprintf(pos, REPL_OUTBUF_SIZE - sz, buffer, ap);
    va_end(ap);
    if (len <= 0)
    {
        return REPL_OK;
    }

    repl_putstr(repl_message);

    return REPL_OK;
}

static duk_ret_t wrapped_compile_execute(duk_context *ctx, void *udata)
{
    const char *src_data;
    duk_size_t src_len;
    duk_uint_t comp_flags;

    (void)udata;

    // duk_context *ctx = be_get_context();
    /* XXX: Here it'd be nice to get some stats for the compilation result
	 * when a suitable command line is given (e.g. code size, constant
	 * count, function count.  These are available internally but not through
	 * the public API.
	 */

    /* Use duk_compile_lstring_filename() variant which avoids interning
	 * the source code.  This only really matters for low memory environments.
	 */

    /* [ ... bytecode_filename src_data src_len filename ] */

    src_data = (const char *)duk_require_pointer(ctx, -3);
    src_len = (duk_size_t)duk_require_uint(ctx, -2);

    if (src_data != NULL && src_len >= 1 && src_data[0] == (char)0xbf)
    {
        /* Bytecode. */
        void *buf;
        buf = duk_push_fixed_buffer(ctx, src_len);
        memcpy(buf, (const void *)src_data, src_len);
        duk_load_function(ctx);
    }
    else
    {
        /* Source code. */
        comp_flags = DUK_COMPILE_SHEBANG;
        duk_compile_lstring_filename(ctx, comp_flags, src_data, src_len);
    }

    duk_push_global_object(ctx); /* 'this' binding */
    duk_call_method(ctx, 0);
    /*
        *  In interactive mode, write to stdout so output won't
        *  interleave as easily.
        *
        *  NOTE: the ToString() coercion may fail in some cases;
        *  for instance, if you evaluate:
        *
        *    ( {valueOf: function() {return {}},
        *       toString: function() {return {}}});
        *
        *  The error is:
        *
        *    TypeError: coercion to primitive failed
        *            duk_api.c:1420
        *
        *  These are handled now by the caller which also has stack
        *  trace printing support.  User code can print out errors
        *  safely using duk_safe_to_string().
        */

    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, -1, "dukFormat");
    duk_dup(ctx, -3);
    /* duk_call(ctx, 1);  /\* -> [ ... res stash formatted ] *\/ */
    repl_printf("%s", duk_to_string(ctx, -1));


    return 0; /* duk_safe_call() cleans up */
}

static void repl_handle_input(void *inbuf)
{
    int retval = 0;

    repl_printf("\r\n");
    duk_context *ctx = be_get_context();
    duk_push_pointer(ctx, inbuf);
    duk_push_uint(ctx, (duk_uint_t)strlen((char *)inbuf));
    duk_push_string(ctx, "input");

    int rc = duk_safe_call(ctx, wrapped_compile_execute, NULL /*udata*/, 3 /*nargs*/, 1 /*nret*/);

    if (rc != DUK_EXEC_SUCCESS)
    {
        /* in interactive mode, write to stdout */
        repl_printf("%s", duk_safe_to_stacktrace(ctx, -1));
        retval = -1; /* an error 'taints' the execution */
    }
    else
    {
        duk_pop(ctx);
    }

    repl_printf("\r\n");
    g_repl_tag[0] = '\x0';
    g_repl_tag_len = 0;
    repl_printf(REPL_PROMPT);
    execute_finish = 1;
}

static void repl_history_input(void)
{
    char *inbuf = g_repl->inbuf;
    int32_t charnum = strlen(g_repl->inbuf) + 1;
    int32_t his_cur = g_repl->his_cur;
    int32_t left_num = REPL_INBUF_SIZE - his_cur;

    char lastchar;
    int32_t tmp_idx;

    g_repl->his_idx = his_cur;

    if (left_num >= charnum)
    {
        tmp_idx = his_cur + charnum - 1;
        lastchar = g_repl->history[tmp_idx];
        strncpy(&(g_repl->history[his_cur]), inbuf, charnum);
    }
    else
    {
        tmp_idx = (his_cur + charnum - 1) % REPL_INBUF_SIZE;
        lastchar = g_repl->history[tmp_idx];

        strncpy(&(g_repl->history[his_cur]), inbuf, left_num);
        strncpy(&(g_repl->history[0]), inbuf + left_num, charnum - left_num);
    }
    tmp_idx = (tmp_idx + 1) % REPL_INBUF_SIZE;

    g_repl->his_cur = tmp_idx;

    /*overwrite*/
    if ('\0' != lastchar)
    {

        while (g_repl->history[tmp_idx] != '\0')
        {
            g_repl->history[tmp_idx] = '\0';

            tmp_idx = (tmp_idx + 1) % REPL_INBUF_SIZE;
        }
    }
}

static void repl_up_history(char *inaddr)
{
    int index;
    int lastindex = 0;

    lastindex = g_repl->his_idx;
    index = (g_repl->his_idx - 1 + REPL_INBUF_SIZE) % REPL_INBUF_SIZE;

    while ((g_repl->history[index] == '\0') && (index != g_repl->his_idx))
    {
        index = (index - 1 + REPL_INBUF_SIZE) % REPL_INBUF_SIZE;
    }
    if (index != g_repl->his_idx)
    {
        while (g_repl->history[index] != '\0')
        {
            index = (index - 1 + REPL_INBUF_SIZE) % REPL_INBUF_SIZE;
        }
        index = (index + 1) % REPL_INBUF_SIZE;
    }
    g_repl->his_idx = index;

    while (g_repl->history[lastindex] != '\0')
    {
        *inaddr++ = g_repl->history[lastindex];
        lastindex = (lastindex + 1) % REPL_INBUF_SIZE;
    }
    *inaddr = '\0';

    return;
}

static void repl_down_history(char *inaddr)
{
    int index;
    int lastindex = 0;

    lastindex = g_repl->his_idx;
    index = g_repl->his_idx;

    while ((g_repl->history[index] != '\0'))
    {
        index = (index + 1) % REPL_INBUF_SIZE;
    }
    if (index != g_repl->his_idx)
    {
        while (g_repl->history[index] == '\0')
        {
            index = (index + 1) % REPL_INBUF_SIZE;
        }
    }
    g_repl->his_idx = index;

    while (g_repl->history[lastindex] != '\0')
    {
        *inaddr++ = g_repl->history[lastindex];
        lastindex = (lastindex + 1) % REPL_INBUF_SIZE;
    }

    *inaddr = '\0';

    return;
}

/**
 * @brief Get an input line
 *
 * @param[in/out] inbuf poiner to the input buffer
 * @param[out]    bp    the current buffer pointer
 *
 * @return 1 if there is input, 0 if the line should be ignored
 *
 */
static int32_t repl_get_input(char *inbuf, uint32_t *bp)
{
    char c;
    int32_t esc = 0;
    int32_t key1 = -1;
    int32_t key2 = -1;
    uint8_t repl_tag_len = 0;

    if (inbuf == NULL)
    {
        repl_printf("input null\r\n");
        return 0;
    }

    while (repl_getchar(&c) == 1)
    {
        if (c == RET_CHAR || c == END_CHAR)
        { /* end of input line */
            inbuf[*bp] = '\0';
            *bp = 0;
            if (repl_tag_len > 0)
            {
                g_repl_tag_len = repl_tag_len;
                repl_tag_len = 0;
            }
            return 1;
        }

        if (c == 0x1b)
        { /* escape sequence */
            esc = 1;
            key1 = -1;
            key2 = -1;
            continue;
        }

        if (c == 0x3)
        { /* CTRL+C */
            // task_to_cancel = debug_task_find("cpuusage");
            // if (task_to_cancel != NULL) {
            //     krhino_task_cancel(task_to_cancel);
            // }
            continue;
        }

        if (esc)
        {
            if (key1 < 0)
            {
                key1 = c;
                if (key1 != 0x5b)
                {
                    /* not '[' */
                    inbuf[*bp] = 0x1b;
                    (*bp)++;

                    inbuf[*bp] = key1;
                    (*bp)++;

                    if (!g_repl->echo_disabled)
                    {
                        repl_printf("\x1b%c", key1); /* Ignore the cli tag */
                    }
                    esc = 0;
                }
                continue;
            }

            if (key2 < 0)
            {
                key2 = c;
                if (key2 == 't')
                {
                    g_repl_tag[0] = 0x1b;
                    g_repl_tag[1] = key1;
                    repl_tag_len = 2;
                }
            }

            if (key2 != 0x41 && key2 != 0x42 && key2 != 't')
            {
                if(key2 == 0x43 || key2 == 0x44)
                {
                    /* TODO: LEFT/RIGHT key */
                }

                /* not UP key, not DOWN key, not ESC_TAG */
                inbuf[*bp] = 0x1b;
                (*bp)++;

                inbuf[*bp] = key1;
                (*bp)++;

                inbuf[*bp] = key2;
                (*bp)++;

                g_repl_tag[0] = '\x0';
                repl_tag_len = 0;
                esc = 0;

                if (!g_repl->echo_disabled)
                {
                    repl_printf("\x1b%c%c", key1, key2);
                }
                continue;
            }
            if (key2 == 0x41 || key2 == 0x42)
            {
                /*  clear last input */
                int backspace = *bp - 1;
                while(backspace && inbuf[backspace--]){
                    repl_printf("\b \b");
                }

                /* UP or DWOWN key */
                if (key2 == 0x41)
                {
                    repl_up_history(inbuf);
                }
                else
                {
                    repl_down_history(inbuf);
                }

                *bp = strlen(inbuf);
                g_repl_tag[0] = '\x0';
                repl_tag_len = 0;
                esc = 0;

                repl_printf("\r" REPL_PROMPT "%s", inbuf);
                continue;
            }
            if (key2 == 't')
            {
                /* ESC_TAG */
                if (repl_tag_len >= sizeof(g_repl_tag))
                {
                    g_repl_tag[0] = '\x0';
                    repl_tag_len = 0;
                    esc = 0;

                    amp_error(MOD_STR, "Error: cli tag buffer overflow\r\n");
                    continue;
                }

                g_repl_tag[repl_tag_len++] = c;
                if (c == 'm')
                {
                    g_repl_tag[repl_tag_len++] = '\x0';

                    if (!g_repl->echo_disabled)
                    {
                        repl_printf("%s", g_repl_tag);
                    }
                    esc = 0;
                }
                continue;
            }
        }

        inbuf[*bp] = c;
        if ((c == 0x08) || (c == 0x7f))
        {
            if (*bp > 0)
            {
                (*bp)--;

                if (!g_repl->echo_disabled)
                {
                    repl_printf("%c %c", 0x08, 0x08);
                }
            }
            continue;
        }

        if (c == '\t')
        {
            // inbuf[*bp] = '\0';
            continue;
        }

        if (!g_repl->echo_disabled)
        {
            repl_printf("%c", c);
        }

        (*bp)++;
        if (*bp >= REPL_INBUF_SIZE)
        {
            amp_error(MOD_STR, "Error: input buffer overflow\r\n");
            repl_printf(REPL_PROMPT);
            *bp = 0;
            return 0;
        }
    }

    return 0;
}

/**
 * @brief Print out a bad command string
 *
 * @param[in] cmd_string the command string
 *
 * @return none
 *
 * @Note print including a hex representation of non-printable characters.
 * Non-printable characters show as "\0xXX".
 */
static void repl_print_bad_command(char *cmd_string)
{
    if (cmd_string != NULL)
    {
        amp_error(MOD_STR, "command '%s' not found\r\n", cmd_string);
    }
}

/**
 * @brief Main CLI processing loop
 *
 * @param[in] data pointer to the process arguments
 *
 * @return none
 *
 * @Note Waits to receive a command buffer pointer from an input collector,
 * and then process. it must cleanup the buffer when done with it.
 * Input collectors handle their own lexical analysis and must pass complete
 * command lines to CLI.
 *
 */
void repl_main(void *data)
{
    int32_t ret;

#ifdef AOS_COMP_CLI
    aos_cli_suspend();
#endif

    while (1)
    {
        if (repl_get_input(g_repl->inbuf, &g_repl->bp) != 0)
        {
            if (strcmp(g_repl->inbuf, "exit") == 0) {
                break;
            }
            if (strlen(g_repl->inbuf) > 0)
            {
                repl_history_input();
                amp_task_schedule_call(repl_handle_input, g_repl->inbuf);
                /* wait for js code execute finished */
                execute_finish = 0;
                while(!execute_finish)
                {
                    aos_msleep(100);
                }
            }
            else
            {
                repl_printf("\r\n");
                g_repl_tag[0] = '\x0';
                g_repl_tag_len = 0;
                repl_printf(REPL_PROMPT);
            }
        }
    }

    amp_warn(MOD_STR, "CLI exited\r\n");
    aos_free(g_repl);
    g_repl = NULL;
#ifdef AOS_COMP_CLI
    aos_cli_resume();
#endif
    return;
}

int32_t repl_init(void)
{
    int32_t ret;
    aos_task_t repl_task;

    if (g_repl) {
        amp_debug(MOD_STR, "REPL is already inited!");
        return REPL_OK;
    }

    g_repl = (struct repl_status *)aos_malloc(sizeof(struct repl_status));
    if (g_repl == NULL)
    {
        return REPL_ERR_NOMEM;
    }

    g_repl_uart.port = AMP_REPL_UART_PORT;
    g_repl_uart.config.baud_rate = AMP_REPL_UART_BAUDRATE;
    g_repl_uart.config.data_width = DATA_WIDTH_8BIT;
    g_repl_uart.config.flow_control = FLOW_CONTROL_DISABLED;
    g_repl_uart.config.mode = MODE_TX_RX;
    g_repl_uart.config.parity = NO_PARITY;
    g_repl_uart.config.stop_bits = STOP_BITS_1;
    aos_hal_uart_init(&g_repl_uart);

    memset((void *)g_repl, 0, sizeof(struct repl_status));

    if (aos_task_new_ext(&repl_task, "repl_task", repl_main, NULL, REPL_STACK_SIZE, AOS_DEFAULT_APP_PRI) != 0)
    {
        amp_error(MOD_STR, "Error: Failed to create cli thread: %d\r\n", ret);
        goto init_err;
    }

    g_repl->inited = 1;
    g_repl->echo_disabled = 0;

    amp_debug(MOD_STR, "REPL Enabled\r\n");
    return REPL_OK;

init_err:
    if (g_repl != NULL)
    {
        aos_free(g_repl);
        g_repl = NULL;
    }

    return ret;
}

#ifdef AOS_COMP_CLI
void jsrepl_startup()
{
    repl_init();
}
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(jsrepl_startup, jsrepl, "start js amp repl")
#endif
