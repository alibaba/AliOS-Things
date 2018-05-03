#include <stdio.h>

#include "soc_types.h"
#include "cli.h"

#include "lmdma/drv_lmdma.h"

#define MAX_LENGTH              (65536UL)

#define SPI_FLASH_BASE_ADDRESS  (0x30000000UL)
/*---------------------------------- CMDs -----------------------------------*/
typedef struct _stParam
{
    char    *argv[10];
    int     argc;
} stParam;

extern stParam param;

static uint8_t gBuffer[MAX_LENGTH + 4] = {0};

static void lmdma (int32_t argc, char *argv[])
{
    int32_t i;
    int32_t retval;

    printf ("Local Memory DMA test program (Build: %s %s)\n", __DATE__, __TIME__);

    drv_lmdma_init ();

    retval = drv_lmdma_em2lmcpy32 (
        gBuffer,
        (void *) SPI_FLASH_BASE_ADDRESS,
        MAX_LENGTH,
        NULL
        );
    if (retval == 0) {
        printf ("The Local Memory DMA transfer is complete, and compares two memory buffers...\n");

        for (i = 0; i < MAX_LENGTH; i++) {
            if (*((volatile uint8_t *)(SPI_FLASH_BASE_ADDRESS + i)) != gBuffer[i]) {
                printf (
                    "The comparison failed: SpiFlash[0x%08lX] = 0x%02X ,gBuffer[0x%05lX] = 0x%02X\n",
                    (SPI_FLASH_BASE_ADDRESS + i),
                    *((volatile uint8_t *)(SPI_FLASH_BASE_ADDRESS + i)),
                    i,
                    gBuffer[i]
                    );
                printf ("Test result: Fail.\n");

                return;
            }
        }

        printf ("The comparison completed successfully! (Length = %ld)\n", i);
        printf ("Test result: Pass!\n");

    } else {
        printf ("The DMA transfer is not complete.\n");
        printf ("Test result: Fail.\n");

        return;
    }
}

static void help (int32_t argc, char *argv[])
{
    printf ("Usage: lmdma\n");
    printf ("Options: None\n");
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
extern void _notify_host_reset();
const CLICmds gCliCmdTable[] =
{
    {"help",            help,               "lmdma help"},
    {"lmdma",           lmdma,              "lmdma test"},
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};
