/*
 * Copyright (c) 2002 Florian Schulze.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of the contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ftpd.c - This file is part of the FTP daemon for lwIP
 *
 */
#include "include.h"
#if CFG_USE_FTPD_UPGRADE
#include "mem_pub.h"
#include "lwip/debug.h"

#include "lwip/stats.h"

#include "ftpd.h"

#include "lwip/tcp.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "vfs.h"

//#define FTPD_DEBUG
#include "uart_pub.h"
#ifdef FTPD_DEBUG
#define dbg_printf      os_printf
#else
#ifdef _MSC_VER
#define dbg_printf(x) /* x */
#else
#define dbg_printf(f, ...) /* */
#endif
#endif

#define EINVAL 1
#define ENOMEM 2
#define ENODEV 3

#define msg110 "110 MARK %s = %s."
/*
         110 Restart marker reply.
             In this case, the text is exact and not left to the
             particular implementation; it must read:
                  MARK yyyy = mmmm
             Where yyyy is User-process data stream marker, and mmmm
             server's equivalent marker (note the spaces between markers
             and "=").
*/
#define msg120 "120 Service ready in nnn minutes."
#define msg125 "125 Data connection already open; transfer starting."
#define msg150 "150 File status okay; about to open data connection."
#define msg150recv "150 Opening BINARY mode data connection for %s (%i bytes)."
#define msg150stor "150 Opening BINARY mode data connection for %s."
#define msg200 "200 Command okay."
#define msg202 "202 Command not implemented, superfluous at this site."
#define msg211 "211 System status, or system help reply."
#define msg212 "212 Directory status."
#define msg213 "213 File status."
#define msg214 "214 %s."
/*
             214 Help message.
             On how to use the server or the meaning of a particular
             non-standard command.  This reply is useful only to the
             human user.
*/
#define msg214SYST "214 %s system type."
/*
         215 NAME system type.
             Where NAME is an official system name from the list in the
             Assigned Numbers document.
*/
#define msg220 "220 lwIP FTP Server ready."
/*
         220 Service ready for new user.
*/
#define msg221 "221 Goodbye."
/*
         221 Service closing control connection.
             Logged out if appropriate.
*/
#define msg225 "225 Data connection open; no transfer in progress."
#define msg226 "226 Closing data connection."
/*
             Requested file action successful (for example, file
             transfer or file abort).
*/
#define msg227 "227 Entering Passive Mode (%i,%i,%i,%i,%i,%i)."
/*
         227 Entering Passive Mode (h1,h2,h3,h4,p1,p2).
*/
#define msg230 "230 User logged in, proceed."
#define msg250 "250 Requested file action okay, completed."
#define msg257PWD "257 \"%s\" is current directory."
#define msg257 "257 \"%s\" created."
/*
         257 "PATHNAME" created.
*/
#define msg331 "331 User name okay, need password."
#define msg332 "332 Need account for login."
#define msg350 "350 Requested file action pending further information."
#define msg421 "421 Service not available, closing control connection."
/*
             This may be a reply to any command if the service knows it
             must shut down.
*/
#define msg425 "425 Can't open data connection."
#define msg426 "426 Connection closed; transfer aborted."
#define msg450 "450 Requested file action not taken."
/*
             File unavailable (e.g., file busy).
*/
#define msg451 "451 Requested action aborted: local error in processing."
#define msg452 "452 Requested action not taken."
/*
             Insufficient storage space in system.
*/
#define msg500 "500 Syntax error, command unrecognized."
/*
             This may include errors such as command line too long.
*/
#define msg501 "501 Syntax error in parameters or arguments."
#define msg502 "502 Command not implemented."
#define msg503 "503 Bad sequence of commands."
#define msg504 "504 Command not implemented for that parameter."
#define msg530 "530 Not logged in."
#define msg532 "532 Need account for storing files."
#define msg550 "550 Requested action not taken."
/*
             File unavailable (e.g., file not found, no access).
*/
#define msg551 "551 Requested action aborted: page type unknown."
#define msg552 "552 Requested file action aborted."
/*
             Exceeded storage allocation (for current directory or
             dataset).
*/
#define msg553 "553 Requested action not taken."
/*
             File name not allowed.
*/

enum ftpd_state_e
{
    FTPD_USER,
    FTPD_PASS,
    FTPD_IDLE,
    FTPD_NLST,
    FTPD_LIST,
    FTPD_RETR,
    FTPD_RNFR,
    FTPD_STOR,
    FTPD_QUIT
};

static const char *month_table[12] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dez"
};

