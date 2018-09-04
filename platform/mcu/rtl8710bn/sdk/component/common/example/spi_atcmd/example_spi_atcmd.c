/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/

#include <platform_opts.h>

#if CONFIG_EXAMPLE_SPI_ATCMD

#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "semphr.h"
#include "device.h"
#include "osdep_api.h"
#include "osdep_service.h"
#include "device_lock.h"

#include "spi_atcmd/example_spi_atcmd.h"

#include "at_cmd/log_service.h"
#include "at_cmd/atcmd_wifi.h"
#include "at_cmd/atcmd_lwip.h"

#include "flash_api.h"

#include "spi_api.h"
#include "spi_ex_api.h"

#include "gpio_irq_api.h"
#include "gpio_irq_ex_api.h"

/**** SPI FUNCTIONS ****/
spi_t spi_obj;
gpio_t gpio_cs;

#define SPI_RX_BUFFER_SIZE ATSTRING_LEN/2
#define SPI_TX_BUFFER_SIZE ATSTRING_LEN/2
uint16_t spi_chunk_buffer[ATSTRING_LEN/2];

_Sema master_rx_done_sema;
_Sema master_tx_done_sema;

#define SPI_USE_STREAM (0)
#define SPI_USE_DMA    (1)

/**** SLAVE HARDWARE READY ****/
gpio_t gpio_hrdy;

#define SPI_SLAVE_BUSY 0
#define SPI_SLAVE_READY 1

volatile int spi_slave_status = SPI_SLAVE_BUSY;
_Sema spi_check_hrdy_sema;

#define BLOCKING    1
#define NONBLOCKING 0

volatile int hrdy_pull_down_counter = 0;

/**** SLAVE SYNC ****/

gpio_irq_t gpio_sync;

#define SPI_STATE_MISO 0
#define SPI_STATE_MOSI 1

int spi_state = SPI_STATE_MISO;

/**** TASK THREAD ****/

_Sema spi_check_trx_sema;

/**** LOG SERVICE ****/
char at_string[ATSTRING_LEN];

extern char log_buf[LOG_SERVICE_BUFLEN];
extern xSemaphoreHandle log_rx_interrupt_sema;

#define LOG_TX_BUFFER_SIZE 48*1024
char log_tx_buffer[LOG_TX_BUFFER_SIZE];
uint32_t log_tx_idx = 0;
uint32_t log_rx_idx = 0;

/**** DATA FORMAT ****/
#define PREAMBLE_COMMAND     0x6000
#define PREAMBLE_DATA_READ   0x1000
#define PREAMBLE_DATA_WRITE  0x0000

#define COMMAND_DUMMY             0x0000
#define COMMAND_BEGIN             0x0304
#define COMMAND_END               0x0305
#define COMMAND_READ_BEGIN        0x0012
#define COMMAND_READ_RAW          0x0013
#define COMMAND_WRITE_BEGIN       0x0014
#define COMMAND_READ_WRITE_END    0x0015

#define REGISTER_ADDR        0x2000

void atcmd_update_partition_info(AT_PARTITION id, AT_PARTITION_OP ops, u8 *data, u16 len) {
	flash_t flash;
	int size, offset, i;
	u32 read_data;

	switch(id){
		case AT_PARTITION_SPI:
			size = SPI_CONF_DATA_SIZE;
			offset = SPI_CONF_DATA_OFFSET;
			break;
		case AT_PARTITION_WIFI:
			size = WIFI_CONF_DATA_SIZE;
			offset = WIFI_CONF_DATA_OFFSET;
			break;
		case AT_PARTITION_LWIP:
			size = LWIP_CONF_DATA_SIZE;
			offset = LWIP_CONF_DATA_OFFSET;
			break;
		case AT_PARTITION_ALL:
			size = 0x1000;
			offset = 0;
			break;
		default:
			printf("partition id is invalid!\r\n");
			return;
	}

	device_mutex_lock(RT_DEV_LOCK_FLASH);

	if(id == AT_PARTITION_ALL && ops == AT_PARTITION_ERASE){
		flash_erase_sector(&flash, SPI_SETTING_SECTOR);
		goto exit;
	}
	
	if(ops == AT_PARTITION_READ){
		flash_stream_read(&flash, SPI_SETTING_SECTOR+offset, len, data);
		goto exit;
	}

	//erase BACKUP_SECTOR
	flash_erase_sector(&flash, SPI_SETTING_BACKUP_SECTOR);

	if(ops == AT_PARTITION_WRITE){
		// backup new data
		flash_stream_write(&flash, SPI_SETTING_BACKUP_SECTOR+offset, len, data);
	}
	
	//backup front data to backup sector
	for(i = 0; i < offset; i += sizeof(read_data)){
		flash_read_word(&flash, SPI_SETTING_SECTOR + i, &read_data);
		flash_write_word(&flash, SPI_SETTING_BACKUP_SECTOR + i,read_data);
	}
	
	//backup rear data
	for(i = (offset + size); i < 0x1000; i += sizeof(read_data)){
		flash_read_word(&flash, SPI_SETTING_SECTOR + i, &read_data);
		flash_write_word(&flash, SPI_SETTING_BACKUP_SECTOR + i,read_data);
	}
	
	//erase UART_SETTING_SECTOR
	flash_erase_sector(&flash, SPI_SETTING_SECTOR);
	
	//retore data to UART_SETTING_SECTOR from UART_SETTING_BACKUP_SECTOR
	for(i = 0; i < 0x1000; i+= sizeof(read_data)){
		flash_read_word(&flash, SPI_SETTING_BACKUP_SECTOR + i, &read_data);
		flash_write_word(&flash, SPI_SETTING_SECTOR + i,read_data);
	}
	
	//erase BACKUP_SECTOR
	flash_erase_sector(&flash, SPI_SETTING_BACKUP_SECTOR);

exit:
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
	return;
}

