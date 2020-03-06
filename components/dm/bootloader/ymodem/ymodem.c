
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "bootloader.h"

#define YMODEM_OK          0
#define YMODEM_ERR         (-1)
#define YMODEM_FILE_TOOBIG (-2)

#define YMODEM_SOH 0x01
#define YMODEM_STX 0x02
#define YMODEM_EOT 0x04
#define YMODEM_ACK 0x06
#define YMODEM_NAK 0x15
#define YMODEM_CAN 0x18
#define YMODEM_CCHAR 'C'
#define SOH_DATA_LEN 128
#define STX_DATA_LEN 1024

#define UART_RECV_TIMEOUT 400000

#define YMODEM_STATE_INIT      0
#define YMODEM_STATE_WAIT_HEAD 1
#define YMODEM_STATE_WAIT_DATA 2
#define YMODEM_STATE_WAIT_END  3
#define YMODEM_STATE_WAIT_NEXT 4

#define YMODEM_MAX_CHAR_NUM    64
#define YMODEM_ERR_NAK_NUM     5

static unsigned int ymodem_flash_addr     = 0;
static unsigned int ymodem_flash_size     = 0;
static unsigned int ymodem_max_write_size = 0;

unsigned int ymodem_str2int(char *buf, unsigned int buf_len)
{
    int type  = 10;
    int value = 0;
    int i     = 0;

    if((buf[0] == '0') && (buf[1] == 'x' || buf[1] == 'X')) {
        type = 16;
        buf += 2;
    }
    for (i = 0; i < buf_len; buf++, i++) {
        if(*buf == 0) {
            return value;
        }
        if (*buf >= '0' && *buf <= '9') {
            value = value * type + *buf - '0';
        } else {
            if(10 == type) {
                return value;
            }
            if (*buf >= 'A' && *buf <= 'F') {
                value = value * 16 + *buf - 'A' + 10;
            }
            else if (*buf >= 'a' && *buf <= 'f') {
                value = value * 16 + *buf - 'a' + 10;
            } else {
                return value;
            }
        }
    }
    return value;
}

unsigned int ymodem_recv_bytes(unsigned char *buffer, unsigned int nbytes, unsigned int timeout)
{
    int ret = 0;
    unsigned char c = 0;
    unsigned int  i = 0;
    unsigned int  t = 0;

    while ((i < nbytes) && (t < timeout)) {
        ret = uart_recv_byte(&c);
        if (1 == ret) {
            buffer[i] = c;
            i++;
        }
        t++;
    }
    return i;
}

int ymodem_data_head_parse(unsigned char data_type)
{
    int    i   = 0;
    int    ret = YMODEM_ERR;
    int    lp  = 0;
    unsigned int buf_len = 0;
    unsigned char *buffer = NULL;
    unsigned short crc    = 0;
    unsigned int   value  = 0;

    buf_len = ((YMODEM_SOH == data_type) ? SOH_DATA_LEN : STX_DATA_LEN) + 4;
    buffer  = malloc(buf_len);
    memset(buffer, 0, buf_len);
    /* SOH HEAD */
    value = ymodem_recv_bytes(buffer, buf_len, UART_RECV_TIMEOUT);
    if( (buf_len != value)  || (0 != buffer[0]) || (0xFF != buffer[1]) ) {
        goto err_exit;
    }

    /* check CRC */
    crc = crc16_computer(&buffer[2], buf_len-4);
    if (((crc >> 8) != buffer[buf_len - 2]) || ((crc & 0xFF) != buffer[buf_len - 1])) {
        goto err_exit;
    }
    /* parse file name && file length */
    for(i = 2; i < buf_len - 2; i++) {
        if((0 == buffer[i]) && (0 == lp)) {
            lp = i + 1;
            continue;
        }

        if((0 == buffer[i]) && (0 != lp)) {
            /* from buffer[lp] to buffer[i] is file length ascii */
            value = ymodem_str2int((char *)&buffer[lp], i - lp);
            if (0 == value) {
                goto err_exit;
            }
            ymodem_flash_size = value;
            if(value > ymodem_max_write_size) {
                ret = YMODEM_FILE_TOOBIG;
                goto err_exit;
            }
            break;
        }
    }

    for (i = 2; i < buf_len - 4; i ++) {
        if (buffer[i] != 0) {
            ret = YMODEM_OK;
        }
    }

err_exit:
    free(buffer);
    return ret;
}

