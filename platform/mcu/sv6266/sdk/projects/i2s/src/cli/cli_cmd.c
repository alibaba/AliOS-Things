//#define __SFILE__ "cli_cmd.c"
#include <stdio.h>
#include "soc_types.h"
#include "cli.h"
//#include "single_tone_wave.h"
//#include "cli_cmd.h"
#include "drv/i2s/drv_i2s.h"
#include "drv/i2s/hal_i2s.h"
#include "drv/spimst/drv_spimst.h"
#include "drv/dmac/drv_dmac.h"
#include "sys/intc.h"
#include "osal.h"
#include "raw_pcm.h"

uint8_t rx_buff[1024];
uint8_t tx_buff[1024];

OsBufQ xi2s_recv_buff = NULL;
OsEvent g_i2s_loopback_event;
OsEvent g_i2s_pcm_event;
OsTaskHandle i2s_loopback_handler;
OsTaskHandle i2s_pcm_handler;

void i2s_pcm_task(void *param)__attribute__((section(".prog_in_sram")));
void i2s_loopback_task(void *param)__attribute__((section(".prog_in_sram")));

/*---------------------------------- CMDs -----------------------------------*/

void i2s_codec_set_by_spi(i2s_mode_t Mode, sample_rate_t sample_rate, bit_resolution_t resolution) {
    
    uint32_t k = 0;
    uint32_t ahb_rdata;
    uint32_t clk_MHz = 0x0;
    
    uint8_t ret = 0x0; 
    uint8_t cpha = 0x0;
    uint8_t cpol = 0x0;
    uint8_t data_length = 0x0;
    uint8_t write_buf[4];
    uint8_t read_buf[4];

    clk_MHz = 400000;
    cpha = 0;
    cpol = 0;
    data_length = 2;

    ret = drv_spi_mst_init(clk_MHz, cpha, cpol);

    //RESET	
    write_buf[0] = 0x1e;
    write_buf[1] = 0x00;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
    //Power Down
    write_buf[0] = 0x0c;
    write_buf[1] = 0x00;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);

    //Interface Format
    if(mode_master == Mode) {
        if(Resolution_16bit == resolution) {
            //Interface Format:Slave,I2S  16bit
            write_buf[0] = 0x0e;
            write_buf[1] = 0x02;
        }
        else if(Resolution_24bit == resolution){
            //Interface Format:Slave,I2S  24bit
            write_buf[0] = 0x0e;
            write_buf[1] = 0x0a;
        }
        else {
            //Interface Format:Slave,I2S  32bit
            write_buf[0] = 0x0e;
            write_buf[1] = 0x0e;
        }
	} 
    else {			
		if(Resolution_16bit == resolution) {
            //Interface Format:Master,I2S  16bit
            write_buf[0] = 0x0e;
            write_buf[1] = 0x42;
        }
        else if(Resolution_24bit == resolution){
            //Interface Format:Master,I2S  24bit
            write_buf[0] = 0x0e;
            write_buf[1] = 0x4a;
        }
        else {
            //Interface Format:Master,I2S  32bit
            write_buf[0] = 0x0e;
            write_buf[1] = 0x4e;
        }
	}
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);

    //Sample Rate Ctl
    if(mode_master == Mode) {
		
		write_buf[0] = 0x10;
        write_buf[1] = 0x82;
	} 
    else {
        if(SampleRate_44kHZ == sample_rate) {
            write_buf[0] = 0x10;
            write_buf[1] = 0x22;
        }
        else {
            write_buf[0] = 0x10;
            write_buf[1] = 0x3e;
        }
	}
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
    //left ch headphone vol
    write_buf[0] = 0x04;
    write_buf[1] = 0xff;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
    //right ch headphone vol
    write_buf[0] = 0x06;
    write_buf[1] = 0xff;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
    //left input ch vol ctl
    write_buf[0] = 0x00;
    write_buf[1] = 0x1f;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
    //right input ch vol ctl
    write_buf[0] = 0x02;
    write_buf[1] = 0x1f;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
    //digital audio path ctl
    write_buf[0] = 0x0a;
    write_buf[1] = 0x06;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
    //analog audio path ctl
    write_buf[0] = 0x08;
    write_buf[1] = 0x10;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
    //Interface Activation
    write_buf[0] = 0x12;
    write_buf[1] = 0x01;
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length);
    
}

