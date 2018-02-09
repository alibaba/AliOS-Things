#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "basic_types.h"
#include "platform_opts.h"

#if CONFIG_EXAMPLE_INIC_GSPI_HOST

#include "src/include/autoconf.h"
#include "lwip_netconf.h"
#include "freertos/inic_intf.h"

#include "spi_api.h"
#include "spi_ex_api.h"

#ifdef CONFIG_RTL8711B
#define SPI0_MOSI  	PA_23
#define SPI0_MISO  	PA_22
#define SPI0_SCLK  	PA_18
#define SPI0_CS    	PA_19 // This pin is redundant
#define GPIO_CS		PA_19
#else
// SPI0
#define SPI0_MOSI  	PC_2
#define SPI0_MISO  	PC_3
#define SPI0_SCLK  	PC_1
#define SPI0_CS    	PC_0 // This pin is redundant			
#define GPIO_CS		PB_2
#endif

#define SPI0_FREQUENCY  20000000
// SPI master configuration
#define SPI_BITS		8 // Ameba SPI support 8bits and 16bits mode

static spi_t spi0_master;
static gpio_t gpio_cs;

volatile bool txDone = FALSE;
volatile bool rxDone = FALSE;

volatile bool txbusIdle = FALSE;

#define SLAVE_SELECT()		gpio_write(&gpio_cs, 0)
#define SLAVE_DESELECT()	gpio_write(&gpio_cs, 1)

int spi_transfer(uint8_t* buf, uint32_t buf_len)
{
    int i = 0;
	int ret = 0;
	SLAVE_SELECT();

	txbusIdle = FALSE; // ensure TX done 
	rxDone = FALSE; // ensure RX done

	if(spi_master_write_read_stream(&spi0_master, buf, buf, buf_len)!=0x00){
		ret = -1;
	}else{
		while((!txbusIdle) || (!rxDone)){
			wait_ms(1);
			if (++i > 2000) {
				printf("SPI write and read Timeout...\r\n");
				break;
			}
		}
	}
	
	/* Chip Select Pull High */
	SLAVE_DESELECT();
	return ret;
}


//  SPI master interrupt callback if use interrupt mode
void spi_tx_rx_intr_callback(void *pdata, SpiIrq event){

	switch(event){
		case SpiRxIrq:
			rxDone = TRUE;
			break;
		case SpiTxIrq:
			txDone = TRUE;
			break;
		default:
			printf("unknown interrput evnent!\n");
	} 
}

void spi_tx_bus_idle_callback(void *pdata, SpiIrq event){
	txbusIdle = TRUE;
}

void spi_init_master(){
	// init spi master 
	#ifdef CONFIG_RTL8711B
	spi0_master.spi_idx=MBED_SPI1;
	#endif
	spi_init(&spi0_master, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi0_master, SPI_BITS, 0, 0);
	spi_frequency(&spi0_master, SPI0_FREQUENCY);
	
	printf("spi master frequency %d Hz\n",SPI0_FREQUENCY);

	// init spi master tx/rx done interrupt
	spi_irq_hook(&spi0_master, (spi_irq_handler)spi_tx_rx_intr_callback, NULL);

	// init spi master tx bus idle interrupt
	spi_bus_tx_done_irq_hook(&spi0_master, (spi_irq_handler)spi_tx_bus_idle_callback, NULL);

	gpio_init(&gpio_cs, GPIO_CS);
	gpio_mode(&gpio_cs, PullDown);
	gpio_dir(&gpio_cs, PIN_OUTPUT);
        
	SLAVE_DESELECT(); // deselect slave
}


void example_inic_gspi_thread(void* param){
	int ret;
	char pbuf_SSID[] = "ATW0=bonjour";	// set your own SSID
	char pbuf_PASS[] = "****";			// set password
	char pbuf_CONN[] = "ATWC";
	printf("iNIC master example start...\n");

	/* init host spi contorller */
	spi_init_master();

	/* init inic driver */
	ret = rltk_inic_init();
	if(ret == 0){// inic init successfully
		rltk_inic_start();
	}
	/* start io control test*/
	// set SSID
	atcmd_ctl(pbuf_SSID, sizeof(pbuf_SSID));
	// set password
	//atcmd_ctl(pbuf_PASS, sizeof(pbuf_PASS));
	// start connecttion
	atcmd_ctl(pbuf_CONN, sizeof(pbuf_CONN));
fail:
	vTaskDelete(NULL);
}


void example_inic_gspi(void)
{
	if(xTaskCreate(example_inic_gspi_thread, ((const char*)"example_inic_gspi_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(example_inic_gspi_thread) failed", __FUNCTION__);
}
#endif
