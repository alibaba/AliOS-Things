/*
 * Copyright (c) Cameron Rich
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
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <string.h>

//MT7687
//#include <hal_rtc.h>

// Httpd
#include "os_port.h"
#include "axhttp.h"
#include "base64.h"
#include "auth_check.h"
#include "cgi_api.h"
#include "cgi.h"

/* Be careful, do not move the header file.
   webpage.h generate in the compare flow, it must be generated before compare proc.h
   so makefile set file dependence "src/proc.o: webpage.h" */
#include "webpage.h"

static const char * rfc1123_format = "%a, %d %b %Y %H:%M:%S GMT";

static void send_error(struct connstruct *cn, int err);
static int hexit(char c);
static void urldecode(char *buf);
static void buildactualfile(struct connstruct *cn);
static int sanitizefile(const char *buf);
static int sanitizehost(char *buf);
static void getCurrentTime(char *date);
static void getFileTime(const time_t fileTime, char *date);
static const char *getmimetype(const char *name);
static int init_read_post_data(char *buf, char *data, struct connstruct *cn, int old_rv);
static void decode_path_info(struct connstruct *cn, char *path_info);
static void decode_content_data(struct connstruct *cn);
static void proccgi(struct connstruct *cn);
#if defined(CONFIG_HTTP_DIRECTORIES)
static void urlencode(const uint8_t *s, char *t);
static void procdirlisting(struct connstruct *cn);
#endif

//Ecos
static int read_file(webpage_entry* entry);
static int get_file_handle(char *filename);


/* Wrapper function for strncpy() that guarantees
   a null-terminated string. This is to avoid any possible
   issues due to strncpy()'s behaviour.
 */
static char *my_strncpy(char *dest, const char *src, size_t n)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] my_strncpy() - dest = %s, src = %s, n = %d\n", dest, src, n));

    strncpy(dest, src, n);
    dest[n-1] = '\0';
    return dest;
}

/* Returns 1 if elems should continue being read, 0 otherwise */
static int procheadelem(struct connstruct *cn, char *buf)
{
    char *delim, *value;

    HTTPD_DEBUGF((httpd, "[HTTPD] procheadelem() - cn = %p, buf = %s\n", cn, buf));

    if ((delim = strchr(buf, ' ')) == NULL)
        return 0;

    *delim = 0;
    value = delim+1;

    if (strcmp(buf, "GET") == 0 || strcmp(buf, "HEAD") == 0 ||
                                            strcmp(buf, "POST") == 0)
    {
        if (buf[0] == 'H')
            cn->reqtype = TYPE_HEAD;
        else if (buf[0] == 'P')
            cn->reqtype = TYPE_POST;
        else if (buf[0] == 'G')
            cn->reqtype = TYPE_GET;

        if ((delim = strchr(value, ' ')) == NULL)       /* expect HTTP type */
            return 0;

        *delim++ = 0;
        urldecode(value);

        if (sanitizefile(value) == 0)
        {
            send_error(cn, 403);
            return 0;
        }

        decode_path_info(cn, value);

        // cn->if_modified_since = -1;
        cn->if_modified_since = 0;
        if (strcmp(delim, "HTTP/1.0") == 0) /* v1.0 HTTP? */
            cn->is_v1_0 = 1;
    }
    else if (strcasecmp(buf, "Host:") == 0)
    {
        if (sanitizehost(value) == 0)
        {
            removeconnection(cn);
            return 0;
        }

        my_strncpy(cn->server_name, value, MAXREQUESTLENGTH);
    }
    else if (strcasecmp(buf, "Connection:") == 0 && strcmp(value, "close") == 0)
    {
        cn->close_when_done = 1;
    }
    else if (strcasecmp(buf, "If-Modified-Since:") == 0)
    {
        cn->if_modified_since = tdate_parse(value);
        HTTPD_DEBUGF((httpd, "[HTTPD] procheadelem() - cn->if_modified_since = %ld\n", (long)cn->if_modified_since));
    }
    else if (strcasecmp(buf, "Expect:") == 0)
    {
		/* supposed to be safe to ignore 100-continue */
		if (strcasecmp(value, "100-continue") != 0) {
			send_error(cn, 417); /* expectation failed */
			return 0;
		}
    }
    else if (strcasecmp(buf, "Authorization:") == 0 &&
                                    strncmp(value, "Basic ", 6) == 0)
    {
        int size = sizeof(cn->authorization);
        unsigned char *out = NULL;
        out = base64_decode(
                        (const unsigned char *)(&value[6]),
                        (size_t)strlen(&value[6]),
                        (size_t *)(&size));

        HTTPD_DEBUGF((httpd, "[HTTPD] out = %s, size = %d\n", out, size));

        if (out == NULL)
            cn->authorization[0] = 0;   /* error */
        else
        {
            memcpy(cn->authorization, out, size);
            cn->authorization[size] = 0;
            ax_free(out); // free memory
        }
    }
    else if (strcasecmp(buf, "Content-Length:") == 0)
    {
        sscanf(value, "%d", &cn->content_length);
    }

    return 1;
}

