/**
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 BEKEN Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of BEKEN Corporation.
 *
 */
#include "sys_rtos.h"
#include "rtos_pub.h"
#include "error.h"

#include "wlan_cli_pub.h"
#include "stdarg.h"

#include "include.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "uart_pub.h"
#include "BkDriverFlash.h"
#include "wlan_ui_pub.h"
#include "param_config.h"
#include "gpio_pub.h"
#include "sys_ctrl_pub.h"
#include "icu_pub.h"
#include "power_save_pub.h"
#include "cmd_rx_sensitivity.h"
#include "cmd_evm.h"
#include "BkDriverGpio.h"
#include "ieee802_11_demo.h"
#include "demos_start.h"
#include "command_line.h"
#include "role_launch.h"
#include "wdt_pub.h"
#include "saradc_pub.h"
#include "bk7011_cal_pub.h"
#include "flash_pub.h"
#include "mcu_ps_pub.h"
#include "manual_ps_pub.h"

#if CFG_SUPPORT_BOOTLOADER
#include "wdt_pub.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
#include "security_pub.h"
extern void sec_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif

#include "temp_detect_pub.h"
#include "utils_httpc.h"

#ifndef MOC
static void task_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv );
#endif

int mico_debug_enabled;
char log_buf[LOG_SERVICE_BUFLEN];
static struct cli_st *pCli = NULL;
beken_semaphore_t log_rx_interrupt_sema = NULL;

extern u8* wpas_get_sta_psk(void);
extern int cli_putstr(const char *msg);
static void bkreg_cmd_handle_input(char *inbuf, int len);
extern int hexstr2bin(const char *hex, u8 *buf, size_t len);
extern void make_tcp_server_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
extern int httpclient_common(httpclient_t *client,
                             const char *url,
                             int port,
                             const char *ca_crt,
                             int method,
                             uint32_t timeout_ms,
                             httpclient_data_t *client_data);

u32 airkiss_process(u8 start);

#if CFG_SARADC_CALIBRATE
static void adc_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif 

static void cli_rx_callback(int uport, void *param);

static void efuse_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void efuse_mac_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

#if CFG_SUPPORT_BKREG
#define BKREG_MAGIC_WORD0                 (0x01)
#define BKREG_MAGIC_WORD1                 (0xE0)
#define BKREG_MAGIC_WORD2                 (0xFC)
#define BKREG_MIN_LEN                     3
#endif

/* Find the command 'name' in the cli commands table.
* If len is 0 then full match will be performed else upto len bytes.
* Returns: a pointer to the corresponding cli_command struct or NULL.
*/
static const struct cli_command *lookup_command(char *name, int len)
{
    int i = 0;
    int n = 0;

    while (i < MAX_COMMANDS && n < pCli->num_commands)
    {
        if (pCli->commands[i]->name == NULL)
        {
            i++;
            continue;
        }
        /* See if partial or full match is expected */
        if (len != 0)
        {
            if (!os_strncmp(pCli->commands[i]->name, name, len))
                return pCli->commands[i];
        }
        else
        {
            if (!os_strcmp(pCli->commands[i]->name, name))
                return pCli->commands[i];
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
    struct
    {
        unsigned inArg: 1;
        unsigned inQuote: 1;
        unsigned done: 1;
    } stat;
    static char *argv[16];
    int argc = 0;
    int i = 0;
    const struct cli_command *command = NULL;
    const char *p;

    os_memset((void *)&argv, 0, sizeof(argv));
    os_memset(&stat, 0, sizeof(stat));

    do
    {
        switch (inbuf[i])
        {
        case '\0':
            if (stat.inQuote)
                return 2;
            stat.done = 1;
            break;

        case '"':
            if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg)
            {
                os_memcpy(&inbuf[i - 1], &inbuf[i],
                          os_strlen(&inbuf[i]) + 1);
                --i;
                break;
            }
            if (!stat.inQuote && stat.inArg)
                break;
            if (stat.inQuote && !stat.inArg)
                return 2;

            if (!stat.inQuote && !stat.inArg)
            {
                stat.inArg = 1;
                stat.inQuote = 1;
                argc++;
                argv[argc - 1] = &inbuf[i + 1];
            }
            else if (stat.inQuote && stat.inArg)
            {
                stat.inArg = 0;
                stat.inQuote = 0;
                inbuf[i] = '\0';
            }
            break;

        case ' ':
            if (i > 0 && inbuf[i - 1] == '\\' && stat.inArg)
            {
                os_memcpy(&inbuf[i - 1], &inbuf[i],
                          os_strlen(&inbuf[i]) + 1);
                --i;
                break;
            }
            if (!stat.inQuote && stat.inArg)
            {
                stat.inArg = 0;
                inbuf[i] = '\0';
            }
            break;

        default:
            if (!stat.inArg)
            {
                stat.inArg = 1;
                argc++;
                argv[argc - 1] = &inbuf[i];
            }
            break;
        }
    }
    while (!stat.done && ++i < INBUF_SIZE);

    if (stat.inQuote)
        return 2;

    if (argc < 1)
        return 0;

    if (!pCli->echo_disabled)
        os_printf("\r\n");

    /*
    * Some comamands can allow extensions like foo.a, foo.b and hence
    * compare commands before first dot.
    */
    i = ((p = os_strchr(argv[0], '.')) == NULL) ? 0 :
        (p - argv[0]);
    command = lookup_command(argv[0], i);
    if (command == NULL)
        return 1;

    os_memset(pCli->outbuf, 0, OUTBUF_SIZE);
    cli_putstr("\r\n");
	
    #if CFG_USE_STA_PS
    /*if cmd,exit dtim ps*/
    if (os_strncmp(command->name, "ps", 2))
    {
    }
    #endif

    command->function(pCli->outbuf, OUTBUF_SIZE, argc, argv);
    cli_putstr(pCli->outbuf);
    return 0;
}

/* Perform basic tab-completion on the input buffer by string-matching the
* current input line against the cli functions table.  The current input line
* is assumed to be NULL-terminated. */
static void tab_complete(char *inbuf, unsigned int *bp)
{
    int i, n, m;
    const char *fm = NULL;

    os_printf("\r\n");

    /* show matching commands */
    for (i = 0, n = 0, m = 0; i < MAX_COMMANDS && n < pCli->num_commands;
            i++)
    {
        if (pCli->commands[i]->name != NULL)
        {
            if (!os_strncmp(inbuf, pCli->commands[i]->name, *bp))
            {
                m++;
                if (m == 1)
                    fm = pCli->commands[i]->name;
                else if (m == 2)
                    os_printf("%s %s ", fm,
                              pCli->commands[i]->name);
                else
                    os_printf("%s ",
                              pCli->commands[i]->name);
            }
            n++;
        }
    }

    /* there's only one match, so complete the line */
    if (m == 1 && fm)
    {
        n = os_strlen(fm) - *bp;
        if (*bp + n < INBUF_SIZE)
        {
            os_memcpy(inbuf + *bp, fm + *bp, n);
            *bp += n;
            inbuf[(*bp)++] = ' ';
            inbuf[*bp] = '\0';
        }
    }

    /* just redraw input line */
    os_printf("%s%s", PROMPT, inbuf);
}

/* Get an input line.
*
* Returns: 1 if there is input, 0 if the line should be ignored. */
static int get_input(char *inbuf, unsigned int *bp)
{
    if (inbuf == NULL)
    {
        os_printf("inbuf_null\r\n");
        return 0;
    }

    while (cli_getchar(&inbuf[*bp]) == 1)
    {
#if CFG_SUPPORT_BKREG
        if((0x01U == (UINT8)inbuf[*bp]) && (*bp == 0)) {
            (*bp)++;
            continue;
        } else if((0xe0U == (UINT8)inbuf[*bp]) && (*bp == 1)) {
            (*bp)++;
            continue;
        } else if((0xfcU == (UINT8)inbuf[*bp]) && (*bp == 2)) {
            (*bp)++;
            continue;
        } else {
            if((0x01U == (UINT8)inbuf[0])
                && (0xe0U == (UINT8)inbuf[1])
                && (0xfcU == (UINT8)inbuf[2])
                && (*bp == 3))
            {
                char ch = inbuf[*bp];
                int left = (int)ch, len = 4 + (int)ch;
                inbuf[*bp] = ch;
                (*bp)++;
                
                while(left--) {
                    cli_getchar(&ch);
                    inbuf[*bp] = ch;
                    (*bp)++;
                }

                extern int bkreg_run_command(const char *content, int cnt);
                bkreg_run_command(inbuf, len);
                memset(inbuf, 0, len);
                *bp = 0;
                continue;
            }
        }
#endif
        if (inbuf[*bp] == RET_CHAR)
            continue;
        if (inbuf[*bp] == END_CHAR)  	/* end of input line */
        {
            inbuf[*bp] = '\0';
            *bp = 0;
            return 1;
        }

        if ((inbuf[*bp] == 0x08) ||	/* backspace */
                (inbuf[*bp] == 0x7f))  	/* DEL */
        {
            if (*bp > 0)
            {
                (*bp)--;
                if (!pCli->echo_disabled)
                    os_printf("%c %c", 0x08, 0x08);
            }
            continue;
        }

        if (inbuf[*bp] == '\t')
        {
            inbuf[*bp] = '\0';
            tab_complete(inbuf, bp);
            continue;
        }

        if (!pCli->echo_disabled)
            os_printf("%c", inbuf[*bp]);

        (*bp)++;
        if (*bp >= INBUF_SIZE)
        {
            os_printf("Error: input buffer overflow\r\n");
            os_printf(PROMPT);
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
    if (cmd_string != NULL)
    {
        char *c = cmd_string;
        os_printf("command '");
        while (*c != '\0')
        {
            if (is_print(*c))
            {
                os_printf("%c", *c);
            }
            else
            {
                os_printf("\\0x%x", *c);
            }
            ++c;
        }
        os_printf("' not found\r\n");
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
static void cli_main( uint32_t data )
{
    bk_uart_set_rx_callback(CLI_UART, cli_rx_callback, NULL);
    while (1)
    {
        int ret;
        char *msg = NULL;

        rtos_get_semaphore(&log_rx_interrupt_sema, BEKEN_NEVER_TIMEOUT);

        if(get_input(pCli->inbuf, &pCli->bp))
        {
            msg = pCli->inbuf;

            if (os_strcmp(msg, EXIT_MSG) == 0)
                break;
   
            ret = handle_input(msg);
            if (ret == 1)
                print_bad_command(msg);
            else if (ret == 2)
                os_printf("syntax error\r\n");

            os_printf(PROMPT);
        }
    }

    os_printf("CLI exited\r\n");
    os_free(pCli);
    pCli = NULL;

    bk_uart_set_rx_callback(CLI_UART, NULL, NULL);
    rtos_delete_thread(NULL);
}

#ifndef MOC
static void task_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    rtos_print_thread_status( pcWriteBuffer, xWriteBufferLen );
}
#endif

void tftp_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
}

static void partShow_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    bk_partition_t i;
    bk_logic_partition_t *partition;

    for( i = BK_PARTITION_BOOTLOADER; i <= BK_PARTITION_MAX; i++ )
    {
        partition = bk_flash_get_info( i );
        if (partition == NULL)
            continue;

        os_printf( "%4d | %11s |  Dev:%d  | 0x%08lx | 0x%08lx |\r\n", i,
                   partition->partition_description, partition->partition_owner,
                   partition->partition_start_addr, partition->partition_length);
    };

}

static void uptime_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("UP time %ldms\r\n", rtos_get_time());
}

void tftp_ota_thread( beken_thread_arg_t arg )
{
    rtos_delete_thread( NULL );
}

void ota_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    rtos_create_thread( NULL,
                        BEKEN_APPLICATION_PRIORITY,
                        "LOCAL OTA",
                        (beken_thread_function_t)tftp_ota_thread,
                        0x4096,
                        0 );
}