/*
------------------------------------------------------------
	SFIFO 1.3
------------------------------------------------------------
 * Simple portable lock-free FIFO
 * (c) 2000-2002, David Olofson
 *
 * Platform support:
 *	gcc / Linux / x86:		Works
 *	gcc / Linux / x86 kernel:	Works
 *	gcc / FreeBSD / x86:		Works
 *	gcc / NetBSD / x86:		Works
 *	gcc / Mac OS X / PPC:		Works
 *	gcc / Win32 / x86:		Works
 *	Borland C++ / DOS / x86RM:	Works
 *	Borland C++ / Win32 / x86PM16:	Untested
 *	? / Various Un*ces / ?:		Untested
 *	? / Mac OS / PPC:		Untested
 *	gcc / BeOS / x86:		Untested
 *	gcc / BeOS / PPC:		Untested
 *	? / ? / Alpha:			Untested
 *
 * 1.2: Max buffer size halved, to avoid problems with
 *	the sign bit...
 *
 * 1.3:	Critical buffer allocation bug fixed! For certain
 *	requested buffer sizes, older version would
 *	allocate a buffer of insufficient size, which
 *	would result in memory thrashing. (Amazing that
 *	I've manage to use this to the extent I have
 *	without running into this... *heh*)
 */

/*
 * Porting note:
 *	Reads and writes of a variable of this type in memory
 *	must be *atomic*! 'int' is *not* atomic on all platforms.
 *	A safe type should be used, and  sfifo should limit the
 *	maximum buffer size accordingly.
 */
typedef int sfifo_atomic_t;
#ifdef __TURBOC__
#	define	SFIFO_MAX_BUFFER_SIZE	0x7fff
#else /* Kludge: Assume 32 bit platform */
#	define	SFIFO_MAX_BUFFER_SIZE	0x7fffffff
#endif

typedef struct sfifo_t
{
    char *buffer;
    int size;			/* Number of bytes */
    sfifo_atomic_t readpos;		/* Read position */
    sfifo_atomic_t writepos;	/* Write position */
} sfifo_t;

#define SFIFO_SIZEMASK(x)	((x)->size - 1)

#define sfifo_used(x)	(((x)->writepos - (x)->readpos) & SFIFO_SIZEMASK(x))
#define sfifo_space(x)	((x)->size - 1 - sfifo_used(x))

#define DBG(x)

/*
 * Alloc buffer, init FIFO etc...
 */
static int sfifo_init(sfifo_t *f, int size)
{
    memset(f, 0, sizeof(sfifo_t));

    if(size > SFIFO_MAX_BUFFER_SIZE)
        return -EINVAL;

    /*
     * Set sufficient power-of-2 size.
     *
     * No, there's no bug. If you need
     * room for N bytes, the buffer must
     * be at least N+1 bytes. (The fifo
     * can't tell 'empty' from 'full'
     * without unsafe index manipulations
     * otherwise.)
     */
    f->size = 1;
    for(; f->size <= size; f->size <<= 1)
        ;

    /* Get buffer */
    if( 0 == (f->buffer = (void *)os_malloc(f->size)) )
        return -ENOMEM;

    return 0;
}

/*
 * Dealloc buffer etc...
 */
static void sfifo_close(sfifo_t *f)
{
    if(f->buffer)
        os_free(f->buffer);
}

/*
 * Write bytes to a FIFO
 * Return number of bytes written, or an error code
 */
static int sfifo_write(sfifo_t *f, const void *_buf, int len)
{
    int total;
    int i;
    const char *buf = (const char *)_buf;

    if(!f->buffer)
        return -ENODEV;	/* No buffer! */

    /* total = len = min(space, len) */
    total = sfifo_space(f);
    DBG(dbg_printf("sfifo_space() = %d\r\n", total));
    if(len > total)
        len = total;
    else
        total = len;

    i = f->writepos;
    if(i + len > f->size)
    {
        memcpy(f->buffer + i, buf, f->size - i);
        buf += f->size - i;
        len -= f->size - i;
        i = 0;
    }
    memcpy(f->buffer + i, buf, len);
    f->writepos = i + len;

    return total;
}

struct ftpd_datastate
{
    int connected;
    vfs_dir_t *vfs_dir;
    vfs_dirent_t *vfs_dirent;
    vfs_file_t *vfs_file;
    sfifo_t fifo;
    struct tcp_pcb *msgpcb;
    struct ftpd_msgstate *msgfs;
};

struct ftpd_msgstate
{
    enum ftpd_state_e state;
    sfifo_t fifo;
    vfs_t *vfs;
    struct ip_addr dataip;
    u16_t dataport;
    struct tcp_pcb *datapcb;
    struct ftpd_datastate *datafs;
    int passive;
    char *renamefrom;
};

static void send_msg(struct tcp_pcb *pcb, struct ftpd_msgstate *fsm, char *msg, ...);

static void ftpd_dataerr(void *arg, err_t err)
{
    struct ftpd_datastate *fsd = arg;

    dbg_printf("ftpd_dataerr: %s (%i)\r\n", lwip_strerr(err), err);
    if (fsd == NULL)
        return;
    fsd->msgfs->datafs = NULL;
    fsd->msgfs->state = FTPD_IDLE;
    os_free(fsd);
}

