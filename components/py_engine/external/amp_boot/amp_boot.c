/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_boot.h"
#include "app_mgr.h"

#define AMP_BOOT_WAIT_TIME 500
#define AMP_BOOT_MAX_KV_LEN 256

#define AMP_BOOT_KV_VALUE_END_STR "[amp_value_end]\n"

static char g_pyamp_boot_sync[] = "amp_boot";
static char g_pyamp_boot_syncsd[] = "amp_boot_sd";

void pyamp_boot_init()
{
    // pyamp_boot_uart_init();
}

void pyamp_print_usage()
{
}

void pyamp_boot_delay(uint32_t ms)
{
    uint64_t begin_ms = aos_now_ms();

    while (1) {
        uint64_t now_ms = aos_now_ms();
        if (now_ms - begin_ms >= ms) {
            break;
        }
    }
}

void pyamp_boot_loop(int line)
{
    while (1) {
        pyamp_boot_delay(500);
    }
}

void pyamp_boot_flash_js()
{
    // pyamp_recovery_appbin();
}

void pyamp_boot_file_transfer()
{
    pyamp_recovery_file_transfer();
}

void pyamp_boot_flash_kv()
{
    char key[64] = { 0 };
    char kv_value_end_str[] = AMP_BOOT_KV_VALUE_END_STR;
    uint8_t *data = NULL;
    int num = 0;
    int ret = 0;

    pyamp_boot_delay(1);
    num = pyamp_boot_uart_recv_line(key, 64, 50000);
    if ((num == 0) || (num >= 64)) {
        aos_printf("recv key error %d\n", num);
        return;
    }
    pyamp_boot_delay(1);
    key[num] = '\0';
    aos_printf("[key]=%s\n", key);

    for (int i = 0; i < 1; i++) {
        pyamp_boot_uart_send_str("[value]=");
    }
    pyamp_boot_delay(2);
    aos_printf("[value]=");

    data = (uint8_t *)aos_malloc(AMP_BOOT_MAX_KV_LEN);
    if (data == NULL) {
        return;
    }
    memset(data, 0, AMP_BOOT_MAX_KV_LEN);
    num = 0;
    while (num < AMP_BOOT_MAX_KV_LEN) {
        unsigned char c = 0;
        if (1 == pyamp_boot_uart_recv_byte(&c)) {
            data[num] = (uint8_t)c;
            num++;
            if ((c == '\n') && (num > strlen(kv_value_end_str))) {
                if (strncmp(kv_value_end_str, (char *)&data[num - strlen(kv_value_end_str)],
                            strlen(kv_value_end_str)) == 0) {
                    break;
                }
            }
        }

        pyamp_boot_delay(1);
    }
    aos_printf("\n");
    num -= strlen(kv_value_end_str);

    data[num] = 0;

    aos_printf("begin write kv %s, size %d value 0x%x, 0x%x 0x%x\n", key, num, (uint32_t)(*(uint16_t *)(data)),
               (uint32_t)data[0], (uint32_t)data[1]);

    ret = aos_kv_set(key, data, num, 1);
    if (ret == 0) {
        pyamp_boot_uart_send_str("[kvok]");
        aos_printf("write kv [%s] success\n", key);
    }
    aos_free(data);
}

