/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     user_cmd_parse.c
* @brief    Parse user command from lower Data UART data.
* @details  Parse user commands and execute right commands.
* @author
* @date     2016-02-18
* @version  v0.1
*********************************************************************************************************
*/
#include <string.h>
#include <data_uart.h>
#include <user_cmd_parse.h>

/**
 * @brief   Check if a character is a white space.
 *
 * @param c     Char data to check.
 * @return  Check result.
 * @retval  1  true.
 * @retval  0  false.
 */
static bool user_cmd_is_white_space(char c)
{
    return (((c >= 9) && (c <= 13)) || (c == 32));
}

/**
 * @brief  Skip white spaces in buffer.
 *
 * @param  buffer    Address of the buffer.
 * @return pointer to skipped white spaces' new buffer.
 */
static char *user_cmd_skip_spaces(char *buffer)
{
    char *p = buffer;

    while (user_cmd_is_white_space(*p)) /* white space */
    {
        p++;
    }
    return p;
}

/**
 * @brief  Find end of a word.
 *
 * @param buffer    Address of the buffer.
 * @return
 */
static char *user_cmd_find_end_of_word(char *buffer)
{
    char *p = buffer;

    while (!user_cmd_is_white_space(*p) && (*p != '\0'))
    {
        p++;
    }
    return p;
}

/**
 * @brief  Read ASCII string and convert to uint32_t.
 *
 * @param p  String address.
 * @return
 */
static uint32_t user_cmd_str_to_uint32(char *p)
{
    uint32_t result = 0;
    bool     hex = false;

    /* check if value is dec */
    if (p[0] == 'x')
    {
        hex = true;
        p = &p[1];
    }
    else if ((p[0] == '0') && (p[1] == 'x'))
    {
        hex = true;
        p = &p[2];
    }

    for (;;)
    {
        char     ch;
        ch = *(p++) | 0x20;                 /* convert to lower case */

        if (hex)                            /* dec value */
        {
            /* hex value */
            if ((ch >= 'a') && (ch <= 'f'))
            {
                ch -= ('a' - 10);
            }
            else if ((ch >= '0') && (ch <= '9'))
            {
                ch -= '0';
            }
            else
            {
                break;
            }
            result = (result << 4);
            result += (ch & 0x0f);
        }
        else
        {
            if (ch < '0' || ch > '9')
            {
                break;    /* end of string reached */
            }
            result = 10 * result + ch - '0';
        }
    }
    return (result);
}

/**
 * @brief  Send result, display in UART Assitant.
 *
 * @param result    Command parse result.
 * @return none
*/
static void cmd_send_result(T_USER_CMD_PARSE_RESULT result)
{
    switch (result)
    {
    case RESULT_ERR:
        data_uart_print("%s\r\n", "CMD:Error");
        break;
    case RESULT_GAP_CAUSE_ALREADY_IN_REQ:
        data_uart_print("%s\r\n", "GAP_CAUSE_ALREADY_IN_REQ");
        break;
    case RESULT_GAP_CAUSE_INVALID_STATE:
        data_uart_print("%s\r\n", "GAP_CAUSE_INVALID_STATE");
        break;
    case RESULT_GAP_CAUSE_INVALID_PARAM:
        data_uart_print("%s\r\n", "GAP_CAUSE_INVALID_PARAM");
        break;
    case RESULT_GAP_CAUSE_NON_CONN:
        data_uart_print("%s\r\n", "GAP_CAUSE_NON_CONN");
        break;
    case RESULT_GAP_CAUSE_NOT_FIND_IRK:
        data_uart_print("%s\r\n", "GAP_CAUSE_NOT_FIND_IRK");
        break;
    case RESULT_GAP_CAUSE_ERROR_CREDITS:
        data_uart_print("%s\r\n", "GAP_CAUSE_ERROR_CREDITS");
        break;
    case RESULT_GAP_CAUSE_SEND_REQ_FAILED:
        data_uart_print("%s\r\n", "GAP_CAUSE_SEND_REQ_FAILED");
        break;
    case RESULT_GAP_CAUSE_NO_RESOURCE:
        data_uart_print("%s\r\n", "GAP_CAUSE_NO_RESOURCE");
        break;
    case RESULT_GAP_CAUSE_INVALID_PDU_SIZE:
        data_uart_print("%s\r\n", "GAP_CAUSE_INVALID_PDU_SIZE");
        break;
    case RESULT_GAP_CAUSE_NOT_FIND:
        data_uart_print("%s\r\n", "GAP_CAUSE_NOT_FIND");
        break;
    case RESULT_GAP_CAUSE_CONN_LIMIT:
        data_uart_print("%s\r\n", "GAP_CAUSE_CONN_LIMIT");
        break;
    case RESULT_CMD_NOT_FOUND:
        data_uart_print("%s\r\n", "CMD:Command not found");
        break;
    case RESULT_CMD_ERR_PARAM_NUM:
        data_uart_print("%s\r\n", "CMD:Wrong number of parameters");
        break;
    case RESULT_CMD_ERR_PARAM:
        data_uart_print("%s\r\n", "CMD:Wrong parameter");
        break;
    case RESULT_CMD_OUT_OF_RANGE:
        data_uart_print("%s\r\n", "CMD:Value out of range");
        break;
    case RESULT_CMD_NOT_SUPPORT:
        data_uart_print("%s\r\n", "CMD:Not support");
        break;
    case RESULT_GAP_CAUSE_ERROR_UNKNOWN:
        data_uart_print("%s\r\n", "GAP_CAUSE_ERROR_UNKNOWN");
        break;
    default:
        break;
    }
}