static void ftpd_dataclose(struct tcp_pcb *pcb, struct ftpd_datastate *fsd)
{
    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);
    fsd->msgfs->datafs = NULL;
    sfifo_close(&fsd->fifo);
    os_free(fsd);
    tcp_arg(pcb, NULL);
    tcp_close(pcb);
}

static void send_data(struct tcp_pcb *pcb, struct ftpd_datastate *fsd)
{
    err_t err;
    u16_t len;

    if (sfifo_used(&fsd->fifo) > 0)
    {
        int i;

        /* We cannot send more data than space available in the send
           buffer. */
        if (tcp_sndbuf(pcb) < sfifo_used(&fsd->fifo))
        {
            len = tcp_sndbuf(pcb);
        }
        else
        {
            len = (u16_t) sfifo_used(&fsd->fifo);
        }

        i = fsd->fifo.readpos;
        if ((i + len) > fsd->fifo.size)
        {
            err = tcp_write(pcb, fsd->fifo.buffer + i, (u16_t)(fsd->fifo.size - i), 1);
            if (err != ERR_OK)
            {
                dbg_printf("send_data: error writing!\r\n");
                return;
            }
            len -= fsd->fifo.size - i;
            fsd->fifo.readpos = 0;
            i = 0;
        }

        err = tcp_write(pcb, fsd->fifo.buffer + i, len, 1);
        if (err != ERR_OK)
        {
            dbg_printf("send_data: error writing!\r\n");
            return;
        }
        fsd->fifo.readpos += len;
    }
}

static void send_file(struct ftpd_datastate *fsd, struct tcp_pcb *pcb)
{
    if (!fsd->connected)
        return;

    if (fsd->vfs_file)
    {
        char buffer[2048];
        int len;

        len = sfifo_space(&fsd->fifo);
        if (len == 0)
        {
            send_data(pcb, fsd);
            return;
        }
        if (len > 2048)
            len = 2048;
        len = vfs_read(buffer, 1, len, fsd->vfs_file);
        if (len == 0)
        {
            if (vfs_eof(fsd->vfs_file) == 0)
                return;
            vfs_close(fsd->vfs_file);
            fsd->vfs_file = NULL;
            return;
        }
        sfifo_write(&fsd->fifo, buffer, len);
        send_data(pcb, fsd);
    }
    else
    {
        struct ftpd_msgstate *fsm;
        struct tcp_pcb *msgpcb;

        if (sfifo_used(&fsd->fifo) > 0)
        {
            send_data(pcb, fsd);
            return;
        }
        fsm = fsd->msgfs;
        msgpcb = fsd->msgpcb;

        vfs_close(fsd->vfs_file);
        fsd->vfs_file = NULL;
        ftpd_dataclose(pcb, fsd);
        fsm->datapcb = NULL;
        fsm->datafs = NULL;
        fsm->state = FTPD_IDLE;
        send_msg(msgpcb, fsm, msg226);
        return;
    }
}

static void send_next_directory(struct ftpd_datastate *fsd, struct tcp_pcb *pcb, int shortlist)
{
    char buffer[1024];
    int len;

    while (1)
    {
        if (fsd->vfs_dirent == NULL)
            fsd->vfs_dirent = vfs_readdir(fsd->vfs_dir);

        if (fsd->vfs_dirent)
        {
            if (shortlist)
            {
                len = sprintf(buffer, "%s\r\n", fsd->vfs_dirent->name);
                if (sfifo_space(&fsd->fifo) < len)
                {
                    send_data(pcb, fsd);
                    return;
                }
                sfifo_write(&fsd->fifo, buffer, len);
                fsd->vfs_dirent = NULL;
            }
            else
            {
                vfs_stat_t st;
                time_t current_time;
                int current_year;
                struct tm *s_time;

                time(&current_time);
                s_time = gmtime(&current_time);
                current_year = s_time->tm_year;

                vfs_stat(fsd->msgfs->vfs, fsd->vfs_dirent->name, &st);
                s_time = gmtime(&st.st_mtime);
                if (s_time->tm_year == current_year)
                    len = sprintf(buffer, "-rw-rw-rw-   1 user     ftp  %11ld %s %02i %02i:%02i %s\r\n", st.st_size, month_table[s_time->tm_mon], s_time->tm_mday, s_time->tm_hour, s_time->tm_min, fsd->vfs_dirent->name);
                else
                    len = sprintf(buffer, "-rw-rw-rw-   1 user     ftp  %11ld %s %02i %5i %s\r\n", st.st_size, month_table[s_time->tm_mon], s_time->tm_mday, s_time->tm_year + 1900, fsd->vfs_dirent->name);
                if (VFS_ISDIR(st.st_mode))
                    buffer[0] = 'd';
                if (sfifo_space(&fsd->fifo) < len)
                {
                    send_data(pcb, fsd);
                    return;
                }
                sfifo_write(&fsd->fifo, buffer, len);
                fsd->vfs_dirent = NULL;
            }
        }
        else
        {
            struct ftpd_msgstate *fsm;
            struct tcp_pcb *msgpcb;

            if (sfifo_used(&fsd->fifo) > 0)
            {
                send_data(pcb, fsd);
                return;
            }
            fsm = fsd->msgfs;
            msgpcb = fsd->msgpcb;

            vfs_closedir(fsd->vfs_dir);
            fsd->vfs_dir = NULL;
            ftpd_dataclose(pcb, fsd);
            fsm->datapcb = NULL;
            fsm->datafs = NULL;
            fsm->state = FTPD_IDLE;
            send_msg(msgpcb, fsm, msg226);
            return;
        }
    }
}