void help_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

/*
*  Command buffer API
*/
void wifiscan_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
#if CFG_ROLE_LAUNCH
    LAUNCH_REQ param;

    param.req_type = LAUNCH_REQ_PURE_STA_SCAN;
    rl_sta_request_enter(&param, 0);
#else	
    demo_scan_app_init();
#endif 
}

void wifiadvscan_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t *ap_ssid;

    if ( argc < 2 )
    {
        os_printf("Please input ssid\r\n");
        return;
    }
    ap_ssid = argv[1];

    demo_scan_adv_app_init(ap_ssid);
}

void softap_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *ap_ssid = NULL;
    char *ap_key;

    os_printf("SOFTAP_COMMAND\r\n\r\n");
    if (argc == 2)
    {
        ap_ssid = argv[1];
        ap_key = "1";
    }
    else if (argc == 3)
    {
        ap_ssid = argv[1];
        ap_key = argv[2];
    }

    if(ap_ssid)
    {
        demo_softap_app_init(ap_ssid, ap_key);
    }
}

void stop_wlan_intface_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t inface = 0;

    os_printf("stop_wlan_intface_Command\r\n");
    if (argc == 2)
    {
        inface = strtoul(argv[1], NULL, 0);
        os_printf("stop wlan intface:%d\r\n", inface);

        if(inface == 0)
        {
            bk_wlan_stop(SOFT_AP);
        }
        else if(inface == 1)
        {
            bk_wlan_stop(STATION);
        }
    }
    else
    {
        os_printf("bad parameters\r\n");
    }
}

void add_virtual_intface(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    VIF_ADDCFG_ST cfg;
    u8 argc_cnt = 1;

    if(argc <= 1)
        return;

    if (!os_strncmp(argv[argc_cnt], "softap", sizeof("softap")))
    {
        cfg.wlan_role = SOFT_AP;
    }
    else if (!os_strncmp(argv[argc_cnt], "sta", sizeof("sta")))
    {
        cfg.wlan_role = STATION;
    }
    else
    {
        os_printf("role error:%s, must be softap or sta\r\n", argv[argc_cnt]);
        return;
    }
    argc_cnt++;

    if(argc == argc_cnt)
        cfg.ssid = "default ssid";
    else
        cfg.ssid = argv[argc_cnt];
    argc_cnt++;

    if(argc == argc_cnt)
        cfg.key = "1234567890";
    else
        cfg.key = argv[argc_cnt];
    argc_cnt++;

    if((cfg.wlan_role == STATION) && (argc > argc_cnt))
    {
        if (!os_strncmp(argv[argc_cnt], "adv", sizeof("adv")))
            cfg.adv = 1;
        else
            cfg.adv = 0;
    }
    else
    {
        cfg.adv = 0;
    }
    argc_cnt++;

    cfg.name = NULL;

    os_printf("role: %d\r\n", cfg.wlan_role);
    os_printf("ssid: %s\r\n", cfg.ssid);
    os_printf("key : %s\r\n", cfg.key);
    os_printf("adv : %d\r\n", cfg.adv);

    demo_wlan_app_init(&cfg);
}