int ymodem_write_data_to_flash(unsigned char *buffer, unsigned int addr, unsigned int len)
{
    int             ret = 0;
    uint32_t        offset = 0;
    hal_partition_t index;
    unsigned int    write_len = len;

    if(addr + len > ymodem_flash_addr + ymodem_flash_size) {
        write_len = ymodem_flash_addr + ymodem_flash_size - addr;
    }

    ret = flash_get_partition(addr, write_len, &index, &offset);
    if (ret != 0) {
        return -1;
    }

    return hal_flash_write(index, &offset, (const void *)buffer, write_len);
}

int ymodem_data_parse(unsigned char data_type, unsigned int *addr)
{
    int ret = YMODEM_ERR;
    unsigned int buf_len = 0;
    unsigned short crc    = 0;
    unsigned int   value  = 0;
    unsigned char *buffer = NULL;

    buf_len = ((YMODEM_SOH == data_type) ? SOH_DATA_LEN : STX_DATA_LEN) + 4;
    buffer = malloc(buf_len);
    memset(buffer, 0, buf_len);

    if(NULL == addr) {
        goto err_exit;
    }
    /* SOH HEAD */
    value = ymodem_recv_bytes(buffer, buf_len, UART_RECV_TIMEOUT);
    if ((buf_len != value) || (0xFF != buffer[0] + buffer[1])) {
        goto err_exit;
    }

    /* check CRC */
    crc = crc16_computer(&buffer[2], buf_len - 4);
    if (((crc >> 8) != buffer[buf_len - 2]) || ((crc & 0xFF) != buffer[buf_len - 1])) {
        goto err_exit;
    }

    /* write data fo flash */
    ymodem_write_data_to_flash(&buffer[2], *addr, buf_len - 4);
    *addr += buf_len - 4;
    ret = YMODEM_OK;

err_exit :
    free(buffer);
    return ret;
}

int ymodem_recv_file(unsigned int flash_addr)
{
    int i        = 0;
    int ret      = YMODEM_OK;
    int state    = 0;
    int end_flag = 0;
    unsigned char c     = 0;
    unsigned int  bytes = 0;
    unsigned int  addr  = flash_addr;

    /* send C */
    while (1) {
        if(state != YMODEM_STATE_INIT) {
            bytes = ymodem_recv_bytes(&c, 1, 5000);
        }

        switch (state)
        {
        case YMODEM_STATE_INIT: /* send 'C' */
            if(i % 500 == 0) {
                uart_send_byte(YMODEM_CCHAR);
            }
            state = YMODEM_STATE_WAIT_HEAD;
            break;

        case YMODEM_STATE_WAIT_HEAD: /* wait SOH */
            if(1 != bytes) {
                i ++;
                state = 0;
                break;
            }
            if(( YMODEM_SOH == c ) || ( YMODEM_STX == c )) {
                ret = ymodem_data_head_parse(c);
                if (ret == YMODEM_OK) {
                    uart_send_byte(YMODEM_ACK);
                    boot_delay(100);
                    uart_send_byte(YMODEM_CCHAR);
                    state = YMODEM_STATE_WAIT_DATA;
                    break;
                } else {
                    /* end */
                    uart_send_byte(YMODEM_ACK);
                    boot_delay(200);
                    if(end_flag == 1) {
                        ret = YMODEM_OK;
                    } else {
                        for(i = 0; i < YMODEM_ERR_NAK_NUM; i++) {
                            uart_send_byte(YMODEM_NAK);
                        }
                    }
                    return ret;
                }
            } else if (3 == c) {  /* ctrl+c abort ymodem */
                printf("Abort\n");
                return YMODEM_ERR;
            }
            break;
        case YMODEM_STATE_WAIT_DATA: /* receive data */
            if(1 == bytes) {
                if( (YMODEM_SOH == c) || (YMODEM_STX == c) ) {
                    ret = ymodem_data_parse(c, &addr);
                    if (ret == YMODEM_OK) {
                        uart_send_byte(YMODEM_ACK);
                    }
                } else if( YMODEM_EOT == c ) {
                    uart_send_byte(YMODEM_NAK);
                    state = YMODEM_STATE_WAIT_END;
                }
            }
            break;
        case YMODEM_STATE_WAIT_END: /* receive end eot */
            if ((1 == bytes) && (YMODEM_EOT == c)) {
                uart_send_byte(YMODEM_ACK);
                i     = 0;
                state = YMODEM_STATE_INIT;
                end_flag = 1;
            }
            break;
        default:
            state = YMODEM_STATE_INIT;
            break;
        }
    }

    return YMODEM_OK;
}