static err_t ftpd_datasent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    struct ftpd_datastate *fsd = arg;

    switch (fsd->msgfs->state)
    {
    case FTPD_LIST:
        send_next_directory(fsd, pcb, 0);
        break;
    case FTPD_NLST:
        send_next_directory(fsd, pcb, 1);
        break;
    case FTPD_RETR:
        send_file(fsd, pcb);
        break;
    default:
        break;
    }

    return ERR_OK;
}

static err_t ftpd_datarecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct ftpd_datastate *fsd = arg;

    if (err == ERR_OK && p != NULL)
    {
        struct pbuf *q;
        u16_t tot_len = 0;

        for (q = p; q != NULL; q = q->next)
        {
            int len;

            len = vfs_write(q->payload, 1, q->len, fsd->vfs_file);
            tot_len += len;
            if (len != q->len)
                break;
        }

        /* Inform TCP that we have taken the data. */
        tcp_recved(pcb, tot_len);

        pbuf_free(p);
    }
    if (err == ERR_OK && p == NULL)
    {
        struct ftpd_msgstate *fsm;
        struct tcp_pcb *msgpcb;

        fsm = fsd->msgfs;
        msgpcb = fsd->msgpcb;

        vfs_close(fsd->vfs_file);
        fsd->vfs_file = NULL;
        ftpd_dataclose(pcb, fsd);
        fsm->datapcb = NULL;
        fsm->datafs = NULL;
        fsm->state = FTPD_IDLE;
        send_msg(msgpcb, fsm, msg226);
    }

    return ERR_OK;
}

static err_t ftpd_dataconnected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    struct ftpd_datastate *fsd = arg;

    fsd->msgfs->datapcb = pcb;
    fsd->connected = 1;

    /* Tell TCP that we wish to be informed of incoming data by a call
       to the http_recv() function. */
    tcp_recv(pcb, ftpd_datarecv);

    /* Tell TCP that we wish be to informed of data that has been
       successfully sent by a call to the ftpd_sent() function. */
    tcp_sent(pcb, ftpd_datasent);

    tcp_err(pcb, ftpd_dataerr);

    switch (fsd->msgfs->state)
    {
    case FTPD_LIST:
        send_next_directory(fsd, pcb, 0);
        break;
    case FTPD_NLST:
        send_next_directory(fsd, pcb, 1);
        break;
    case FTPD_RETR:
        send_file(fsd, pcb);
        break;
    default:
        break;
    }

    return ERR_OK;
}

static err_t ftpd_dataaccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    struct ftpd_datastate *fsd = arg;

    fsd->msgfs->datapcb = pcb;
    fsd->connected = 1;

    /* Tell TCP that we wish to be informed of incoming data by a call
       to the http_recv() function. */
    tcp_recv(pcb, ftpd_datarecv);

    /* Tell TCP that we wish be to informed of data that has been
       successfully sent by a call to the ftpd_sent() function. */
    tcp_sent(pcb, ftpd_datasent);

    tcp_err(pcb, ftpd_dataerr);

    switch (fsd->msgfs->state)
    {
    case FTPD_LIST:
        send_next_directory(fsd, pcb, 0);
        break;
    case FTPD_NLST:
        send_next_directory(fsd, pcb, 1);
        break;
    case FTPD_RETR:
        send_file(fsd, pcb);
        break;
    default:
        break;
    }

    return ERR_OK;
}

static int open_dataconnection(struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    if (fsm->passive)
        return 0;

    /* Allocate memory for the structure that holds the state of the
       connection. */
    fsm->datafs = (struct ftpd_datastate *)os_malloc(sizeof(struct ftpd_datastate));

    if (fsm->datafs == NULL)
    {
        send_msg(pcb, fsm, msg451);
        return 1;
    }
    memset(fsm->datafs, 0, sizeof(struct ftpd_datastate));
    fsm->datafs->msgfs = fsm;
    fsm->datafs->msgpcb = pcb;
    sfifo_init(&fsm->datafs->fifo, 2000);

    fsm->datapcb = tcp_new();
    tcp_bind(fsm->datapcb, (ip_addr_t *)&pcb->local_ip, 20);
    /* Tell TCP that this is the structure we wish to be passed for our
       callbacks. */
    tcp_arg(fsm->datapcb, fsm->datafs);
    ip_addr_t dataip;
    ip_addr_copy(dataip, fsm->dataip);
    tcp_connect(fsm->datapcb, &dataip, fsm->dataport, ftpd_dataconnected);

    return 0;
}