void del_virtual_intface(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *type;
    u8 role = 0xff; 
	
	#if CFG_ROLE_LAUNCH   
    LAUNCH_REQ param;
	#endif

    if(argc <= 1)
        return;

    type = argv[1];

    if (!os_strncmp(type, "softap", sizeof("softap")))
    {
        role = SOFT_AP;
		
		#if CFG_ROLE_LAUNCH
        param.req_type = LAUNCH_REQ_DELIF_AP;

		rl_ap_request_enter(&param, 0);
		#endif
    }
    else if(!os_strncmp(type, "sta", sizeof("sta")))
    {
        role = STATION;
		
		#if CFG_ROLE_LAUNCH
        param.req_type = LAUNCH_REQ_DELIF_STA;

		rl_sta_request_enter(&param, 0);
		#endif
    }

    if(role == 0xff)
    {
        os_printf("%s virtual intface not found\r\n", type );
        return;
    }

#if (0 == CFG_ROLE_LAUNCH)
    bk_wlan_stop(role);
#endif	
}

void show_virtual_intface(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u8 i = 1;
    u8 *mac_ptr;
    char *role;
    const char *name;
    struct netif *lwip_if;
    VIF_INF_PTR vif_entry;

    os_printf("\r\nshow_virtual_intface:\r\n");
    vif_entry = (VIF_INF_PTR)rwm_mgmt_is_vif_first_used();
    while(vif_entry)
    {
        mac_ptr = (u8 *)&vif_entry->mac_addr;
        lwip_if = (struct netif *)vif_entry->priv;
        name = lwip_if->hostname;
        if(vif_entry->type == VIF_AP)
        {
            role = "softap\0";
        }
        else if(vif_entry->type == VIF_STA)
        {
            role = "sta\0";
        }
        else
        {
            role = "others\0";
        }

        os_printf("%1d: %s, %s, %02x:%02x:%02x:%02x:%02x:%02x\r\n", i, name, role,
                  mac_ptr[0], mac_ptr[1], mac_ptr[2], mac_ptr[3], mac_ptr[4], mac_ptr[5]);

        vif_entry = (VIF_INF_PTR)rwm_mgmt_next(vif_entry);
        i++;
    }

    os_printf("\r\nend of show\r\n");

}
#if 1
uint32_t channel_count = 0;
void cli_monitor_cb(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    uint32_t count, i;

    count = MIN(32, len);
    os_printf("cli_monitor_cb:%d:%d\r\n", count, len);
    for(i = 0; i < count; i ++)
    {
        os_printf("%x ", data[i]);
    }
    os_printf("\r\n");

    channel_count ++;
}

xTaskHandle  mtr_thread_handle = NULL;
uint32_t  mtr_stack_size = 2000;
#define THD_MTR_PRIORITY                    5

void mtr_thread_main( void *arg )
{
    OSStatus ret;
    static uint32_t channel_num = 1;

    // stop monitor mode, need stop hal mac first
    bk_wlan_stop_monitor();
    // then set monitor callback
    bk_wlan_register_monitor_cb(NULL);

    // start monitor, need set callback
    bk_wlan_register_monitor_cb(cli_monitor_cb);
    // then start hal mac
    bk_wlan_start_monitor();

    while(1)
    {
        channel_count = 0;

        bk_wlan_set_channel_sync(channel_num);
        channel_num ++;

        if(14 == channel_num)
        {
            channel_num = 1;
        }

        rtos_delay_milliseconds(100);
        os_printf("channel:%d count:%x\r\n", channel_num, channel_count);
    }
}
#endif

void mtr_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    OSStatus ret;

    if(argc != 2)
    {
        os_printf("monitor_parameter invalid\r\n");
        return;
    }
#if 0
    if(NULL == mtr_thread_handle)
    {
        ret = rtos_create_thread(&mtr_thread_handle,
                                 THD_MTR_PRIORITY,
                                 "mtr",
                                 (beken_thread_function_t)mtr_thread_main,
                                 (unsigned short)mtr_stack_size,
                                 (beken_thread_arg_t)0);
        ASSERT(kNoErr == ret);
    }
#else
    uint32_t channel_num;

    channel_num = os_strtoul(argv[1], NULL, 10);
    if(99 == channel_num)
    {
        bk_wlan_stop_monitor();
    }
    else
    {
        bk_wlan_register_monitor_cb(cli_monitor_cb);
        bk_wlan_start_monitor();
        bk_wlan_set_channel_sync(channel_num);
    }
#endif
}

void sta_adv_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *oob_ssid = NULL;
    char *connect_key;

    os_printf("sta_adv_Command\r\n");
    if (argc == 2)
    {
        oob_ssid = argv[1];
        connect_key = "1";
    }
    else if (argc == 3)
    {
        oob_ssid = argv[1];
        connect_key = argv[2];
    }
    else
    {
        os_printf("parameter invalid\r\n");
        return;
    }

    if(oob_ssid)
    {
        demo_sta_adv_app_init(oob_ssid, connect_key);
    }
}

void show_sta_psk(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	uint8_t *psk;
	int i;

	psk = wpas_get_sta_psk();
	os_printf("John# show_sta_psk.r\n");
	for ( i = 0 ; i < 32 ; i++ )
	{
	    os_printf("%02x ", psk[i]);
	}
	os_printf("\r\n");
}

void sta_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char *oob_ssid = NULL;
    char *connect_key;

    os_printf("sta_Command\r\n");
    if (argc == 2)
    {
        oob_ssid = argv[1];
        connect_key = "1";
    }
    else if (argc == 3)
    {
        oob_ssid = argv[1];
        connect_key = argv[2];
    }
    else
    {
        os_printf("parameter invalid\r\n");
    }

    if(oob_ssid)
    {
        demo_sta_app_init(oob_ssid, connect_key);
    }
}


void easylink_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    /*	mxchip_easy_link_start(120);*/
}

void airkiss_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u8 start = 0;

    if(argc != 2)
    {
        os_printf("need 2 parameters: airkiss 1(start), 0(stop)\r\n");
        return;
    }

    start = strtoul(argv[1], NULL, 0);

    airkiss_process(start);
}

#if CFG_USE_TEMPERATURE_DETECT
void temp_detect_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    u8 start = 0;

    if(argc != 2)
    {
        os_printf("need 2 parameters: airkiss 1(start), 0(stop)\r\n");
        return;
    }

    start = strtoul(argv[1], NULL, 0);

    if(start == 0)
        temp_detect_pause_timer();
    else 
        temp_detect_restart_detect();
}
#endif

void wifistate_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("wifistate_Command\r\n");
    demo_state_app_init();
}

void wifidebug_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("wifidebug_Command\r\n");
}

void ifconfig_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    demo_ip_app_init();
}

void arp_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("arp_Command\r\n");
}

void ping_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("ping_Command\r\n");
}

void dns_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("dns_Command\r\n");
}

void socket_show_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("socket_show_Command\r\n");
}