/**
 * @brief   List cmd.
 *
 * @param p_cmd_table   Command table, include user self-definition command function.
 * @return  Command execute result.
*/
static T_USER_CMD_PARSE_RESULT user_cmd_list(const T_USER_CMD_TABLE_ENTRY *p_cmd_table)
{
    int32_t i = 0;
    T_USER_CMD_PARSE_RESULT result = RESULT_CMD_NOT_FOUND;

    /* find command in table */
    while ((p_cmd_table + i)->p_cmd != NULL)
    {
        data_uart_print("%s", (p_cmd_table + i)->p_option);
        data_uart_print("%s", "  *");
        data_uart_print("%s", (p_cmd_table + i)->p_help);
        result = RESULT_SUCESS;
        i++;
    };

    data_uart_print(",.\r\n  *up down\r\n");
    data_uart_print("[]\r\n  *left right\r\n");
    data_uart_print("/\\\r\n  *home end\r\n");
    data_uart_print("backspace\r\n  *delete\r\n");

    return result;
}

/**
 * @brief  Execute command.
 *
 * @param p_parse_value     Command parse value.
 * @param p_cmd_table       Command table, include user self-definition command function.
 * @return  Command execute result.
*/
static T_USER_CMD_PARSE_RESULT user_cmd_execute(T_USER_CMD_PARSED_VALUE *p_parse_value,
                                                const T_USER_CMD_TABLE_ENTRY *p_cmd_table)
{
    int32_t i = 0;
    T_USER_CMD_PARSE_RESULT result = RESULT_CMD_NOT_FOUND;

    if (strcmp((const char *)p_parse_value->p_cmd, (const char *)"?") == 0)
    {
        user_cmd_list(p_cmd_table);
        return RESULT_SUCESS;
    }

    /* find command in table */
    while ((p_cmd_table + i)->p_cmd != NULL)
    {
        if (strcmp((const char *)(p_cmd_table + i)->p_cmd, (const char *)p_parse_value->p_cmd) == 0)
        {
            /* check if user wants help */
            if (p_parse_value->param_count && *p_parse_value->p_param[0] == '?')
            {
                data_uart_print("%s", (p_cmd_table + i)->p_option);
                data_uart_print("%s", "  *");
                data_uart_print("%s", (p_cmd_table + i)->p_help);
                result = RESULT_SUCESS;
            }
            else
            {
                /* execute command function */
                result = (p_cmd_table + i)->func(p_parse_value);
            }
            /* exit while */
            break;
        }
        i++;
    };

    return result;
}

