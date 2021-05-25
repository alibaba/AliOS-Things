/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aos/cli.h"
#include "aos/kernel.h"
#include "cli_adapt.h"
#include "cli_api.h"
#include "cli_console.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if CLI_IOBOX_ENABLE
#include "path_helper.h"
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if AOS_COMP_DEBUG
#include "aos/debug.h"
#endif

#define RET_CHAR '\n'
#define END_CHAR '\r'
#define PROMPT   "# "
#define EXIT_MSG "exit"

typedef struct {
    int32_t argc;
    char *argv[CLI_MAX_ARG_NUM];
} _cmd_arg_t;

typedef struct {
    _cmd_arg_t arg[CLI_MAX_ONCECMD_NUM];
} cmd_arg_t;

typedef enum {
    CLI_WAIT_NORMAL,
    CLI_WAIT_SPEC_KEY,
    CLI_WAIT_FUNC_KEY,
} cli_input_stat;

struct cli_status {
    int32_t inited;
    uint32_t num;
    int32_t echo_disabled;

    char inbuf[CLI_INBUF_SIZE];

    const struct cli_command *cmds[CLI_MAX_COMMANDS];

    uint32_t his_idx;
    uint32_t his_cur;
    char history[CLI_INBUF_SIZE];

    uint32_t cmd_cur_pos;
    uint32_t cmd_end_pos;

    cli_input_stat input_status;
};

static struct cli_status *g_cli = NULL;
extern cli_console cli_uart_console;
extern void hal_reboot(void);
#if CLI_IOBOX_ENABLE
extern uint32_t vfs_get_match_dev_node(const char *name, char *match_name);
#endif
int32_t cli_va_printf(const char *fmt, va_list va);
int32_t cli_printf(const char *fmt, ...);

static inline void cli_prefix_print(void)
{
#if CLI_IOBOX_ENABLE
    char _buf[PATH_MAX] = {0};
    cli_printf("(%s:%s)"PROMPT, cli_task_get_console_name(),
               getcwd(_buf, sizeof(_buf)));
#else
    cli_printf("(%s)"PROMPT, cli_task_get_console_name());
#endif
}

static const struct cli_command *lookup_command(char *name, int32_t len)
{
    int32_t i = 0;
    int32_t n = 0;

    while (i < CLI_MAX_COMMANDS && n < g_cli->num) {
        if (g_cli->cmds[i]->name == NULL) {
            i++;
            continue;
        }
        /* See if partial or full match is expected */
        if (len != 0) {
            if (!strncmp(g_cli->cmds[i]->name, name, len)) {
                return g_cli->cmds[i];
            }
        } else {
            if (!strcmp(g_cli->cmds[i]->name, name)) {
                return g_cli->cmds[i];
            }
        }

        i++;
        n++;
    }

    return NULL;
}

int32_t proc_onecmd(int32_t argc, char *argv[])
{
    int32_t i = 0;
    int32_t ret = 0;
    uint8_t tmp = 0;
    const char *p = NULL;

    const struct cli_command *command = NULL;

    if (argc < 1) {
        ret = CLI_ERR_INVALID;
        goto cmd_err;
    }

    if (!g_cli->echo_disabled) {
        tmp = cli_console_get_tag_len(NULL);
        cli_console_set_tag_len(0, NULL);
        cli_printf("\r\n");

        cli_console_set_tag_len(tmp, NULL);
    }

    /*
     * Some comamands can allow extensions like foo.a, foo.b and hence
     * compare commands before first dot.
     */
    i = ((p = strchr(argv[0], '.')) == NULL) ? 0 : (p - argv[0]);

    command = lookup_command(argv[0], i);
    if (command == NULL) {
        ret = CLI_ERR_CMDNOTEXIST;
        goto cmd_err;
    }

    command->function(NULL, CLI_OUTBUF_SIZE, argc, argv);

    ret = 0;

cmd_err:
    return ret;
}

int32_t cli_handle_input(char *inbuf)
{
    struct {
        unsigned inArg:1;
        unsigned inQuote:1;
        unsigned done:1;
    } stat;

    cmd_arg_t *cmd_arg_all = (cmd_arg_t *)aos_zalloc(sizeof(cmd_arg_t));
    if (!cmd_arg_all) {
        printf("%s malloc fail\r\n", __func__);
        return -1;
    }

    int32_t cmdnum = 0;
    int32_t *pargc = &(cmd_arg_all->arg[0].argc);
    int32_t i = 0;
    int32_t ret = 0;

    memset(&stat, 0, sizeof(stat));

    do {
        switch (inbuf[i]) {
            case '\0':
                if (stat.inQuote) {
                    ret = CLI_ERR_SYNTAX;
                    goto exit;
                }
                stat.done = 1;
                break;

            case '"':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i],
                           strlen((const char *)(&inbuf[i]) + 1));
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg) {
                    break;
                }
                if (stat.inQuote && !stat.inArg) {
                    ret = CLI_ERR_SYNTAX;
                    goto exit;
                }

                if (!stat.inQuote && !stat.inArg) {
                    stat.inArg = 1;
                    stat.inQuote = 1;
                    (*pargc)++;
                    (cmd_arg_all->arg[cmdnum]).argv[(*pargc) - 1] =
                        &inbuf[i + 1];
                } else if (stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    stat.inQuote = 0;
                    inbuf[i] = '\0';
                }
                break;

            case ' ':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i],
                           strlen((const char *)(&inbuf[i]) + 1));
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    inbuf[i] = '\0';
                }
                break;

            case ';':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i],
                           strlen((const char *)(&inbuf[i]) + 1));
                    --i;
                    break;
                }
                if (stat.inQuote) {
                    ret = CLI_ERR_SYNTAX;
                    goto exit;
                }
                if (!stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    inbuf[i] = '\0';

                    if (*pargc) {
                        if (++cmdnum < CLI_MAX_ONCECMD_NUM) {
                            pargc = &((cmd_arg_all->arg[cmdnum]).argc);
                        }
                    }
                }

                break;

            default:
                if (!stat.inArg) {
                    stat.inArg = 1;
                    (*pargc)++;
                    (cmd_arg_all->arg[cmdnum]).argv[(*pargc) - 1] = &inbuf[i];
                }
                break;
        }
    } while (!stat.done && ++i < CLI_INBUF_SIZE &&
             cmdnum < CLI_MAX_ONCECMD_NUM && (*pargc) < CLI_MAX_ARG_NUM);

    if (stat.inQuote) {
        ret = CLI_ERR_SYNTAX;
        goto exit;
    }

    for (i = 0; i <= cmdnum && i < CLI_MAX_ONCECMD_NUM; i++) {
        ret |=
            proc_onecmd((cmd_arg_all->arg[i]).argc, (cmd_arg_all->arg[i]).argv);
    }