void procreadhead(struct connstruct *cn)
{
    char *readbuf;
    char *tp, *next;
    int rv;

    HTTPD_DEBUGF((httpd, "[HTTPD] procreadhead() - cn = %p\n", cn));

    readbuf = (char *)ax_malloc(MAXREADLENGTH);
	if (readbuf == NULL )
	{
        HTTPD_DEBUGF((httpd, "[HTTPD] procreadhead() - readbuf = NULL\n"));
		return ;
	}

    memset(readbuf, 0, MAXREADLENGTH);
    rv = special_read(cn->networkdesc, readbuf, MAXREADLENGTH - 1);
    if (rv <= 0)
    {
        if (rv < 0) /* really dead? */
            removeconnection(cn);
        else if(rv == 0) // TODO: Issue
            removeconnection(cn);

        ax_free(readbuf);
        return;
    }

    readbuf[rv] = '\0';
    next = tp = readbuf;

    HTTPD_DEBUGF((httpd, "[HTTPD] procreadhead() - readbuf = %s\n", readbuf));

    cn->authorization[0] = 0;

    /* Split up lines and send to procheadelem() */
    while (*next != '\0')
    {
        /* If we have a blank line, advance to next stage */
        if (*next == '\r' || *next == '\n')
        {
            if (cn->reqtype == TYPE_POST && cn->content_length > 0)
            {
                if (init_read_post_data(readbuf, next, cn, rv) == 0)
                {
                    ax_free(readbuf);
                    return;
                }
            }
            decode_content_data(cn);
            buildactualfile(cn);
            cn->state = STATE_WANT_TO_SEND_HEAD;

            ax_free(readbuf);
            return;
        }

        while (*next != '\r' && *next != '\n' && *next != '\0')
            next++;

        if (*next == '\r')
        {
            *next = '\0';
            next += 2;
        }
        else if (*next == '\n')
            *next++ = '\0';

        if (procheadelem(cn, tp) == 0)
        {
            ax_free(readbuf);
            return;
        }

        tp = next;
    }

    ax_free(readbuf);
}

/* In this function we assume that the file has been checked for
 * maliciousness (".."s, etc) and has been decoded
 */
