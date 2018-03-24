/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <aos/aos.h>

#define RET_CHAR  '\n'
#define END_CHAR  '\r'
#define PROMPT    "# "
#define EXIT_MSG  "exit"

static struct cli_st *cli = NULL;
static int            cliexit = 0;
char                  esc_tag[64] = {0};
static uint8_t        esc_tag_len = 0;
extern void hal_reboot(void);
extern void log_cli_init(void);

#ifdef CONFIG_AOS_CLI_BOARD
extern int board_cli_init(void);
#endif

#ifdef VCALL_RHINO
extern uint32_t krhino_version_get(void);
#endif
extern int32_t aos_uart_send(void *data, uint32_t size, uint32_t timeout);
extern int32_t aos_uart_recv(void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout);
int cli_getchar(char *inbuf);

int cli_putstr(char *msg);

/* Find the command 'name' in the cli commands table.
* If len is 0 then full match will be performed else upto len bytes.
* Returns: a pointer to the corresponding cli_command struct or NULL.
*/
static const struct cli_command *lookup_command(char *name, int len)
{
    int i = 0;
    int n = 0;

    while (i < MAX_COMMANDS && n < cli->num_commands) {
        if (cli->commands[i]->name == NULL) {
            i++;
            continue;
        }
        /* See if partial or full match is expected */
        if (len != 0) {
            if (!strncmp(cli->commands[i]->name, name, len)) {
                return cli->commands[i];
            }
        } else {
            if (!strcmp(cli->commands[i]->name, name)) {
                return cli->commands[i];
            }
        }

        i++;
        n++;
    }

    return NULL;
}


/*proc one cli cmd and to run the according funtion
* Returns: 0 on success:
           1 fail 
*/
static int proc_onecmd(int argc,char * argv[])
{
    int i = 0;
    const char *p;
    const struct cli_command *command = NULL;

    if (argc < 1) {
        return 0;
    }

    if (!cli->echo_disabled) {
        csp_printf("\r\n");
        fflush(stdout);
    }

    /*
    * Some comamands can allow extensions like foo.a, foo.b and hence
    * compare commands before first dot.
    */
    i = ((p = strchr(argv[0], '.')) == NULL) ? 0 : (p - argv[0]);

    command = lookup_command(argv[0], i);
    if (command == NULL) {
        return 1;
    }

    memset(cli->outbuf, 0, OUTBUF_SIZE);

    command->function(cli->outbuf, OUTBUF_SIZE, argc, argv);
    cli_putstr(cli->outbuf);
    return 0;
}




/* Parse input line and locate arguments (if any), keeping count of the number
* of arguments and their locations.  Look up and call the corresponding cli
* function if one is found and pass it the argv array.
*
* Returns: 0 on success: the input line contained at least a function name and
*          that function exists and was called.
*          1 on lookup failure: there is no corresponding function for the
*          input line.
*          2 on invalid syntax: the arguments list couldn't be parsed
*/
static int handle_input(char *inbuf)
{
    struct {
        unsigned inArg: 1;
        unsigned inQuote: 1;
        unsigned done: 1;
    } stat;
    static char *argvall[CLI_MAX_ONCECMD_NUM][CLI_MAX_ARG_NUM];
    int argcall[CLI_MAX_ONCECMD_NUM] = {0};
    /*
    static char *argv[CLI_MAX_ONCECMD_NUM][CLI_MAX_ARG_NUM];
    int argc = 0;*/
    int cmdnum = 0;
    int * pargc = &argcall[0];
    int i = 0;
    int ret = 0;

    memset((void *)&argvall, 0, sizeof(argvall));
    memset((void *)&argcall, 0, sizeof(argcall));
    memset(&stat, 0, sizeof(stat));

    do {
        switch (inbuf[i]) {
            case '\0':
                if (stat.inQuote) {
                    return 2;
                }
                stat.done = 1;
                break;

            case '"':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i], strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (!stat.inQuote && stat.inArg) {
                    break;
                }
                if (stat.inQuote && !stat.inArg) {
                    return 2;
                }

                if (!stat.inQuote && !stat.inArg) {
                    stat.inArg = 1;
                    stat.inQuote = 1;
                    (*pargc)++;
                    argvall[cmdnum][(*pargc) - 1] = &inbuf[i + 1];
                } else if (stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    stat.inQuote = 0;
                    inbuf[i] = '\0';
                }
                break;

            case ' ':
                if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg) {
                    memcpy(&inbuf[i - 1], &inbuf[i],
                           strlen(&inbuf[i]) + 1);
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
                           strlen(&inbuf[i]) + 1);
                    --i;
                    break;
                }
                if (stat.inQuote) {
                    return 2;
                }
                if (!stat.inQuote && stat.inArg) {
                    stat.inArg = 0;
                    inbuf[i] = '\0';

                    if(*pargc) {
                        if(++cmdnum < CLI_MAX_ONCECMD_NUM) {
                            pargc = &argcall[cmdnum];
                        }
                    }    
                }

                break;

            default:
                if (!stat.inArg) {
                    stat.inArg = 1;
                    (*pargc)++;
                    argvall[cmdnum][(*pargc) - 1] = &inbuf[i];
                }
                break;
        }
    } while (!stat.done && ++i < INBUF_SIZE && cmdnum < CLI_MAX_ONCECMD_NUM && (*pargc) < CLI_MAX_ARG_NUM);

    if (stat.inQuote) {
        return 2;
    }

    for( i = 0; i <= cmdnum && i < CLI_MAX_ONCECMD_NUM ; i++ )
    {
        ret |= proc_onecmd(argcall[i],argvall[i]);
    }

    return ret;
}

