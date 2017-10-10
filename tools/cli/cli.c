/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <aos/aos.h>
#include "hal/soc/soc.h"
#include "dumpsys.h"

#ifndef STDIO_UART
#define STDIO_UART 0
#endif

#define RET_CHAR  '\n'
#define END_CHAR  '\r'
#define PROMPT    "\r\n# "
#define EXIT_MSG  "exit"
#define CLI_TAG   "\e[63m"  /* CLI TAG, use ESC characters, c(cli) ascii is 63 */

#ifdef CONFIG_NET_LWIP
#include "lwip/ip_addr.h"
#include "lwip/apps/tftp.h"
#endif /* CONFIG_NET_LWIP */

static struct cli_st *cli = NULL;
static int            cliexit = 0;
extern uart_dev_t     uart_0;
extern void hal_reboot(void);

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

/*
 * This function is used to locate the command prefix (if any)
 * incorperated inside '<' and '>' paire. The leading spaces will be
 * skipped (if any) first. the prefix begins on '<' and ends on '>'.
 * the prefix length is calculated from '<' to '>'. If '<' and '>'
 * are not paired, error will be reported.
 *
 * @param inbuf [in] - the original command string.
 *        prefix [in|out] - holder of the thead of command prefix.
 *        len [in|out] - holder of the prefix length.
 * @return -1 - error.
 *          0 - no prefix found.
 *         a number>0 - the skipped length, including leading spaces
 *                      and prefix string.
 *
 * Example valid inputs:
 *   1. "help"
 *   2. " help "
 *   3. "<123>help"
 *   4. " <12 3> help "
 */
