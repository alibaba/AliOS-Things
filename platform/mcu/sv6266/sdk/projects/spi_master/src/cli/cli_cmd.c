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
    
    if ((argc != 5) || (clk_MHz < 1) || (clk_MHz > 20) || (data_length > 1024) || (data_length < 1)|| (cpha > 1) || (cpol > 1)) {
        printf("Usage           : spi_dma_transfer <clk_Hz> <csn_gpio> <length> <cpha> <cpol>\n");
        printf("<clk_Hz>        : clk speed(1M-20M)\n");
        printf("<length>        : data length(1-1024)\n");
        printf("<cpha>          : CPHA(0, 1)\n");
        printf("<cpol>          : CPOL(0, 1)\n");
        return;
    }

    clk_MHz = clk_MHz * 1000000;
    
    for(i = 0; i < data_length; i ++) {
        write_buf[i] = i;
        read_buf[i] = 0;
    }

    write_buf[0] = 0x03;
    write_buf[1] = 0x00;
    write_buf[2] = 0x00;
    write_buf[3] = 0x00;
    
    ret = drv_spi_mst_init(clk_MHz, cpha, cpol);
    
    ret = drv_spi_mst_dma_trx(write_buf, read_buf, data_length, spi_trx_dma_done_isr);
    
    while (dma_trx_done != TRUE) {
        __nds32__isb();
    };

    for(i = 0; i < data_length; i ++) {
        //printf("<%s> %d : write_buf[%d] = 0x%x\n", __func__, __LINE__, i, write_buf[i]);
    }

    printf("======================================\n");

    for(i = 0; i < data_length; i ++) {
        printf("<%s> %d : read_data[%d] = 0x%x\n", __func__, __LINE__, i, read_buf[i]);
    }

    for(i = 0; i < data_length; i ++) {
        write_buf[i] = 0x5a;
    }

    if(0 != memcmp(write_buf, read_buf+4, data_length)){
        printf("\n[compare r/w data fail!]\n");
        printf("\n[SPI Master Test : FAIL!]\n");
    }else{
        printf("\n[compare r/w data success!]\n");
        printf("\n[SPI Master DMA Test : PASS!]\n");
    }
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
    printf ("       This function can test the SPI master to read flash data by dma mode.\n");
    printf ("Note:\n");
    printf ("       When csn_gpio change to other gpio, GPIO_13 can't do anything.\n");
    printf ("Options:\n");
    printf ("<clk_Hz>                       The PWM target number(0-4).\n");
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
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};