void memory_show_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    cmd_printf("free memory %d\r\n", xPortGetFreeHeapSize());
}

void memory_dump_Command( char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv )
{
    int i;
    uint8_t *pstart;
    uint32_t start, length;

    if (argc != 3)
    {
        cmd_printf("Usage: memdump <addr> <length>.\r\n");
        return;
    }

    start = strtoul(argv[1], NULL, 0);
    length = strtoul(argv[2], NULL, 0);
    pstart = (uint8_t *)start;

    for(i = 0; i < length; i++)
    {
        cmd_printf("%02x ", pstart[i]);
        if (i % 0x10 == 0x0F)
        {
            cmd_printf("\r\n");

        }
    }
}


void memory_set_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("memory_set_Command\r\n");
}

void driver_state_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("driver_state_Command\r\n");
}

void get_version(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    os_printf("get_version\r\n");
}

void reboot(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    FUNCPTR reboot = 0;
    UINT32 wdt_val = 1;
    
#if CFG_USE_STA_PS
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    if(power_save_if_ps_rf_dtim_enabled()
        && power_save_if_rf_sleep())
    {
        power_save_wkup_event_set(NEED_DISABLE_BIT | NEED_REBOOT_BIT);
    }
    else
    {
#endif        

    os_printf("wdt reboot\r\n");
    sddev_control(WDT_DEV_NAME, WCMD_SET_PERIOD, &wdt_val);
    sddev_control(WDT_DEV_NAME, WCMD_POWER_UP, NULL);
#if CFG_USE_STA_PS
    }
    GLOBAL_INT_RESTORE();
#endif        
}

static void echo_cmd_handler(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc == 1)
    {
        os_printf("Usage: echo on/off. Echo is currently %s\r\n",
                  pCli->echo_disabled ? "Disabled" : "Enabled");
        return;
    }

    if (!os_strcasecmp(argv[1], "on"))
    {
        os_printf("Enable echo\r\n");
        pCli->echo_disabled = 0;
    }
    else if (!os_strcasecmp(argv[1], "off"))
    {
        os_printf("Disable echo\r\n");
        pCli->echo_disabled = 1;
    }
}

static void cli_exit_handler(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    // exit command not executed
}

/*
CMD FORMAT: GPIO CMD index PARAM
exmaple:GPIO 0 18 2               (config GPIO18 input & pull-up)
*/
static void Gpio_op_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint32_t ret, id, mode, i;
    char cmd0 = 0;
    char cmd1 = 0;
    char cmd;

    for(i = 0; i < argc; i++)
    {
        os_printf("Argument %d = %s\r\n", i + 1, argv[i]);
    }

    if(argc == 4)
    {
        cmd = argv[1][0];
        mode = argv[3][0];

        cmd0 = argv[2][0] - 0x30;
        cmd1 = argv[2][1] - 0x30;

        id = (uint32_t)(cmd0 * 10 + cmd1);
        os_printf("---%x,%x----\r\n", id, mode);
        ret = BKGpioOp(cmd, id, mode);
        os_printf("gpio op:%x\r\n", ret);
    }
    else
        os_printf("cmd param error\r\n");
}

void test_fun(char para)
{
    os_printf("---%d---\r\n", para);
}
/*
cmd format: GPIO_INT cmd index  triggermode
enable: GPIO_INT 1 18 0
*/
static void Gpio_int_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint32_t id, mode;
    char cmd0 = 0;
    char cmd1 = 0;
    char cmd;

    if(argc == 4)
    {
        cmd = argv[1][0] - 0x30;
        mode = argv[3][0] - 0x30;

        cmd0 = argv[2][0] - 0x30;
        cmd1 = argv[2][1] - 0x30;

        id = (uint32_t)(cmd0 * 10 + cmd1);
        BKGpioIntcEn(cmd, id, mode, test_fun);
    }
    else
        os_printf("cmd param error\r\n");

}


#if CFG_USE_SDCARD_HOST
/*
sdtest I 0 --
sdtest R secnum
sdtest W secnum
*/
extern uint32_t sdcard_intf_test(void);
extern uint32_t test_sdcard_read(uint32_t blk);
extern uint32_t test_sdcard_write(uint32_t blk);
extern void sdcard_intf_close(void);

static void sd_operate(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint32_t cmd;
    uint32_t blknum;
    uint32_t ret;
    if(argc == 3)
    {
        cmd = argv[1][0];
        blknum = os_strtoul(argv[2], NULL, 16);
        switch(cmd)
        {
        case 'I':
            ret = sdcard_intf_test();
            os_printf("init :%x\r\n", ret);
            break;
        case 'R':
            ret = test_sdcard_read(blknum);
            os_printf("read :%x\r\n", ret);
            break;
        case 'W':
            ret = test_sdcard_write(blknum);
            os_printf("write :%x\r\n", ret);
            break;
        case 'C':
            sdcard_intf_close();
            os_printf("close \r\n");
            break;
        default:
            break;
        }
    }
    else
        os_printf("cmd param error\r\n");
}

#endif
/*
format: FLASH  E/R/W  0xABCD
example:	    FLASH  R  0x00100

*/

extern OSStatus test_flash_write(volatile uint32_t start_addr, uint32_t len);
extern OSStatus test_flash_erase(volatile uint32_t start_addr, uint32_t len);
extern OSStatus test_flash_read(volatile uint32_t start_addr, uint32_t len);
extern OSStatus test_flash_read_time(volatile uint32_t start_addr, uint32_t len);

static void flash_command_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char cmd = 0;
    uint32_t len = 0;
    uint32_t addr = 0;
	
    if(argc == 4)
    {
        cmd = argv[1][0];
		addr = atoi(argv[2]);
		len = atoi(argv[3]);

       	switch(cmd)
        {
	        case 'E':
				bk_flash_enable_security(FLASH_PROTECT_NONE);
				test_flash_erase(addr,len);
				bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
	         break;
			  
        	case 'R':
				test_flash_read(addr,len);
	            break;
	        case 'W':
		        bk_flash_enable_security(FLASH_PROTECT_NONE);
				test_flash_write(addr,len);
				bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
	         break;
	//to check whether protection mechanism can work
			case 'N':
				test_flash_erase(addr,len);
				break;
			case 'M':
				test_flash_write(addr,len);
				break;
            case 'T':
				test_flash_read_time(addr,len);
				break;
        	default:
            break;
        }
    }
	else
	{
		os_printf("FLASH <R/W/E/M/N/T> <start_addr> <len>\r\n");
	}
}

/*UART  I  index
example:   UART I 0
*/
static void Uart_command_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    char cmd;
    uint8_t index = 0;
    if(argc == 3)
    {
        cmd = argv[1][0];
        index = argv[2][0] - 0x30;

        if(cmd == 'I')
        {
            bk_uart_initialize_test(0, index, NULL);
        }
    }
    else
        os_printf("cmd param error\r\n");

}
static void tx_evm_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int ret = do_evm(NULL, 0, argc, argv);
    if(ret)
    {
        os_printf("tx_evm bad parameters\r\n");
    }
}

static void rx_sens_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int ret = do_rx_sensitivity(NULL, 0, argc, argv);
    if(ret)
    {
        os_printf("rx sensitivity bad parameters\r\n");
    }
}

