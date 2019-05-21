#include "include.h"
#include "arm_arch.h"
#include "rwnx_config.h"
#include "app.h"
#include "target_util_pub.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "saradc_pub.h"
#include "uart_pub.h"
#include "sys_rtos.h"
#include "rtos_pub.h"
#include "error.h"
#include "fake_clock_pub.h"
#include "rw_pub.h"
#include "tftpclient.h"
#include "udp.h"
#include "flash_pub.h"
#include "fake_clock_pub.h"

#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#if CFG_SUPPORT_BOOTLOADER
extern void flash_protection_op(UINT8 mode,PROTECT_TYPE type);
#if (CFG_SUPPORT_OTA_TFTP || CFG_SUPPORT_OTA_HTTP)
SEND_PTK_HD send_hd, send_hd_bk;
static u32 os_data_addr = OS1_FLASH_ADDR;
uint32_t tftp_crc = 0;
IMG_HEAD img_hd = {0,};
#endif

#if CFG_SUPPORT_OTA_TFTP

beken_semaphore_t sm_tftp_server;
beken_timer_t tm_tftp_server;
beken_thread_t  tftp_thread_handle = NULL;

int udp_tftp_listen_fd = -1;
struct sockaddr_in server_addr;
int tftp_s_addr;
socklen_t s_addr_len = sizeof(server_addr);
char *tftp_buf = NULL;
char		BootFile[128] = "rwnx_fmac_crc.bin";		/* Boot File name			*/
static int	TftpServerPort;		/* The UDP port at their end		*/
static int	TftpOurPort;		/* The UDP port at our end		*/
static int	TftpTimeoutCount;
static uint64_t	TftpBlock;		/* packet sequence number		*/
static uint64_t	TftpLastBlock;		/* last packet sequence number received */
static uint64_t	TftpBlockWrap;		/* count of sequence number wraparounds */
static uint64_t	TftpBlockWrapOffset;	/* memory offset due to wrapping	*/
static int	TftpState;

static unsigned short TftpBlkSize = TFTP_BLOCK_SIZE;
static unsigned short TftpBlkSizeOption = TFTP_MTU_BLOCKSIZE;

void string_to_ip(char *s)
{
    tftp_s_addr = inet_addr(s);
    return ;
}

static void
TftpSend (void)
{
    uint8_t pkt_buf[700];
    volatile uint8_t 	*pkt;
    volatile uint8_t 	*xp;
    int			len = 0;
    volatile uint16_t *s;

    memset(pkt_buf, 0, 700);
    pkt = pkt_buf;

    TFTP_WARN("TftpState-- %d\r\n", TftpState);

    switch (TftpState)
    {

    case STATE_RRQ:
        xp = pkt;
        s = (uint16_t *)pkt;
        *s++ = htons(TFTP_RRQ);
        pkt = (uint8_t *)s;
        os_strcpy ((char *)pkt, BootFile);
        pkt += os_strlen(BootFile) + 1;
        os_strcpy ((char *)pkt, "octet");
        pkt += 5 /*strlen("octet")*/ + 1;
        os_strcpy ((char *)pkt, "timeout");
        pkt += 7 /*strlen("timeout")*/ + 1;
        sprintf((char *)pkt, "%lu", TIMEOUT);
#ifdef ET_DEBUG
        TFTP_PRT("send option \"timeout %s\"\n", (char *)pkt);
#endif
        pkt += os_strlen((char *)pkt) + 1;
        /* try for more effic. blk size */
        pkt += sprintf((char *)pkt, "blksize%c%d%c",
                       0, TftpBlkSizeOption, 0);

        len = pkt - xp;
        break;

    case STATE_OACK:
    case STATE_DATA:
        xp = pkt;
        s = (uint16_t *)pkt;
        *s++ = htons(TFTP_ACK);
        *s++ = htons(TftpBlock);
        pkt = (uint8_t *)s;
        len = pkt - xp;
        break;

    case STATE_TOO_LARGE:
        xp = pkt;
        s = (uint16_t *)pkt;
        *s++ = htons(TFTP_ERROR);
        *s++ = htons(3);
        pkt = (uint8_t *)s;
        os_strcpy ((char *)pkt, "File too large");
        pkt += 14 /*strlen("File too large")*/ + 1;
        len = pkt - xp;
        break;

    case STATE_BAD_MAGIC:
        xp = pkt;
        s = (uint16_t *)pkt;
        *s++ = htons(TFTP_ERROR);
        *s++ = htons(2);
        pkt = (uint8_t *)s;
        os_strcpy ((char *)pkt, "File has bad magic");
        pkt += 18 /*strlen("File has bad magic")*/ + 1;
        len = pkt - xp;
        break;
    }

    len = sendto( udp_tftp_listen_fd, pkt_buf, len, 0 , (struct sockaddr *) &server_addr, s_addr_len);
    TFTP_PRT( "Server p: %x\r\n", server_addr.sin_port );
}