void i2s_loopback_task(void *param) {

    uint32_t i = 0;
    uint8_t wait_tick = 100;
    uint32_t byte_length = 960;

    while(1) {
        if (OS_EventWait(g_i2s_loopback_event, portMAX_DELAY) == OS_SUCCESS) {
            
            drv_i2s_rx_read_data(byte_length, rx_buff, wait_tick);
            memcpy(tx_buff, rx_buff, byte_length);
            drv_i2s_tx_write_data(byte_length, tx_buff, wait_tick);
            
            if(5000 == i) {
                printf("\n");
                OS_SysInfo();
                printf("\n");
                i = 0;
            }
            else {
                i++;
            }
            OS_EventSet(g_i2s_loopback_event);
        }
    }
}



void Cmd_i2s_loopback_start(s32 argc, char *argv[]) {
    printf("Cmd_i2s_init...%s %s\n" , __DATE__ , __TIME__);
    
    uint8_t val_1 = 0;
    uint8_t val_2 = 0;
    uint8_t val_3 = 0;
    uint32_t byte_length = 960;
    
    i2s_data_format_t data_format = DRV_I2S_PCM;
    i2s_transfer_mode_t transfer_mode = mode_trx;
    i2s_mode_t mode;
    sample_rate_t sample_rate = SampleRate_44kHZ;;
    bit_resolution_t resolution;

    val_1 = strtoul(argv[1], NULL, 10);
    val_2 = strtoul(argv[2], NULL, 10);

    if ((argc != 3) || (val_1 < 0) || (val_1 > 1) || (val_2 < 0) || (val_2 > 2)) {
        printf ("   loopback_init <mode> <resolution>\n");
        printf ("     <mode>                0:master  1:slave\n");
        printf ("     <resolution>          0:16bit   1:24bit   2:32bit\n");
        return;
    }
    
    mode = val_1;

    if(0 == val_2) {
        resolution = Resolution_16bit;
    }
    else if(1 == val_3){
        resolution = Resolution_24bit;
    }
    else {
        resolution = Resolution_32bit;
    }
    
    i2s_codec_set_by_spi(mode, sample_rate, resolution);

    OS_MsDelay(10);
    
    drv_i2s_init(data_format, transfer_mode, mode, sample_rate, resolution);
    drv_i2s_set_transfer_size(byte_length);
    drv_i2s_enable();
    
    OS_EventCreate(&g_i2s_loopback_event);
    
    if (!OS_TaskCreate(i2s_loopback_task, "i2s_loopback", 512, NULL, 3, &i2s_loopback_handler)) {
        printf("[%s] create i2s_loopback_test failed.\n", __func__);
        return -1;
    }
    
    OS_EventSet(g_i2s_loopback_event);
    
}


void Cmd_i2s_loopback_stop(s32 argc, char *argv[]) {
    printf("loopback_stop...%s %s\n" , __DATE__ , __TIME__);

    if(g_i2s_loopback_event != NULL) { 
        OS_EventDelete(g_i2s_loopback_event);
        g_i2s_loopback_event = NULL;
    }

    if(i2s_loopback_handler != NULL) {
    	OS_TaskDelete(i2s_loopback_handler);
		i2s_loopback_handler = NULL;
	}

    drv_i2s_disable();
}

void i2s_pcm_task(void *param) {

    uint32_t i = 0;
    uint32_t byte_length = 960;

    while(1) {
        if (OS_EventWait(g_i2s_pcm_event, portMAX_DELAY) == OS_SUCCESS) {
            for(i = 0; i < (_raw_pcm_raw_len - byte_length); i = i + byte_length ) {
                drv_i2s_tx_write_data(byte_length, (_raw_pcm_raw + i), 100);
            }
            OS_EventSet(g_i2s_pcm_event);
        }
    }
}