#if (CFG_SOC_NAME != SOC_BK7231)
static void efuse_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t addr, data;
    
    if(argc == 3)
    {
        if (os_strncmp(argv[1], "-r", 2) == 0) {
            hexstr2bin(argv[2], &addr, 1);
            os_printf("efuse read: addr-0x%02x, data-0x%02x\r\n",
                        addr, wifi_read_efuse(addr));
        } 
    } 
    else if(argc == 4) 
    {
        if(os_strncmp(argv[1], "-w", 2) == 0)  {
            hexstr2bin(argv[2], &addr, 1);
            hexstr2bin(argv[3], &data, 6);
            os_printf("efuse write: addr-0x%02x, data-0x%02x, ret:%d\r\n",
                        addr, data, wifi_write_efuse(addr, data));
        }
    }
    else {
        os_printf("efuse [-r addr] [-w addr data]\r\n");
    }
}

static void efuse_mac_cmd_test(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t mac[6];
    
    if (argc == 1)
    {
        if(wifi_get_mac_address_from_efuse(mac))
            os_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else if(argc == 2)
    {
        if (os_strncmp(argv[1], "-r", 2) == 0) {
            if(wifi_get_mac_address_from_efuse(mac))
                os_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        } 
    } 
    else if(argc == 3) 
    {
        if(os_strncmp(argv[1], "-w", 2) == 0)  {
            hexstr2bin(argv[2], mac, 6);
            //if(wifi_set_mac_address_to_efuse(mac))
                os_printf("Set MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        }
    }
    else {
        os_printf("efusemac [-r] [-w] [mac]\r\n");
    }
}
#endif //(CFG_SOC_NAME != SOC_BK7231)


#if CFG_SUPPORT_BKREG
#define BKCMD_RXSENS_R      'r'
#define BKCMD_RXSENS_X      'x'
#define BKCMD_RXSENS_s      's'

#define BKCMD_TXEVM_T       't'
#define BKCMD_TXEVM_X       'x'
#define BKCMD_TXEVM_E       'e'

static void bkreg_cmd_handle_input(char *inbuf, int len)
{
    if(((char)BKREG_MAGIC_WORD0 == inbuf[0])
            && ((char)BKREG_MAGIC_WORD1 == inbuf[1])
            && ((char)BKREG_MAGIC_WORD2 == inbuf[2]))
    {
        if(cli_getchars(inbuf, len))
        {
            bkreg_run_command(inbuf, len);
            os_memset(inbuf, 0, len);
        }
    }
    else if((((char)BKCMD_RXSENS_R == inbuf[0])
             && ((char)BKCMD_RXSENS_X == inbuf[1])
             && ((char)BKCMD_RXSENS_s == inbuf[2]))
            || (((char)BKCMD_TXEVM_T == inbuf[0])
                && ((char)BKCMD_TXEVM_X == inbuf[1])
                && ((char)BKCMD_TXEVM_E == inbuf[2])) )
    {
        if(cli_getchars(inbuf, len))
        {
            handle_input(inbuf);
            os_memset(inbuf, 0, len);
        }
    }

}
#endif

#if CFG_SUPPORT_OTA_TFTP
extern void tftp_start(void);
extern void string_to_ip(char *s);
static void tftp_ota_get_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    short len = 0,i;
    extern char		BootFile[] ;
 
    if(argc > 3 )
        {
        os_printf("ota server_ip ota_file\r\n");
        return;
    }

     os_printf("%s\r\n",argv[1]);

    os_strcpy(BootFile,argv[2]);
    os_printf("%s\r\n",BootFile);
    string_to_ip (argv[1]);


    tftp_start();

    return;

}
#endif

#define HTTP_RESP_CONTENT_LEN   (256)
/*
*when HTTP_WR_TO_FLASH = 1 & CFG_SUPPORT_OTA_HTTP = 0 http data will write to flash ,addr #define HTTP_FLASH_ADDR  0xff000
*when HTTP_WR_TO_FLASH = 1 & CFG_SUPPORT_OTA_HTTP = 1 http data will write to flash ,with bk ota fromat
*when HTTP_WR_TO_FLASH = 0 can get http data at http_data_process()
*/
void http_client_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int ret;
    httpclient_t httpclient;
    httpclient_data_t httpclient_data;
    char http_content[HTTP_RESP_CONTENT_LEN];

    if ( argc != 2 )
    {
        goto HTTP_CMD_ERR;
    }    
    os_memset(&httpclient, 0, sizeof(httpclient_t));
    os_memset(&httpclient_data, 0, sizeof(httpclient_data));
    os_memset(&http_content, 0, sizeof(HTTP_RESP_CONTENT_LEN));
    httpclient.header = "Accept: text/xml,text/html,\r\n"; 
    httpclient_data.response_buf = http_content; 
    httpclient_data.response_content_len = HTTP_RESP_CONTENT_LEN;
    ret = httpclient_common(&httpclient,
        argv[1],  
        80,  
        NULL,
        HTTPCLIENT_GET, 
        5000,
        &httpclient_data); 
    if (0 != ret) { 
        os_printf("request epoch time from remote server failed.");
        } else {  
        os_printf("sucess.\r\n");
    }

    return;
HTTP_CMD_ERR:
    os_printf("Usage:httpc [url:]\r\n");
        
}

static const struct cli_command built_ins[] =
{
    {"help", NULL, help_command},
    {"version", NULL, get_version},
    {"echo", NULL, echo_cmd_handler},
    {"exit", "CLI exit", cli_exit_handler},

    /// WIFI
    {"scan", "scan ap", wifiscan_Command},
    {"advscan", "scan ap", wifiadvscan_Command},
    {"softap", "softap ssid key", softap_Command},
    {"stopintf", "stopintf intfacename", stop_wlan_intface_Command},
    {"sta", "sta ap_ssid key", sta_Command},
    {"adv", "adv", sta_adv_Command},
    {"mtr", "mtr channel", mtr_Command},
    {"addif", "addif param", add_virtual_intface},
    {"delif", "delif role", del_virtual_intface},
    {"showif", "show", show_virtual_intface},
    {"psk", "show psk", show_sta_psk},

    {"wifistate", "Show wifi state", wifistate_Command},

    // network
    {"ifconfig", "Show IP address", ifconfig_Command},
    {"ping", "ping <ip>", ping_Command},
    {"dns", "show/clean/<domain>", dns_Command},
    {"sockshow", "Show all sockets", socket_show_Command},
    // os
    {"tasklist", "list all thread name status", task_Command},

    // others
    {"memshow", "print memory information", memory_show_Command},
    {"memdump", "<addr> <length>", memory_dump_Command},
    {"os_memset", "<addr> <value 1> [<value 2> ... <value n>]", memory_set_Command},
    {"memp", "print memp list", memp_dump_Command},

    {"reboot", "reboot system", reboot},
    
    {"time",     "system time",                 uptime_Command},
    {"flash",    "Flash memory map",            partShow_Command},

    {"GPIO", "GPIO <cmd> <arg1> <arg2>", Gpio_op_Command},
    {"GPIO_INT", "GPIO_INT <cmd> <arg1> <arg2>", Gpio_int_Command},
    {"FLASH", "FLASH <cmd(R/W/E/N)>", flash_command_test},
    {"UART", "UART I <index>", Uart_command_test},

    {"txevm", "txevm [-m] [-c] [-l] [-r] [-w]", tx_evm_cmd_test},
    {"rxsens", "rxsens [-m] [-d] [-c] [-l]", rx_sens_cmd_test},
    #if (CFG_SOC_NAME != SOC_BK7231)
    {"efuse",       "efuse [-r addr] [-w addr data]", efuse_cmd_test},
    {"efusemac",    "efusemac [-r] [-w] [mac]",       efuse_mac_cmd_test},
    #endif // (CFG_SOC_NAME != SOC_BK7231)

#if CFG_SARADC_CALIBRATE
    {"adc", "adc [func] [param]", adc_command},
#endif

    {"easylink", "start easylink", easylink_Command},
    {"airkiss", "start airkiss", airkiss_Command},
#if CFG_SUPPORT_OTA_TFTP	
	{"tftpota", "tftpota [ip] [file]", tftp_ota_get_Command},
#endif

#if CFG_USE_SDCARD_HOST
    {"sdtest", "sdtest <cmd>", sd_operate},
#endif

#if CFG_USE_TEMPERATURE_DETECT
    {"tmpdetect", "tmpdetect <cmd>", temp_detect_Command},
#endif

	{"httpc", "http client", http_client_Command},

};

/* Built-in "help" command: prints all registered commands and their help
* text string, if any. */
void help_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int i, n;
    uint32_t build_in_count = sizeof(built_ins) / sizeof(struct cli_command);

#if (DEBUG)
    build_in_count++; //For command: micodebug
#endif

    os_printf( "====Build-in Commands====\r\n" );
    for (i = 0, n = 0; i < MAX_COMMANDS && n < pCli->num_commands; i++)
    {
        if (pCli->commands[i]->name)
        {
            os_printf("%s: %s\r\n", pCli->commands[i]->name,
                      pCli->commands[i]->help ?
                      pCli->commands[i]->help : "");
            n++;
            if( n == build_in_count )
            {
                os_printf("\r\n====User Commands====\r\n");
            }
        }
    }
}