/* Perform basic tab-completion on the input buffer by string-matching the
 * current input line against the cli functions table.  The current input line
 * is assumed to be NULL-terminated.
 */
static void tab_complete(char *inbuf, unsigned int *bp)
{
    int i, n, m;
    const char *fm = NULL;

    aos_cli_printf("\r\n");

    /* show matching commands */
    for (i = 0, n = 0, m = 0; i < MAX_COMMANDS && n < cli->num_commands;
         i++) {
        if (cli->commands[i]->name != NULL) {
            if (!strncmp(inbuf, cli->commands[i]->name, *bp)) {
                m++;
                if (m == 1) {
                    fm = cli->commands[i]->name;
                } else if (m == 2)
                    aos_cli_printf("%s %s ", fm,
                                   cli->commands[i]->name);
                else
                    aos_cli_printf("%s ",
                                   cli->commands[i]->name);
            }
            n++;
        }
    }

    /* there's only one match, so complete the line */
    if (m == 1 && fm) {
        n = strlen(fm) - *bp;
        if (*bp + n < INBUF_SIZE) {
            memcpy(inbuf + *bp, fm + *bp, n);
            *bp += n;
            inbuf[(*bp)++] = ' ';
            inbuf[*bp] = '\0';
        }
    }

    /* just redraw input line */
    aos_cli_printf("%s%s", PROMPT, inbuf);
}

/* Get an input line.
 *
 * Returns: 1 if there is input, 0 if the line should be ignored.
 */