void procsendhead(struct connstruct *cn)
{
    char date[32];
    char last_modified[32];
    webpage_entry * fileptr;
    time_t last_time;
    char *sendbuf = NULL;
    int write_len = 0;

    HTTPD_DEBUGF((httpd, "[HTTPD] procsendhead() - cn = %p\n", cn));

    if (cn->is_cgi)
    {
        proccgi(cn);
        return;
    }

    /* Get handle */
    fileptr = (webpage_entry *)get_file_handle(cn->actualfile);
    if(fileptr)
    {
		switch (cn->reqtype)
		{
		case TYPE_GET:
			if (!(fileptr->flag & NO_AUTH))
			{
                if (auth_check(cn))     /* see if there is a '.htpasswd' file */
                {
                    return;
                }
			}
		    CGI_do_cmd(cn);
			break;
		case TYPE_POST:
			if (fileptr->flag & NO_AUTH)
			{
                send_error(cn, 400);
                return;
			}

            if (auth_check(cn))     /* see if there is a '.htpasswd' file */
            {
                return;
            }
        #if 0
			new_file = cgiFunc(fileptr, req);
			if (new_file)
			{
				fileptr = get_file_handle(new_file);
				if (!fileptr)
				{
					err_code = HTTP_ERROR;
					goto err_out;
				}
			}
        #endif
            CGI_do_cmd(cn);
            break;
		default:
            send_error(cn, 405);
            return;
		}
    }
    else
    {
        // file Miss
        if(0 == strcmp(cn->actualfile, CONFIG_HTTP_HOME_HTML))
        {
        #if defined(CONFIG_HTTP_DIRECTORIES)
        /* If not, we do a directory listing of it */
            procdirlisting(cn);
        #else
            send_error(cn, 404);
        #endif
        }
        else
        {
            send_error(cn, 404);
        }

        return;
    }

	// Now read the file
	if (!read_file(fileptr))
	{
		// Read file fail
        send_error(cn, 404);
        return;
	}

    getCurrentTime(date);
    getFileTime(time_pages_created ,last_modified);

    /* has the file been read before? */
    // if (cn->if_modified_since != -1)
    if (cn->if_modified_since != 0)
    {
        if (fileptr->flag & CACHE)
        {
            if (cn->if_modified_since >= time_pages_created)
            {
                sendbuf = (char *)ax_malloc(MAXREQUESTLENGTH);
            	if (sendbuf == NULL )
            	{
                    HTTPD_DEBUGF((httpd, "[HTTPD] procsendhead() - sendbuf = NULL\n"));
            		return ;
            	}
                memset(sendbuf, 0, MAXREQUESTLENGTH);
                snprintf(sendbuf, MAXREQUESTLENGTH - 1, HTTP_VERSION" 304 Not Modified\nServer: "
                    "%s\nDate: %s\n\n", server_version, date);

                // no matter special_write result, also need to removeconnection
                special_write(cn->networkdesc, sendbuf, strlen(sendbuf));
                ax_free(sendbuf);
                removeconnection(cn);
                return;
            }
        }
        else
        {
    		/* we don't hope browser get pages from cache */
		    last_time = (cn->if_modified_since < time_pages_created) ?
			                    time_pages_created : cn->if_modified_since + 1;

            getFileTime(last_time ,last_modified);
        }
    }


    sendbuf = (char *)ax_malloc(MAXREQUESTLENGTH);
	if (sendbuf == NULL )
	{
        HTTPD_DEBUGF((httpd, "[HTTPD] procsendhead() - sendbuf = NULL\n"));
		return ;
	}
    memset(sendbuf, 0, MAXREQUESTLENGTH);
    snprintf(sendbuf, MAXREQUESTLENGTH - 1, HTTP_VERSION" 200 OK\r\n"
        "Server: %s\r\n"
        "Date: %s\r\n"
        "Pragma: no-cache\r\n"
        "Cache-Control: no-cache\r\n"
        "Content-Type: %s\r\n"
        "Last-Modified: %s\r\n"
        "Connection: close\r\n\r\n",
        date, server_version, getmimetype(cn->actualfile), last_modified);

    write_len = special_write(cn->networkdesc, sendbuf, strlen(sendbuf));
    ax_free(sendbuf);
    if(write_len <= 0)
    {
        if (write_len < 0) /* really dead? */
            removeconnection(cn);
        else if(write_len == 0) // TODO: Issue
            removeconnection(cn);
        return;
    }

    // MT7687 soc do not support file system
    procreadfile(cn);
}

void procreadfile(struct connstruct *cn)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] procreadfile() - cn = %p\n", cn));

    cn->state = STATE_WANT_TO_SEND_FILE;
}

// Just Run procsendfile once which is different with original axTLS logic
void procsendfile(struct connstruct *cn)
{
    int rv = 0;
    webpage_entry * fileptr;

    HTTPD_DEBUGF((httpd, "[HTTPD] procsendfile() - cn = %p, cn->networkdesc = %d\n", cn, cn->networkdesc));

    /* Get handle */
    fileptr = (webpage_entry *)get_file_handle(cn->actualfile);
    rv = (*fileptr->fileFunc)(cn, fileptr->param);

    HTTPD_DEBUGF((httpd, "[HTTPD] procsendfile() - rv = %d\n", rv));

    if (rv <= 0)
    {
        removeconnection(cn);
    }
    //else if (rv == cn->numbytes)
    else
    {
        close(cn->filedesc);
        cn->filedesc = -1;

        // Must close when send success becase HTTP server do not send content-length to client
        removeconnection(cn);
    }
}