static void cmd_user(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    send_msg(pcb, fsm, msg331);
    fsm->state = FTPD_PASS;
    /*
       send_msg(pcb, fs, msgLoginFailed);
       fs->state = FTPD_QUIT;
     */
}

static void cmd_pass(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    send_msg(pcb, fsm, msg230);
    fsm->state = FTPD_IDLE;
    /*
       send_msg(pcb, fs, msgLoginFailed);
       fs->state = FTPD_QUIT;
     */
}

static void cmd_port(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    int nr;
    unsigned pHi, pLo;
    unsigned ip[4];

    nr = sscanf(arg, "%u,%u,%u,%u,%u,%u", &(ip[0]), &(ip[1]), &(ip[2]), &(ip[3]), &pHi, &pLo);
    if (nr != 6)
    {
        send_msg(pcb, fsm, msg501);
    }
    else
    {
        IP4_ADDR(&fsm->dataip, (u8_t) ip[0], (u8_t) ip[1], (u8_t) ip[2], (u8_t) ip[3]);
        fsm->dataport = ((u16_t) pHi << 8) | (u16_t) pLo;
        send_msg(pcb, fsm, msg200);
    }
}

static void cmd_quit(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    send_msg(pcb, fsm, msg221);
    fsm->state = FTPD_QUIT;
}

static void cmd_cwd(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    if (!vfs_chdir(fsm->vfs, arg))
    {
        send_msg(pcb, fsm, msg250);
    }
    else
    {
        send_msg(pcb, fsm, msg550);
    }
}

static void cmd_cdup(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    if (!vfs_chdir(fsm->vfs, ".."))
    {
        send_msg(pcb, fsm, msg250);
    }
    else
    {
        send_msg(pcb, fsm, msg550);
    }
}

static void cmd_pwd(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    char *path;

    path = vfs_getcwd(fsm->vfs, NULL, 0);
    if (path)
    {
        send_msg(pcb, fsm, msg257PWD, path);
        os_free(path);
    }
}

static void cmd_list_common(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm, int shortlist)
{
    vfs_dir_t *vfs_dir;
    char *cwd;

    cwd = vfs_getcwd(fsm->vfs, NULL, 0);
    if ((!cwd))
    {
        send_msg(pcb, fsm, msg451);
        return;
    }
    vfs_dir = vfs_opendir(fsm->vfs, cwd);
    os_free(cwd);
    if (!vfs_dir)
    {
        send_msg(pcb, fsm, msg451);
        return;
    }

    if (open_dataconnection(pcb, fsm) != 0)
    {
        vfs_closedir(vfs_dir);
        return;
    }

    fsm->datafs->vfs_dir = vfs_dir;
    fsm->datafs->vfs_dirent = NULL;
    if (shortlist != 0)
        fsm->state = FTPD_NLST;
    else
        fsm->state = FTPD_LIST;

    send_msg(pcb, fsm, msg150);
}

static void cmd_nlst(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    cmd_list_common(arg, pcb, fsm, 1);
}

static void cmd_list(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    cmd_list_common(arg, pcb, fsm, 0);
}

static void cmd_retr(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    vfs_file_t *vfs_file;
    vfs_stat_t st;

    vfs_stat(fsm->vfs, arg, &st);
    if (!VFS_ISREG(st.st_mode))
    {
        send_msg(pcb, fsm, msg550);
        return;
    }
    vfs_file = vfs_open(fsm->vfs, arg, "rb");
    if (!vfs_file)
    {
        send_msg(pcb, fsm, msg550);
        return;
    }

    send_msg(pcb, fsm, msg150recv, arg, st.st_size);

    if (open_dataconnection(pcb, fsm) != 0)
    {
        vfs_close(vfs_file);
        return;
    }

    fsm->datafs->vfs_file = vfs_file;
    fsm->state = FTPD_RETR;
}

static void cmd_stor(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    vfs_file_t *vfs_file;

    vfs_file = vfs_open(fsm->vfs, arg, "wb");
    if (!vfs_file)
    {
        send_msg(pcb, fsm, msg550);
        return;
    }

    send_msg(pcb, fsm, msg150stor, arg);

    if (open_dataconnection(pcb, fsm) != 0)
    {
        vfs_close(vfs_file);
        return;
    }

    fsm->datafs->vfs_file = vfs_file;
    fsm->state = FTPD_STOR;
}

static void cmd_noop(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    send_msg(pcb, fsm, msg200);
}

static void cmd_syst(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    send_msg(pcb, fsm, msg214SYST, "UNIX");
}