int ymodem_upgrade(void)
{
    int    i = 0;
    int  ret = 0;
    int flag = 0;
    unsigned char c   = 0;
    unsigned int addr = 0;
    char buf[YMODEM_MAX_CHAR_NUM];
    hal_logic_partition_t *app_info = flash_get_info(HAL_PARTITION_APPLICATION);
    hal_logic_partition_t *ota_info = flash_get_info(HAL_PARTITION_OTA_TEMP);
    if((app_info == NULL) || (ota_info == NULL)) {
        printf("\r\nGet Flash info err !\n");
        return -1;
    }
    printf("\r\nOS  part addr:0x%08x, len:0x%08x\n", app_info->partition_start_addr,
            app_info->partition_length);
    printf("\r\nOTA part addr:0x%08x, len:0x%x\n", ota_info->partition_start_addr,
            ota_info->partition_length);
    printf("\r\nOnly the addr of OS and OTA part are allowed to be written\n");
    printf("\r\nPlease input flash addr: ");
    memset(buf, 0, YMODEM_MAX_CHAR_NUM);
    while(1) {
        if(uart_recv_byte(&c)) {
            printf("%c", c);
            if((c == '\r') || (i >= YMODEM_MAX_CHAR_NUM)) {
                break;
            }
            buf[i] = c;
            i ++;
        }
    }
    printf("\r\nflash addr: %s \n", buf);
    addr = ymodem_str2int(buf, YMODEM_MAX_CHAR_NUM);
    if(addr == 0) {
        printf("\r\naddr is invalid!\n");
        return -1;
    }
    if((addr >= app_info->partition_start_addr) && (addr <= app_info->partition_start_addr + app_info->partition_length)) {
        flag = 1;
        ymodem_max_write_size = app_info->partition_length;
        ret = hal_flash_erase(HAL_PARTITION_APPLICATION, 0, app_info->partition_length);
        if(ret != 0) {
            printf("erase err.\n");
            return -1;
        }
    }
    if((addr >= ota_info->partition_start_addr) && (addr <= ota_info->partition_start_addr + ota_info->partition_length)){
        flag = 1;
        ymodem_max_write_size = ota_info->partition_length;
        ret = hal_flash_erase(HAL_PARTITION_OTA_TEMP, 0, ota_info->partition_length);
        if(ret != 0) {
            printf("erase err.\n");
            return -1;
        }
    }
    if(flag == 0) {
        printf("\r\naddr 0x%x is err \n", addr);
        return -1;
    }
    printf("\r\nPlease start ymodem ... (press ctrl+c to cancel)\r\n");
    ymodem_flash_addr = addr;
    ret = ymodem_recv_file(addr);
    while((1) && (ret != YMODEM_OK)) {
        if(uart_recv_byte(&c)) {
            break;
        }
    }
    if(ret == YMODEM_OK) {
        printf("\r\nRecv flash addr: 0x%08x, len:0x%08x OK \n", addr, ymodem_flash_size);
    } else if(ret == YMODEM_FILE_TOOBIG) {
        printf("\r\nfile too big len:0x%08x !!!\n", ymodem_flash_size);
    } else {
        printf("\r\nYmodem recv file Err:%d !!!\n", ret);
    }
    return ret;
}