exit:
    if (cmd_arg_all) {
        free(cmd_arg_all);
    }
    return ret;
}

/**
 * @brief Perform basic tab-completion on the input buffer
 *
 * @param[in] inbuf the input buffer
 * @param[in] bp    the current buffer pointer
 *
 * @return none
 *
 */
static void cli_tab_complete(char *inbuf, uint32_t *idx)
{
    int32_t i, n, m;

    const char *fm = NULL;

    i = n = m = 0;

    /* show matching commands */
    for (i = 0; i < CLI_MAX_COMMANDS && n < g_cli->num; i++) {
        if (g_cli->cmds[i]->name != NULL) {
            if (!strncmp(inbuf, g_cli->cmds[i]->name, *idx)) {
                m++;
                if (m == 1) {
                    fm = g_cli->cmds[i]->name;
                } else if (m == 2) {
                    cli_printf("\r\n%-24s    %-24s", fm, g_cli->cmds[i]->name);
                } else if (m > 2 && m % 4 == 1) {
                    cli_printf("\r\n%-24s", g_cli->cmds[i]->name);
                } else {
                    cli_printf("    %-24s", g_cli->cmds[i]->name);
                }
            }
            n++;
        }
    }

    /* there's only one match, so complete the line */
    if (m == 1 && fm) {
        n = strlen((const char *)fm) - *idx;
        if (*idx + n < CLI_INBUF_SIZE) {
            memcpy(inbuf + *idx, fm + *idx, n);
            *idx += n;
            inbuf[*idx] = '\0';
        }
    }
    if (m >= 2) {
        cli_printf("\r\n");
        cli_prefix_print();
    }

    /* just redraw input line */
    cli_printf("%s", inbuf);
}

#if CLI_IOBOX_ENABLE
static char *find_last(char *inbuf, uint32_t *idx)
{
    char *ret = inbuf + (--(*idx));

    if (*ret == ' ')
        return NULL;

    while ((*ret != ' ') && (*idx > 0)) {
        ret--;
        (*idx)--;
    }

    if (*ret == ' ') {
        ret++;
        (*idx)++;
    } else {
        ret = NULL;
    }

    return ret;
}

static void refine_dir(char *dir, char **last_name)
{
    int slash_cnt = 0;
    char *p = dir, *last_slash = dir;

    while (*p != '\0') {
        if (*p == '/') {
            last_slash = p;
            slash_cnt++;
        }

        p++;
    }

    *last_name = last_slash + 1;
    if (slash_cnt < 2) {
        (*last_name)[strlen((const char *)(*last_name)) + 1] = '\0';
        memmove((*last_name) + 1, *last_name, strlen(*last_name));
        **last_name = '\0';
        (*last_name)++;
    } else {
        *last_slash = '\0';
    }
}

#define CLI_AUTO_PATH_DEBUG(...) /* cli_printf(__VA_ARGS__) */