static void cmd_pasv(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    static u16_t port = 4096;
    static u16_t start_port = 4096;
    struct tcp_pcb *temppcb;

    /* Allocate memory for the structure that holds the state of the
       connection. */
    fsm->datafs = (struct ftpd_datastate *)os_malloc(sizeof(struct ftpd_datastate));

    if (fsm->datafs == NULL)
    {
        send_msg(pcb, fsm, msg451);
        return;
    }
    memset(fsm->datafs, 0, sizeof(struct ftpd_datastate));

    fsm->datapcb = tcp_new();
    if (!fsm->datapcb)
    {
        os_free(fsm->datafs);
        send_msg(pcb, fsm, msg451);
        return;
    }

    sfifo_init(&fsm->datafs->fifo, 2000);

    start_port = port;

    while (1)
    {
        err_t err;

        if(++port > 0x7fff)
            port = 4096;

        fsm->dataport = port;
        err = tcp_bind(fsm->datapcb, (ip_addr_t *)&pcb->local_ip, fsm->dataport);
        if (err == ERR_OK)
            break;
        if (start_port == port)
            err = ERR_CLSD;
        if (err == ERR_USE)
            continue;
        if (err != ERR_OK)
        {
            ftpd_dataclose(fsm->datapcb, fsm->datafs);
            fsm->datapcb = NULL;
            fsm->datafs = NULL;
            return;
        }
    }

    fsm->datafs->msgfs = fsm;

    temppcb = tcp_listen(fsm->datapcb);
    if (!temppcb)
    {
        ftpd_dataclose(fsm->datapcb, fsm->datafs);
        fsm->datapcb = NULL;
        fsm->datafs = NULL;
        return;
    }
    fsm->datapcb = temppcb;

    fsm->passive = 1;
    fsm->datafs->connected = 0;
    fsm->datafs->msgpcb = pcb;

    /* Tell TCP that this is the structure we wish to be passed for our
       callbacks. */
    tcp_arg(fsm->datapcb, fsm->datafs);

    tcp_accept(fsm->datapcb, ftpd_dataaccept);
    send_msg(pcb, fsm, msg227, ip4_addr1(&pcb->local_ip), ip4_addr2(&pcb->local_ip), ip4_addr3(&pcb->local_ip), ip4_addr4(&pcb->local_ip), (fsm->dataport >> 8) & 0xff, (fsm->dataport) & 0xff);
}

static void cmd_abrt(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    if (fsm->datafs != NULL)
    {
        tcp_arg(fsm->datapcb, NULL);
        tcp_sent(fsm->datapcb, NULL);
        tcp_recv(fsm->datapcb, NULL);
        tcp_arg(fsm->datapcb, NULL);
        tcp_abort(pcb);
        sfifo_close(&fsm->datafs->fifo);
        os_free(fsm->datafs);
        fsm->datafs = NULL;
    }
    fsm->state = FTPD_IDLE;
}

static void cmd_type(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    dbg_printf("Got TYPE -%s-\r\n", arg);
    send_msg(pcb, fsm, msg502);
}

static void cmd_mode(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    dbg_printf("Got MODE -%s-\r\n", arg);
    send_msg(pcb, fsm, msg502);
}

static void cmd_rnfr(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    if (arg == NULL)
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (*arg == '\0')
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (fsm->renamefrom)
        os_free(fsm->renamefrom);
    fsm->renamefrom = (char *)os_malloc(strlen(arg) + 1);
    if (fsm->renamefrom == NULL)
    {
        send_msg(pcb, fsm, msg451);
        return;
    }
    strcpy(fsm->renamefrom, arg);
    fsm->state = FTPD_RNFR;
    send_msg(pcb, fsm, msg350);
}

static void cmd_rnto(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    if (fsm->state != FTPD_RNFR)
    {
        send_msg(pcb, fsm, msg503);
        return;
    }
    fsm->state = FTPD_IDLE;
    if (arg == NULL)
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (*arg == '\0')
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (vfs_rename(fsm->vfs, fsm->renamefrom, arg))
    {
        send_msg(pcb, fsm, msg450);
    }
    else
    {
        send_msg(pcb, fsm, msg250);
    }
}

static void cmd_mkd(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    if (arg == NULL)
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (*arg == '\0')
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (vfs_mkdir(fsm->vfs, arg, VFS_IRWXU | VFS_IRWXG | VFS_IRWXO) != 0)
    {
        send_msg(pcb, fsm, msg550);
    }
    else
    {
        send_msg(pcb, fsm, msg257, arg);
    }
}

static void cmd_rmd(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    vfs_stat_t st;

    if (arg == NULL)
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (*arg == '\0')
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (vfs_stat(fsm->vfs, arg, &st) != 0)
    {
        send_msg(pcb, fsm, msg550);
        return;
    }
    if (!VFS_ISDIR(st.st_mode))
    {
        send_msg(pcb, fsm, msg550);
        return;
    }
    if (vfs_rmdir(fsm->vfs, arg) != 0)
    {
        send_msg(pcb, fsm, msg550);
    }
    else
    {
        send_msg(pcb, fsm, msg250);
    }
}