int cli_register_command(const struct cli_command *command)
{
    int i;
    if (!command->name || !command->function)
        return 1;

    if (pCli->num_commands < MAX_COMMANDS)
    {
        /* Check if the command has already been registered.
        * Return 0, if it has been registered.
        */
        for (i = 0; i < pCli->num_commands; i++)
        {
            if (pCli->commands[i] == command)
                return 0;
        }
        pCli->commands[pCli->num_commands++] = command;
        return 0;
    }

    return 1;
}

int cli_unregister_command(const struct cli_command *command)
{
    int i;
    if (!command->name || !command->function)
        return 1;

    for (i = 0; i < pCli->num_commands; i++)
    {
        if (pCli->commands[i] == command)
        {
            pCli->num_commands--;
            int remaining_cmds = pCli->num_commands - i;
            if (remaining_cmds > 0)
            {
                os_memmove(&pCli->commands[i], &pCli->commands[i + 1],
                           (remaining_cmds *
                            sizeof(struct cli_command *)));
            }
            pCli->commands[pCli->num_commands] = NULL;
            return 0;
        }
    }

    return 1;
}


int cli_register_commands(const struct cli_command *commands, int num_commands)
{
    int i;
    for (i = 0; i < num_commands; i++)
        if (cli_register_command(commands++))
            return 1;
    return 0;
}

int cli_unregister_commands(const struct cli_command *commands,
                            int num_commands)
{
    int i;
    for (i = 0; i < num_commands; i++)
        if (cli_unregister_command(commands++))
            return 1;

    return 0;
}

static void micodebug_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc == 1)
    {
        os_printf("Usage: micodebug on/off. _BK_ debug is currently %s\r\n",
                  mico_debug_enabled ? "Enabled" : "Disabled");
        return;
    }

    if (!os_strcasecmp(argv[1], "on"))
    {
        os_printf("Enable _BK_ debug\r\n");
        mico_debug_enabled = 1;
    }
    else if (!os_strcasecmp(argv[1], "off"))
    {
        os_printf("Disable _BK_ debug\r\n");
        mico_debug_enabled = 0;
    }
}

void monitor(uint8_t *data, int len, hal_wifi_link_info_t *info)
{
    int i;

    os_printf("[%d]: ", len);
    for(i = 0; i < len; i++)
    {
        os_printf("%02x ", data[i]);
    }
    os_printf("\r\n");
}

static void monitor_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (argc == 1)
    {
        os_printf("Usage: monitor on/off.");
        return;
    }

    if (!os_strcasecmp(argv[1], "on"))
    {
        cmd_printf("start monitor\r\n");
        // start monitor, need set callback
        bk_wlan_register_monitor_cb(monitor);
        // then start hal mac
        bk_wlan_start_monitor();
    }
    else if (!os_strcasecmp(argv[1], "off"))
    {
        cmd_printf("stop monitor\r\n");
        mico_debug_enabled = 0;

        // stop monitor mode, need stop hal mac first
        bk_wlan_stop_monitor();
        // then set monitor callback
        bk_wlan_register_monitor_cb(NULL);
    }
}

static void channel_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int channel, i = 0;

    if (argc == 1)
    {
        os_printf("Usage: channel [1~13].");
        return;
    }

    while(argv[1][i])
    {
        if((argv[1][i] < '0') || (argv[1][i] > '9'))
        {
            os_printf("parameter should be a number\r\n");
            return ;
        }
        i++;
    }

    channel = atoi(argv[1]);

    if((channel < 1) || (channel > 13))
    {
        os_printf("Invalid channel number \r\n");
        return ;
    }
    cmd_printf("set to channel %d\r\n", channel);
    bk_wlan_set_channel_sync(channel);
}

static void pwr_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int pwr = 0;

    if(argc != 2)
    {
        os_printf("Usage: pwr [hex:5~15].");
        return;
    }

    pwr = os_strtoul(argv[1], NULL, 16);

    rw_msg_set_power(0,pwr);
}

static void Ps_Command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    UINT32 gpio_index = 0;
    UINT32 param;
    UINT32 ret;
    UINT32 standby_time = 0;
    UINT32 dtim = 0;
    UINT32 dtim_wait_time = 0;
    UINT32 dtim_data_wait_time = 0;
    UINT32 dtim_uart_wait_time = 0;

    if(argc < 3)
    {
        goto IDLE_CMD_ERR;
    }
#if  CFG_USE_DEEP_PS
    if(0 == os_strcmp(argv[1], "deepps"))
    {
        if(argc != 4)
        {
            goto IDLE_CMD_ERR;
        }
    
        gpio_index = os_strtoul(argv[2], NULL, 16);
        dtim = os_strtoul(argv[3], NULL, 16);

        bk_enter_deep_sleep(gpio_index,dtim);
    }
