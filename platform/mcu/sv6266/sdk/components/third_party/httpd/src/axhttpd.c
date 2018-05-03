/*
 * Copyright (c) 2007, Cameron Rich
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
//#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
//#include <sys/stat.h>
//#include <pwd.h>

// HTTPD
#include "os_port.h"
#include "axhttp.h"
#include "auth_check.h"
#include "httpd.h"
#include "httpd_iprot.h"

struct serverstruct *servers = NULL;
struct connstruct *usedconns = NULL;
struct connstruct *freeconns = NULL;
const char * const server_version = "axhttpd/"AXTLS_VERSION;

static void addtoservers(int sd);
static int openlistener(char *address, int port);
static void handlenewconnection(int listenfd);
static void addconnection(int sd, char *ip);

/* clean up memory for valgrind */
void sigint_cleanup(int sig)
{
    struct serverstruct *sp;
    struct connstruct *tp;

    HTTPD_DEBUGF((httpd, "[HTTPD] sigint_cleanup() - sig = %d\n", sig));

    while (usedconns != NULL)
    {
        removeconnection(usedconns);
    }

    while (freeconns != NULL)
    {
        if((freeconns->query_tbl != NULL) || (freeconns->post_data != NULL))// no possibility
            HTTPD_DEBUGF((httpd, "[HTTPD] sigint_cleanup() - query_tbl = %p, post_data = %p\n", freeconns->query_tbl, freeconns->post_data));
        tp = freeconns->next;
        ax_free(freeconns);
        freeconns = tp;
    }

    while (servers != NULL)
    {
        SOCKET_CLOSE(servers->sd);
        sp = servers->next;
        ax_free(servers);
        servers = sp;
    }

    return;
}