/* AT cmd V2 API */
void spi_at_send_string(char *str) {
    spi_at_send_buf(str, strlen(str));
}

/* AT cmd V2 API */
void spi_at_send_buf(u8 *buf, u32 len) {
    int i;
    int spi_tx_tail_next;

	if( !len || (!buf) ){
		return;
	}

    if (buf == log_tx_buffer) {
        log_tx_idx = len;
    } else {
        for (i=0; i<len; i++) {
            if (log_tx_idx == LOG_TX_BUFFER_SIZE) {
                // overflow!
                break;
            }
            log_tx_buffer[log_tx_idx++] = buf[i];
        }
    }

    if (__get_IPSR() != 0) {
        RtlUpSema(&spi_check_trx_sema);
    } else {
        RtlUpSemaFromISR(&spi_check_trx_sema);
    }
}

/* IRQ handler called when SPI TX/RX finish */
void master_trx_done_callback(void *pdata, SpiIrq event) {
    switch(event){
        case SpiRxIrq:
            RtlUpSemaFromISR(&master_rx_done_sema);
            //DBG_8195A("Master RX done!\n");
            break;
        case SpiTxIrq:
            //DBG_8195A("Master TX done!\n");
            break;
        default:
            DBG_8195A("unknown interrput evnent!\n");
    }
}

/* IRQ handler called when SPI TX finish */
static void master_tx_done_callback(uint32_t id) {
	RtlUpSemaFromISR(&master_tx_done_sema);
}

/* IRQ handler as gpio hrdy hit rising edge */
void slave_hrdy_change_callback(uint32_t id) {
    gpio_irq_disable(&gpio_hrdy);

    if (spi_slave_status == SPI_SLAVE_BUSY) {
        // Transition from LOW to HIGH. Change to listen IRQ_LOW
        spi_slave_status = SPI_SLAVE_READY;
        hrdy_pull_down_counter++;

        gpio_irq_set(&gpio_hrdy, IRQ_LOW, 1);
        gpio_irq_enable(&gpio_hrdy);
    } else {
        // Transition from HIGH to LOW. Change to listen IRQ_HIGH
        spi_slave_status = SPI_SLAVE_BUSY;

        gpio_irq_set(&gpio_hrdy, IRQ_HIGH, 1);
        gpio_irq_enable(&gpio_hrdy);
    }

    RtlUpSemaFromISR(&spi_check_hrdy_sema);
}

/* IRQ Handler as gpio sync state change */
void slave_sync_chagne_callback(uint32_t id)
{
    gpio_irq_disable(&gpio_sync);

    if (spi_state == SPI_STATE_MISO) {
        // Transition from LOW to HIGH. Change to listen IRQ_LOW
        spi_state = SPI_STATE_MOSI;

        gpio_irq_set(&gpio_sync, IRQ_LOW, 1);
        gpio_irq_enable(&gpio_sync);
    } else {
        // Transition from HIGH to LOW. Change to listen IRQ_HIGH
        spi_state = SPI_STATE_MISO;

        gpio_irq_set(&gpio_sync, IRQ_HIGH, 1);
        gpio_irq_enable(&gpio_sync);
    }

    RtlUpSemaFromISR(&spi_check_trx_sema);
}