/**
 * @brief  Parse a command line and return the found command and parameters in "p_parse_value"
 *
 * @param p_user_cmd_if     Command parsed.
 * @param p_parse_value     Command parse value.
 * @return  Command parse result.
*/
static T_USER_CMD_PARSE_RESULT user_cmd_parse(T_USER_CMD_IF *p_user_cmd_if,
                                              T_USER_CMD_PARSED_VALUE *p_parse_value)
{
    int32_t i;
    char *p, *q;

    /* clear all results */
    p_parse_value->p_cmd       = NULL;
    p_parse_value->param_count = 0;
    for (i = 0 ; i < USER_CMD_MAX_PARAMETERS; i++)
    {
        p_parse_value->p_param[i]  = NULL;
        p_parse_value->dw_param[i] = 0;
    }

    /* Parse line */
    p = p_user_cmd_if->cmdline_buf;

    /*ignore leading spaces */
    p = user_cmd_skip_spaces(p);
    if (*p == '\0')                     /* empty command line ? */
    {
        return RESULT_CMD_EMPTY_LINE;
    }

    /* find end of word */
    q = user_cmd_find_end_of_word(p);
    if (p == q)                         /* empty command line ? */
    {
        return RESULT_CMD_EMPTY_LINE;
    }

    p_parse_value->p_cmd = p;
    *q = '\0';                        /* mark end of command */
    p = q + 1;

    /* parse parameters */
    if (*p != '\0')                   /* end of line ? */
    {
        int32_t j;

        j = 0;
        do
        {
            uint32_t d;
            /* ignore leading spaces */
            p = user_cmd_skip_spaces(p);
            d = user_cmd_str_to_uint32(p);

            p_parse_value->p_param[j]    = p;
            p_parse_value->dw_param[j++] = d;

            if (j >= USER_CMD_MAX_PARAMETERS)
            {
                break;
            }

            /* find next parameter */
            p  = user_cmd_find_end_of_word(p);
            *p++ = '\0';                        /* mark end of parameter */
        }
        while (*p != '\0');

        p_parse_value->param_count = j;
    }

    return RESULT_SUCESS;
}

/**
 * @brief  Clear command line buffer.
 *
 * @param p_user_cmd_if     Command parsed.
 * @return none.
*/
static void cmd_clear(T_USER_CMD_IF *p_user_cmd_if)
{
    p_user_cmd_if->accum_cmd_len = 0;
    p_user_cmd_if->cmd_cur = 0;
    memset(p_user_cmd_if->cmdline_buf, 0, sizeof(p_user_cmd_if->cmdline_buf));
}

static void cmd_move_back(T_USER_CMD_IF *p_user_cmd_if)
{
    for (uint8_t loop = 0; loop < p_user_cmd_if->accum_cmd_len - p_user_cmd_if->cmd_cur; loop ++)
    {
        p_user_cmd_if->cmdline_buf[p_user_cmd_if->accum_cmd_len - loop] =
            p_user_cmd_if->cmdline_buf[p_user_cmd_if->accum_cmd_len - loop - 1];
    }
}

static void cmd_move_forward(T_USER_CMD_IF *p_user_cmd_if)
{
    for (uint8_t loop = 0; loop < p_user_cmd_if->accum_cmd_len - p_user_cmd_if->cmd_cur; loop ++)
    {
        p_user_cmd_if->cmdline_buf[p_user_cmd_if->cmd_cur + loop - 1] =
            p_user_cmd_if->cmdline_buf[p_user_cmd_if->cmd_cur + loop];
    }
}
static void cmd_clear_screen(T_USER_CMD_IF *p_user_cmd_if)
{
    if (p_user_cmd_if->cmd_cur < p_user_cmd_if->accum_cmd_len)
    {
        data_uart_print("%s", p_user_cmd_if->cmdline_buf + p_user_cmd_if->cmd_cur);
    }

    while (p_user_cmd_if->accum_cmd_len != 0)
    {
        p_user_cmd_if->accum_cmd_len--;
        p_user_cmd_if->cmdline_buf[p_user_cmd_if->accum_cmd_len] = '\0';
        data_uart_print("\b \b");
    }
    p_user_cmd_if->cmd_cur = 0;
}

/**
 * @brief  Collect command characters.
 *
 * @param[in] p_user_cmd_if   Store parsed commands.
 * @param[in] p_data          Data to be parsed.
 * @param[in] len             Length of data to be command parsed.
 * @param[in] p_cmd_table     Command table to execute function.
 * @return  Command collect result.
 * @retval 1 true.
 * @retval 0 false.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_io_msg(T_IO_MSG io_msg)
    {
        uint16_t msg_type = io_msg.type;
        uint8_t rx_char;

        switch (msg_type)
        {
        case IO_MSG_TYPE_UART:
            // We handle user command informations from Data UART in this branch.
            rx_char = (uint8_t)io_msg.subtype;
            user_cmd_collect(&user_cmd_if, &rx_char, sizeof(rx_char), user_cmd_table);
            break;
        default:
            break;
        }
    }
 * \endcode
 */