static int check_cmd_prefix(const char *inbuf, char **prefix, int *len)
{
    char *p = (char *)inbuf;
    int ret = 0;

    if (!inbuf || !prefix || !len) {
        return -1;
    }

    while (*p == ' ') {
        p++;
        ret++;
    }

    if (*p != '<') {
        return 0;
    }

    *prefix = p; /* prefix start with '<' */
    while (*p != '>' && *p != '\0') {
        *len += 1;
        p++;
        ret++;
    }

    if (*p == '\0') {
        return -1; /* no tailing '>' found */
    }

    *len += 1; /* prefix on with '>' */
    ret++;

    return ret;
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
    static char *argv[16];
    int argc = 0;
    int i = 0;
    const struct cli_command *command = NULL;
    const char *p;
    char *cmd_prefix = NULL;
    int cmd_prefix_len = 0, skipped_len = 0;

    memset((void *)&argv, 0, sizeof(argv));
    memset(&stat, 0, sizeof(stat));

    /* Added for testbed cli, no impact on normal cli <beginning> */
    skipped_len = check_cmd_prefix(inbuf, &cmd_prefix, &cmd_prefix_len);
    if (skipped_len < 0) {
        aos_cli_printf("\r\ncheck_cmd_prefix func failed\r\n");
        return 2;
    }

    if (cmd_prefix_len > 0) {
        inbuf += skipped_len;
    }

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
                    argc++;
                    argv[argc - 1] = &inbuf[i + 1];
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

            default:
                if (!stat.inArg) {
                    stat.inArg = 1;
                    argc++;
                    argv[argc - 1] = &inbuf[i];
                }
                break;
        }
    } while (!stat.done && ++i < INBUF_SIZE);

    if (stat.inQuote) {
        return 2;
    }

    if (argc < 1) {
        return 0;
    }

    if (!cli->echo_disabled) {
        printf("\r\n");
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
    cli_putstr("\r\n");

    /* Added for testbed cli, no impact on normal cli <beginning> */
    if (cmd_prefix_len > 0) {
        *(cmd_prefix + cmd_prefix_len) = '\0'; /* safe to operate in place now */
        aos_cli_printf(cmd_prefix);
        aos_cli_printf("\r\n");
    }
    /* testbed cli <end> */

    command->function(cli->outbuf, OUTBUF_SIZE, argc, argv);
    cli_putstr(cli->outbuf);

    /* Added for testbed cli, no impact on normal cli <beginning> */
    if (cmd_prefix_len > 0) {
        /* Wait for output from special commands: umesh ping/autotest */
        if ((strcmp(command->name, "umesh") == 0) &&
            ((strcmp(argv[1], "ping") == 0) ||
             (strcmp(argv[1], "autotest") == 0))) {
            aos_msleep(500);
        }

        aos_cli_printf(cmd_prefix);
        aos_cli_printf("\r\n");
    }
    /* testbed cli <end> */

    return 0;
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
    if (inbuf == NULL) {
        aos_cli_printf("inbuf_null\r\n");
        return 0;
    }

    while (cli_getchar(&inbuf[*bp]) == 1) {
        if (inbuf[*bp] == RET_CHAR) {
            continue;
        }
        if (inbuf[*bp] == END_CHAR) {   /* end of input line */
            inbuf[*bp] = '\0';
            *bp = 0;
            return 1;
        }

        if ((inbuf[*bp] == 0x08) || /* backspace */
            (inbuf[*bp] == 0x7f)) { /* DEL */
            if (*bp > 0) {
                (*bp)--;
                if (!cli->echo_disabled) {
                    printf("%c %c", 0x08, 0x08);
                    fflush(stdout);
                }
            }
            continue;
        }

        if (inbuf[*bp] == '\t') {
            inbuf[*bp] = '\0';
            tab_complete(inbuf, bp);
            continue;
        }

        if (!cli->echo_disabled) {
            printf("%c", inbuf[*bp]);
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
        char *c = cmd_string;
        aos_cli_printf("command '");
        while (*c != '\0') {
            if ((*c) >= 0x20 && (*c) <= 0x7f) {
                aos_cli_printf("%c", *c);
            } else {
                aos_cli_printf("\\0x%x", *c);
            }
            ++c;
        }
        aos_cli_printf("' not found\r\n");
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
            ret = handle_input(msg);
            if (ret == 1) {
                print_bad_command(msg);
            } else if (ret == 2) {
                aos_cli_printf("syntax error\r\n");
            }

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
static void task_cmd(char *buf, int len, int argc, char **argv);
static void devname_cmd(char *buf, int len, int argc, char **argv);
static void dumpsys_cmd(char *buf, int len, int argc, char **argv);
static void reboot_cmd(char *buf, int len, int argc, char **argv);
static void uptime_cmd(char *buf, int len, int argc, char **argv);
static void ota_cmd(char *buf, int len, int argc, char **argv);
static void wifi_debug_cmd(char *buf, int len, int argc, char **argv);
#ifdef CONFIG_NET_LWIP
static void tftp_cmd(char *buf, int len, int argc, char **argv);
#endif /* CONFIG_NET_LWIP */
static void udp_cmd(char *buf, int len, int argc, char **argv);

static const struct cli_command built_ins[] = {
    {"help",        NULL,       help_cmd},
    {"sysver",      NULL,       version_cmd},
    {"echo",        NULL,       echo_cmd},
    {"exit",        "CLI exit", exit_cmd},

    /* os */
    {"tasklist",    "list all thread info", task_cmd},

    /* net */
#ifndef CONFIG_NO_TCPIP
#ifdef CONFIG_NET_LWIP
    {"tftp",        "tftp server/client control", tftp_cmd},
#endif /* CONFIG_NET_LWIP */
    {"udp",         "[ip] [port] [string data] send udp data", udp_cmd},
#endif

    /* others */
    {"devname",     "print device name", devname_cmd},
    {"dumpsys",     "dump system info",  dumpsys_cmd},
    {"reboot",      "reboot system",     reboot_cmd},
    {"time",        "system time",       uptime_cmd},
    {"ota",         "system ota",        ota_cmd},
    {"wifi_debug",  "wifi debug mode",   wifi_debug_cmd},
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
    for (i = 0, n = 0; i < MAX_COMMANDS && n < cli->num_commands; i++) {
        if (cli->commands[i]->name) {
            aos_cli_printf("%s: %s\r\n", cli->commands[i]->name,
                           cli->commands[i]->help ?
                           cli->commands[i]->help : "");
            n++;
            if ( n == build_in_count - 1 ) {
                aos_cli_printf("\r\n");
                aos_cli_printf("====User Commands====\r\n");
            }
        }
    }
}


static void version_cmd(char *buf, int len, int argc, char **argv)
{
#ifdef VCALL_RHINO
    aos_cli_printf("kernel version :%s\r\n", krhino_version_get());
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

#ifndef CONFIG_NO_TCPIP
#include <aos/network.h>
static void udp_cmd(char *buf, int len, int argc, char **argv)
{
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(argv[2]));
    saddr.sin_addr.s_addr = inet_addr(argv[1]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        aos_cli_printf("error creating socket!\n");
        return;
    }

    int ret = sendto(sockfd, argv[3], strlen(argv[3]), 0,
                     (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        aos_cli_printf("error send data %d!\n", ret);
    }

    close(sockfd);
}
#endif

static void task_cmd(char *buf, int len, int argc, char **argv)
{
    dumpsys_task_func(NULL, 0, 1);
}

static void devname_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("%s\r\n", SYSINFO_DEVICE_NAME);
}

static void dumpsys_cmd(char *buf, int len, int argc, char **argv)
{
#ifdef VCALL_RHINO
    dumpsys_func(buf, len, argc, argv);
#endif
}

#ifdef CONFIG_NET_LWIP
static void tftp_get_done(int error, int len)
{
    if (error == 0) {
        aos_cli_printf("tftp client get succeed\r\n", len);
    } else {
        aos_cli_printf("tftp client get failed\r\n");
    }
}

extern tftp_context_t client_ctx;
extern tftp_context_t ota_ctx;
void ota_get_done(int error, int len);
static void tftp_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc < 3) {
        goto tftp_print_usage;
    }

    if (strncmp(argv[1], "server", 6) == 0) {
        if (strncmp(argv[2], "start", 5) == 0) {
            err_t err = tftp_server_start();
            aos_cli_printf("tftp start server %s\r\n", err == ERR_OK ? "done" : "failed");
            return;
        } else if (strncmp(argv[2], "stop", 4) == 0) {
            tftp_server_stop();
            aos_cli_printf("tftp stop server done\r\n");
            return;
        }
        goto tftp_print_usage;
    } else if (strncmp(argv[1], "get", 3) == 0) {
        ip_addr_t dst_addr;
        ipaddr_aton(argc == 4 ? argv[2] : "10.0.0.2", &dst_addr);
        tftp_client_get(&dst_addr, argv[argc - 1], &client_ctx, tftp_get_done);
        return;
    } else if (strncmp(argv[1], "ota", 3) == 0) {
        ip_addr_t dst_addr;
        uint8_t   gw_ip[4] = {10, 0 , 0, 2};
        memcpy(&dst_addr, gw_ip, 4);
        tftp_client_get(&dst_addr, argv[2], &ota_ctx, ota_get_done);
        return;
    }

tftp_print_usage:
    aos_cli_printf("Usage: tftp server start/stop\r\n");
    aos_cli_printf("       tftp get path/to/file\r\n");
}
#endif /* CONFIG_NET_LWIP */

static void reboot_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("reboot\r\n");

    hal_reboot();
}

