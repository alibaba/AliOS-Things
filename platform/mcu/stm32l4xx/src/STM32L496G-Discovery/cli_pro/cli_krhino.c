/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <aos/aos.h>
#include <aos/network.h>
#include "hal/soc/soc.h"
#include "hal/wifi.h"
#include "dumpsys.h"

#ifndef STDIO_UART
#define STDIO_UART 0
#endif

#define RET_CHAR  '\n'
#define END_CHAR  '\r'
#define PROMPT    "# "
#define EXIT_MSG  "exit"

#ifdef CONFIG_NET_LWIP
#include "lwip/ip_addr.h"
#include "lwip/apps/tftp.h"
#endif /* CONFIG_NET_LWIP */

static struct cli_st *cli = NULL;
static int            cliexit = 0;
char                  esc_tag[64] = {0};
static uint8_t        esc_tag_len = 0;
extern uart_dev_t     uart_0;
extern void hal_reboot(void);

#ifdef CONFIG_AOS_CLI_BOARD
extern int board_cli_init(void);
#endif

#ifdef VCALL_RHINO
extern uint32_t krhino_version_get(void);
#endif

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

    memset((void *)&argv, 0, sizeof(argv));
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
void cli_main(void *data)
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
static void task_cmd(char *buf, int len, int argc, char **argv);
static void devname_cmd(char *buf, int len, int argc, char **argv);
static void dumpsys_cmd(char *buf, int len, int argc, char **argv);
static void reboot_cmd(char *buf, int len, int argc, char **argv);
static void uptime_cmd(char *buf, int len, int argc, char **argv);
static void ota_cmd(char *buf, int len, int argc, char **argv);
static void wifi_debug_cmd(char *buf, int len, int argc, char **argv);
#ifndef CONFIG_NO_TCPIP
#ifdef CONFIG_NET_LWIP
static void tftp_cmd(char *buf, int len, int argc, char **argv);
#endif /* CONFIG_NET_LWIP */
static void udp_cmd(char *buf, int len, int argc, char **argv);
#endif
static void log_cmd(char *buf, int len, int argc, char **argv);
static void mac_cmd(char *buf, int len, int argc, char **argv);

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
    {"loglevel",    "set log level",     log_cmd},
    {"mac",         "get/set mac",       mac_cmd},
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

static void log_cmd(char *buf, int len, int argc, char **argv)
{
    const char *lvls[] = {
        [AOS_LL_FATAL] = "fatal",
        [AOS_LL_ERROR] = "error",
        [AOS_LL_WARN]  = "warn",
        [AOS_LL_INFO]  = "info",
        [AOS_LL_DEBUG] = "debug",
    };

    if (argc < 2) {
        aos_cli_printf("log level : %02x\r\n", aos_get_log_level());
        return;
    }

    int i;
    for (i=0;i<sizeof(lvls)/sizeof(lvls[0]);i++) {
        if (strncmp(lvls[i], argv[1], strlen(lvls[i])+1) != 0)
            continue;

        aos_set_log_level((aos_log_level_t)i);
        aos_cli_printf("set log level success\r\n");
        return;
    }
    aos_cli_printf("set log level fail\r\n");
}
/*
static uint8_t hex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return 0;
}

static void hexstr2bin(const char *macstr, uint8_t *mac, int len)
{
    int i;
    for (i=0;i < len && macstr[2 * i];i++) {
        mac[i] = hex(macstr[2 * i]) << 4;
        mac[i] |= hex(macstr[2 * i + 1]);
    }
}*/

static void mac_cmd(char *buf, int len, int argc, char **argv)
{
/*
    uint8_t mac[6];

    if (argc == 1)
    {
        hal_wifi_get_mac_addr(NULL, mac);
        aos_cli_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else if(argc == 2)
    {
        hexstr2bin(argv[1], mac, 6);
        hal_wifi_set_mac_addr(NULL, mac);
        aos_cli_printf("Set MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else
    {
        aos_cli_printf("invalid cmd\r\n");
    }*/
}

static void task_cmd(char *buf, int len, int argc, char **argv)
{
    dumpsys_task_func(NULL, 0, 1);
}

static void devname_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("device name: %s\r\n", SYSINFO_DEVICE_NAME);
}

static void dumpsys_cmd(char *buf, int len, int argc, char **argv)
{
#ifdef VCALL_RHINO
    dumpsys_func(buf, len, argc, argv);
#endif
}

#ifndef CONFIG_NO_TCPIP
static void udp_cmd(char *buf, int len, int argc, char **argv)
{
    struct sockaddr_in saddr;

    if (argc < 4) {
        return;
    }

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
#endif

static void reboot_cmd(char *buf, int len, int argc, char **argv)
{
    aos_cli_printf("reboot\r\n");
/*
    hal_reboot();*/
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

static void wifi_debug_cmd(char *buf, int len, int argc, char **argv)
{
   /* hal_wifi_start_debug_mode(NULL);*/
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

#ifdef CONFIG_AOS_CLI_BOARD
    board_cli_init();
#endif

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
        hal_uart_send(&uart_0, msg, strlen(msg), 0);
    }

    return 0;
}

int cli_getchar(char *inbuf)
{
    int ret = 0;

    ret = hal_uart_recv_II(&uart_0, inbuf, 1, NULL, HAL_WAIT_FOREVER);
    if (ret == 0) {
        return 1;
    } else {
        return 0;
    }
}