//int httpd_main(int argc, char *argv[])
int httpd_main(void *argv)
{
    fd_set rfds, wfds, efds;
    struct connstruct *tp, *to;
    struct serverstruct *sp;
    int rnum, wnum, active;
    int i = 1;
    char *httpAddress = NULL;
    int httpPort = CONFIG_HTTP_PORT;

    if(argv) // customerization input parameter
    {
        httpd_para *para = (httpd_para *)argv;
        HTTPD_DEBUGF((httpd, "[HTTPD] httpd_main() - start, addr = %s, port = %s\n", para->str_ip_addr, para->str_port));

        if(*para->str_ip_addr != '\0')
        {
            httpAddress = para->str_ip_addr;
        }

        if(*para->str_port != '\0')
        {
            httpPort = atoi(para->str_port);
        }
    }

    tdate_init();

    // Auth Username/Password config
    user_config(1, 1);

    for (i = 0; i < INITIAL_CONNECTION_SLOTS; i++)
    {
        tp = freeconns;
        freeconns = (struct connstruct *)ax_calloc(sizeof(struct connstruct));
        if(freeconns == NULL)
            httpd_stop_handle(HTTPD_STOP_REASON_NO_MEMORY);
        memset(freeconns, 0, sizeof(struct connstruct));
        freeconns->next = tp;
    }

    if ((active = openlistener(httpAddress, httpPort)) == -1)
    {
        httpd_stop_handle(HTTPD_STOP_REASON_BEARER_CONN_FAIL);
        return -1;
    }

    addtoservers(active);

    httpd_start_handle();

    /* httpd_main loop */
    while (1)
    {
        struct timeval tv = { 10, 0 };
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);
        rnum = wnum = -1;

        HTTPD_DEBUGF((httpd, "[HTTPD] httpd_main() - while(1) start, usedconns = %p\n", usedconns));

#if 0
        if(usedconns)
        {
            // TODO: Just support one connection
            // listen the conn_fd;
        }
        else
#endif
        {
            sp = servers;
            while (sp != NULL)  /* read each server port */
            {
                FD_SET(sp->sd, &rfds);
                FD_SET(sp->sd, &efds);

                if (sp->sd > rnum)
                    rnum = sp->sd;
                sp = sp->next;
            }
        }

        /* Add the established sockets */
        tp = usedconns;

        while (tp != NULL)
        {
            HTTPD_DEBUGF((
                httpd, "[HTTPD] httpd_main() - tp = %p, tp->state = %d, tp->networkdesc = %d\n",
                tp, tp->state, tp->networkdesc));

            if (tp->state == STATE_WANT_TO_READ_HEAD)
            {
                FD_SET(tp->networkdesc, &rfds);
                if (tp->networkdesc > rnum)
                    rnum = tp->networkdesc;
            }

            if (tp->state == STATE_WANT_TO_SEND_HEAD)
            {
                FD_SET(tp->networkdesc, &wfds);
                if (tp->networkdesc > wnum)
                    wnum = tp->networkdesc;
            }

            // MT7687 soc do not support file system,
            if (tp->state == STATE_WANT_TO_READ_FILE)
            {
                FD_SET(tp->filedesc, &rfds);
                if (tp->filedesc > rnum)
                    rnum = tp->filedesc;
            }

            if (tp->state == STATE_WANT_TO_SEND_FILE)
            {
                FD_SET(tp->networkdesc, &wfds);
                if (tp->networkdesc > wnum)
                    wnum = tp->networkdesc;
            }

        #if defined(CONFIG_HTTP_DIRECTORIES)
            if (tp->state == STATE_DOING_DIR)
            {
                FD_SET(tp->networkdesc, &wfds);
                if (tp->networkdesc > wnum)
                    wnum = tp->networkdesc;
            }
        #endif

            tp = tp->next;
        }


        HTTPD_DEBUGF((
            httpd, "[HTTPD] httpd_main() - select() start, wnum = %d, rnum = %d\n", wnum, rnum));

        active = select(wnum > rnum ? wnum+1 : rnum+1,
                rnum != -1 ? &rfds : NULL,
                wnum != -1 ? &wfds : NULL,
                &efds, &tv/*usedconns ? &tv : NULL*/);

        HTTPD_DEBUGF((httpd, "[HTTPD] httpd_main() - select() end, active = %d\n", active));

        if(HTTPD_STATUS_STOPPING == httpd_get_status())
        {
            httpd_stop_handle(HTTPD_STOP_REASON_BY_COMMAND);
            return -1;
        }

        /* timeout? */
        if (active == 0)
        {
        #if 0
            // TODO: Just support one connection
            // timeout and disconnect;
            if(usedconns)
                removeconnection(usedconns);
        #endif
            continue;
        }

        /* New connection? */
        sp = servers;
        while (active > 0 && sp != NULL)
        {
            // Listened sd exception, cleanup and Stop HTTPD
            if (FD_ISSET(sp->sd, &efds))
            {
                httpd_stop_handle(HTTPD_STOP_REASON_BEARER_DISCONN);
                return -1;
            }

            if (FD_ISSET(sp->sd, &rfds))
            {
                handlenewconnection(sp->sd);
                active--;
            }

            sp = sp->next;
        }

        /* Handle the established sockets */
        tp = usedconns;

        while (active > 0 && tp != NULL)
        {
            to = tp;
            tp = tp->next;

            HTTPD_DEBUGF((
                httpd, "[HTTPD] httpd_main() - to = %p, tp = %p, to->state = %d, to->networkdesc = %d\n",
                to, to, to->state, to->networkdesc));

            if (to->state == STATE_WANT_TO_READ_HEAD &&
                        FD_ISSET(to->networkdesc, &rfds))
            {
                active--;
                if (to->post_state)
                    read_post_data(to);
                else
                    procreadhead(to);
            }

            if (to->state == STATE_WANT_TO_SEND_HEAD &&
                        FD_ISSET(to->networkdesc, &wfds))
            {
                active--;
                procsendhead(to);
            }

            // MT7687 soc do not support file system,
            if (to->state == STATE_WANT_TO_READ_FILE &&
                        FD_ISSET(to->filedesc, &rfds))
            {
                active--;
                procreadfile(to);
            }

            if (to->state == STATE_WANT_TO_SEND_FILE &&
                        FD_ISSET(to->networkdesc, &wfds))
            {
                active--;
                procsendfile(to);
            }

        #if defined(CONFIG_HTTP_DIRECTORIES)
            if (to->state == STATE_DOING_DIR &&
                        FD_ISSET(to->networkdesc, &wfds))
            {
                active--;
                procdodir(to);
            }
        #endif
        }
    }

    // return 0; // Qian
}

static void addtoservers(int sd)
{
    struct serverstruct *tp = (struct serverstruct *)ax_calloc(sizeof(struct serverstruct));

    tp->next = servers;
    tp->sd = sd;
    servers = tp;

    HTTPD_DEBUGF((httpd, "[HTTPD] addtoservers() - sd = %d, servers = %p\n", sd, servers));
}

#ifdef CONFIG_HTTP_HAS_IPV6
static void handlenewconnection(int listenfd)
{
    struct sockaddr_in6 their_addr;
    socklen_t tp = sizeof(their_addr);
    char ipbuf[100];
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);

    HTTPD_DEBUGF((httpd, "[HTTPD] handlenewconnection() - listenfd = %d, connfd = %d, ip = %s:%d\n",
        listenfd, connfd, ipbuf, ntohs(their_addr.sin_port)));

    if (tp == sizeof(struct sockaddr_in6))
        inet_ntop(AF_INET6, &their_addr.sin6_addr, ipbuf, sizeof(ipbuf));
    else if (tp == sizeof(struct sockaddr_in))
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&their_addr)->sin_addr),
                ipbuf, sizeof(ipbuf));
    else
        *ipbuf = '\0';

    if (connfd != -1) /* check for error condition */
    {
        addconnection(connfd, ipbuf);
    }
    else
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] handlenewconnection() - accept() error\n"));
    }
}
#else
static void handlenewconnection(int listenfd)
{
    struct sockaddr_in their_addr;
    socklen_t tp = sizeof(struct sockaddr_in);
    int connfd = accept(listenfd, (struct sockaddr *)&their_addr, &tp);

    HTTPD_DEBUGF((httpd, "[HTTPD] handlenewconnection() - listenfd = %d, connfd = %d, ip = %s:%d\n",
        listenfd, connfd, inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port)));

    if (connfd != -1) /* check for error condition */
    {
        addconnection(connfd, inet_ntoa(their_addr.sin_addr));
    }
    else
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] handlenewconnection() - accept() error\n"));
    }
}
#endif /* CONFIG_HTTP_HAS_IPV6 */