static void decode_path_info(struct connstruct *cn, char *path_info)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] decode_path_info() - cn = %p, path_info = %s\n", cn, path_info));

    /* the bit at the start must be the script name */
    my_strncpy(cn->filereq, path_info, MAXREQUESTLENGTH);

    cn->query = strchr(cn->filereq, '?');
    if (cn->query != (char*) 0)
    {
        *cn->query++ = '\0'; // separate cn->filereq and cn->query
        cn->query_len = strlen(cn->query);
    }

    if (cgi_filename_match(cn->filereq))
    {
        cn->is_cgi = 1;     /* definitely a CGI script */
    }
}

static void proccgi(struct connstruct *cn)
{
    char cgienv[MAXREQUESTLENGTH];
    struct cgi_para para;
    CGI_FUNC cgi_func;

    HTTPD_DEBUGF((httpd, "[HTTPD] proccgi() - cn = %p\n", cn));

    snprintf(cgienv, sizeof(cgienv), HTTP_VERSION" 200 OK\r\n"
        "Server: %s\r\n"
        "Content-Type: text/html\r\n\r\n",
        server_version);

    special_write(cn->networkdesc, cgienv, strlen(cgienv));

    if (cn->reqtype == TYPE_HEAD)
    {
        removeconnection(cn);
        return;
    }

    para.sd = cn->networkdesc;
    para.func = special_write;
    para.cmd = cn->query;
    para.cmd_len = cn->query_len;

    cgi_func = cgi_filename_match(cn->filereq);
    cgi_func(&para);

    removeconnection(cn);
    return;
}

static int init_read_post_data(char *buf, char *data,
                                struct connstruct *cn, int old_rv)
{
    char *next = data;
    int rv = old_rv;
    char *post_data;

    HTTPD_DEBUGF((httpd, "[HTTPD] init_read_post_data() - cn = %p, old_rv = %d\n", cn, old_rv));

    /* Too much Post data to send. MAXPOSTDATASIZE should be
       configured (now it can be changed in the header file) */
    if (cn->content_length > MAXPOSTDATASIZE)
    {
       send_error(cn, 418);
       return 0;
    }

    /* remove CRLF */
    while ((*next == '\r' || *next == '\n') && (next < &buf[rv]))
       next++;

    if (cn->post_data == NULL)
    {
       /* Allocate buffer for the POST data that will be used by proccgi
          to send POST data to the CGI script */
       cn->post_data = (char *)ax_calloc(cn->content_length + 1);
    }

    cn->post_state = 0;
    cn->post_read = 0;
    post_data = cn->post_data;

    while (next < &buf[rv])
    {
       /* copy POST data to buffer */
       *post_data++ = *next++;
       cn->post_read++;
       if (cn->post_read == cn->content_length)
       {
           /* No more POST data to be copied */
           *post_data = '\0';
           return 1;
       }
    }

    /* More POST data has to be read. read_post_data will continue with that */
    cn->post_state = 1;
    return 0;
}

void read_post_data(struct connstruct *cn)
{
    char *readbuf, *next;
    char *post_data;
    int rv;

    HTTPD_DEBUGF((httpd, "[HTTPD] read_post_data() - cn = %p\n", cn));

    readbuf = (char *)ax_malloc(MAXREADLENGTH);
    if (readbuf == NULL )
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] read_post_data() - readbuf = NULL\n"));
        return ;
    }

    memset(readbuf, 0, MAXREADLENGTH);
    rv = special_read(cn->networkdesc, readbuf, MAXREADLENGTH - 1);
    if (rv <= 0)
    {
        if (rv < 0) /* really dead? */
            removeconnection(cn);
        else if(rv == 0) // TODO: Issue
            removeconnection(cn);

        ax_free(readbuf);
        return;
    }

    readbuf[rv] = '\0';
    next = readbuf;
    post_data = &cn->post_data[cn->post_read];

    while (next < &readbuf[rv])
    {
        *post_data++ = *next++;
        cn->post_read++;

        if (cn->post_read == cn->content_length)
        {
            /* No more POST data to be copied */
            *post_data='\0';
            cn->post_state = 0;
            decode_content_data(cn);
            buildactualfile(cn);
            cn->state = STATE_WANT_TO_SEND_HEAD;

            ax_free(readbuf);
            return;
        }
    }

    /* More POST data to read */
}