static void cli_tab_complete_path(char *inbuf, uint32_t *idx)
{
    int32_t n, m;
    bool match_all = false;
    struct stat s;
    uint32_t last_idx = *idx;
    char *last_str = NULL, *last_name = NULL;
    char *dir = NULL, abspath[256] = {0};
    DIR *pdir = NULL;
    struct dirent *entry = NULL;
    char fpath[256], fm[256] = {0};
    int len, fm_type = 0;
    char tmpdir[256] = {0};

    CLI_AUTO_PATH_DEBUG("%s %d, inbuf: %s, idx: %d\r\n", __func__, __LINE__,
                        inbuf, *idx);
    last_str = find_last(inbuf, &last_idx);
    CLI_AUTO_PATH_DEBUG("%s %d, last: %s, last_idx: %d\r\n", __func__, __LINE__,
                        last_str, last_idx);

    if (!last_str) {
        // tab after string, match all entries
        match_all = true;
        dir = getcwd(abspath, sizeof(abspath));
        CLI_AUTO_PATH_DEBUG("%s %d, dir: %s\r\n", __func__, __LINE__, dir);
    } else {
        // tab after string
        // get realp path
        if (last_str[strlen((const char *)last_str) - 1] == '/')
            match_all = true;

        CLI_AUTO_PATH_DEBUG("%s %d, match_all: %d\r\n", __func__, __LINE__,
                            match_all);
        // get real path, without tailing '/' for dir!
        dir = get_realpath(last_str, abspath, sizeof(abspath));
        if (!dir) {
            cli_printf("Failed to get real path!\r\n");
            return;
        }

        strcpy(tmpdir, dir);
        CLI_AUTO_PATH_DEBUG(
            "%s %d, inbuf: %s, idx: %d, last_str:%s, tmpdir:%s, dir:%s\r\n",
            __func__, __LINE__, inbuf, *idx, last_str, tmpdir, dir);

        if (!match_all) {
            // get base name and last name, dir string will be reformed!
            refine_dir(dir, &last_name);
            CLI_AUTO_PATH_DEBUG("%s %d, dir: %s, last_name: %s\r\n", __func__,
                                __LINE__, dir, last_name);
        }

        // update inbuf with only base name
        *idx -= strlen((const char *)tmpdir) - strlen((const char *)dir);
        // strcpy(last_str, dir);

        if (inbuf[*idx] == ' ')
            /*skip to replace white space.*/
            (*idx)++;
        inbuf[*idx] = '\0';
        // *idx = strlen(inbuf);
        CLI_AUTO_PATH_DEBUG("%s %d, inbuf: %s, idx: %d\r\n", __func__, __LINE__,
                            inbuf, *idx);
    }

    n = m = 0;

    if (stat(dir, &s)) {
        cli_printf("%s not existed\r\n", dir);
        return;
    }

    if (!S_ISDIR(s.st_mode)) {
        cli_printf("%s is not a valid dir\r\n", dir);
        return;
    }

    pdir = opendir(dir);
    if (!pdir) {
        cli_printf("Failed to open dir %s\r\n", dir);
        return;
    }

    /*  device fs process */
    if (!strncmp(dir, "/dev", strlen("/dev"))) {
        m = vfs_get_match_dev_node(last_name, fm);
    } else {
        uint32_t match_count = 0;
        while ((entry = readdir(pdir))) {
            /*skip "./" and "../" directory.*/
            if ((!strncmp(entry->d_name, ".", 1)) ||
                (!strncmp(entry->d_name, "..", 2)))
                continue;

            memset(&s, 0, sizeof(s));
            memset(fpath, 0, 128);
            snprintf(fpath, 128, "%s%s%s", dir,
                     dir[strlen((const char *)dir) - 1] == '/' ? "" : "/",
                     entry->d_name);
            CLI_AUTO_PATH_DEBUG("%s %d, fpath: %s, m: %d, n: %d\r\n", __func__,
                                __LINE__, fpath, m, n);

            if (stat(fpath, &s)) {
                cli_printf("stat %s failed - %s\n", fpath, strerror(errno));
                continue;
            }

            char dname[256] = {0};
            if (S_ISDIR(s.st_mode))
                snprintf(dname, 256, "%s%s", entry->d_name, "/");
            else
                snprintf(dname, 256, "%s", entry->d_name);

            if (match_all) {
                match_count++;
                /*print all file or directory under current dir.*/
                if (match_count % 4 == 1) {
                    cli_printf("\r\n%-24s", dname);
                } else {
                    cli_printf("    %-24s", dname);
                }
                continue;
            } else {
                CLI_AUTO_PATH_DEBUG("%s %d, last_name: %s, entryname: %s\r\n",
                                    __func__, __LINE__, last_name,
                                    entry->d_name);
                if (!strncmp(last_name, entry->d_name,
                             strlen((const char *)last_name))) {
                    m++;

                    if (m == 1) {
                        // fm = entry->d_name;
                        snprintf(fm, sizeof(fm), "%s", entry->d_name);
                        fm_type = S_ISDIR(s.st_mode) ? 1 : 0;
                        CLI_AUTO_PATH_DEBUG("%s %d, fm: %s, fm_type: %d\r\n",
                                            __func__, __LINE__, fm, fm_type);
                    } else if (m == 2) {
                        cli_printf("\r\n%-24s    %-24s", fm, dname);
                    } else if (m > 2 && m % 4 == 1) {
                        cli_printf("\r\n%-24s", dname);
                    } else {
                        cli_printf("    %-24s", dname);
                    }
                }
                n++;
            }
        }
    }

    closedir(pdir);

    // if match all case, work done here.
    if (match_all) {
        cli_printf("\r\n");
        goto redraw;
    }

    CLI_AUTO_PATH_DEBUG("%s %d, inbuf: %s, idx: %d, m: %d, n: %d\r\n", __func__,
                        __LINE__, inbuf, *idx, m, n);
    /* there's only one match, so complete the line */
    if (m == 1 && fm[0] != 0) {
        n = strlen((const char *)fm) + ((fm_type == 1) ? 2 : 1);
        if ((*idx + n + 1) < CLI_INBUF_SIZE) {
            // memcpy(inbuf + *idx, fm + *idx, n);
            snprintf(inbuf + *idx, CLI_INBUF_SIZE - *idx, "%s%s%s",
                     (inbuf[(*idx) - 1] == '/' || inbuf[(*idx) - 1] == ' ') ?
                         "" :
                         "/",
                     fm, fm_type == 1 ? "/" : "");
            *idx += n;
            inbuf[*idx] = '\0';
        }
    }

    if (m >= 2 || m < 1) {
        if (m >= 2) {
            cli_printf("\r\n");
        }
        // strncpy(inbuf + *idx, last_name, CLI_INBUF_SIZE - *idx);
        snprintf(inbuf + *idx, CLI_INBUF_SIZE - *idx, "%s%s",
                 (inbuf[(*idx) - 1] == '/' || inbuf[(*idx) - 1] == ' ') ? "" :
                                                                          "/",
                 last_name);
        *idx = strlen((const char *)inbuf);
    }

    CLI_AUTO_PATH_DEBUG("%s %d, inbuf: %s, idx: %d\r\n", __func__, __LINE__,
                        inbuf, *idx);

redraw:
    /* just redraw input line */
    if (match_all) {
        len = strlen(inbuf);
        if (inbuf[len - 1] != '/')
            inbuf[len++] = '/';
        inbuf[len] = '\0';
    }

    if (m >= 2 || match_all) {
        cli_prefix_print();
    }
    cli_printf("%s", inbuf);
}
#endif