static int openlistener(char *address, int port)
{
    int sd;
    int tp = 1;

    HTTPD_DEBUGF((httpd, "[HTTPD] openlistener() - address = %s, port = %d\n", address, port));

#ifndef CONFIG_HTTP_HAS_IPV6
    struct sockaddr_in my_addr;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] openlistener() - socket() error\n"));
        return -1;
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons((short)port);
    my_addr.sin_addr.s_addr = address == NULL ?
                        INADDR_ANY : inet_addr(address);
#else
    struct sockaddr_in6 my_addr;

    if ((sd = socket(AF_INET6, SOCK_STREAM, 0)) == -1)
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] !!!!!! openlistener() - socket() error\n"));
        return -1;
    }

    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port = htons(port);

    if (address == NULL)
        my_addr.sin6_addr = in6addr_any;
    else
        inet_pton(AF_INET6, address, &my_addr.sin6_addr);
#endif /* CONFIG_HTTP_HAS_IPV6 */

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &tp, sizeof(tp));

    if (bind(sd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] !!!!!! openlistener() - bind(%d) error\n", sd));
        close(sd);
        return -1;
    }

    listen(sd, BACKLOG);

    HTTPD_DEBUGF((httpd, "[HTTPD] openlistener() - sd = %d\n", sd));

    return sd;
}

static void addconnection(int sd, char *ip)
{
    struct connstruct *tp;

    /* Get ourselves a connstruct */
    if (freeconns == NULL)
        tp = (struct connstruct *)ax_calloc(sizeof(struct connstruct));
    else
    {
        tp = freeconns;
        freeconns = tp->next;
    }

    HTTPD_DEBUGF((
        httpd, "[HTTPD] addconnection() - tp = %p, usedconns = %p, tp->networkdesc = %d, ip = %s\n",
        tp, usedconns, sd, ip));

    /* Attach it to the used list */
    tp->next = usedconns;
    usedconns = tp;
    tp->networkdesc = sd;
    tp->filedesc = -1;
    *tp->actualfile = '\0';
    *tp->filereq = '\0';
    tp->state = STATE_WANT_TO_READ_HEAD;
    tp->reqtype = TYPE_GET;
    tp->close_when_done = 0;

    tp->query_len = 0;
    tp->query = NULL;
    tp->content_length = 0;
    tp->query_tbl = NULL;

    tp->is_cgi = 0;

    tp->post_data = NULL;
    tp->post_read = 0;
    tp->post_state = 0;
}

void removeconnection(struct connstruct *cn)
{
    struct connstruct *tp;
    int shouldret = 0;

    HTTPD_DEBUGF((
        httpd, "[HTTPD] removeconnection() - cn = %p, usedconns = %p, freeconns = %p\n",
        cn, usedconns, freeconns));

    tp = usedconns;

    if (tp == NULL || cn == NULL)
        shouldret = 1;
    else if (tp == cn)
        usedconns = tp->next;
    else
    {
        while (tp != NULL)
        {
            if (tp->next == cn)
            {
                tp->next = (tp->next)->next;
                shouldret = 0;
                break;
            }

            tp = tp->next;
            shouldret = 1;
        }
    }

    // if shouldret = 1, mean connection crash. Reboot or Stop ???
    if (shouldret)
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] removeconnection() - shouldret\n"));
        return;
    }

    HTTPD_DEBUGF((
        httpd, "[HTTPD] removeconnection() - cn->networkdesc = %d, cn->filedesc = %d\n",
        cn->networkdesc, cn->filedesc));

    /* If we did, add it to the free list */
    cn->next = freeconns;
    freeconns = cn;

    /* Close it all down */
    if (cn->networkdesc != -1)
    {
        shutdown(cn->networkdesc, SHUT_WR);
        SOCKET_CLOSE(cn->networkdesc);
    }

    if (cn->filedesc != -1)
        close(cn->filedesc);

    /* currently, after send HTML response, HTTPD must
       removeconnection, so we can free memory in this funtion.
       If the logic change, such as keep alive and do not close the socket.
       we must choose other timing to free  memory. */
    if(cn->query_tbl)
    {
        ax_free(cn->query_tbl);
    }

    if(cn->post_data)
    {
        ax_free(cn->post_data);
    }
}