void pyamp_boot_cli_menu()
{
    unsigned char c = 0;

    pyamp_print_usage();

    while (1) {
        int boot_cmd = 0;
        boot_cmd = pyamp_boot_get_cmd(5000);

        switch (boot_cmd) {
        case AMP_BOOT_CMD_NULL:
            aos_printf("no command 3 seconds \r\n");
            return;
            break;

        case AMP_BOOT_CMD_ERROR:
            aos_printf("command not supported  \r\n");
            break;

        case AMP_BOOT_CMD_EXIT:
            aos_printf("aos boot finished \r\n");
            return;

        case AMP_BOOT_CMD_FLASH_JS:
            pyamp_boot_cmd_begin(AMP_BOOT_CMD_FLASH_JS);
            pyamp_boot_flash_js();
            pyamp_boot_cmd_end(AMP_BOOT_CMD_FLASH_JS);
            break;

        case AMP_BOOT_CMD_FLASH_KV:
            pyamp_boot_cmd_begin(AMP_BOOT_CMD_FLASH_KV);
            pyamp_boot_flash_kv();
            pyamp_boot_cmd_end(AMP_BOOT_CMD_FLASH_KV);
            break;

        case AMP_BOOT_CMD_FILE_TRANSFER:
            // pyamp_boot_cmd_begin(AMP_BOOT_CMD_FILE_TRANSFER);
            pyamp_boot_file_transfer();
            // pyamp_boot_cmd_end(AMP_BOOT_CMD_FILE_TRANSFER);
            break;

        default:
            pyamp_print_usage();
            return;
        }
        // aos_printf("\r\namp boot# ");
    }
}

#define YMODEM_PATH_TRANSFER_OK 0
#define YMODEM_PATH_TRANSFER_ERR (-1)

#define YMODEM_PATH_TRANSFER_SOH 0x01
#define YMODEM_PATH_TRANSFER_STX 0x02
#define YMODEM_PATH_TRANSFER_EOT 0x04
#define YMODEM_PATH_TRANSFER_ACK 0x06

#define YMODEM_PATH_TRANSFER_INIT 0
#define YMODEM_PATH_TRANSFER_WAIT_DATA 1
#define YMODEM_PATH_TRANSFER_WAIT_END 2

bool pyamp_boot_cli_receive_path()
{
    unsigned char c = 0;
    int state = 0;
    unsigned int bytes = 0;
    unsigned int buf_len = 0;
    unsigned int value = 0;
    unsigned char *buffer = NULL;
    int ret = 0;

    buf_len = 256;
    buffer = aos_malloc(buf_len);
    memset(buffer, 0, buf_len);

    /* send C */
    while (1) {
        pyamp_boot_delay(10);

        switch (state) {
        case YMODEM_PATH_TRANSFER_INIT:
            // wait read start flag
            bytes = pyamp_ymodem_recv_bytes(&c, 1, 50000);
            if ((YMODEM_PATH_TRANSFER_SOH == c) || (YMODEM_PATH_TRANSFER_STX == c)) {
                pyamp_boot_uart_send_byte(YMODEM_PATH_TRANSFER_ACK);
                pyamp_boot_delay(100);
                state = YMODEM_PATH_TRANSFER_WAIT_DATA;
            }

            break;

        case YMODEM_PATH_TRANSFER_WAIT_DATA:
            value = pyamp_ymodem_recv_bytes(buffer, buf_len, 500000);
            // save value & set status
            if (value >= 0) {
                ret = strcmp(buffer, "NULL");
                if (ret != 0) {
                    pyamp_ymodem_save_file_path(buffer, value);
                    pyamp_boot_uart_send_byte(YMODEM_PATH_TRANSFER_ACK);
                    state = YMODEM_PATH_TRANSFER_WAIT_END;
                } else {
                    // reset value and use defalut path
                    value = 0;
                    pyamp_ymodem_save_file_path(buffer, 0);
                    pyamp_boot_uart_send_byte(YMODEM_PATH_TRANSFER_ACK);
                    state = YMODEM_PATH_TRANSFER_WAIT_END;
                }
            } else {
                pyamp_boot_uart_send_byte(YMODEM_PATH_TRANSFER_EOT);
                state = YMODEM_PATH_TRANSFER_INIT;
                return YMODEM_PATH_TRANSFER_ERR;
            }

            break;

        case YMODEM_PATH_TRANSFER_WAIT_END:
            bytes = pyamp_ymodem_recv_bytes(&c, 1, 50000);
            if (YMODEM_PATH_TRANSFER_EOT == c) {
                state = YMODEM_PATH_TRANSFER_INIT;
                // check the path is ok
                if (value > 0) {
                    ret = pyamp_ymodem_check_file_path(buffer, value);
                    if (ret == 0) {
                        pyamp_boot_uart_send_byte(YMODEM_PATH_TRANSFER_ACK);
                        return YMODEM_PATH_TRANSFER_OK;
                    } else {
                        pyamp_boot_uart_send_byte(YMODEM_PATH_TRANSFER_EOT);
                        return YMODEM_PATH_TRANSFER_ERR;
                    }
                } else {
                    // use default path, no need checked
                    pyamp_boot_uart_send_byte(YMODEM_PATH_TRANSFER_ACK);
                    return YMODEM_PATH_TRANSFER_OK;
                }
            } else {
                state = YMODEM_PATH_TRANSFER_INIT;
                return YMODEM_PATH_TRANSFER_ERR;
            }

            break;

        default:
            state = YMODEM_PATH_TRANSFER_INIT;
            break;
        }
    }

    return YMODEM_PATH_TRANSFER_OK;
}