/* Decode string %xx -> char (in place) */
static void urldecode(char *buf)
{
    int v;
    char *p, *s, *w;

    w = p = buf;

    while (*p)
    {
        v = 0;

        if (*p == '%')
        {
            s = p;
            s++;

            if (isxdigit((int) s[0]) && isxdigit((int) s[1]))
            {
                v = hexit(s[0])*16 + hexit(s[1]);

                if (v)
                {
                    /* do not decode %00 to null char */
                    *w = (char)v;
                    p = &s[1];
                }
            }

        }

        if (!v) *w=*p;
        p++;
        w++;
    }

    *w='\0';
}

static int hexit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else
        return 0;
}

static void buildactualfile(struct connstruct *cn)
{
    char *cp;

    cp = cn->filereq;
    while (*cp == '/' || *cp == '\\')
        cp++;

    if(*cp == '\0')
    {
        memcpy(cn->actualfile, CONFIG_HTTP_HOME_HTML, MAXREQUESTLENGTH);
    }
    else
    {
        memcpy(cn->actualfile, cp, MAXREQUESTLENGTH);
    }

    HTTPD_DEBUGF((httpd, "[HTTPD] buildactualfile() - cn->filereq = %s, cn->actualfile = %s\n",
        cn->filereq, cn->actualfile));
}

static int sanitizefile(const char *buf)
{
    int len, i;

    HTTPD_DEBUGF((httpd, "[HTTPD] sanitizefile() - buf = %s\n", buf));

    /* Don't accept anything not starting with a / */
    if (*buf != '/')
        return 0;

    len = strlen(buf);
    for (i = 0; i < len; i++)
    {
        /* Check for "/." i.e. don't send files starting with a . */
        if (buf[i] == '/' && buf[i+1] == '.')
            return 0;
    }

    return 1;
}

static int sanitizehost(char *buf)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] sanitizehost() - buf = %s\n", buf));

    while (*buf != '\0')
    {
        /* Handle the port */
        if (*buf == ':')
        {
            *buf = '\0';
            return 1;
        }

        /* Enforce some basic URL rules... */
        if ((isalnum((int)(*buf)) == 0 && *buf != '-' && *buf != '.') ||
                (*buf == '.' && *(buf+1) == '.') ||
                (*buf == '.' && *(buf+1) == '-') ||
                (*buf == '-' && *(buf+1) == '.'))
            return 0;

        buf++;
    }

    return 1;
}

static void send_error(struct connstruct *cn, int err)
{
    char buf[MAXREQUESTLENGTH];
    char *title;
    char *text;

    HTTPD_DEBUGF((httpd, "[HTTPD] send_error() - err = %d\n", err));

    switch (err)
    {
        case HTTPD_BAD_REQUEST:
            title = "Bad Request";
            text = title;
            break;

        case HTTPD_FORBIDDEN:
            title = "Forbidden";
            text = "File is protected";
            break;

        case HTTPD_NOT_FOUND:
            title = "Not Found";
            text = title;
            break;

        case HTTPD_METHOD_NA:
            title = "Not Support";
            text = "Method not supported.";
            break;

        default:
            title = "Unknown";
            text = title;
            break;
    }

    snprintf(buf, sizeof(buf), HTTP_VERSION " 200 OK\n"
            "Content-Type: text/html\r\n\r\n"
            "<html><body>\n<title>%s</title>\n"
            "<h1>Error %d - %s</h1>\n</body></html>\n",
            title, err, text);

    // no matter special_write result, also need to removeconnection
    special_write(cn->networkdesc, buf, strlen(buf));
    removeconnection(cn);
}