static int get_input(char *inbuf, unsigned int *bp)
{
    char c;
    int  esc = 0, key1 = -1, key2 = -1;
    if (inbuf == NULL) {
        aos_cli_printf("inbuf_null\r\n");
        return 0;
    }

    cli->his_idx = (cli->his_cur + HIS_SIZE - 1) % HIS_SIZE;
    while (cli_getchar(&c) == 1) {
        if (c == RET_CHAR || c == END_CHAR) {   /* end of input line */
            inbuf[*bp] = '\0';
            *bp = 0;
            return 1;
        }

        if (c == 0x1b) { /* escape sequence */
            esc = 1;
            key1 = -1;
            key2 = -1;
            continue;
        }

        if (esc) {
            if (key1 < 0) {
                key1 = c;
                if (key1 != 0x5b) {
                    /* not '[' */
                    inbuf[(*bp)] = 0x1b;
                    (*bp) ++;
                    inbuf[*bp] = key1;
                    (*bp) ++;
                    if (!cli->echo_disabled) {
                        csp_printf("\x1b%c", key1);
                        fflush(stdout);
                    }
                    esc = 0; /* quit escape sequence */
                }
                continue;
            }

            if (key2 < 0) {
                key2 = c;
                if (key2 == 't') {
                    esc_tag[0] = 0x1b;
                    esc_tag[1] = key1;
                    esc_tag_len = 2;
                }
            }

            if (key2 != 0x41 && key2 != 0x42 && key2 != 't') {
                /*unsupported esc sequence*/
                inbuf[(*bp)] = 0x1b;
                (*bp) ++;
                inbuf[*bp] = key1;
                (*bp) ++;
                inbuf[*bp] = key2;
                (*bp) ++;
                if (!cli->echo_disabled) {
                    csp_printf("\x1b%c%c", key1, key2);
                    fflush(stdout);
                }
                esc_tag[0] = '\x0';
                esc_tag_len = 0;
                esc = 0; /* quit escape sequence */
                continue;
            }

            if (key2 == 0x41) { /* UP */
                char *cmd = cli->history[cli->his_idx];
                cli->his_idx = (cli->his_idx + HIS_SIZE - 1) % HIS_SIZE;
                strncpy(inbuf, cmd, INBUF_SIZE);
                csp_printf("\r\n" PROMPT "%s", inbuf);
                *bp = strlen(inbuf);
                esc_tag[0] = '\x0';
                esc_tag_len = 0;
                esc = 0; /* quit escape sequence */
                continue;
            }

            if (key2 == 0x42) { /* DOWN */
                char *cmd = cli->history[cli->his_idx];
                cli->his_idx = (cli->his_idx + 1) % HIS_SIZE;
                strncpy(inbuf, cmd, INBUF_SIZE);
                csp_printf("\r\n" PROMPT "%s", inbuf);
                *bp = strlen(inbuf);
                esc_tag[0] = '\x0';
                esc_tag_len = 0;
                esc = 0; /* quit escape sequence */
                continue;
            }


            /* ESC_TAG */
            if (esc_tag_len >= sizeof(esc_tag)) {
                esc_tag[0] = '\x0';
                esc_tag_len = 0;
                esc = 0; /* quit escape sequence */
                csp_printf("Error: esc_tag buffer overflow\r\n");
                fflush(stdout);
                continue;
            }
            esc_tag[esc_tag_len++] = c;
            if (c == 'm') {
                esc_tag[esc_tag_len++] = '\x0';
                if (!cli->echo_disabled) {
                    csp_printf("%s", esc_tag);
                    fflush(stdout);
                }
                esc = 0; /* quit escape sequence */
            }
            continue;
        }

        inbuf[*bp] = c;
        if ((c == 0x08) || /* backspace */
            (c == 0x7f)) { /* DEL */
            if (*bp > 0) {
                (*bp)--;
                if (!cli->echo_disabled) {
                    csp_printf("%c %c", 0x08, 0x08);
                    fflush(stdout);
                }
            }
            continue;
        }

        if (c == '\t') {
            inbuf[*bp] = '\0';
            tab_complete(inbuf, bp);
            continue;
        }

        if (!cli->echo_disabled) {
            csp_printf("%c", c);
            fflush(stdout);
        }

        (*bp)++;
        if (*bp >= INBUF_SIZE) {
            aos_cli_printf("Error: input buffer overflow\r\n");
            aos_cli_printf(PROMPT);
            *bp = 0;
            return 0;
        }
    }

    return 0;
}

/* Print out a bad command string, including a hex
 * representation of non-printable characters.
 * Non-printable characters show as "\0xXX".
 */
static void print_bad_command(char *cmd_string)
{
    if (cmd_string != NULL) {
        aos_cli_printf("command '%s' not found\r\n", cmd_string);
    }
}

/* Main CLI processing thread
 *
 * Waits to receive a command buffer pointer from an input collector, and
 * then processes.  Note that it must cleanup the buffer when done with it.
 *
 * Input collectors handle their own lexical analysis and must pass complete
 * command lines to CLI.
 */
static void cli_main(void *data)
{
    while (!cliexit) {
        int ret;
        char *msg = NULL;

        if (get_input(cli->inbuf, &cli->bp)) {
            msg = cli->inbuf;
#if 0
            if (strcmp(msg, EXIT_MSG) == 0) {
                break;
            }
#endif
            if (strlen(cli->inbuf) > 0) {
                strncpy(cli->history[cli->his_cur], cli->inbuf, INBUF_SIZE);
                cli->his_cur = (cli->his_cur + 1) % HIS_SIZE;
            }

            ret = handle_input(msg);
            if (ret == 1) {
                print_bad_command(msg);
            } else if (ret == 2) {
                aos_cli_printf("syntax error\r\n");
            }

            aos_cli_printf("\r\n");
            esc_tag[0] = '\x0';
            esc_tag_len = 0;
            aos_cli_printf(PROMPT);
        }
    }

    aos_cli_printf("CLI exited\r\n");
    aos_free(cli);
    cli = NULL;

    aos_task_exit(0);
}