bool pyamp_boot_cli_in()
{
    int begin_num = 0;
    int i = 0;
    int use_sdpath = 0;
    int sdpath = 0;
    unsigned char c = 0;
    uint64_t begin_time = aos_now_ms();

    pyamp_boot_uart_send_str("amp shakehand begin...\n");

    while (1) {
        c = 0;
        if ((pyamp_boot_uart_recv_byte(&c) != 1) || (c == 0)) {
            pyamp_boot_delay(1);

            if ((aos_now_ms() - begin_time) > AMP_BOOT_WAIT_TIME) {
                return false;
            }
            continue;
        }

        if (c == 0xA5) {
            begin_num++;
            pyamp_boot_uart_send_byte(0x5A);
            // reset begin time
            begin_time = aos_now_ms();
        } else {
            begin_num = 0;
        }

        if (begin_num == 4) {
            break;
        }
    }
    // reset begin time
    begin_time = aos_now_ms();

    // judge file save path
    int state = 0;
    unsigned int bytes = 0;
    unsigned int buf_len = 0;
    unsigned int value = 0;
    unsigned char *buffer = NULL;
    int ret = 0;

    buf_len = 32;
    buffer = aos_malloc(buf_len);
    memset(buffer, 0, buf_len);

    while (1) {
        value = pyamp_ymodem_recv_bytes(buffer, buf_len, 300000);
        if (value != 0) {
            ret = strcmp(buffer, g_pyamp_boot_syncsd);
            if (ret == 0) {
                pyamp_boot_uart_send_str("received save sdcard\t\n");
                pyamp_save_path_defult_set(PYAMP_SAVE_PATH_DEFULT_SDCARD);
                break;
            }
            ret = strcmp(buffer, g_pyamp_boot_sync);
            if (ret == 0) {
                pyamp_boot_uart_send_str("received save local\t\n");
                pyamp_save_path_defult_set(PYAMP_SAVE_PATH_DEFULT_DATA);
                break;
            }
        } else {
            pyamp_boot_uart_send_str("received NULL\t\n");
            pyamp_save_path_defult_set(PYAMP_SAVE_PATH_DEFULT_DATA);
        }
    }

    pyamp_boot_uart_send_str("amp shakehand success\n");

    return true;
}

void pyamp_boot_main()
{
    int ret = 0;
    unsigned char c = 0;
    unsigned int i = 0;

#ifdef SUPPORT_SET_DRIVER_TRACE_FLAG
    aos_set_driver_trace_flag(0);
#endif

    pyamp_boot_init();

    if (pyamp_boot_cli_in() == false) {
#ifdef SUPPORT_SET_DRIVER_TRACE_FLAG
        aos_set_driver_trace_flag(1);
#endif
        return;
    }

    // ret = pyamp_boot_cli_receive_path();
    //     if(ret != 0){
    // #ifdef SUPPORT_SET_DRIVER_TRACE_FLAG
    //         aos_set_driver_trace_flag(1);
    // #endif
    //         return;
    //     }

    pyamp_boot_cli_menu();
#ifdef SUPPORT_SET_DRIVER_TRACE_FLAG
    aos_set_driver_trace_flag(1);
#endif
    return;
}
