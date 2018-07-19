//#define __SFILE__ "cli_cmd.c"
#include "soc_types.h"
#include "cli.h"
#include "cli_cmd.h"
#include "osal.h"
#include "spimst/drv_spimst.h"

//#include <regs.h>

//#include <regs.h>
//OsMutex g_spi_raw_rx_mutex;

#define SPI_RAW_DATA_GOIO_INT       (8)


/*---------------------------------- CMDs -----------------------------------*/


#define SPI_MAX_SIZE      (1024)
uint8_t write_buf[SPI_MAX_SIZE];
uint8_t read_buf[SPI_MAX_SIZE];
bool dma_trx_done = 0;

struct st_flash_cmd_buffer
{
	uint8_t cmd_buf[4];
	uint8_t data_buf[SPI_MAX_SIZE];

};
struct st_flash_cmd_buffer flash_rx_cmd_buffer, flash_tx_cmd_buffer;

void spi_trx_dma_done_isr(void) { 
    printf("spi_trx_dma_done\n");
    dma_trx_done = TRUE;
}

int test_spi_master_dma_data_transfer(s32 argc, s8 *argv[]) { 
	uint8_t ret = 0x0; 
	uint32_t i = 0;
	uint32_t data_length = 0;
	uint32_t clk_MHz = 0;
	uint8_t cpha = 0;
	uint8_t cpol = 0;

	dma_trx_done = FALSE;

	clk_MHz = strtoul(argv[1], NULL, 10);
	data_length = strtoul(argv[2], NULL, 10);
	cpha = strtoul(argv[3], NULL, 10);
	cpol = strtoul(argv[4], NULL, 10);

	if ((argc != 5) || (clk_MHz < 1) || (clk_MHz > 40) || (data_length > 256) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
		printf("Usage			: spi_dma_transfer <clk_Hz> <csn_gpio> <length> <cpha> <cpol>\n");
		printf("<clk_Hz>		: SPI Clock (BUS 40MHz : 1M - 20M, BUS 80MHz : 1M - 40M)\n");
		printf("<length>		: data length(1-256)\n");
		printf("<cpha>			: CPHA(0, 1)\n");
		printf("<cpol>			: CPOL(0, 1)\n");
		return;
	}

	clk_MHz = clk_MHz * 1000000;

	/* Initialize tx/rx buffer. */
	for(i = 0; i < data_length; i ++) {
		flash_tx_cmd_buffer.data_buf[i] = i;
		flash_rx_cmd_buffer.data_buf[i] = 0;
	}

	/* Initialize SPI HW. */
	ret = drv_spi_mst_init(clk_MHz, cpha, cpol);

	/* Set flash command : Read Manufacturer/Device ID. */
	flash_tx_cmd_buffer.cmd_buf[0] = 0x90;
	flash_tx_cmd_buffer.cmd_buf[1] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[2] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[3] = 0x00;
	/* Enable SPI HW. */
	ret = drv_spi_mst_dma_trx(flash_tx_cmd_buffer.cmd_buf, flash_rx_cmd_buffer.cmd_buf, 
										/* The size "+2" is for manufacturer ID and device ID. */
										(sizeof(flash_tx_cmd_buffer.cmd_buf)+2));

	printf("======================================\n");
	printf("Manufacturer ID:  0x%x\n", flash_rx_cmd_buffer.data_buf[0]);
	printf("Device ID:  0x%x\n", flash_rx_cmd_buffer.data_buf[1]);


	/* Set flash command : Write Enable. */
	flash_tx_cmd_buffer.cmd_buf[0] = 0x06;
	flash_tx_cmd_buffer.cmd_buf[1] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[2] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[3] = 0x00;
	/* Enable SPI HW. */
	ret = drv_spi_mst_dma_trx(flash_tx_cmd_buffer.cmd_buf, flash_rx_cmd_buffer.cmd_buf, 0x01);

	/* Set flash command : Sector Erase. */
	flash_tx_cmd_buffer.cmd_buf[0] = 0x20;
	flash_tx_cmd_buffer.cmd_buf[1] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[2] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[3] = 0x00;
	/* Enable SPI HW. */
	ret = drv_spi_mst_dma_trx(flash_tx_cmd_buffer.cmd_buf, flash_rx_cmd_buffer.cmd_buf, sizeof(flash_tx_cmd_buffer.cmd_buf));

	/* Wait for 1 second. */
	OS_MsDelay(1000);

	/* Set flash command : Write Enable. */
	flash_tx_cmd_buffer.cmd_buf[0] = 0x06;
	flash_tx_cmd_buffer.cmd_buf[1] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[2] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[3] = 0x00;
	/* Enable SPI HW. */
	ret = drv_spi_mst_dma_trx(flash_tx_cmd_buffer.cmd_buf, flash_rx_cmd_buffer.cmd_buf, 0x01);

	/* Set flash command : Page Program. */
	flash_tx_cmd_buffer.cmd_buf[0] = 0x02;
	flash_tx_cmd_buffer.cmd_buf[1] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[2] = 0x00;
	flash_tx_cmd_buffer.cmd_buf[3] = 0x00;
	/* Enable SPI HW. */
	ret = drv_spi_mst_dma_trx(flash_tx_cmd_buffer.cmd_buf, flash_rx_cmd_buffer.cmd_buf, (sizeof(flash_tx_cmd_buffer.cmd_buf)+data_length));

	/* Wait for 1 second. */
	OS_MsDelay(1000);

	/* Set flash command : Read Data. */
	flash_rx_cmd_buffer.cmd_buf[0] = 0x03;
	flash_rx_cmd_buffer.cmd_buf[1] = 0x00;
	flash_rx_cmd_buffer.cmd_buf[2] = 0x00;
	flash_rx_cmd_buffer.cmd_buf[3] = 0x00;
	/* Enable SPI HW. */
	ret = drv_spi_mst_dma_trx(flash_rx_cmd_buffer.cmd_buf, flash_rx_cmd_buffer.cmd_buf, (sizeof(flash_rx_cmd_buffer.cmd_buf)+data_length));

	printf("======================================\n");

	for(i = 0; i < data_length; i ++) {
		printf("<%s> %d : read_data[%d] = 0x%x\n", __func__, __LINE__, i, flash_rx_cmd_buffer.data_buf[i]);
	}

	if(0 != memcmp(flash_tx_cmd_buffer.data_buf, flash_rx_cmd_buffer.data_buf, data_length)){
		printf("\n[compare r/w data fail!]\n");
		printf("\n[SPI Master Test : FAIL!]\n");
	}else{
		printf("\n[compare r/w data success!]\n");
		printf("\n[SPI Master DMA Test : PASS!]\n");
	}
	return 0;
}