static void uptime_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("UP time %ldms\r\n", aos_now_ms());
}

void tftp_ota_thread(void *arg)
{
    aos_task_exit(0);
}

static void ota_cmd(char *buf, int len, int argc, char **argv)
{
    aos_task_new("LOCAL OTA", tftp_ota_thread, 0, 4096);
}

static void wifi_debug_cmd(char *buf, int len, int argc, char **argv)
{
    hal_wifi_start_debug_mode();
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

int aos_cli_register_commands(const struct cli_command *cmds, int num_cmds)
{
    int i;
    int err;
    for (i = 0; i < num_cmds; i++) {
        if ((err = aos_cli_register_command(cmds++)) != 0) {
            return err;
        }
    }

    return 0;
}

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

__attribute__ ((weak)) int board_cli_init(void)
{
    return 0;
}

int aos_cli_stop(void)
{
    cliexit = 1;

    return 0;
}

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

    ret = aos_task_new_ext(&task, "cli", cli_main, 0, 4096, AOS_DEFAULT_APP_PRI);
    if (ret != 0) {
        aos_cli_printf("Error: Failed to create cli thread: %d\r\n",
                       ret);
        goto init_general_err;
    }

    cli->initialized = 1;
    cli->echo_disabled = 0;

    board_cli_init();

    return 0;

init_general_err:
    if (cli) {
        aos_free(cli);
        cli = NULL;
    }

    return ret;
}

int aos_cli_printf(const char *msg, ...)
{
    va_list ap;

    char *pos, message[256];
    int sz;
    int len;

    memset(message, 0, 256);

    strcpy(message, CLI_TAG);
    sz = strlen(CLI_TAG);
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

int cli_putstr(char *msg)
{
    if (msg[0] != 0) {
        hal_uart_send(&uart_0, msg, strlen(msg), 0);
    }

    return 0;
}

int cli_getchar(char *inbuf)
{
    if (hal_uart_recv(&uart_0, inbuf, 1, NULL, 0xFFFFFFFF) == 0) {
        return 1;
    } else {
        return 0;
    }
}