void Cmd_i2s_pcm_start(s32 argc, char *argv[]) {
    printf("Cmd_i2s_init...%s %s\n" , __DATE__ , __TIME__);

    uint8_t val_1 = 0;
    uint8_t val_2 = 0;
    uint32_t byte_length = 960;
    
    i2s_data_format_t data_format = DRV_I2S_PCM;
    i2s_transfer_mode_t transfer_mode = mode_tx;
    bit_resolution_t resolution = Resolution_24bit;
    sample_rate_t sample_rate = SampleRate_44kHZ;
    i2s_mode_t mode;

    val_1 = strtoul(argv[1], NULL, 10);

    if ((argc != 2) || (val_1 < 0) || (val_1 > 1)) {
        printf ("   pcm_init <mode> <sample_rate>\n");
        printf ("     <mode>            0:master  1:slave\n");
        return;
    }
    
    mode = val_1;
    
    i2s_codec_set_by_spi(mode, sample_rate, resolution);
    
    drv_i2s_init(data_format, transfer_mode, mode, sample_rate, resolution);
    drv_i2s_set_transfer_size(byte_length);
    drv_i2s_enable();

    OS_EventCreate(&g_i2s_pcm_event);
    
    if (!OS_TaskCreate(i2s_pcm_task, "i2s_pcm", 512, NULL, 1, &i2s_pcm_handler)) {
        printf("[%s] create i2s_loopback_test failed.\n", __func__);
        return -1;
    }

    OS_EventSet(g_i2s_pcm_event);
}


void Cmd_i2s_pcm_stop(s32 argc, char *argv[]) {
    printf("pcm_stop...%s %s\n" , __DATE__ , __TIME__);

    if(g_i2s_pcm_event != NULL) { 
        OS_EventDelete(g_i2s_pcm_event);
        g_i2s_pcm_event = NULL;
    }

    if(i2s_pcm_handler != NULL) {
    	OS_TaskDelete(i2s_pcm_handler);
		i2s_pcm_handler = NULL;
	}

    drv_i2s_disable();
}

void Cmd_i2s_sys_info(s32 argc, char *argv[]) {
    printf("loopback_start...%s %s\n" , __DATE__ , __TIME__);

    OS_SysInfo();
}


static void Cmd_help (int32_t argc, char *argv[])
{
    printf ("\n*************************************************************************\n");
    printf ("GPIO Setting : \n");
    printf ("       I2S : \n");
    printf ("           GPIO_05 - I2S0_BCLK\n");
    printf ("           GPIO_06 - I2S0_DI\n");
    printf ("           SIO_07  - I2S0_DO\n");
    printf ("           GPIO_09 - I2S0_LRCLK\n");
    printf ("           SIO_20  - I2S0_MCLK\n");
    printf ("       SPI_MST : \n");
    printf ("           GPIO_08 - SPI_M_CLK\n");
    printf ("           GPIO_10 - SPI_M_MISO\n");
    printf ("           GPIO_12 - SPI_M_MOSI\n");
    printf ("           GPIO_13 - SPI_M_CS\n");
    
    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       loopback_start <mode> <resolution>\n");
    printf ("brief:\n");
    printf ("       This function initializes the setting and start loopback(TRX) test.\n");
    printf ("Options:\n");
    printf ("   <mode>                          0:master  1:slave\n");
    printf ("   <resolution>                    0:16bit   1:24bit   2:32bit\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       loopback_stop \n");
    printf ("brief:\n");
    printf ("       This function sets loopback(TRX) test stop.\n");
    printf ("Options:\n");
    printf ("       None\n");
    
    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       pcm_start <mode>\n");
    printf ("brief:\n");
    printf ("       This function initializes the loopback(TRX) setting.\n");
    printf ("Options:\n");
    printf ("   <mode>                          0:master  1:slave\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       pcm_stop \n");
    printf ("brief:\n");
    printf ("       This function sets pcm(TX) test stop.\n");
    printf ("Options:\n");
    printf ("       None\n");
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
const CLICmds gCliCmdTable[] =
{
    
    { "help",                       Cmd_help,                           "i2s test help"                                     },
    { "loopback_start",             Cmd_i2s_loopback_start,             "loopback_start"                                    },
    { "loopback_stop",              Cmd_i2s_loopback_stop,              "loopback_stop"                                     },
    
    { "pcm_start",                  Cmd_i2s_pcm_start,                  "pcm_start"                                         },
    { "pcm_stop",                   Cmd_i2s_pcm_stop,                   "pcm_stop"                                          },
    //{ "OS_SysInfo",                 Cmd_i2s_sys_info,                   "OS_SysInfo"                                      },
    
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};



