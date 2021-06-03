#include <stdint.h>
#include <stdbool.h>
#include <aos/kernel.h>
#include <cli_console.h>
#include <aos/cli.h>
#include <sys/socket.h>
#include <lwip/apps/telnetserver.h>
#include <aos/hal/uart.h>

#define RECV_BUFF_SIZE    (1024)
typedef struct {
    uint32_t    write_idx;
    uint32_t    read_idx;
    aos_sem_t   telnet_rx_sem;
    uint8_t     data[RECV_BUFF_SIZE];
} telnet_buff;

static telnet_buff *telnet_recv_buf;


int telnet_write_to_buffer(char c)
{
    telnet_recv_buf->data[telnet_recv_buf->write_idx++] = c;
    if (telnet_recv_buf->write_idx >= RECV_BUFF_SIZE) {
        telnet_recv_buf->write_idx = 0;
    }
    aos_sem_signal(&telnet_recv_buf->telnet_rx_sem);
    return 1;
}

int telnet_read_from_buffer(char *c)
{
    if (telnet_recv_buf->read_idx == telnet_recv_buf->write_idx) {
        return 0;
    } else {
        *c = telnet_recv_buf->data[telnet_recv_buf->read_idx++];
        if (telnet_recv_buf->read_idx >= RECV_BUFF_SIZE) {
            telnet_recv_buf->read_idx = 0;
        }
    }
    return 1;
}

int telnet_console_write(const void *buf, size_t len, void *privata_data)
{
    char *ptr = (char *)buf;
    if (buf == NULL) {
        return 0;
    }
    ptr[len] = 0;
    TelnetWriteString((char *)buf);

    return len;
}


int telnet_console_read(void *buf, size_t len, void *privata_data)
{
    char *inbuf = (char *)buf;
    int ret;

    ret = telnet_read_from_buffer(inbuf);
    if (ret == 0) {
        aos_sem_wait(&telnet_recv_buf->telnet_rx_sem, 20);
        ret = telnet_read_from_buffer(inbuf);
    }
    return ret;
}

static int telnet_console_init(void *private_data)
{
    telnet_recv_buf = aos_zalloc(sizeof(telnet_buff));
    aos_sem_new(&telnet_recv_buf->telnet_rx_sem, 0);
    return 1;
}

static int telnet_console_deinit(void *private_data)
{
    if (telnet_recv_buf) {
        aos_sem_free(&telnet_recv_buf->telnet_rx_sem);
        aos_free(telnet_recv_buf);
        telnet_recv_buf = NULL;
    }
    return 1;
}

static device_console telnet_console = {
    .name = "telnet-console",
    .write = telnet_console_write,
    .read = telnet_console_read,
    .init = telnet_console_init,
    .deinit = telnet_console_deinit
};

cli_console cli_telnet_console = {
    .i_list = {0},
    .name = "cli-telnet",
    .dev_console = &telnet_console,
    .init_flag = 0,
    .exit_flag = 0,
    .alive = 1,
    .private_data = NULL,
    .cli_tag = {0},
    .cli_tag_len = 0,
    .task_list = {0},
    .finsh_callback = NULL,
    .start_callback = NULL,
};

int32_t telnet_console_create(int argc, char **argv)
{
    int32_t ret;
    ret = cli_console_task_create(&cli_telnet_console, CLI_CONFIG_STACK_SIZE, CLI_TASK_PRIORITY);
    if (ret != 0) {
        printf("Error: Failed to create telnet cli thread: %d\r\n", ret);
        return -1;
    }
    printf("success to create telnet cli thread: %d\r\n", ret);
    return 1;
}

int telnet_console_destory(void)
{
    cli_console_task_destory(&cli_telnet_console);
    printf("telnet_console_destory\r\n");
    return 0;
}

ALIOS_CLI_CMD_REGISTER(telnet_console_create, telnetcli, Console Telnet Create)