static void help_cmd(char *buf, int len, int argc, char **argv);
static void version_cmd(char *buf, int len, int argc, char **argv);
static void echo_cmd(char *buf, int len, int argc, char **argv);
static void exit_cmd(char *buf, int len, int argc, char **argv);
static void devname_cmd(char *buf, int len, int argc, char **argv);
static void reboot_cmd(char *buf, int len, int argc, char **argv);
static void uptime_cmd(char *buf, int len, int argc, char **argv);
static void ota_cmd(char *buf, int len, int argc, char **argv);

static const struct cli_command built_ins[] = {
    /*cli self*/
    {"help",        NULL,       help_cmd},
    {"echo",        NULL,       echo_cmd},
    {"exit",        "CLI exit", exit_cmd},
    {"devname",     "print device name", devname_cmd},

    /*rhino*/
    {"sysver",      NULL,       version_cmd},
    {"reboot",      "reboot system",     reboot_cmd},

    /*aos_rhino*/
    {"time",        "system time",       uptime_cmd},
    {"ota",         "system ota",        ota_cmd},

};

/* Built-in "help" command: prints all registered commands and their help
 * text string, if any.
 */
static void help_cmd(char *buf, int len, int argc, char **argv)
{
    int i, n;
    uint32_t build_in_count = sizeof(built_ins) / sizeof(struct cli_command);

#if (DEBUG)
    build_in_count++;
#endif

    aos_cli_printf( "====Build-in Commands====\r\n" );
    aos_cli_printf( "====Support six cmds once, seperate by ; ====\r\n" );

    for (i = 0, n = 0; i < MAX_COMMANDS && n < cli->num_commands; i++) {
        if (cli->commands[i]->name) {
            aos_cli_printf("%s: %s\r\n", cli->commands[i]->name,
                           cli->commands[i]->help ?
                           cli->commands[i]->help : "");
            n++;
            if ( n == build_in_count ) {
                aos_cli_printf("\r\n");
                aos_cli_printf("====User Commands====\r\n");
            }
        }
    }

}


static void version_cmd(char *buf, int len, int argc, char **argv)
{
#ifdef VCALL_RHINO
    aos_cli_printf("kernel version :%d\r\n", krhino_version_get());
#else
    aos_cli_printf("kernel version :posix\r\n");
#endif
}

static void echo_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc == 1) {
        aos_cli_printf("Usage: echo on/off. Echo is currently %s\r\n",
                       cli->echo_disabled ? "Disabled" : "Enabled");
        return;
    }

    if (!strcmp(argv[1], "on")) {
        aos_cli_printf("Enable echo\r\n");
        cli->echo_disabled = 0;
    } else if (!strcmp(argv[1], "off")) {
        aos_cli_printf("Disable echo\r\n");
        cli->echo_disabled = 1;
    }
}

static void exit_cmd(char *buf, int len, int argc, char **argv)
{
    cliexit = 1;
    return;
}

static void devname_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("device name: %s\r\n", SYSINFO_DEVICE_NAME);
}


static void reboot_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("reboot\r\n");

    hal_reboot();
}

static void uptime_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("UP time %ld ms\r\n", (long)aos_now_ms());
}

void tftp_ota_thread(void *arg)
{
    aos_task_exit(0);
}

static void ota_cmd(char *buf, int len, int argc, char **argv)
{
    aos_task_new("LOCAL OTA", tftp_ota_thread, 0, 4096);
}

/* ------------------------------------------------------------------------- */

int aos_cli_register_command(const struct cli_command *cmd)
{
    int i;

    if (!cli) {
        return 1;
    }

    if (!cmd->name || !cmd->function) {
        return -EINVAL;
    }

    if (cli->num_commands < MAX_COMMANDS) {
        /* Check if the command has already been registered.
         * Return 0, if it has been registered.
         */
        for (i = 0; i < cli->num_commands; i++) {
            if (cli->commands[i] == cmd) {
                return 0;
            }
        }
        cli->commands[cli->num_commands++] = cmd;
        return 0;
    }

    return -ENOMEM;
}
AOS_EXPORT(int, aos_cli_register_command, const struct cli_command *);