static const char *getmimetype(const char *name)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] getmimetype() - name = %s\n", name));

    /* only bother with a few mime types - let the browser figure the rest out */
    if (strstr(name, ".htm"))
        return "text/html";
    else if (strstr(name, ".css"))
        return "text/css";
    else if (strstr(name, ".php"))
        return "application/x-http-php";
    else if (strstr(name, ".js"))
        return "application/x-javascript";
    else if (strstr(name, ".gif"))
        return "image/gif";
    else if (strstr(name, ".jpg"))
        return "image/jpeg";
    else if (strstr(name, ".jpeg"))
        return "image/jpeg";
    else
        return "application/octet-stream";
}

//Ecos
static int get_file_handle(char *filename)
{
    webpage_entry *entry;
    char *p;
    int offset;

	offset = 0;
    entry = &webpage_table[0];

    HTTPD_DEBUGF((httpd, "[HTTPD] get_file_handle() - filename = %s\n", filename));

    while (entry->path)
    {
		p = entry->path;
		while (*p == '/' || *p == '\\')
		    p++;

		if (strcmp(filename, p) == 0)
			break;

	   	offset += entry->size;
		entry++;
    }

	if (entry->path)
	{
		if (zweb_location == 0 && (entry->flag & WEBP_LOCKED) == 0)
			return 0;

		entry->offset = offset;
		return (int)entry;
	}

	return 0;
}

static int read_file(webpage_entry* entry)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] read_file() - entry = %p\n", entry));

    if (entry->path)
    {
		if (entry->flag & WEBP_COMPRESS)
        {
            // TODO:
        }
        else
		{
			entry->param = (void *)(zweb_location + entry->offset);
        }
		return (int)entry;
	}

	return 0;
}

static void decode_content_data(struct connstruct *cn)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] decode_content_data() - cn = %p, cn->content_length = %d\n", cn, cn->content_length));

    // TODO:
    if (cn->content_length)
    {
        cn->query = cn->post_data;
        cn->query_len = cn->content_length;

        HTTPD_DEBUGF((
            httpd, "[HTTPD] decode_content_data() - cn->post_data = %s, cn->content_length = %d\n",
            cn->post_data, cn->content_length));
    }
}

#if defined(CONFIG_HTTP_DIRECTORIES)
static void procdirlisting(struct connstruct *cn)
{
    char buf[MAXREQUESTLENGTH];
    int write_len = 0;

    HTTPD_DEBUGF((httpd, "[HTTPD] procdirlisting() - cn = %p\n", cn));

    if (cn->reqtype == TYPE_HEAD)
    {
        send_error(cn, 405);
        return;
    }

    snprintf(buf, sizeof(buf), HTTP_VERSION
            " 200 OK\nContent-Type: text/html\r\n\r\n"
            "<html><body>\n<title>Directory Listing</title>\n"
            "<h3>Directory listing of http://%s%s</h3><br />\n",
            cn->server_name, cn->filereq);

    write_len = special_write(cn->networkdesc, buf, strlen(buf));
    if(write_len <= 0)
    {
        if (write_len < 0) /* really dead? */
            removeconnection(cn);
        else if(write_len == 0) // TODO: Issue
            removeconnection(cn);
        return;
    }

    cn->state = STATE_DOING_DIR;
}

void procdodir(struct connstruct *cn)
{
    char buf[MAXREQUESTLENGTH];
    char encbuf[1024];
    char *file;
    char *p;
    webpage_entry *entry = &webpage_table[0];
    int write_len = 0;

    HTTPD_DEBUGF((httpd, "[HTTPD] procdodir() - cn = %p\n", cn));

    do
    {
        if (entry->path)
        {
            p = entry->path;
            while (*p == '/' || *p == '\\')
                p++;
            entry++;

            file = p;
        }
        else
        {
            snprintf(buf, sizeof(buf), "</body></html>\n");

            // no matter special_write result, also need to removeconnection
            special_write(cn->networkdesc, buf, strlen(buf));
            removeconnection(cn);
            return;
        }

        urlencode((const uint8_t *)file, encbuf);
        snprintf(buf, sizeof(buf), "<a href=\"%s%s\">%s</a><br />\n",
                cn->filereq, encbuf, file);
    } while ((write_len = special_write(cn->networkdesc, buf, strlen(buf)))>0);

    if(write_len <= 0)
    {
        if (write_len < 0) /* really dead? */
            removeconnection(cn);
        else if(write_len == 0) // TODO: Issue
            removeconnection(cn);
        return;
    }
}