static void cli_history_input(char *cli_console_inbuf)
{
    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    char *inbuf = cli_console_inbuf;
    int32_t charnum = strlen((const char *)cli_console_inbuf) + 1;
    int32_t his_cur = g_cli->his_cur;
    int32_t left_num = CLI_INBUF_SIZE - his_cur;

    char lastchar;
    int32_t tmp_idx;

    g_cli->his_idx = his_cur;

    if (left_num >= charnum) {
        tmp_idx = his_cur + charnum - 1;
        lastchar = g_cli->history[tmp_idx];
        strncpy(&(g_cli->history[his_cur]), inbuf, charnum);

    } else {
        tmp_idx = (his_cur + charnum - 1) % CLI_INBUF_SIZE;
        lastchar = g_cli->history[tmp_idx];

        strncpy(&(g_cli->history[his_cur]), inbuf, left_num);
        strncpy(&(g_cli->history[0]), inbuf + left_num, charnum - left_num);
    }
    tmp_idx = (tmp_idx + 1) % CLI_INBUF_SIZE;

    g_cli->his_cur = tmp_idx;

    /*overwrite*/
    if ('\0' != lastchar) {
        while (g_cli->history[tmp_idx] != '\0') {
            g_cli->history[tmp_idx] = '\0';

            tmp_idx = (tmp_idx + 1) % CLI_INBUF_SIZE;
        }
    }
    RHINO_CPU_INTRPT_ENABLE();
}

static void cli_up_history(char *inaddr)
{
    uint32_t index, detectindex, lastindex;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    lastindex = g_cli->his_idx;
    index = (g_cli->his_idx - 1 + CLI_INBUF_SIZE) % CLI_INBUF_SIZE;
    detectindex = (index - 1 + CLI_INBUF_SIZE) % CLI_INBUF_SIZE;

    while ((g_cli->history[index] == '\0') &&
           g_cli->history[detectindex] != '\0' &&
           (g_cli->his_idx != g_cli->his_cur ||
            g_cli->history[g_cli->his_idx] == '\0') &&
           (index != g_cli->his_idx)) {
        index = (index - 1 + CLI_INBUF_SIZE) % CLI_INBUF_SIZE;
    }
    if (index != g_cli->his_idx) {
        while (g_cli->history[index] != '\0' && index != g_cli->his_cur) {
            index = (index - 1 + CLI_INBUF_SIZE) % CLI_INBUF_SIZE;
        }
        index = (index + 1) % CLI_INBUF_SIZE;
    }
    g_cli->his_idx = index;

    while (g_cli->history[lastindex] != '\0') {
        *inaddr++ = g_cli->history[lastindex];
        lastindex = (lastindex + 1) % CLI_INBUF_SIZE;
    }
    *inaddr = '\0';

    RHINO_CPU_INTRPT_ENABLE();
    return;
}