bool user_cmd_collect(T_USER_CMD_IF *p_user_cmd_if, uint8_t *p_data, int32_t len,
                      const T_USER_CMD_TABLE_ENTRY *p_cmd_table)
{
    T_USER_CMD_PARSED_VALUE parse_result;

    while (len--)
    {
        char c = *p_data++;

        if (c != 0x0)                   /* not ESC character received */
        {
            switch (c)                  /* Normal handling */
            {
            case '\n':
            case '\r':                  /* end of line */
                data_uart_print("\r\n");
                p_user_cmd_if->history_cur = USER_CMD_MAX_HISTORY_LINE;

                if (p_user_cmd_if->accum_cmd_len > 0)  /* at least one character in command line ? */
                {
                    T_USER_CMD_PARSE_RESULT result;

                    // save cmd first
                    if (p_user_cmd_if->history_head == USER_CMD_MAX_HISTORY_LINE)
                    {
                        p_user_cmd_if->history_head = 0;
                        p_user_cmd_if->history_tail = 0;
                    }
                    else
                    {
                        p_user_cmd_if->history_tail = (p_user_cmd_if->history_tail + 1) % USER_CMD_MAX_HISTORY_LINE;
                        if (p_user_cmd_if->history_tail == p_user_cmd_if->history_head)
                        {
                            p_user_cmd_if->history_head = (p_user_cmd_if->history_head + 1) % USER_CMD_MAX_HISTORY_LINE;
                        }
                    }
                    p_user_cmd_if->cmd_history_len[p_user_cmd_if->history_tail] = p_user_cmd_if->accum_cmd_len;
                    memcpy(p_user_cmd_if->cmd_history[p_user_cmd_if->history_tail], p_user_cmd_if->cmdline_buf,
                           p_user_cmd_if->accum_cmd_len);

                    p_user_cmd_if->cmdline_buf[p_user_cmd_if->accum_cmd_len] = '\0';
                    result = user_cmd_parse(p_user_cmd_if, &parse_result);
                    if (result == RESULT_SUCESS)
                    {
                        result = user_cmd_execute(&parse_result, p_cmd_table);
                    }

                    if (result != RESULT_SUCESS)
                    {
                        cmd_send_result(result);
                    }
                }

                cmd_clear(p_user_cmd_if);
                break;

            case '\b':                        /* backspace */
                if (p_user_cmd_if->accum_cmd_len > 0 && p_user_cmd_if->cmd_cur > 0)
                {
                    uint8_t loop;

                    cmd_move_forward(p_user_cmd_if);
                    p_user_cmd_if->accum_cmd_len--;
                    p_user_cmd_if->cmd_cur--;
                    p_user_cmd_if->cmdline_buf[p_user_cmd_if->accum_cmd_len] = '\0';
                    data_uart_print("\b%s", p_user_cmd_if->cmdline_buf + p_user_cmd_if->cmd_cur);
                    data_uart_print(" \b");
                    for (loop = 0; loop < p_user_cmd_if->accum_cmd_len - p_user_cmd_if->cmd_cur; loop++)
                    {
                        data_uart_print("\b");
                    }
                }
                break;

            case 44:                            /* up: < */
                if (p_user_cmd_if->history_head != USER_CMD_MAX_HISTORY_LINE)
                {
                    cmd_clear_screen(p_user_cmd_if);
                    if (p_user_cmd_if->history_cur == USER_CMD_MAX_HISTORY_LINE)
                    {
                        p_user_cmd_if->history_cur = p_user_cmd_if->history_tail;
                    }
                    else
                    {
                        if (p_user_cmd_if->history_cur != p_user_cmd_if->history_head)
                        {
                            p_user_cmd_if->history_cur = (p_user_cmd_if->history_cur + USER_CMD_MAX_HISTORY_LINE - 1) %
                                                         USER_CMD_MAX_HISTORY_LINE;
                        }
                        else
                        {
                            p_user_cmd_if->history_cur = USER_CMD_MAX_HISTORY_LINE;
                            break;
                        }
                    }
                    p_user_cmd_if->accum_cmd_len = p_user_cmd_if->cmd_history_len[p_user_cmd_if->history_cur];
                    p_user_cmd_if->cmd_cur = p_user_cmd_if->accum_cmd_len;
                    memcpy(p_user_cmd_if->cmdline_buf, p_user_cmd_if->cmd_history[p_user_cmd_if->history_cur],
                           p_user_cmd_if->accum_cmd_len);
                    data_uart_print("%s", p_user_cmd_if->cmdline_buf);
                }
                break;

            case 46:                            /* down: > */
                if (p_user_cmd_if->history_head != USER_CMD_MAX_HISTORY_LINE)
                {
                    cmd_clear_screen(p_user_cmd_if);
                    if (p_user_cmd_if->history_cur == USER_CMD_MAX_HISTORY_LINE)
                    {
                        p_user_cmd_if->history_cur = p_user_cmd_if->history_head;
                    }
                    else
                    {
                        if (p_user_cmd_if->history_cur != p_user_cmd_if->history_tail)
                        {
                            p_user_cmd_if->history_cur = (p_user_cmd_if->history_cur + 1) % USER_CMD_MAX_HISTORY_LINE;
                        }
                        else
                        {
                            p_user_cmd_if->history_cur = USER_CMD_MAX_HISTORY_LINE;
                            break;
                        }
                    }
                    p_user_cmd_if->accum_cmd_len = p_user_cmd_if->cmd_history_len[p_user_cmd_if->history_cur];
                    p_user_cmd_if->cmd_cur = p_user_cmd_if->accum_cmd_len;
                    memcpy(p_user_cmd_if->cmdline_buf, p_user_cmd_if->cmd_history[p_user_cmd_if->history_cur],
                           p_user_cmd_if->accum_cmd_len);
                    data_uart_print("%s", p_user_cmd_if->cmdline_buf);
                }
                break;

            case 91:                           /* left: { */
                if (p_user_cmd_if->cmd_cur > 0)
                {
                    data_uart_print("\b");
                    p_user_cmd_if->cmd_cur--;
                }
                break;

            case 93:                           /* right: } */
                if (p_user_cmd_if->cmd_cur < p_user_cmd_if->accum_cmd_len)
                {
                    data_uart_print("%c", p_user_cmd_if->cmdline_buf[p_user_cmd_if->cmd_cur]);
                    p_user_cmd_if->cmd_cur++;
                }
                break;

            case 92:                            /* end: \ */
                if (p_user_cmd_if->cmd_cur < p_user_cmd_if->accum_cmd_len)
                {
                    data_uart_print("%s", p_user_cmd_if->cmdline_buf + p_user_cmd_if->cmd_cur);
                    p_user_cmd_if->cmd_cur = p_user_cmd_if->accum_cmd_len;
                }
                break;

            case 47:                            /* begin: / */
                while (p_user_cmd_if->cmd_cur > 0)
                {
                    data_uart_print("\b");
                    p_user_cmd_if->cmd_cur--;
                }
                break;

            default:
                /* Put character in command buffer */
                if (p_user_cmd_if->accum_cmd_len < USER_CMD_MAX_COMMAND_LINE)
                {
                    uint8_t loop;

                    cmd_move_back(p_user_cmd_if);
                    p_user_cmd_if->cmdline_buf[p_user_cmd_if->cmd_cur] = c;
                    data_uart_print("%s", p_user_cmd_if->cmdline_buf + p_user_cmd_if->cmd_cur);
                    p_user_cmd_if->accum_cmd_len++;
                    p_user_cmd_if->cmd_cur++;
                    for (loop = 0; loop < p_user_cmd_if->accum_cmd_len - p_user_cmd_if->cmd_cur; loop++)
                    {
                        data_uart_print("\b");
                    }
                }
                break;
            }
        }
    }

    return true;
}

/**
 * @brief  Initiate command interface structure
 * @param[in] p_user_cmd_if   Store parsed commands.
 * @param[in] project_name    Initiate project name.
 * @return void
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_main_task(void *p_param)
    {
        char event;

        os_msg_queue_create(&io_queue_handle, MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
        os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(unsigned char));

        gap_start_bt_stack(evt_queue_handle, io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);

        data_uart_init(evt_queue_handle, io_queue_handle);
        user_cmd_init(&user_cmd_if, "central");
        ......
    }
 * \endcode
 */
void user_cmd_init(T_USER_CMD_IF *p_user_cmd_if, char *project_name)
{
    memset(p_user_cmd_if, 0, sizeof(T_USER_CMD_IF));
    p_user_cmd_if->history_head = USER_CMD_MAX_HISTORY_LINE;
    p_user_cmd_if->history_tail = USER_CMD_MAX_HISTORY_LINE;
    p_user_cmd_if->history_cur  = USER_CMD_MAX_HISTORY_LINE;
    data_uart_print(">> Command Parse Init (%s) <<\r\n", project_name);
}