void Tftp_Uninit(void)
{
    rtos_deinit_timer(&tm_tftp_server);
    close( udp_tftp_listen_fd );

    if(tftp_buf)
    {
        os_free(tftp_buf);
        tftp_buf = NULL;
    }

    if(tftp_thread_handle)
    {
        rtos_delete_thread(&tftp_thread_handle);
        tftp_thread_handle = 0;
    }
}

static void
TftpHandler (
    char *p, unsigned len, u16_t port)
{
    uint16_t proto;
    uint16_t *s;
    int i;
    volatile uint8_t 	*pkt;

    if (TftpState != STATE_RRQ && port != TftpServerPort)
    {
        os_printf("err %d %x %x\r\n", TftpState, port, TftpServerPort);
        return;
    }

    if (len < 2)
    {
        return;
    }
    len -= 2;
    /* warning: don't use increment (++) in ntohs() macros!! */
    s = (uint16_t *)p;
    proto = *s++;
    pkt = (uint8_t *)s;

    TFTP_WARN ("get proto:%d \r\n", proto);

    switch (ntohs(proto))
    {

    case TFTP_RRQ:
    case TFTP_WRQ:
    case TFTP_ACK:
        break;
    default:
        break;

    case TFTP_OACK:
#ifdef ET_DEBUG
        TFTP_PRT("Got OACK: %s %s\n", pkt, pkt + os_strlen((UINT8 *)pkt) + 1);
#endif
        TftpState = STATE_OACK;
        TftpServerPort = port;
        /*
         * Check for 'blksize' option.
         * Careful: "i" is signed, "len" is unsigned, thus
         * something like "len-8" may give a *huge* number
         */
        for (i = 0; i + 8 < len; i++)
        {
            if (os_strcmp ((char *)pkt + i, "blksize") == 0)
            {
                TftpBlkSize = (unsigned short)
                              os_strtoul((char *)pkt + i + 8, NULL, 10);
#ifdef ET_DEBUG
                TFTP_PRT ("Blocksize ack: %s, %d\n",
                          (char *)pkt + i + 8, TftpBlkSize);
#endif
                break;
            }
        }
        TftpSend (); /* Send ACK */
        break;
    case TFTP_DATA:
        if (len < 2)
            return;
        len -= 2;
        TftpBlock = ntohs(*(uint16_t *)pkt);

        /*
         * RFC1350 specifies that the first data packet will
         * have sequence number 1. If we receive a sequence
         * number of 0 this means that there was a wrap
         * around of the (16 bit) counter.
         */
        if (TftpBlock == 0)
        {
            TftpBlockWrap++;
            TftpBlockWrapOffset += TftpBlkSize * TFTP_SEQUENCE_SIZE;
            TFTP_PRT ("\n\t %lu MB received\n\t ", TftpBlockWrapOffset >> 20);
        }
        else
        {
            if (((TftpBlock - 1) % 10) == 0)
            {
                TFTP_PRT ("#");
            }
            else if ((TftpBlock % (10 * HASHES_PER_LINE)) == 0)
            {
                TFTP_PRT ("\n\t ");
            }
        }

#ifdef ET_DEBUG
        if (TftpState == STATE_RRQ)
        {
            TFTP_PRT ("Server did not acknowledge timeout option!\n");
        }
#endif

        if (TftpState == STATE_RRQ || TftpState == STATE_OACK)
        {
            /* first block received */
            TftpState = STATE_DATA;
            TftpServerPort = port;
            TftpLastBlock = 0;
            TftpBlockWrap = 0;
            TftpBlockWrapOffset = 0;

            if (TftpBlock != 1)  	/* Assertion */
            {
                TFTP_PRT ("\nTFTP error: "
                          "First block is not block 1 (%ld)\n"
                          "Starting again\n\n",
                          TftpBlock);
                break;
            }
        }

        if (TftpBlock == TftpLastBlock)
        {
            /*
             *	Same block again; ignore it.
             */
            TftpSend ();
            break;
        }
        TFTP_WARN("TftpState %d\r\n", TftpState);

        TftpLastBlock = TftpBlock;
        store_block (TftpBlock - 1, (UINT8 *)(pkt + 2), len);

        /*
         *	Acknoledge the block just received, which will prompt
         *	the server for the next one.
         */
        TftpSend ();

        if (len < TftpBlkSize)
        {
            /*
             *	We received the whole thing.  Try to
             *	run it.
             */
            os_printf ("\ntftp succeed\n");
            Tftp_Uninit();
        }
        break;

    case TFTP_ERROR:
        TFTP_PRT ("\nTFTP error: '%s' (%d)\n",
                  pkt + 2, ntohs(*(uint16_t *)pkt));
        TFTP_PRT ("Starting again\n\n");
        break;
    }
}