int aos_cli_unregister_command(const struct cli_command *cmd)
{
    int i;
    if (!cmd->name || !cmd->function) {
        return -EINVAL;
    }

    for (i = 0; i < cli->num_commands; i++) {
        if (cli->commands[i] == cmd) {
            cli->num_commands--;
            int remaining_cmds = cli->num_commands - i;
            if (remaining_cmds > 0) {
                memmove(&cli->commands[i], &cli->commands[i + 1],
                        (remaining_cmds * sizeof(struct cli_command *)));
            }
            cli->commands[cli->num_commands] = NULL;
            return 0;
        }
    }

    return -ENOMEM;
}
AOS_EXPORT(int, aos_cli_unregister_command, const struct cli_command *);

int aos_cli_register_commands(const struct cli_command *cmds, int num_cmds)
{
    int i;
    int err;
    if (!cli) {
        return 1;
    }
    for (i = 0; i < num_cmds; i++) {
        if ((err = aos_cli_register_command(cmds++)) != 0) {
            return err;
        }
    }

    return 0;
}
AOS_EXPORT(int, aos_cli_register_commands, const struct cli_command *, int);

int aos_cli_unregister_commands(const struct cli_command *cmds, int num_cmds)
{
    int i;
    int err;
    for (i = 0; i < num_cmds; i++) {
        if ((err = aos_cli_unregister_command(cmds++)) != 0) {
            return err;
        }
    }

    return 0;
}
AOS_EXPORT(int, aos_cli_unregister_commands, const struct cli_command *, int);

int aos_cli_stop(void)
{
    cliexit = 1;

    return 0;
}
AOS_EXPORT(int, aos_cli_stop, void);

#ifndef CONFIG_AOS_CLI_STACK_SIZE
#define CONFIG_AOS_CLI_STACK_SIZE 2048
#endif

int aos_cli_init(void)
{
    int ret;
    aos_task_t task;

    cli = (struct cli_st *)aos_malloc(sizeof(struct cli_st));
    if (cli == NULL) {
        return -ENOMEM;
    }

    memset((void *)cli, 0, sizeof(struct cli_st));

    /* add our built-in commands */
    if ((ret = aos_cli_register_commands(&built_ins[0],
                                         sizeof(built_ins) / sizeof(struct cli_command))) != 0) {
        goto init_general_err;
    }

    ret = aos_task_new_ext(&task, "cli", cli_main, 0, CONFIG_AOS_CLI_STACK_SIZE, AOS_DEFAULT_APP_PRI + 1);
    if (ret != 0) {
        aos_cli_printf("Error: Failed to create cli thread: %d\r\n",
                       ret);
        goto init_general_err;
    }

    cli->initialized = 1;
    cli->echo_disabled = 0;

#ifdef CONFIG_AOS_CLI_BOARD
    board_cli_init();
#endif

    log_cli_init();

    return 0;

init_general_err:
    if (cli) {
        aos_free(cli);
        cli = NULL;
    }

    return ret;
}
AOS_EXPORT(int, aos_cli_init, void);

const char *aos_cli_get_tag(void)
{
    return esc_tag;
}
AOS_EXPORT(const char *, aos_cli_get_tag, void);

#if defined BUILD_BIN || defined BUILD_KERNEL
int aos_cli_printf(const char *msg, ...)
{
    va_list ap;

    char *pos, message[256];
    int sz;
    int len;

    memset(message, 0, 256);

    sz = 0;
    if (esc_tag_len) {
        strcpy(message, esc_tag);
        sz = strlen(esc_tag);
    }
    pos = message + sz;

    va_start(ap, msg);
    len = vsnprintf(pos, 256 - sz, msg, ap);
    va_end(ap);

    if (len <= 0) {
        return 0;
    }

    cli_putstr(message);

    return 0;
}
#endif

int cli_putstr(char *msg)
{
    if (msg[0] != 0) {
        aos_uart_send(msg, strlen(msg), 0);
    }

    return 0;
}

int cli_getchar(char *inbuf)
{
    if (aos_uart_recv(inbuf, 1, NULL, 0xFFFFFFFF) == 0) {
        return 1;
    } else {
        return 0;
    }
}

