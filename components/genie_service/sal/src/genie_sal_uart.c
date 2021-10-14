#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <hal/hal.h>
#include <k_api.h>
#include <aos/hal/uart.h>
#include <aos/kernel.h>

#define GENIE_MCU_UART_PORT (1)
#define GENIE_MCU_UART_BAUDRATE (9600)
#define FRAME_BUFFER_SIZE (256)
#define SEND_BUFFER_SIZE (64)
#define FRAME_TIEMOUT (50) //unit ms

static uart_dev_t mcu_uart, *p_mcu_uart = NULL;
static unsigned char frame_buff_index = 0;
static unsigned char frame_buff[FRAME_BUFFER_SIZE];
static aos_timer_t uart_frame_end_timer;

extern int genie_at_cmd_parser(char data[]);
extern int genie_bin_cmds_handle(uint8_t *p_data, uint8_t data_len);

static void uart_frame_end_timer_cb(void *arg1, void *arg2)
{
     if (frame_buff_index > 0) //received some datas
     {
#ifdef CONIFG_GENIE_MESH_BINARY_CMD
          genie_bin_cmds_handle(frame_buff, frame_buff_index);
#endif
#ifdef CONFIG_GENIE_MESH_AT_CMD
          genie_at_cmd_parser((char *)frame_buff);
#endif

          frame_buff_index = 0; //reset this index
          memset(frame_buff, 0, FRAME_BUFFER_SIZE);
     }
}

int genie_sal_uart_init(void)
{
     memset(frame_buff, 0, FRAME_BUFFER_SIZE);

     mcu_uart.port = GENIE_MCU_UART_PORT;
     mcu_uart.config.baud_rate = GENIE_MCU_UART_BAUDRATE;
     mcu_uart.config.mode = MODE_TX_RX;
     mcu_uart.config.flow_control = FLOW_CONTROL_DISABLED;
     mcu_uart.config.stop_bits = STOP_BITS_1;
     mcu_uart.config.parity = NO_PARITY;
     mcu_uart.config.data_width = DATA_WIDTH_8BIT;

     if (0 != hal_uart_init(&mcu_uart))
     {
          printf("uart(%d) init fail\r\n", GENIE_MCU_UART_PORT);
          return -1;
     }
     else
     {
          p_mcu_uart = &mcu_uart;
     }

     return aos_timer_new(&uart_frame_end_timer, uart_frame_end_timer_cb, NULL, FRAME_TIEMOUT, 0);
}

int32_t genie_sal_uart_send_one_byte(uint8_t byte)
{
     uint8_t send_data = 0;

     if (p_mcu_uart == NULL)
     {
          printf("uart(%d) not init\r\n", GENIE_MCU_UART_PORT);
          return -1;
     }

     send_data = byte;

     return hal_uart_send(p_mcu_uart, &send_data, 1, 0);
}

int32_t genie_sal_uart_send_str(const char *fmt, ...)
{
     int ret = 0;
     va_list params;
     int8_t send_buff[SEND_BUFFER_SIZE];

     if (p_mcu_uart == NULL)
     {
          printf("uart(%d) not init\r\n", GENIE_MCU_UART_PORT);
          return -1;
     }

     va_start(params, fmt);
     memset(send_buff, 0, SEND_BUFFER_SIZE);
     ret = vsnprintf((char *)send_buff, SEND_BUFFER_SIZE - 1, fmt, params);
     va_end(params);

     if (ret <= 0)
     {
          printf("uart(%d) send fail,no mem\r\n", GENIE_MCU_UART_PORT);
          return -1;
     }

     if (0 != hal_uart_send(p_mcu_uart, (void *)send_buff, (uint32_t)strlen((const char *)send_buff), HAL_WAIT_FOREVER))
     {
          printf("uart(%d) send fail\r\n", GENIE_MCU_UART_PORT);
          return -1;
     }

     return 0;
}

int genie_sal_uart_read(uint32_t timeout)
{
     int8_t read_buff;
     int32_t ret = 0;
     uint32_t recv_size = 0;

     if (p_mcu_uart == NULL)
     {
          printf("uart(%d) not init\r\n", GENIE_MCU_UART_PORT);
          return -1;
     }

     ret = hal_uart_recv_II(p_mcu_uart, &read_buff, 1, &recv_size, timeout);
     if ((ret == 0) && (recv_size == 1))
     {
          if (frame_buff_index > FRAME_BUFFER_SIZE)
          {
               frame_buff_index = 0;
          }

          frame_buff[frame_buff_index++] = read_buff;
          aos_timer_stop(&uart_frame_end_timer);
          aos_timer_start(&uart_frame_end_timer);
     }

     return 0;
}
