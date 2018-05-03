#include <stdio.h>

#include "soc_types.h"
#include "cli.h"

#include "dmac/drv_dmac.h"

#define MAX_BLOCK               (0xFFFUL)
#define MAX_COUNT               (MAX_BLOCK << 2)
#define MAX_LENGTH              (MAX_COUNT << 2)

#define SPI_FLASH_BASE_ADDRESS  (0x30000000UL)
/*---------------------------------- CMDs -----------------------------------*/
typedef struct _stParam
{
    char    *argv[10];
    int     argc;
} stParam;

extern stParam param;

static uint8_t gBuffer[MAX_LENGTH + 4] = {0};

static void dmac (int32_t argc, char *argv[])
{
    int32_t i;
    int32_t j;
    int32_t retval;

    uint8_t *dst = gBuffer;
    volatile uint8_t *src = (volatile uint8_t *) SPI_FLASH_BASE_ADDRESS;

    printf ("DMAC test program (Build: %s %s)\n", __DATE__, __TIME__);

    drv_dmac_init ();

    for (i = 0;
        i < MAX_LENGTH;
        i += MAX_COUNT, dst += MAX_COUNT, src += MAX_COUNT) {

        retval = drv_dmac_memcpy32 (
            (void *) dst,
            (void *) src,
            MAX_COUNT,
            NULL
            );
        if (retval == 0) {
            printf ("The DMAC transfer is complete, and compares two memory buffers...\n");

            for (j = 0; j < MAX_COUNT; j++) {
                if (src[j] != dst[j]) {
                    printf (
                        "The comparison failed: SpiFlash[0x%p] = 0x%02X ,gBuffer[0x%p] = 0x%02X\n",
                        &src[j],
                        src[j],
                        &dst[j],
                        dst[j]
                        );
                    printf ("Test result: Fail.\n");

                    return;
                }
            }

            printf ("The comparison completed successfully! (Length = %ld)\n", i + MAX_COUNT);
            printf ("Test result: Pass!\n\n");

        } else {
            printf ("The DMAC transfer is not complete.\n");
            printf ("Test result: Fail.\n");

            return;
        }
    }
}

static void help (int32_t argc, char *argv[])
{
    printf ("Usage: dmac\n");
    printf ("Options: None\n");
}

/* ---------------------- Registered CMDs to CMD Table ---------------------- */
extern void _notify_host_reset();
const CLICmds gCliCmdTable[] =
{
    {"help",            help,               "dmac help"},
    {"dmac",            dmac,               "dmac test"},
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};
