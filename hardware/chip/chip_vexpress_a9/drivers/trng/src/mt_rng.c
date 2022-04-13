#include "FreeRTOS.h"
#include <driver_api.h>
#include <stdint.h>
#include <mt_printf.h>
#include <mt_reg_base.h>
#include "mt_rng.h"
#include "aos/cli.h"

int32_t hw_get_random_data(uint8_t *buf, uint32_t len)
{
    int32_t retval = 0;

    if (0 == len)
        return 0;

    if (NULL == buf) {
        PRINTF_E("[TRNG] Error: input buffer is NULL\n\n");
        return -1;
    }

    /* ungate TRNG clock */
    writel(TRNG_CG_CLR_REG, TRNG_CG_FIELD);

    /* start TRNG */
    if (TRNG_START != (readl(TRNG_CTRL_REG) & TRNG_START)) {
        writel(TRNG_CTRL_REG, TRNG_START);
        if (TRNG_START != (readl(TRNG_CTRL_REG) & TRNG_START)) {
            PRINTF_E("[TRNG] Error: fail to start TRNG\n");
            return -2;
        }
    }

    /* clear output buffer */
    memset(buf, 0, len);

    /* generate random data with default rings */
    while (len >= sizeof(uint32_t)) {
        if(TRNG_RDY != (readl(TRNG_CTRL_REG) & TRNG_RDY)) {
            //udelay(1);
            continue;
        }
        /* work around for random  data all zero */
        //udelay(10);
        *(uint32_t *)buf = readl(TRNG_DATA_REG);
        retval += sizeof(uint32_t);
        buf += sizeof(uint32_t);
        len -= sizeof(uint32_t);
    }

    /* stop */
    writel(TRNG_CTRL_REG, 0x0);
    /*gate TRNG clock*/
    writel(TRNG_CG_SET_REG, TRNG_CG_FIELD);
    return retval;
}
static int trng_cli(char *buf, int len, int argc, char **argv)
{
    int i;
    int num_bytes = 64;
    unsigned char random_bytes[num_bytes];
    hw_get_random_data(random_bytes, num_bytes);
    for(i = 0; i < num_bytes/4; i++)
        aos_cli_printf("rng = 0x%x\r\n", *((unsigned int *)random_bytes + i));
    return 0;
}
static struct cli_command trng_cli_cmd[] = {
    {"trng", "Commands to test trng", trng_cli},
};
void trng_cli_register(void)
{
    aos_cli_register_commands(&trng_cli_cmd[0],sizeof(trng_cli_cmd) / sizeof(struct cli_command));
}