static void cmd_dele(const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    vfs_stat_t st;

    if (arg == NULL)
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (*arg == '\0')
    {
        send_msg(pcb, fsm, msg501);
        return;
    }
    if (vfs_stat(fsm->vfs, arg, &st) != 0)
    {
        send_msg(pcb, fsm, msg550);
        return;
    }
    if (!VFS_ISREG(st.st_mode))
    {
        send_msg(pcb, fsm, msg550);
        return;
    }
    if (vfs_remove(fsm->vfs, arg) != 0)
    {
        send_msg(pcb, fsm, msg550);
    }
    else
    {
        send_msg(pcb, fsm, msg250);
    }
}

struct ftpd_command
{
    char *cmd;
    void (*func) (const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm);
};

static struct ftpd_command ftpd_commands[] =
{
    {"USER", cmd_user},
    {"PASS", cmd_pass},
    {"PORT", cmd_port},
    {"QUIT", cmd_quit},
    {"CWD", cmd_cwd},
    {"CDUP", cmd_cdup},
    {"PWD", cmd_pwd},
    {"XPWD", cmd_pwd},
    {"NLST", cmd_nlst},
    {"LIST", cmd_list},
    {"RETR", cmd_retr},
    {"STOR", cmd_stor},
    {"NOOP", cmd_noop},
    {"SYST", cmd_syst},
    {"ABOR", cmd_abrt},
    {"TYPE", cmd_type},
    {"MODE", cmd_mode},
    {"RNFR", cmd_rnfr},
    {"RNTO", cmd_rnto},
    {"MKD", cmd_mkd},
    {"XMKD", cmd_mkd},
    {"RMD", cmd_rmd},
    {"XRMD", cmd_rmd},
    {"DELE", cmd_dele},
    //{"PASV", cmd_pasv},
    {NULL, NULL}
};

static void send_msgdata(struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    err_t err;
    u16_t len;

    if (sfifo_used(&fsm->fifo) > 0)
    {
        int i;

        /* We cannot send more data than space available in the send
           buffer. */
        if (tcp_sndbuf(pcb) < sfifo_used(&fsm->fifo))
        {
            len = tcp_sndbuf(pcb);
        }
        else
        {
            len = (u16_t) sfifo_used(&fsm->fifo);
        }

        i = fsm->fifo.readpos;
        if ((i + len) > fsm->fifo.size)
        {
            err = tcp_write(pcb, fsm->fifo.buffer + i, (u16_t)(fsm->fifo.size - i), 1);
            if (err != ERR_OK)
            {
                dbg_printf("send_msgdata: error writing!\r\n");
                return;
            }
            len -= fsm->fifo.size - i;
            fsm->fifo.readpos = 0;
            i = 0;
        }

        err = tcp_write(pcb, fsm->fifo.buffer + i, len, 1);
        if (err != ERR_OK)
        {
            dbg_printf("send_msgdata: error writing!\r\n");
            return;
        }
        fsm->fifo.readpos += len;
    }
}

static void send_msg(struct tcp_pcb *pcb, struct ftpd_msgstate *fsm, char *msg, ...)
{
    va_list arg;
    char buffer[1024];
    int len;

    va_start(arg, msg);
    vsprintf(buffer, msg, arg);
    va_end(arg);
    strcat(buffer, "\r\n");
    len = strlen(buffer);
    if (sfifo_space(&fsm->fifo) < len)
        return;
    sfifo_write(&fsm->fifo, buffer, len);
    dbg_printf("response: %s", buffer);
    send_msgdata(pcb, fsm);
}

static void ftpd_msgerr(void *arg, err_t err)
{
    struct ftpd_msgstate *fsm = arg;

    dbg_printf("ftpd_msgerr: %s (%i)\r\n", lwip_strerr(err), err);
    if (fsm == NULL)
        return;
    if (fsm->datafs)
        ftpd_dataclose(fsm->datapcb, fsm->datafs);
    sfifo_close(&fsm->fifo);
    vfs_close(fsm->vfs);
    fsm->vfs = NULL;
    if (fsm->renamefrom)
        os_free(fsm->renamefrom);
    fsm->renamefrom = NULL;
    os_free(fsm);
}

static void ftpd_msgclose(struct tcp_pcb *pcb, struct ftpd_msgstate *fsm)
{
    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);
    if (fsm->datafs)
        ftpd_dataclose(fsm->datapcb, fsm->datafs);
    sfifo_close(&fsm->fifo);
    vfs_close(fsm->vfs);
    fsm->vfs = NULL;
    if (fsm->renamefrom)
        os_free(fsm->renamefrom);
    fsm->renamefrom = NULL;
    os_free(fsm);
    tcp_arg(pcb, NULL);
    tcp_close(pcb);
}