static void
TftpTimeout (void)
{
    err_t err;

    TFTP_PRT("------\r\n");
    if (++TftpTimeoutCount > TIMEOUT_COUNT)
    {
        TFTP_PRT ("\nRetry count exceeded; starting again\n");

    }
    else
    {
        os_printf ("T ");
        TftpSend ();
    }

}

void
TftpStart (void)
{
#ifdef CONFIG_TFTP_PORT
    char *ep;             /* Environment pointer */
#endif

    if (BootFile[0] == '\0')
    {
        TFTP_PRT ("*** Warning: no boot file name;\r\n");
    }

    TFTP_PRT ("Filename '%s'.\n", BootFile);
    TFTP_PRT ("Load addr: 0x%lx\n", OS1_FLASH_ADDR);
    TFTP_PRT ("Loading: *\b");

    tftp_crc = 0;
    TftpServerPort = WELL_KNOWN_PORT;
    TftpTimeoutCount = 0;
    TftpState = STATE_RRQ;
    /* Use a pseudo-random port unless a specific port is set */
    TftpOurPort = 1024 + (fclk_get_tick() % 3072);

#ifdef CONFIG_TFTP_PORT
    if ((ep = getenv("tftpdstp")) != NULL)
    {
        TftpServerPort = simple_strtol(ep, NULL, 10);
    }
    if ((ep = getenv("tftpsrcp")) != NULL)
    {
        TftpOurPort = simple_strtol(ep, NULL, 10);
    }
#endif
    TftpBlock = 0;
    /* Revert TftpBlkSize to dflt */
    TftpBlkSize = TFTP_BLOCK_SIZE;
}

void tftp_timer_callback(void *data)
{
    TftpTimeout();
}

void tftp_server_process( beken_thread_arg_t arg )
{
    int result;
    (void)( arg );

    OSStatus err = kNoErr;
    int len = 0;
    int i;

    tftp_buf = (char *) os_malloc( TFTP_LEN );
    if(tftp_buf == NULL)
    {
        os_printf("buf == NULL\r\n");
        goto exit;
    }

    udp_tftp_listen_fd = socket( AF_INET, SOCK_DGRAM, 0 );//Make UDP socket
    if(udp_tftp_listen_fd == -1)
    {
        os_printf("udp_listen_fd == -1\r\n");
        goto exit;
    }
    os_memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = tftp_s_addr;
    server_addr.sin_port = htons(WELL_KNOWN_PORT);

    TftpStart();

    result = rtos_init_timer(&tm_tftp_server,
                             TFTP_TIMER / FCLK_DURATION_MS,
                             tftp_timer_callback,
                             (void *)0);
    ASSERT(kNoErr == result);
    result = rtos_start_timer(&tm_tftp_server);
    ASSERT(kNoErr == result);
	flash_protection_op(FLASH_XTX_16M_SR_WRITE_ENABLE, FLASH_PROTECT_NONE);
	
    while ( 1 )
    {
        len = recvfrom( udp_tftp_listen_fd, tftp_buf, TFTP_LEN, 0 , (struct sockaddr *) &server_addr, &s_addr_len);
        TFTP_PRT( "Server port: %x len:%d\r\n", server_addr.sin_port, len );
        TftpHandler(tftp_buf, len, server_addr.sin_port);
    }
	
exit:
    if ( err != kNoErr ) 
		os_printf( "Server listener thread exit with err: %d", err );
		
    close( udp_tftp_listen_fd );
	
    if(tftp_buf)
        os_free(tftp_buf);
		
	flash_protection_op(FLASH_XTX_16M_SR_WRITE_ENABLE, FLASH_UNPROTECT_LAST_BLOCK);
    rtos_delete_thread(&tftp_thread_handle);
}

void tftp_start(void)
{
    UINT32 ret;
    
    Tftp_Uninit();

    TFTP_PRT("tftp c started\r\n");
    {
        rtos_init_semaphore(&sm_tftp_server, 10);

        /* Start ps server listener thread*/
        ret = rtos_create_thread( &tftp_thread_handle, BEKEN_APPLICATION_PRIORITY,
                                  "tftp_ota",
                                  (beken_thread_function_t)tftp_server_process,
                                  0x1000,
                                  0 );

        if (kNoErr != ret)
        {
            os_printf("Create power_sleep failed\r\n");
        }
    }

}
#endif