int test_spi_master_data_transfer(s32 argc, s8 *argv[]) { 
	uint8_t ret = 0x0; 
	uint32_t i = 0;
	uint32_t data_length = 0;
	uint32_t clk_MHz = 0;
	uint8_t cpha = 0;
	uint8_t cpol = 0;
	uint8_t csn = 0;

	clk_MHz = strtoul(argv[1], NULL, 10);
	data_length = strtoul(argv[2], NULL, 10);
	cpha = strtoul(argv[3], NULL, 10);
	cpol = strtoul(argv[4], NULL, 10);
	csn = strtoul(argv[5], NULL, 10);

	if ((argc != 6) || (clk_MHz < 1) || (clk_MHz > 40) || (data_length > 1024) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
		printf("Usage			: spi_transfer <clk_Hz> <csn_gpio> <length> <cpha> <cpol> <csn>\n");
		printf("<clk_Hz>		: SPI Clock (BUS 40MHz : 1M - 20M, BUS 80MHz : 1M - 40M)\n");
		printf("<length>		: data length(1-1024)\n");
		printf("<cpha>			: CPHA(0, 1)\n");
		printf("<cpol>			: CPOL(0, 1)\n");
		printf("<csn>			: Chip Select\n");
		return;
	}

	clk_MHz = clk_MHz * 1000000;

	/* Initialize tx/rx buffer. */
	for(i = 0; i < data_length; i ++) {
		write_buf[i] = i;
		read_buf[i] = 0;
	}

	/* Initialize SPI HW. */
	ret = drv_spi_mst_init(clk_MHz, cpha, cpol);

	/* Start SPI operation. */
	ret = drv_spi_mst_trx(write_buf, read_buf, data_length, csn);

	printf("\n[SPI Master Test : PASS!]\n");

	return 0;
}


static void Cmd_help (int32_t argc, char *argv[])
{
    printf ("\n*************************************************************************\n");
    printf ("Test SPI Target : \n");
    printf ("                   GPIO_08 : SPI_M_CLK\n");
    printf ("                   GPIO_10 : SPI_M_MISO\n");
    printf ("                   GPIO_12 : SPI_M_MOSI\n");
    printf ("                   GPIO_13 : SPI_M_CS\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       spi_dma_transfer  <clk_Hz> <length> <cpha> <cpol>\n");
    printf ("Brief:\n");
    printf ("       This function can test the SPI master to read flash data by dma mode.\n\n");
    
    printf ("Options:\n");
    printf ("<clk_Hz>                       SPI Clock (BUS 40MHz : 1M - 20M, BUS 80MHz : 1M - 40M)\n");
    printf ("<length>                       Data length(1-1024)\n");
    printf ("<cpha>                         CPHA(0, 1)\n");
    printf ("<cpol>                         CPOL(0, 1)\n");
    
}


/* ---------------------- Registered CMDs to CMD Table ---------------------- */
extern void _notify_host_reset();
const CLICmds gCliCmdTable[] =
{	
    { "help",                   Cmd_help,                           "SPI Master test help"          },
    { "spi_dma_transfer",       test_spi_master_dma_data_transfer,  "spi_dma_transfer <clk_Hz> <length> <cpha> <cpol>"      },
	{ "spi_transfer",           test_spi_master_data_transfer,  "spi_transfer <clk_Hz> <length> <cpha> <cpol> <csn>"      },
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};