void spi_atcmd_main(void)
{
    wifi_disable_powersave();    

    // read settings
	SPI_LOG_CONF spiconf;
	spiconf.bits = 16;
    spiconf.frequency = 20000000;
    spiconf.mode = (SPI_SCLK_IDLE_LOW|SPI_SCLK_TOGGLE_MIDDLE);

    // init spi
    spi_init(&spi_obj, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
    spi_frequency(&spi_obj, spiconf.frequency);
    spi_format(&spi_obj, spiconf.bits, spiconf.mode, 0);

    spi_bus_tx_done_irq_hook(&spi_obj, master_tx_done_callback, (uint32_t)&spi_obj);
    spi_irq_hook(&spi_obj, master_trx_done_callback, (uint32_t)&spi_obj);

    // init simulated spi cs
    gpio_init(&gpio_cs, GPIO_CS);
    gpio_dir(&gpio_cs, PIN_OUTPUT);
    gpio_mode(&gpio_cs, PullNone);
    gpio_write(&gpio_cs, 1);

    // init gpio for check if spi slave hw ready 
    gpio_init(&gpio_hrdy, GPIO_HRDY);
    gpio_dir(&gpio_hrdy, PIN_INPUT);
    gpio_mode(&gpio_hrdy, PullDown);

    gpio_irq_init(&gpio_hrdy, GPIO_HRDY, slave_hrdy_change_callback, (uint32_t)&gpio_hrdy);
    gpio_irq_set(&gpio_hrdy, IRQ_HIGH, 1);
    gpio_irq_enable(&gpio_hrdy); 

    // init gpio for check if spi slave want to send data
    gpio_irq_init(&gpio_sync, GPIO_SYNC, slave_sync_chagne_callback,(uint32_t)&gpio_sync);
    gpio_irq_set(&gpio_sync, IRQ_HIGH, 1);
    gpio_irq_enable(&gpio_sync);

    // init semaphore for check hardware ready
    RtlInitSema(&spi_check_hrdy_sema, 1);
    RtlDownSema(&spi_check_hrdy_sema);

    // init semaphore that makes spi tx/rx thread to check something
    RtlInitSema(&spi_check_trx_sema, 1);
    RtlDownSema(&spi_check_trx_sema);

    // init semaphore for master tx
	RtlInitSema(&master_tx_done_sema, 1);
	RtlDownSema(&master_tx_done_sema);

    // init semaphore for master rx
    RtlInitSema(&master_rx_done_sema, 1);
    RtlDownSema(&master_rx_done_sema);
}

int32_t spi_master_send(spi_t *obj, char *tx_buffer, uint32_t length) {
    hrdy_pull_down_counter = 0;
    spi_master_write_stream_dma(obj, tx_buffer, length);
    RtlDownSema(&master_tx_done_sema);

    if (spi_slave_status == SPI_SLAVE_BUSY) {
        while (hrdy_pull_down_counter == 0);
        hrdy_pull_down_counter = 0;
    }
}

int32_t spi_master_recv(spi_t *obj, char *rx_buffer, uint32_t length) {
    hrdy_pull_down_counter = 0;
    spi_flush_rx_fifo(obj);
    spi_master_read_stream_dma(obj, rx_buffer, length);
    RtlDownSema(&master_rx_done_sema);
    RtlDownSema(&master_tx_done_sema);

    if (spi_slave_status == SPI_SLAVE_BUSY) {
        while (hrdy_pull_down_counter == 0);
        hrdy_pull_down_counter = 0;
    }
}

void atcmd_check_special_case(char *buf) {
    int i;
    if (strlen(buf) > 4) {
        if (strncmp(buf, "ATPT", 4) == 0) {
            for (i=0; i<strlen(buf); i++) {
                if (buf[i] == ':') {
                    buf[i] = '\0';
                    break;
                }
            }
        }
    }
}

static void spi_trx_thread(void *param)
{
    uint32_t i;
    uint32_t rxlen, txlen;
    uint32_t recv_len, recv_remain, send_len;

    uint16_t dummy, L_address, H_address, L_size, H_size;

    int slave_ready = 0;
    do {
        slave_ready = gpio_read(&gpio_hrdy);
        rtw_msleep_os(1000);
    } while(slave_ready == 0);

    while(1) {
        RtlDownSema(&spi_check_trx_sema);

        if (spi_state == SPI_STATE_MOSI) {
            if (log_tx_idx > 0) {
                /* Slave hw is ready, and Master has something to send. */

                // stage A, read target address
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_BEGIN;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                gpio_write(&gpio_cs, 0);
                spi_chunk_buffer[0] = PREAMBLE_DATA_READ;
                spi_master_send(&spi_obj, spi_chunk_buffer, 1 * 2);
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                dummy     = spi_chunk_buffer[0];
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                L_address = spi_chunk_buffer[0];
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                H_address = spi_chunk_buffer[0];
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                L_size    = spi_chunk_buffer[0];
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                H_size    = spi_chunk_buffer[0];
                gpio_write(&gpio_cs, 1);

                // stage B, write data
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_WRITE_BEGIN;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                if (log_tx_idx % 2 != 0) {
                    log_tx_buffer[log_tx_idx++] = 0;
                }
                send_len = log_tx_idx / 2;
                L_size = send_len & 0x0000FFFF;
                H_size = (send_len & 0xFFFF0000) >> 16;

                gpio_write(&gpio_cs, 0);
                txlen = 1;
                spi_chunk_buffer[0] = PREAMBLE_DATA_WRITE;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_chunk_buffer[0] = L_address;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_chunk_buffer[0] = H_address;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_chunk_buffer[0] = L_size;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_chunk_buffer[0] = H_size;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                gpio_write(&gpio_cs, 0);
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_DATA_WRITE;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                txlen = log_tx_idx/2;
                spi_master_send(&spi_obj, log_tx_buffer, txlen * 2); // sending raw data
                gpio_write(&gpio_cs, 1);

                // stage C, write data end
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_READ_WRITE_END;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                // stage final
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_END;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                L_size = log_tx_idx & 0x0000FFFF;
                H_size = (log_tx_idx & 0xFFFF0000) >> 16;

                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_DATA_WRITE;
                spi_chunk_buffer[txlen++] = L_size;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_DATA_WRITE;
                spi_chunk_buffer[txlen++] = H_size;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                // finalize
                log_tx_idx = 0;
            }

        } else if (spi_state == SPI_STATE_MISO) {
            /* Slave hw is ready, and Slave want to send something. */
            do {
                // stage A, read target address
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_BEGIN;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_DATA_READ;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                dummy     = spi_chunk_buffer[0];
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                L_address = spi_chunk_buffer[0];
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                H_address = spi_chunk_buffer[0];
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                L_size    = spi_chunk_buffer[0];
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2);
                H_size    = spi_chunk_buffer[0];
                gpio_write(&gpio_cs, 1);

                recv_len = ((H_size << 16) | L_size);

                if (recv_len == 0) {
                    break;
                }

                // Stage B, confirm addr & len
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_READ_BEGIN;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                gpio_write(&gpio_cs, 0);
                txlen = 1;

                spi_chunk_buffer[0] = PREAMBLE_DATA_WRITE;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_chunk_buffer[0] = L_address;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_chunk_buffer[0] = H_address;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_chunk_buffer[0] = L_size;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_chunk_buffer[0] = H_size;
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                // Stage C, begin to read
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_READ_RAW;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_DATA_READ;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                spi_master_recv(&spi_obj, spi_chunk_buffer, 1 * 2); // recv dummy
                rxlen = recv_len;
                spi_master_recv(&spi_obj, log_buf, rxlen * 2);
                log_buf[rxlen*2]= '\0';
                gpio_write(&gpio_cs, 1);

                // Stage D, read end
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_READ_WRITE_END;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                // stage final
                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_COMMAND;
                spi_chunk_buffer[txlen++] = COMMAND_END;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                L_size = (recv_len) & 0x0000FFFF;
                H_size = ((recv_len) & 0xFFFF0000) >> 16;

                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_DATA_WRITE;
                spi_chunk_buffer[txlen++] = L_size;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                txlen = 0;
                spi_chunk_buffer[txlen++] = PREAMBLE_DATA_WRITE;
                spi_chunk_buffer[txlen++] = H_size;

                gpio_write(&gpio_cs, 0);
                spi_master_send(&spi_obj, spi_chunk_buffer, txlen * 2);
                gpio_write(&gpio_cs, 1);

                // finalize
                //printf("%s", log_buf);
                atcmd_check_special_case(log_buf);
                RtlUpSema(&log_rx_interrupt_sema);
                taskYIELD();
            } while (0);
        }
    }

    vTaskDelete(NULL);
}

static void spi_atcmd_thread(void *param)
{
    p_wlan_init_done_callback = NULL;
    atcmd_wifi_restore_from_flash();
    atcmd_lwip_restore_from_flash();
    rtw_msleep_os(20);
    spi_atcmd_main();

    // the rx_buffer of atcmd is to receive and sending out to log_tx
    atcmd_lwip_set_rx_buffer(log_tx_buffer, sizeof(log_tx_buffer));

    at_set_debug_mask(0x0);

    if(xTaskCreate(spi_trx_thread, ((const char*)"spi_trx_thread"), 4096, NULL, tskIDLE_PRIORITY+1 , NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(spi_trx_thread) failed", __FUNCTION__);

    vTaskDelete(NULL);
}

int spi_atcmd_module_init(void){
    if(xTaskCreate(spi_atcmd_thread, ((const char*)"spi_atcmd_thread"), 1024, NULL, tskIDLE_PRIORITY+1 , NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(spi_atcmd_thread) failed", __FUNCTION__);
    return 0;
}

void example_spi_atcmd(void)
{
    p_wlan_init_done_callback = spi_atcmd_module_init;
    return;
}

#endif // #if CONFIG_EXAMPLE_SPI_ATCMD