#if (CFG_SUPPORT_OTA_TFTP || CFG_SUPPORT_OTA_HTTP)
void store_block (unsigned block, uint8_t *src, unsigned len)
{
    uint8_t *f_data;
    UINT32 param , or_crc;
    UINT32 param1;


    TFTP_WARN ("p_len%d \r\n", len);

    os_memcpy(&send_hd, src, sizeof(send_hd));
    TFTP_WARN ("seq%d t_seq:%d\r\n", send_hd.seq, send_hd.total_seq);
    if((block + 1  != send_hd.seq))
    {
        os_printf ("bk:%d seq%d t_seq:%d fail!\r\n", block, send_hd.seq, send_hd.total_seq);
    }

    if(block)
    {
        if(!((send_hd_bk.seq + 1 == send_hd.seq)
                && (send_hd_bk.total_len == send_hd.total_len)
                && (send_hd_bk.os0_ex_addr == send_hd.os0_ex_addr)
                && (send_hd_bk.os0_flash_addr == send_hd.os0_flash_addr)
                && (send_hd_bk.os1_flash_addr == send_hd.os1_flash_addr)
                && (send_hd_bk.total_seq == send_hd.total_seq)))
            TFTP_PRT ("tftp seq head err\r\n");
    }
    else
    {
        os_data_addr = send_hd.os1_flash_addr;
        tftp_crc = 0;
    }
    os_memcpy(&send_hd_bk, &send_hd, sizeof(send_hd));

    if(os_data_addr % 0x1000 == 0)
    {
        param = os_data_addr;
        flash_ctrl(CMD_FLASH_ERASE_SECTOR, &param);
        TFTP_WARN ("erase_addr:%x \r\n", os_data_addr);
        TFTP_WARN ("e");
    }

    TFTP_WARN ("w_addr:%x \r\n", os_data_addr);
    if((u32)os_data_addr >= 0x200000 || (u32)os_data_addr < 0x27000)
    {
        TFTP_PRT ("eerr_addr:%x \r\n", os_data_addr);
        return;
    }

    if((u32)os_data_addr < 0x400000)
    {
        flash_write(src + TFTP_PKT_HD_LEN , len - TFTP_PKT_HD_LEN, (u32)os_data_addr);
        f_data = os_malloc(1024);
        if(f_data)
        {
            flash_read(f_data, len - TFTP_PKT_HD_LEN, (u32)os_data_addr);
            if(!os_memcmp(src + TFTP_PKT_HD_LEN, f_data, len - TFTP_PKT_HD_LEN))
            {
                TFTP_WARN ("block%d WRITE ok !\n", block);
                TFTP_WARN (".");
            }
            else
            {
                TFTP_PRT ("block%d  flash  write err\n", block);
            }
            os_free(f_data);
        }
        else
        {
            TFTP_PRT("malloc fail.\r\n");
        }

        if(send_hd.seq != send_hd.total_seq)
            tftp_crc = co_crc32((UINT32)src, len, tftp_crc);
        else
        {
            TFTP_WARN ("seq%d  send over\n", send_hd.seq);
            os_memcpy(&or_crc, src + len - TFTP_ALL_CRC_LEN, TFTP_ALL_CRC_LEN);
            tftp_crc = co_crc32((UINT32)src, len - TFTP_ALL_CRC_LEN, tftp_crc);
            if(tftp_crc == or_crc)
            {
                TFTP_PRT ("crc OK:%x %x\n", tftp_crc, or_crc);
                img_hd.bkup_addr = send_hd.os1_flash_addr;
                img_hd.bkup_len = send_hd_bk.total_len;
                img_hd.crc = or_crc;
                img_hd.ex_addr = send_hd.os0_ex_addr;
                img_hd.os_addr = send_hd.os0_flash_addr;
                img_hd.hd_addr = send_hd.os_hd_addr;
                img_hd.status = 1;
                param = (u32)send_hd.os_hd_addr;
                flash_ctrl(CMD_FLASH_ERASE_SECTOR, &param);
                flash_write((char *)&img_hd, sizeof(img_hd), (u32)send_hd.os_hd_addr);

                TFTP_WARN ("%X %X %X %X %X \r\n", img_hd.bkup_addr,
                           img_hd.bkup_len, img_hd.crc, img_hd.ex_addr, img_hd.status);
            }
            else
            {
                TFTP_PRT ("crc ERR--:%d %d\n", tftp_crc, or_crc);

            }
        }
        
        os_data_addr += len - TFTP_PKT_HD_LEN;
    }
}

#endif

#endif

//eof