#endif
#if CFG_USE_MCU_PS
    else if(0 == os_strcmp(argv[1], "mcudtim"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }
    
        dtim = os_strtoul(argv[2], NULL, 10);
        if(dtim == 1)
        {
            bk_wlan_mcu_ps_mode_enable();
        }
        else if(dtim == 0)
        {
            bk_wlan_mcu_ps_mode_disable();
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
#endif
#if CFG_USE_STA_PS
    else if(0 == os_strcmp(argv[1], "rfwkup"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }    
        
        bk_wlan_dtim_rf_ps_mode_do_wakeup();
    }
    else if(0 == os_strcmp(argv[1], "rfdtim"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }
    
        dtim = os_strtoul(argv[2], NULL, 10);
        if(dtim == 1)
        {
            bk_wlan_dtim_rf_ps_mode_enable();
        }
        else if(dtim == 0)
        {
            bk_wlan_dtim_rf_ps_disable_send_msg();
        }        
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
    else if(0 == os_strcmp(argv[1], "setwktm"))
    {
        if(argc != 3)
        {
            os_printf("beacon len:%d\r\n", power_save_beacon_len_get());
            os_printf("wktm:%d\r\n", power_save_radio_wkup_get());
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);

        if(dtim)
        {
            power_save_radio_wkup_set(dtim);
            os_printf("set ridio wkup:%d\r\n", dtim);
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
    else if(0 == os_strcmp(argv[1], "bcmc"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);

        if(dtim == 0 || dtim == 1)
        {
            power_save_sm_set_all_bcmc(dtim);
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }  
    else if(0 == os_strcmp(argv[1], "lingtm"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);

        if(dtim >= 0)
        {
            bk_wlan_dtim_rf_ps_set_linger_time(dtim);
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
    else if(0 == os_strcmp(argv[1], "bcn_tm"))
    {
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }

        dtim = os_strtoul(argv[2], NULL, 10);

        if(dtim >= 0)
        {
            power_save_set_keep_timer_time(dtim);
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
    }
    else if(0 == os_strcmp(argv[1], "normal"))
    {
        #if PS_DTIM_WITH_NORMAL
        if(argc != 3)
        {
            goto IDLE_CMD_ERR;
        }
        dtim = os_strtoul(argv[2], NULL, 10);
        if(dtim == 0)
        {
            bk_wlan_dtim_with_normal_close();
        }
        else if(dtim == 1)
        {
            bk_wlan_dtim_with_normal_open();
        }
        else
        {
            goto IDLE_CMD_ERR;
        }
        #endif
    }    
    else if(0 == os_strcmp(argv[1], "listen"))
    {
        if(argc != 4)
        {
            goto IDLE_CMD_ERR;
        }

        if(0 == os_strcmp(argv[2], "dtim"))
        {
            dtim = os_strtoul(argv[3], NULL, 10);
            power_save_set_dtim_multi(dtim);

        }
        else
        {
            goto IDLE_CMD_ERR;
        }

    }
    else if(0 == os_strcmp(argv[1], "dump"))
    {
        mcu_ps_dump();
        power_save_dump();
    }
#endif
    #if PS_SUPPORT_MANUAL_SLEEP
    else if(0 == os_strcmp(argv[1], "pwm"))
    {
        if(argc != 4)
        {
            goto IDLE_CMD_ERR;
        }    

        dtim = os_strtoul(argv[2], NULL, 10);
        standby_time = os_strtoul(argv[3], NULL, 10);
        bk_wlan_ps_wakeup_with_timer(dtim,standby_time);
        os_printf("exit \r\n");

    }
    else if(0 == os_strcmp(argv[1], "peri"))
    {
        if(argc != 5)
        {
            goto IDLE_CMD_ERR;
        }
        dtim_wait_time = os_strtoul(argv[2], NULL, 10);
        dtim = os_strtoul(argv[3], NULL, 16);
        gpio_index = os_strtoul(argv[4], NULL, 16);

        if(dtim_wait_time != 1 && dtim_wait_time != 0)
        {
            goto IDLE_CMD_ERR;
        }
        os_printf("peri %d %d %x\r\n",dtim_wait_time, dtim, gpio_index);
        bk_wlan_ps_wakeup_with_peri(dtim_wait_time,dtim, gpio_index);
        os_printf("exit \r\n");
    }
    else if(0 == os_strcmp(argv[1], "gpio"))
    {
        if(argc != 5)
        {
            goto IDLE_CMD_ERR;
        }
        
        dtim_wait_time = os_strtoul(argv[2], NULL, 10);
        dtim = os_strtoul(argv[3], NULL, 16);
        gpio_index = os_strtoul(argv[4], NULL, 16);

        if(dtim_wait_time != 1 && dtim_wait_time != 0)
        {
            goto IDLE_CMD_ERR;
        }
        os_printf("peri %d %x\r\n", dtim, gpio_index);
        bk_wlan_ps_wakeup_with_gpio(dtim_wait_time,dtim, gpio_index);
        os_printf("exit \r\n");
    }
    
    #endif    
    else
    {
        goto IDLE_CMD_ERR;
    }


    return;
IDLE_CMD_ERR:
    os_printf("Usage:ps [func] [param] []\r\n");
    os_printf("standby:ps deepps [gpio_map] ");
    os_printf("gpio_map is hex and every bits is map to gpio0-gpio31\r\n");
    os_printf("ieee:ps rfdtim [],[1/0] is open or close \r\n\r\n");
    os_printf("ieee:ps mcudtim [] ,[1/0] is open or close \r\n\r\n");

}

static void mac_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t mac[6];

    if (argc == 1)
    {
        wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
        os_printf("MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else if(argc == 2)
    {
        hexstr2bin(argv[1], mac, 6);
        wifi_set_mac_address((char *)mac);
        os_printf("Set MAC address: %02x-%02x-%02x-%02x-%02x-%02x\r\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    else
    {
        os_printf("invalid cmd\r\n");
    }

}

#if CFG_SARADC_CALIBRATE
static void adc_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    UINT32 status;
    DD_HANDLE flash_handle;
    DD_HANDLE saradc_handle;
    saradc_cal_val_t p_ADC_cal;
    float voltage = 0.0;
    saradc_desc_t *p_ADC_drv_desc = NULL;
        
    if(argc < 2)
        goto IDLE_CMD_ERR;

    if(0 == os_strcmp(argv[1], "read"))
    {
        status = manual_cal_load_adc_cali_flash();
        if(status != 0)
        {
            os_printf("Can't read cali value, use default!\r\n");
            os_printf("calibrate low value:[%x]\r\n", saradc_val.low);
            os_printf("calibrate high value:[%x]\r\n", saradc_val.high);
        }
    }
    else if(0 == os_strcmp(argv[1], "set"))
    {
        p_ADC_drv_desc = (saradc_desc_t *)os_malloc(sizeof(saradc_desc_t));
        if (p_ADC_drv_desc == NULL)
        {
            os_printf("malloc1 failed!\r\n");
            return;
        }
        
        saradc_config_param_init(p_ADC_drv_desc);

        p_ADC_drv_desc->data_buff_size = ADC_TEMP_BUFFER_SIZE;
        p_ADC_drv_desc->pData = (UINT16 *)os_malloc(p_ADC_drv_desc->data_buff_size * sizeof(UINT16));
        os_memset(p_ADC_drv_desc->pData, 0x00, p_ADC_drv_desc->data_buff_size * sizeof(UINT16));

        if(p_ADC_drv_desc->pData == NULL)
        {
            os_printf("malloc1 failed!\r\n");
            os_free(p_ADC_drv_desc);
            return;
        }

        saradc_handle = ddev_open(SARADC_DEV_NAME, &status, (UINT32)p_ADC_drv_desc);

        while (1)
        {
            if (p_ADC_drv_desc->current_sample_data_cnt == p_ADC_drv_desc->data_buff_size)
            {
                ddev_close(saradc_handle);
                break;
            }
        }

        if(0 == os_strcmp(argv[2], "low"))
        {
            p_ADC_cal.mode = SARADC_CALIBRATE_LOW;
        }
        else if(0 == os_strcmp(argv[2], "high"))
        {
            p_ADC_cal.mode = SARADC_CALIBRATE_HIGH;
        }
        else
        {
            os_printf("invalid parameter\r\n");
            return;
        }
        p_ADC_cal.val = p_ADC_drv_desc->pData[4];
        if(SARADC_FAILURE == ddev_control(saradc_handle, SARADC_CMD_SET_CAL_VAL, (VOID *)&p_ADC_cal))
        {
            os_printf("set calibrate value failture\r\n");
            os_free(p_ADC_drv_desc->pData);
            os_free(p_ADC_drv_desc);
            return;
        }
        os_printf("set calibrate success\r\n");
        os_printf("type:[%s] value:[0x%x]\r\n", (p_ADC_cal.mode ? "high":"low"), p_ADC_cal.val);
        os_free(p_ADC_drv_desc->pData);
        os_free(p_ADC_drv_desc);
    }
    else if(0 == os_strcmp(argv[1], "write"))
    {
        manual_cal_save_chipinfo_tab_to_flash();
        os_printf("calibrate low value:[%x]\r\n", saradc_val.low);
        os_printf("calibrate high value:[%x]\r\n", saradc_val.high);
    }
    /* for test
    else if(0 == os_strcmp(argv[1], "get"))
    {
        p_ADC_drv_desc = (saradc_desc_t *)os_malloc(sizeof(saradc_desc_t));
        if (p_ADC_drv_desc == NULL)
        {
        	os_printf("malloc1 failed!\r\n");
            return;
        }

        os_memset(p_ADC_drv_desc, 0x00, sizeof(saradc_desc_t));
        p_ADC_drv_desc->channel = 1;
        p_ADC_drv_desc->data_buff_size = ADC_TEMP_BUFFER_SIZE;
        p_ADC_drv_desc->mode = (ADC_CONFIG_MODE_CONTINUE << 0)
                               | (ADC_CONFIG_MODE_36DIV << 2);
                               
        p_ADC_drv_desc->has_data                = 0;
        p_ADC_drv_desc->current_read_data_cnt   = 0;
        p_ADC_drv_desc->current_sample_data_cnt = 0;
        p_ADC_drv_desc->pre_div = 0x10;
        p_ADC_drv_desc->samp_rate = 0x20;
        p_ADC_drv_desc->pData = (UINT16 *)os_malloc(p_ADC_drv_desc->data_buff_size * sizeof(UINT16));
        os_memset(p_ADC_drv_desc->pData, 0x00, p_ADC_drv_desc->data_buff_size * sizeof(UINT16));

        if(p_ADC_drv_desc->pData == NULL)
        {
            os_printf("malloc1 failed!\r\n");
            os_free(p_ADC_drv_desc);
            return;
        }

        saradc_handle = ddev_open(SARADC_DEV_NAME, &status, (UINT32)p_ADC_drv_desc);

        while (1)
        {
            if (p_ADC_drv_desc->current_sample_data_cnt == p_ADC_drv_desc->data_buff_size)
            {
                ddev_close(saradc_handle);
                break;
            }
        }
        voltage = saradc_calculate(p_ADC_drv_desc->pData[4]);
        
        os_printf("voltage is [%f]\r\n", voltage);
        os_free(p_ADC_drv_desc->pData);
        os_free(p_ADC_drv_desc);
        return;
        
    }*/
    else
    {
        goto IDLE_CMD_ERR;
    }
    
    return;
IDLE_CMD_ERR:
    os_printf("Usage:ps [func] [param]\r\n");
}
#endif

static void cli_rx_callback(int uport, void *param)
{
	if(log_rx_interrupt_sema)
    	rtos_set_semaphore(&log_rx_interrupt_sema);
}

/* ========= CLI input&output APIs ============ */
int cli_printf(const char *msg, ...)
{
    va_list ap;
    char *pos, message[256];
    int sz;
    int nMessageLen = 0;

    os_memset(message, 0, 256);
    pos = message;

    sz = 0;
    va_start(ap, msg);
    nMessageLen = vsnprintf(pos, 256 - sz, msg, ap);
    va_end(ap);

    if( nMessageLen <= 0 ) return 0;

    cli_putstr((const char *)message);
    return 0;
}


int cli_putstr(const char *msg)
{
    if (msg[0] != 0)
        bk_uart_send( CLI_UART, (const char *)msg, os_strlen(msg) );

    return 0;
}

int cli_getchar(char *inbuf)
{
    if (bk_uart_recv(CLI_UART, inbuf, 1, BEKEN_WAIT_FOREVER) == 0)
        return 1;
    else
        return 0;
}

int cli_getchars(char *inbuf, int len)
{
    if(bk_uart_recv(CLI_UART, inbuf, len, BEKEN_WAIT_FOREVER) == 0)
        return 1;
    else
        return 0;
}

int cli_getchars_prefetch(char *inbuf, int len)
{
    if(bk_uart_recv_prefetch(CLI_UART, inbuf, len, BEKEN_WAIT_FOREVER) == 0)
        return 1;
    else
        return 0;
}

int cli_get_all_chars_len(void)
{
    return bk_uart_get_length_in_buffer(CLI_UART);
}

#if CFG_IPERF_TEST
extern void iperf(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
#endif // CFG_IPERF_TEST

static const struct cli_command user_clis[] =
{
    {"micodebug", "micodebug on/off", micodebug_Command},
    {"monitor", "monitor on/off", monitor_Command},
    {"channel", "channel []", channel_Command},
    {"mac", "mac <mac>, Get mac/Set mac. <mac>: c89346000001", mac_command},
    {"ps", "ps [func] [param]", Ps_Command},
#ifdef TCP_CLIENT_DEMO
    {"tcp_cont", "tcp_cont [ip] [port]", tcp_make_connect_server_command},
#endif
    {"tcp_server", "tcp_server [ip] [port]",make_tcp_server_command },

#if CFG_IPERF_TEST
    {"iperf", "iperf help", iperf },
#endif // CFG_IPERF_TEST
#if CFG_SUPPORT_TPC_PA_MAP
    {"pwr", "pwr help", pwr_Command },
#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
    {"sec", "sec help", sec_Command },
#endif
};

int cli_init(void)
{
    int ret;

    pCli = (struct cli_st *)os_malloc(sizeof(struct cli_st));
    if (pCli == NULL)
        return kNoMemoryErr;

    os_memset((void *)pCli, 0, sizeof(struct cli_st));
    rtos_init_semaphore(&log_rx_interrupt_sema, 10);

    if (cli_register_commands(&built_ins[0],
                              sizeof(built_ins) / sizeof(struct cli_command)))
    {
        goto init_general_err;
    }

    cli_register_commands(user_clis, sizeof(user_clis) / sizeof(struct cli_command));

    ret = rtos_create_thread(NULL,
                             BEKEN_DEFAULT_WORKER_PRIORITY,
                             "cli",
                             (beken_thread_function_t)cli_main,
                             4096,
                             0);
    if (ret != kNoErr)
    {
        os_printf("Error: Failed to create cli thread: %d\r\n",
                  ret);
        goto init_general_err;
    }

    pCli->initialized = 1;
    pCli->echo_disabled = 0;

    return kNoErr;

init_general_err:
    if(pCli)
    {
        os_free(pCli);
        pCli = NULL;
    }

    return kGeneralErr;
}

// eof