static err_t ftpd_msgsent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    struct ftpd_msgstate *fsm = arg;

    if (pcb->state > ESTABLISHED)
        return ERR_OK;

    if ((sfifo_used(&fsm->fifo) == 0) && (fsm->state == FTPD_QUIT))
    {
        ftpd_msgclose(pcb, fsm);
        return ERR_OK;
    }

    send_msgdata(pcb, fsm);

    return ERR_OK;
}

static err_t ftpd_msgrecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    char *text;
    struct ftpd_msgstate *fsm = arg;

    if (err == ERR_OK && p != NULL)
    {

        /* Inform TCP that we have taken the data. */
        tcp_recved(pcb, p->tot_len);

        text = (char *)os_malloc(p->tot_len + 1);
        if (text)
        {
            char cmd[5];
            struct pbuf *q;
            char *pt = text;
            struct ftpd_command *ftpd_cmd;

            for (q = p; q != NULL; q = q->next)
            {
                bcopy(q->payload, pt, q->len);
                pt += q->len;
            }
            *pt = '\0';

            pt = &text[strlen(text) - 1];
            while (((*pt == '\r') || (*pt == '\n')) && pt >= text)
                *pt-- = '\0';

            dbg_printf("query: %s\r\n", text);

            strncpy(cmd, text, 4);
            for (pt = cmd; isalpha(*pt) && pt < &cmd[4]; pt++)
                *pt = toupper(*pt);
            *pt = '\0';

            for (ftpd_cmd = ftpd_commands; ftpd_cmd->cmd != NULL; ftpd_cmd++)
            {
                if (!strcmp(ftpd_cmd->cmd, cmd))
                    break;
            }

            if (strlen(text) < (strlen(cmd) + 1))
                pt = "";
            else
                pt = &text[strlen(cmd) + 1];

            if (ftpd_cmd->func)
                ftpd_cmd->func(pt, pcb, fsm);
            else
                send_msg(pcb, fsm, msg502);

            os_free(text);
        }
        pbuf_free(p);
    }

    return ERR_OK;
}

static err_t ftpd_msgpoll(void *arg, struct tcp_pcb *pcb)
{
    struct ftpd_msgstate *fsm = arg;

    if (fsm == NULL)
        return ERR_OK;

    if (fsm->datafs)
    {
        if (fsm->datafs->connected)
        {
            switch (fsm->state)
            {
            case FTPD_LIST:
                send_next_directory(fsm->datafs, fsm->datapcb, 0);
                break;
            case FTPD_NLST:
                send_next_directory(fsm->datafs, fsm->datapcb, 1);
                break;
            case FTPD_RETR:
                send_file(fsm->datafs, fsm->datapcb);
                break;
            default:
                break;
            }
        }
    }

    return ERR_OK;
}

static err_t ftpd_msgaccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    struct ftpd_msgstate *fsm;

    /* Allocate memory for the structure that holds the state of the
       connection. */
    fsm = (struct ftpd_msgstate *)os_malloc(sizeof(struct ftpd_msgstate));

    if (fsm == NULL)
    {
        dbg_printf("ftpd_msgaccept: Out of memory\r\n");
        return ERR_MEM;
    }
    memset(fsm, 0, sizeof(struct ftpd_msgstate));

    /* Initialize the structure. */
    sfifo_init(&fsm->fifo, 2000);
    fsm->state = FTPD_IDLE;
    fsm->vfs = vfs_openfs();
    if (!fsm->vfs)
    {
        os_free(fsm);
        return ERR_CLSD;
    }

    /* Tell TCP that this is the structure we wish to be passed for our
       callbacks. */
    tcp_arg(pcb, fsm);

    /* Tell TCP that we wish to be informed of incoming data by a call
       to the http_recv() function. */
    tcp_recv(pcb, ftpd_msgrecv);

    /* Tell TCP that we wish be to informed of data that has been
       successfully sent by a call to the ftpd_sent() function. */
    tcp_sent(pcb, ftpd_msgsent);

    tcp_err(pcb, ftpd_msgerr);

    tcp_poll(pcb, ftpd_msgpoll, 1);

    send_msg(pcb, fsm, msg220);

    return ERR_OK;
}

/*---------------------------------------------------------------------------*/
PROCESS(lwip_ftp_server_process, "LWIP FTP server");
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(lwip_ftp_server_process, ev , data)
{
    struct tcp_pcb *pcb;

    PROCESS_BEGIN();

    pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, 21);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, ftpd_msgaccept);

    while(1)
    {
        PROCESS_WAIT_EVENT();
        if(ev == PROCESS_EVENT_EXIT)
        {
            pcb = NULL;
        }
    }

    PROCESS_END();
}

void ftpd_stop(void)
{
}

void ftpd_start(void)
{
    dbg_printf("ftp server started\r\n");
    process_start(&lwip_ftp_server_process, NULL);
}
#endif

//eof