/* Encode funny chars -> %xx in newly allocated storage */
/* (preserves '/' !) */
static void urlencode(const uint8_t *s, char *t)
{
    const uint8_t *p = s;
    char *tp = t;

    for (; *p; p++)
    {
        if ((*p > 0x00 && *p < ',') ||
                (*p > '9' && *p < 'A') ||
                (*p > 'Z' && *p < '_') ||
                (*p > '_' && *p < 'a') ||
                (*p > 'z' && *p < 0xA1))
        {
            sprintf((char *)tp, "%%%02X", *p);
            tp += 3;
        }
        else
        {
            *tp = *p;
            tp++;
        }
    }

    *tp='\0';

    HTTPD_DEBUGF((httpd, "[HTTPD] urlencode() - input = %s, output = %s\n", s, t));
}
#endif

static void getCurrentTime(char *date)
{
    struct tm gt;
    //hal_rtc_time_t r_time;
    char currentTime[32];

     memset(&gt, 0, sizeof(struct tm));
#if 0
    if(HAL_RTC_STATUS_OK == hal_rtc_get_time(&r_time))
    {
        // TODO: not sure
        // sntp.c sntp_set_system_time()
        gt.tm_year = r_time.rtc_year; // ?
        gt.tm_mon = r_time.rtc_mon; // ?
        gt.tm_mday = r_time.rtc_day;
        gt.tm_wday = r_time.rtc_week;
        gt.tm_hour = r_time.rtc_hour;
        gt.tm_min = r_time.rtc_min;
        gt.tm_sec = r_time.rtc_sec;
        strftime(currentTime, sizeof(currentTime), rfc1123_format, &gt);
    }
    else
#endif
    {
        // return HTML generate time
        getFileTime(time_pages_created, currentTime); // ? + global number
    }

    memcpy(date, currentTime, 32);

    HTTPD_DEBUGF((httpd, "[HTTPD] getCurrentTime() - date = %s\n", date));
}

static void getFileTime(const time_t fileTime, char *date)
{
    struct tm gt;
    char time[32];

    // gmtime_r(&fileTime, &gt);
    gt = *localtime(&fileTime);
    strftime(time, sizeof(time), rfc1123_format, &gt);

    memcpy(date, time, 32);

    HTTPD_DEBUGF((httpd, "[HTTPD] getFileTime() - fileTime = %ld, date = %s\n", (long)fileTime, date));
}

int special_write(int sd, const char *buf, size_t count)
{
    int write_len = 0, total_write_len = 0;

    HTTPD_DEBUGF((httpd, "[HTTPD] special_write() - sd = %d, buff_len = %d\n", sd, count));

    if(count == 0)
    {
        return 0;
    }

    // block send
    do
    {
        write_len = SOCKET_WRITE(sd, buf + total_write_len, count - total_write_len);
        total_write_len += write_len;

        HTTPD_DEBUGF((
            httpd, "[HTTPD] special_write() - write_len = %d, total_write_len = %d\n",
            write_len, total_write_len));
    }
    while((count != total_write_len) && (write_len > 0));

    if(write_len <= 0)
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] special_write() - Error write_len = %d\n", write_len));
        return write_len;
    }
    else
    {
        return total_write_len;
    }
}

int special_read(int sd, void *buf, size_t count)
{
    int read_len = SOCKET_READ(sd, buf, count);

    HTTPD_DEBUGF((
        httpd, "[HTTPD] special_read() - sd = %d, buff_len = %d, read_len = %d\n",
        sd, count, read_len));

    if(read_len <= 0)
        HTTPD_DEBUGF((httpd, "[HTTPD] special_read() - Error read_len = %d\n", read_len));

    return read_len;
}