static void cli_down_history(char *inaddr)
{
    uint32_t index, lastindex;

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    lastindex = g_cli->his_idx;
    index = g_cli->his_idx;

    while ((g_cli->history[index] != '\0')) {
        index = (index + 1) % CLI_INBUF_SIZE;
    }
    if (index != g_cli->his_idx) {
        while (g_cli->history[index] == '\0' && index != g_cli->his_cur) {
            index = (index + 1) % CLI_INBUF_SIZE;
        }
    }
    g_cli->his_idx = index;

    while (g_cli->history[lastindex] != '\0') {
        *inaddr++ = g_cli->history[lastindex];
        lastindex = (lastindex + 1) % CLI_INBUF_SIZE;
    }

    *inaddr = '\0';

    RHINO_CPU_INTRPT_ENABLE();
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
 * Note: the following Linux-like line editing commands are supported:
 *  Key Binding          Editor Action
 *  Ctrl A               Move cursor to start of the line
 *  Ctrl B               Move left one character
 *  Ctrl D               Delete a single character at the cursor position
 *  Ctrl E               Move cursor to end of current line
 *  Ctrl F               Move right one character
 *  Ctrl H or Backspace  Delete character, left (backspace or Del key)
 *  Ctrl K               Delete to the end of the line
 *  Ctrl U               Delete the entire line
 */
static int32_t cli_get_input(char *inbuf, uint32_t size)
{
    char c;
    uint32_t i;

    if (inbuf == NULL) {
        cli_printf("input null\r\n");
        return 0;
    }

    while (cli_getchar(&c) == 1) {
        if (g_cli->cmd_end_pos >= size) {
            cli_printf("\r\nError: input buffer overflow\r\n");
            cli_prefix_print();
            return 0;
        }
        /* received null or error */
        if (c == '\0' || c == 0xFF) {
            continue;
        }

        /* handle end of line, break */
        if (c == RET_CHAR || c == END_CHAR) { /* end of input line */
            g_cli->inbuf[g_cli->cmd_end_pos] = '\0';
            memcpy(inbuf, g_cli->inbuf, size);
            memset(g_cli->inbuf, 0, size);
            g_cli->cmd_cur_pos = 0;
            g_cli->cmd_end_pos = 0;
            return 1;
        }
        /*
         * handle arrow keys
         * up key  : 0x1b 0x5b 0x41
         * down key: 0x1b 0x5b 0x42
         * right key:0x1b 0x5b 0x43
         * left key: 0x1b 0x5b 0x44
         */
        if (c == 0x1b) {
            g_cli->input_status = CLI_WAIT_SPEC_KEY;
            continue;
        } else if (g_cli->input_status == CLI_WAIT_SPEC_KEY) {
            if (c == 0x5b) {
                g_cli->input_status = CLI_WAIT_FUNC_KEY;
                continue;
            }
            g_cli->input_status = CLI_WAIT_NORMAL;
        } else if (g_cli->input_status == CLI_WAIT_FUNC_KEY) {
            g_cli->input_status = CLI_WAIT_NORMAL;
            /* handle up/down/left/right key */
            if (c == 0x41 || c == 0x42) {
                /* UP or DWOWN key */
                if (c == 0x41) {
                    cli_up_history(g_cli->inbuf);
                } else {
                    cli_down_history(g_cli->inbuf);
                }
                g_cli->cmd_cur_pos = strlen((const char *)g_cli->inbuf);
                g_cli->cmd_end_pos = g_cli->cmd_cur_pos;

                /* clear the whole line */
                cli_printf("\33[2K\r");
                cli_prefix_print();
                cli_printf("%s", g_cli->inbuf);
                continue;
            } else if (c == 0x44) {
                /* left key */
                if (g_cli->cmd_cur_pos) {
                    cli_printf("\b");
                    g_cli->cmd_cur_pos--;
                }
                continue;
            } else if (c == 0x43) {
                /* right key */
                if (g_cli->cmd_cur_pos < g_cli->cmd_end_pos) {
                    cli_printf("%c", g_cli->inbuf[g_cli->cmd_cur_pos]);
                    g_cli->cmd_cur_pos++;
                }
                continue;
            }
        }
        /* handle tab key */
        if (c == '\t') {
            /* Move the cursor to the beginning of line */
            for (i = 0; i < g_cli->cmd_cur_pos; i++) {
                cli_printf("\b");
            }

            g_cli->inbuf[g_cli->cmd_end_pos] = '\0';
#if CLI_IOBOX_ENABLE
            if (strstr((const char *)g_cli->inbuf, " ")) {
                cli_tab_complete_path(g_cli->inbuf, &g_cli->cmd_end_pos);
            } else {
#endif
                cli_tab_complete(g_cli->inbuf, &g_cli->cmd_end_pos);
#if CLI_IOBOX_ENABLE
            }
#endif
            g_cli->cmd_cur_pos = strlen((const char *)g_cli->inbuf);
            g_cli->cmd_end_pos = g_cli->cmd_cur_pos;
            continue;
        }
        /* handle backspace or Ctrl H or Del key*/
        if ((c == 0x08) || (c == 0x7F)) {
            if (g_cli->cmd_cur_pos == 0) {
                continue;
            }

            g_cli->cmd_cur_pos--;
            g_cli->cmd_end_pos--;

            if (g_cli->cmd_end_pos > g_cli->cmd_cur_pos) {
                memmove(&g_cli->inbuf[g_cli->cmd_cur_pos],
                        &g_cli->inbuf[g_cli->cmd_cur_pos + 1],
                        g_cli->cmd_end_pos - g_cli->cmd_cur_pos);

                g_cli->inbuf[g_cli->cmd_end_pos] = 0;
                if (!g_cli->echo_disabled) {
                    cli_printf("\b%s  \b", &g_cli->inbuf[g_cli->cmd_cur_pos]);
                }
                /* move cursor */
                for (i = g_cli->cmd_cur_pos; i <= g_cli->cmd_end_pos; i++) {
                    cli_printf("\b");
                }
            } else {
                cli_printf("\b \b");
                g_cli->inbuf[g_cli->cmd_end_pos] = 0;
            }
            continue;
        }
        /* handle Ctrl B */
        if (c == 0x02) {
            /* Same as left key < -- */
            if (g_cli->cmd_cur_pos) {
                cli_printf("\b");
                g_cli->cmd_cur_pos--;
            }
            continue;
        }
        /* handle Ctrl F */
        if (c == 0x06) {
            /* Same as right key  --> */
            if (g_cli->cmd_cur_pos < g_cli->cmd_end_pos) {
                cli_printf("%c", g_cli->inbuf[g_cli->cmd_cur_pos]);
                g_cli->cmd_cur_pos++;
            }
            continue;
        }
        /* handle Ctrl U */
        if (c == 0x15) {
            /* Delete the entire line */
            cli_printf("\33[2K\r");
            cli_prefix_print();
            g_cli->cmd_cur_pos = 0;
            g_cli->cmd_end_pos = 0;
            continue;
        }
        /* handle Ctrl K */
        if (c == 0x0B) {
            /* Delete to the end of the line */
            if (g_cli->cmd_cur_pos < g_cli->cmd_end_pos) {
                for (i = g_cli->cmd_cur_pos; i < g_cli->cmd_end_pos; i++) {
                    cli_printf(" ");
                }
                for (i = g_cli->cmd_cur_pos; i < g_cli->cmd_end_pos; i++) {
                    cli_printf("\b");
                }
                g_cli->cmd_end_pos = g_cli->cmd_cur_pos;
            }
            continue;
        }
        /* handle Ctrl A */
        if (c == 0x01) {
            /* Move cursor to start of the line */
            for (i = 0; i < g_cli->cmd_cur_pos; i++) {
                cli_printf("\b");
            }
            g_cli->cmd_cur_pos = 0;
            continue;
        }
        /* handle Ctrl E */
        if (c == 0x05) {
            /* Move cursor to end of the line */
            if (g_cli->cmd_cur_pos < g_cli->cmd_end_pos) {
                for (i = g_cli->cmd_cur_pos; i < g_cli->cmd_end_pos; i++) {
                    cli_printf("%c", g_cli->inbuf[i]);
                }
                g_cli->cmd_cur_pos = g_cli->cmd_end_pos;
            }
            continue;
        }
        /* handle Ctrl D */
        if (c == 0x04) {
            /* Delete a single character at the cursor position */
            if ((g_cli->cmd_end_pos == 0) ||
                (g_cli->cmd_cur_pos == g_cli->cmd_end_pos)) {
                continue;
            }
            g_cli->cmd_end_pos--;

            if (g_cli->cmd_cur_pos == g_cli->cmd_end_pos) {
                /* cursor point to last character */
                cli_printf(" \b");
            } else {
                /* end_pos > cur_pos */
                memmove(&g_cli->inbuf[g_cli->cmd_cur_pos],
                        &g_cli->inbuf[g_cli->cmd_cur_pos + 1],
                        g_cli->cmd_end_pos - g_cli->cmd_cur_pos);

                g_cli->inbuf[g_cli->cmd_end_pos] = 0;
                if (!g_cli->echo_disabled) {
                    cli_printf("%s  \b", &g_cli->inbuf[g_cli->cmd_cur_pos]);
                }
                /* move cursor */
                for (i = g_cli->cmd_cur_pos; i <= g_cli->cmd_end_pos; i++) {
                    cli_printf("\b");
                }
            }
            continue;
        }

        /* discard large cmd */
        if (g_cli->cmd_end_pos >= size) {
            g_cli->cmd_end_pos = 0;
        }

        /* others: handle normal character */
        if (g_cli->cmd_cur_pos < g_cli->cmd_end_pos) {
            memmove(&g_cli->inbuf[g_cli->cmd_cur_pos + 1],
                    &g_cli->inbuf[g_cli->cmd_cur_pos],
                    g_cli->cmd_end_pos - g_cli->cmd_cur_pos);
            g_cli->inbuf[g_cli->cmd_cur_pos] = c;

            if (!g_cli->echo_disabled) {
                cli_printf("%s", &g_cli->inbuf[g_cli->cmd_cur_pos]);
            }

            /* move cursor to new position */
            for (i = g_cli->cmd_cur_pos; i < g_cli->cmd_end_pos; i++) {
                cli_printf("\b");
            }
        } else {
            g_cli->inbuf[g_cli->cmd_end_pos] = c;
            if (!g_cli->echo_disabled) {
                cli_printf("%c", c);
            }
        }

        g_cli->cmd_cur_pos++;
        g_cli->cmd_end_pos++;
        if (g_cli->cmd_end_pos >= size) {
            g_cli->cmd_cur_pos = 0;
            g_cli->cmd_end_pos = 0;
        }

        c = 0;
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
static void cli_print_bad_command(char *cmd_string)
{
    if (cmd_string != NULL) {
        cli_printf("command '%s' not found\r\n", cmd_string);
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
void cli_main(void *data)
{
    int32_t ret;

    char *msg = NULL;
    cli_console *cur_console = NULL;

    cli_task_set_console(krhino_cur_task_get(), (cli_console *)data);

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();
    if (g_cli == NULL) {
        g_cli = (struct cli_status *)cli_malloc(sizeof(struct cli_status));
        if (g_cli == NULL) {
            printf("%s : %d malloc fail\r\n", __func__, __LINE__);
            RHINO_CPU_INTRPT_ENABLE();
            return;
        }
        memset((void *)g_cli, 0, sizeof(struct cli_status));
    }
    RHINO_CPU_INTRPT_ENABLE();
    if (get_clitask_console() != get_default_console()) {
        cli_printf("\r\n%s\r\n",
                   "             Welcome to AliOS Things          ");
    }

#if CLI_IOBOX_ENABLE
    ret = aos_chdir(CONFIG_LFS_MOUNTPOINT);
    if (ret != 0) {
        cli_printf("Failed to change to %s, errno: %d\r\n",
                   CONFIG_LFS_MOUNTPOINT, ret);
    }
#endif
    cli_prefix_print();

#if CLI_SEPRATED_CONSOLE
    char ch = 0x1F;
    cli_console_set_tag(ch, 0, NULL);
#endif

    while (!cli_console_task_check_exit_flag()) {
        cur_console = get_clitask_console();
        if (cur_console == NULL) {
            break;
        }
        if (cli_get_input(cur_console->cli_console_inbuf, CLI_INBUF_SIZE) !=
            0) {
            msg = cur_console->cli_console_inbuf;
            if (!msg) {
                goto out;
            }
            if (cur_console->start_callback) {
                cur_console->start_callback(cur_console->private_data);
            }
            if (strlen(cur_console->cli_console_inbuf) > 0) {
                cli_history_input(cur_console->cli_console_inbuf);
            }
            ret = cli_handle_input(msg);
            switch (ret) {
                case CLI_ERR_SYNTAX:
                    cli_printf("syntax error\r\n");
                    break;
                case CLI_ERR_CMDNOTEXIST:
                    cli_printf("cmd not found\r\n");
                    break;
                case CLI_ERR_BADCMD:
                    cli_print_bad_command(msg);
                    break;
                default:
                    break;
            }

            if (cur_console->finsh_callback) {
                cur_console->finsh_callback(cur_console->private_data);
            }
        out:
            cli_printf("\r\n");
            cli_prefix_print();
        }
    }
    cli_printf("CLI exited\r\n");

    cli_console_current_task_destory();
    check_console_task_exit();
}

void cli_main_panic(void)
{
    int32_t ret;
    char *msg = NULL;
    char cli_console_inbuf[CLI_INBUF_SIZE] = {0};

    /* set uart console for panic*/
    cli_task_set_console(krhino_cur_task_get(), &cli_uart_console);

    while (1) {
        if (cli_get_input(cli_console_inbuf, CLI_INBUF_SIZE) != 0) {
            msg = cli_console_inbuf;

            if (strlen((const char *)cli_console_inbuf) > 0) {
                cli_history_input(cli_console_inbuf);
            }

            ret = cli_handle_input(msg);
            switch (ret) {
                case CLI_ERR_SYNTAX:
                    cli_printf("syntax error\r\n");
                    break;
                case CLI_ERR_CMDNOTEXIST:
                    cli_printf("cmd not found\r\n");
                    break;
                case CLI_ERR_BADCMD:
                    cli_print_bad_command(msg);
                    break;
                default:
                    break;
            }

            cli_printf("\r\n");
            cli_printf("(panic)#");
        }
    }
}

__attribute__((weak)) struct cli_region _cli_region_begin, _cli_region_end;

void usr_cli_register_init(void)
{
    int32_t ret;
    intptr_t addr;
    struct cli_region *index;
    struct cli_command *cmd;

    for (addr = (intptr_t)&_cli_region_begin; addr < (intptr_t)&_cli_region_end;) {
        index = (struct cli_region *)addr;
        addr += sizeof(struct cli_region);

        cmd = (struct cli_command *)cli_malloc(sizeof(struct cli_command));
        if (cmd == NULL) {
            cli_printf("usr cli malloc fail\n");
            return;
        }

        cmd->name = index->name;
        cmd->help = index->desc;
        cmd->function = (cmd_fun_t)(index->func);

        ret = cli_register_command(cmd);
        if (ret != CLI_OK) {
            cli_printf("usr cli register fail\n");
            return;
        }
    }
}

static void help_cmd(char *buf, int len, int argc, char **argv)
{
    int32_t i, n, commands_num;
    struct cli_command *cmd = NULL;

    commands_num = cli_get_commands_num();

    cli_printf("================ AliOS Things Command List ==============\r\n");

    for (i = 0, n = 0; i < commands_num; i++) {
        cmd = cli_get_command(i);
        if (!cmd) {
            cli_printf("cmd is null\r\n");
            continue;
        }
        if (cmd->name) {
            cli_printf("%-15s: %s\r\n", cmd->name, cmd->help ? cmd->help : "");
            n++;
        }
    }
    cli_printf(
        "****************** Commands Num : %d *******************\r\n\r\n", n);
    cli_printf(
        "================ AliOS Things Command end ===============\r\n\r\n");
}

static void reboot_cmd(char *buf, int len, int argc, char **argv)
{
    hal_reboot();
}

static const struct cli_command built_ins[] = {
    /*cli self*/
    {"help", "print this", help_cmd},
    {"reboot", "reboot system", reboot_cmd},
};

static int32_t cli_register_default_commands(void)
{
    int32_t ret;

    ret = cli_register_commands(built_ins,
                                sizeof(built_ins) / sizeof(struct cli_command));
    if (ret != CLI_OK) {
        return ret;
    }

    return CLI_OK;
}

int32_t cli_init(void)
{
    int32_t ret;

    g_cli = (struct cli_status *)cli_malloc(sizeof(struct cli_status));
    if (g_cli == NULL) {
        return CLI_ERR_NOMEM;
    }

    memset((void *)g_cli, 0, sizeof(struct cli_status));

    ret = create_default_console_task(CLI_CONFIG_STACK_SIZE, CLI_TASK_PRIORITY);
    if (ret != CLI_OK) {
        cli_printf("Error: Failed to create cli thread: %d\r\n", ret);
        goto init_err;
    }

    g_cli->inited = 1;
    g_cli->echo_disabled = 0;

    ret = cli_register_default_commands();
    if (ret != CLI_OK) {
        cli_printf("Error: register built-in commands failed");
        goto init_err;
    }

    /* register cli cmd for ALIOS_CLI_CMD_REGISTER */
    usr_cli_register_init();

#if CLI_UAGENT_ENABLE
    cli_uagent_init();
#endif

    return CLI_OK;

init_err:
    if (g_cli != NULL) {
        cli_free(g_cli);
        g_cli = NULL;
    }

    return ret;
}

int32_t cli_stop(void)
{
    cli_console_set_exit_flag(get_clitask_console());

    return CLI_OK;
}

char *cli_tag_get(void) { return cli_console_get_all_tag(NULL); }

int32_t cli_register_command(const struct cli_command *cmd)
{
    int32_t i = 0;

    if (g_cli == NULL) {
        return CLI_ERR_DENIED;
    }

    if (!cmd->name || !cmd->function) {
        return CLI_ERR_INVALID;
    }

    if (g_cli->num >= CLI_MAX_COMMANDS) {
        return CLI_ERR_NOMEM;
    }

    /*
     * Check if the command has already been registered.
     * Return 0, if it has been registered.
     */
    for (i = 0; i < g_cli->num; i++) {
        if (g_cli->cmds[i] == cmd) {
            return CLI_OK;
        }
    }

    g_cli->cmds[g_cli->num++] = cmd;

    return CLI_OK;
}

int32_t cli_unregister_command(const struct cli_command *cmd)
{
    int32_t remaining_cmds;
    int32_t i = 0;
    if (g_cli == NULL) {
        return CLI_ERR_DENIED;
    }

    if (!cmd->name || !cmd->function) {
        return CLI_ERR_INVALID;
    }

    for (i = 0; i < g_cli->num; i++) {
        if (g_cli->cmds[i] == cmd) {
            g_cli->num--;

            remaining_cmds = g_cli->num - i;
            if (remaining_cmds > 0) {
                memmove(&g_cli->cmds[i], &g_cli->cmds[i + 1],
                        (remaining_cmds * sizeof(struct cli_command *)));
            }

            g_cli->cmds[g_cli->num] = NULL;

            return CLI_OK;
        }
    }

    return CLI_ERR_NOMEM;
}

int32_t cli_register_commands(const struct cli_command *cmds, int32_t num)
{
    int32_t i, err;

    for (i = 0; i < num; i++) {
        if ((err = cli_register_command(cmds++)) != 0) {
            return err;
        }
    }

    return CLI_OK;
}

int32_t cli_unregister_commands(const struct cli_command *cmds, int32_t num)
{
    int32_t i, err;

    for (i = 0; i < num; i++) {
        if ((err = cli_unregister_command(cmds++)) != 0) {
            return err;
        }
    }

    return CLI_OK;
}

static int32_t cli_do_output(char *msg)
{
    if (cli_console_get_tag_len(NULL) > 0) {
        cli_putstr(cli_console_get_all_tag(NULL));
    }
    return cli_putstr(msg);
}

int32_t cli_va_printf(const char *fmt, va_list va)
{
    int32_t len;
    char *message = NULL, *child_message = NULL;

#if AOS_COMP_DEBUG
    int ret;
    (void)ret;
    extern uint32_t debug_cpu_in_crash(void);
    if (debug_cpu_in_crash()) {
#if CLI_SEPRATED_CONSOLE
        /* for smartTrace tool */
        char temp[1] = {0x1F};
        extern int alios_debug_print(const char *buf, int size);
        ret = alios_debug_print(temp, 1);
        if (ret != 1) {
            return 0;
        }
#endif
        extern int print_driver(const char *fmt, va_list ap,
                                unsigned int buf[]);
        print_driver(fmt, va, NULL);
        return 0;
    }
#endif

    message = (char *)cli_malloc(CLI_OUTBUF_SIZE);
    if (message == NULL) {
        return CLI_ERR_NOMEM;
    }
    memset(message, 0, CLI_OUTBUF_SIZE);

    child_message = (char *)cli_malloc(CLI_OUTBUF_SIZE);
    if (child_message == NULL) {
        cli_free(message);
        return CLI_ERR_NOMEM;
    }
    memset(child_message, 0, CLI_OUTBUF_SIZE);

    len = vsnprintf(message, CLI_OUTBUF_SIZE, fmt, va);

    if (len <= 0) {
        cli_free(message);
        cli_free(child_message);
        return CLI_OK;
    }

    /* multi '\n' in one cli_printf */
    int i = 0;
    char *p2 = message;
    char *p;
    while (i != -1) {
        i = ((p = strchr(p2, '\n')) == NULL) ? -1 : (p - p2);
        if (i == -1) {
            // cli_printf("%s", p2);
            if (*p2) {
                cli_do_output(p2);
            }
        } else {
            // cli_printf("%.*s\n", i + 1, p2);    // strlen = pos + 1
            memset(child_message, 0, CLI_OUTBUF_SIZE);
            strncpy(child_message, p2, i + 1);
            cli_do_output(child_message);
        }
        p2 = p + 1; // skip '\n'
    }

    cli_free(message);
    cli_free(child_message);

    return CLI_OK;
}

int32_t cli_printf(const char *fmt, ...)
{
    va_list params;
    int32_t ret;

    va_start(params, fmt);
    ret = cli_va_printf(fmt, params);
    va_end(params);
    return ret;
}

int32_t cli_get_commands_num(void) { return g_cli->num; }

struct cli_command *cli_get_command(int32_t index)
{
    return (struct cli_command *)(g_cli->cmds[index]);
}

int32_t cli_get_echo_status(void) { return g_cli->echo_disabled; }

int32_t cli_set_echo_status(int32_t status)
{
    g_cli->echo_disabled = status;

    return CLI_OK;
}
