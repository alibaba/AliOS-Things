/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * POSIX getopt for Windows
 * Code given out at the 1985 UNIFORUM conference in Dallas.
 *
 * From std-unix@ut-sally.UUCP (Moderator, John Quarterman) Sun Nov  3 14:34:15 1985
 * Relay-Version: version B 2.10.3 4.3bsd-beta 6/6/85; site gatech.CSNET
 * Posting-Version: version B 2.10.2 9/18/84; site ut-sally.UUCP
 * Path: gatech!akgua!mhuxv!mhuxt!mhuxr!ulysses!allegra!mit-eddie!genrad!panda!talcott!harvard!seismo!ut-sally!std-unix
 * From: std-unix@ut-sally.UUCP (Moderator, John Quarterman)
 * Newsgroups: mod.std.unix
 * Subject: public domain AT&T getopt source
 * Message-ID: <3352@ut-sally.UUCP>
 * Date: 3 Nov 85 19:34:15 GMT
 * Date-Received: 4 Nov 85 12:25:09 GMT
 * Organization: IEEE/P1003 Portable Operating System Environment Committee
 * Lines: 91
 * Approved: jsq@ut-sally.UUC
 * Here's something you've all been waiting for:  the AT&T public domain
 * source for getopt(3).  It is the code which was given out at the 1985
 * UNIFORUM conference in Dallas.  I obtained it by electronic mail
 * directly from AT&T.  The people there assure me that it is indeed
 * in the public domain
 * There is no manual page.  That is because the one they gave out at
 * UNIFORUM was slightly different from the current System V Release 2
 * manual page.  The difference apparently involved a note about the
 * famous rules 5 and 6, recommending using white space between an option
 * and its first argument, and not grouping options that have arguments.
 * Getopt itself is currently lenient about both of these things White
 * space is allowed, but not mandatory, and the last option in a group can
 * have an argument.  That particular version of the man page evidently
 * has no official existence, and my source at AT&T did not send a copy.
 * The current SVR2 man page reflects the actual behavor of this getopt.
 * However, I am not about to post a copy of anything licensed by AT&T.
 */
#include "hal/ota.h"
#include "hal/soc/flash.h"
#include "fsl_debug_console.h"

int lpc54102_ota_init(hal_ota_module_t *m, void *something)
{
    PRINTF("lpc54102_ota_init\r\n");

    return 0;
}
int lpc54102_ota_write(hal_ota_module_t *m, volatile uint32_t *off_set,
				 uint8_t *in_buf , uint32_t in_buf_len)
{
    PRINTF("lpc54102_ota_write\r\n");
    return hal_flash_erase_write(HAL_PARTITION_APPLICATION, (uint32_t *)off_set, in_buf, in_buf_len);
}

int lpc54102_ota_read(hal_ota_module_t *m,  volatile uint32_t *off_set,
				uint8_t *out_buf , uint32_t out_buf_len)
{
    PRINTF("lpc54102_ota_read\r\n");
    return 0;
}

int lpc54102_ota_set_boot(hal_ota_module_t *m, void *something)
{
    PRINTF("lpc54102_ota_set_boot\r\n");
    return 0;
}


struct hal_ota_module_s hal_lpc54102_ota_module = {
    .init = lpc54102_ota_init,
    .ota_write = lpc54102_ota_write,
    .ota_read = lpc54102_ota_read,
    .ota_set_boot = lpc54102_ota_set_boot,
};


