/**
 * \file            lwesp_http_server.c
 * \brief           HTTP server based on callback API
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include <ctype.h>
#include "lwesp/apps/lwesp_http_server.h"
#include "lwesp/lwesp_mem.h"

#define LWESP_CFG_DBG_SERVER_TRACE            (LWESP_CFG_DBG_SERVER | LWESP_DBG_TYPE_TRACE)
#define LWESP_CFG_DBG_SERVER_TRACE_WARNING    (LWESP_CFG_DBG_SERVER | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_WARNING)
#define LWESP_CFG_DBG_SERVER_TRACE_DANGER     (LWESP_CFG_DBG_SERVER | LWESP_DBG_TYPE_TRACE | LWESP_DBG_LVL_DANGER)

/* Function prototypes, declarations in lwesp_http_server_fs.c file */
uint8_t     http_fs_data_open_file(const http_init_t* hi, http_fs_file_t* file, const char* path);
uint32_t    http_fs_data_read_file(const http_init_t* hi, http_fs_file_t* file, void** buff, size_t btr, size_t* br);
void        http_fs_data_close_file(const http_init_t* hi, http_fs_file_t* file);

/** Number of opened files in system */
uint16_t http_fs_opened_files_cnt;

#define CRLF                        "\r\n"

static char http_uri[HTTP_MAX_URI_LEN + 1];
static http_param_t http_params[HTTP_MAX_PARAMS];

/* HTTP init structure with user settings */
static const http_init_t* hi;

#if HTTP_USE_METHOD_NOTALLOWED_RESP
/**
 * \brief           Default output for method not allowed response
 */
static const char
http_data_method_not_allowed[] = ""
                                 "HTTP/1.1 405 Method Not Allowed" CRLF
                                 "Server: " HTTP_SERVER_NAME CRLF
                                 "Allow: GET"
#if HTTP_SUPPORT_POST
                                 ", POST"
#endif /* HTTP_SUPPORT_POST */
                                 CRLF
                                 CRLF
                                 "";
#endif /* HTTP_USE_METHOD_NOTALLOWED_RESP */

#if HTTP_DYNAMIC_HEADERS
/**
 * \brief           Intexes for \ref http_dynstrs array
 */
typedef enum {
    /* Response code */
    HTTP_HDR_200,
    HTTP_HDR_400,
    HTTP_HDR_404,

    /* Server response code */
    HTTP_HDR_SERVER,

    /* Content type strings */
    HTTP_HDR_HTML,
    HTTP_HDR_PNG,
    HTTP_HDR_JPG,
    HTTP_HDR_GIF,
    HTTP_HDR_CSS,
    HTTP_HDR_JS,
    HTTP_HDR_ICO,
    HTTP_HDR_XML,
    HTTP_HDR_PLAIN,
} dynamic_headers_index_t;

/**
 * \brief           Array of supported response strings
 */
static const char* const
http_dynstrs[] = {
    /* Response code */
    "HTTP/1.1 200 OK" CRLF,
    "HTTP/1.1 400 Bad Request" CRLF,
    "HTTP/1.1 404 File Not Found" CRLF,

    /* Server response code */
    "Server: " HTTP_SERVER_NAME CRLF,

    /* Content type strings */
    "Content-type: text/html" CRLF CRLF,
    "Content-type: image/png" CRLF CRLF,
    "Content-type: image/jpeg" CRLF CRLF,
    "Content-type: image/gif" CRLF CRLF,
    "Content-type: text/css" CRLF CRLF,
    "Content-type: text/javascript" CRLF CRLF,
    "Content-type: text/x-icon" CRLF CRLF,
    "Content-type: text/xml" CRLF CRLF,
    "Content-type: text/plain" CRLF CRLF,
};

/**
 * \brief           Header index and file suffix pair
 */
typedef struct {
    dynamic_headers_index_t index;
    const char* ext;
} dynamic_headers_pair_t;

/**
 * \brief           Lookup table for file suffix and content type header
 */
static const dynamic_headers_pair_t
dynamic_headers_pairs[] = {
    { HTTP_HDR_HTML,        "html" },
    { HTTP_HDR_HTML,        "htm" },
    { HTTP_HDR_HTML,        "shtml" },
    { HTTP_HDR_HTML,        "shtm" },
    { HTTP_HDR_HTML,        "ssi" },
    { HTTP_HDR_PNG,         "png" },
    { HTTP_HDR_JPG,         "jpg" },
    { HTTP_HDR_JPG,         "jpeg" },
    { HTTP_HDR_GIF,         "gif" },
    { HTTP_HDR_CSS,         "css" },
    { HTTP_HDR_JS,          "js" },
    { HTTP_HDR_ICO,         "ico" },
    { HTTP_HDR_XML,         "xml" },
};

#endif /* HTTP_DYNAMIC_HEADERS */

/**
 * \brief           List of supported file names for index page
 */
static const char*
http_index_filenames[] = {
    "/index.shtml",
    "/index.shtm"
    "/index.ssi",
    "/index.html",
    "/index.htm"
};

/**
 * \brief           List of URI suffixes where SSI tags are supported
 */
static const char*
http_ssi_suffixes[] = {
    ".shtml",
    ".shtm",
    ".ssi"
};

/**
 * \brief           List of 404 URIs
 */
static const char*
http_404_uris[] = {
    "/404.shtml",
    "/404.shtm",
    "/404.ssi",
    "/404.html",
    "/404.htm",
};

/**
 * \brief           Compare 2 strings in case insensitive way
 * \param[in]       a: String a to compare
 * \param[in]       b: String b to compare
 * \return          0 if equal, non-zero otherwise
 */
int
strcmpa(const char* a, const char* b) {
    int d;
    for (;; ++a, ++b) {
        d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a) {
            return d;
        }
    }
}

/**
 * \brief           Parse URI from HTTP request and copy it to linear memory location
 * \param[in]       p: Chain of pbufs from request
 * \return          \ref lwespOK if successfully parsed, member of \ref lwespr_t otherwise
 */
static lwespr_t
http_parse_uri(lwesp_pbuf_p p) {
    size_t pos_s, pos_e, pos_crlf, uri_len;

    pos_s = lwesp_pbuf_strfind(p, " ", 0);      /* Find first " " in request header */
    if (pos_s == LWESP_SIZET_MAX || (pos_s != 3 && pos_s != 4)) {
        return lwespERR;
    }
    pos_crlf = lwesp_pbuf_strfind(p, CRLF, 0);  /* Find CRLF position */
    if (pos_crlf == LWESP_SIZET_MAX) {
        return lwespERR;
    }
    pos_e = lwesp_pbuf_strfind(p, " ", pos_s + 1);  /* Find second " " in request header */
    if (pos_e == LWESP_SIZET_MAX) {             /* If there is no second " " */
        /*
         * HTTP 0.9 request is "GET /\r\n" without
         * space between request URI and CRLF
         */
        pos_e = pos_crlf;                       /* Use the one from CRLF */
    }

    uri_len = pos_e - pos_s - 1;                /* Get length of uri */
    if (uri_len > HTTP_MAX_URI_LEN) {
        return lwespERR;
    }
    lwesp_pbuf_copy(p, http_uri, uri_len, pos_s + 1);   /* Copy data from pbuf to linear memory */
    http_uri[uri_len] = 0;                      /* Set terminating 0 */

    return lwespOK;
}

/**
 * \brief           Extract parameters from user request URI
 * \param[in]       params: RAM variable with parameters
 * \return          Number of parameters extracted
 */
static size_t
http_get_params(char* params) {
    size_t cnt = 0;
    char* amp, *eq;

    if (params != NULL) {
        for (size_t i = 0; params != NULL && i < HTTP_MAX_PARAMS; ++i, ++cnt) {
            http_params[i].name = params;

            eq = params;
            amp = strchr(params, '&');          /* Find next & in a sequence */
            if (amp != NULL) {                  /* In case we have it */
                *amp = 0;                       /* Replace it with 0 to end current param */
                params = ++amp;                 /* Go to next one */
            } else {
                params = NULL;
            }

            eq = strchr(eq, '=');               /* Find delimiter */
            if (eq != NULL) {
                *eq = 0;
                http_params[i].value = eq + 1;
            } else {
                http_params[i].value = NULL;
            }
        }
    }
    return cnt;
}

#if HTTP_DYNAMIC_HEADERS
/**
 * \brief           Prepare dynamic headers to be sent as response to user
 * \param[in]       hs: HTTP state
 * \param[in]       uri: Request URI excluding optional parameters
 */
static void
prepare_dynamic_headers(http_state_t* hs, const char* uri) {
    char* ext, *u;
    size_t i;

    hs->dyn_hdr_idx = 0;
    hs->dyn_hdr_pos = 0;

    hs->dyn_hdr_strs[1] = http_dynstrs[HTTP_HDR_SERVER];/* Set server name */
    if (!hs->rlwesp_file_opened) {              /* This should never be the case as 404.html file exists as static */
        hs->dyn_hdr_strs[0] = http_dynstrs[HTTP_HDR_404];   /* 404 Not Found */
        hs->dyn_hdr_strs[HTTP_MAX_HEADERS - 1] = http_dynstrs[HTTP_HDR_HTML];   /* Content type text/html */
    } else {
        /*
         * Try to find CRLFCRLF sequence on static files and remove
         * the headers if dynamic headers are used
         */
        if (hs->rlwesp_file.is_static) {
            char* crlfcrlf;
            crlfcrlf = strstr((const char*)hs->rlwesp_file.data, CRLF CRLF);
            if (crlfcrlf != NULL) {             /* Skip header part of file */
                hs->rlwesp_file.size -= (const char*)crlfcrlf - (const char*)hs->rlwesp_file.data + 4;  /* Decrease file size first! */
                hs->rlwesp_file.data += (const char*)crlfcrlf - (const char*)hs->rlwesp_file.data + 4;  /* Advance file pointer */
            }
        }

        /*
         * Process with content-length response header
         *
         * In case request does not include SSI parsing,
         * script has known final content length.
         *
         * Include length in header output
         */
        hs->dyn_hdr_strs[2] = NULL;             /* No content length involved */
#if HTTP_DYNAMIC_HEADERS_CONTENT_LEN
        if (!hs->is_ssi) {
            sprintf(hs->dyn_hdr_cnt_len, "Content-Length: %d" CRLF, (int)hs->rlwesp_file.size);
            hs->dyn_hdr_strs[2] = hs->dyn_hdr_cnt_len;
        }
#endif /* HTTP_DYNAMIC_HEADERS_CONTENT_LEN */

        /*
         * Determine if file is 404 or normal user file.
         *
         * 404 file should be in type of "/404.xxx" where "xxx" includes optional extension
         */
        if (strstr(uri, "/404.") != NULL) {     /* Do we have a 404 in file name? */
            hs->dyn_hdr_strs[0] = http_dynstrs[HTTP_HDR_404];   /* 404 Not found */
        } else {
            hs->dyn_hdr_strs[0] = http_dynstrs[HTTP_HDR_200];   /* 200 OK */
        }

        /*
         * Try to find content type by inspecting file extensions
         *
         * At this point, uri should not include parameters in string as we are searching for actual file
         *
         * Step 1: Find file extension
         * Step 2: Compare file extension with table of known extensions and content types
         */

        /* Step 1: Find extension of request path */
        ext = NULL;                             /* No extension on beginning */
        u = strchr(uri, '.');                   /* Find first dot in string */
        while (u != NULL) {
            ext = u + 1;                        /* Set current u as extension but skip dot character */
            u = strchr(u + 1, '.');             /* Find next dot */
        }

        /* Step 2: Compare extension against known pairs */
        i = 0;
        if (ext != NULL) {                      /* Do we have an extension? */
            for (; i < LWESP_ARRAYSIZE(dynamic_headers_pairs); ++i) {
                if (!strcmpa(ext, dynamic_headers_pairs[i].ext)) {
                    break;
                }
            }
        }

        /* Finally set the output content type header */
        if (ext != NULL && i < LWESP_ARRAYSIZE(dynamic_headers_pairs)) {
            hs->dyn_hdr_strs[HTTP_MAX_HEADERS - 1] = http_dynstrs[dynamic_headers_pairs[i].index];  /* Set response from index directly */
        } else {
            hs->dyn_hdr_strs[HTTP_MAX_HEADERS - 1] = http_dynstrs[HTTP_HDR_PLAIN];  /* Plain text, unknown type */
        }
    }
}

/**
* \brief           Sends dynamic headers to output before sending actual content
* \param[in]       hs: HTTP state
*/
static void
send_dynamic_headers(http_state_t* hs) {
    size_t to_write, rem_len;
    if (hs->dyn_hdr_idx >= HTTP_MAX_HEADERS) {
        return;
    }

    /* Get available memory in write buffer */
    lwesp_conn_write(hs->conn, NULL, 0, 0, &hs->conn_mem_available);

    /* Try to write as much data as possible */
    while (hs->conn_mem_available && hs->dyn_hdr_idx < HTTP_MAX_HEADERS) {
        if (hs->dyn_hdr_strs[hs->dyn_hdr_idx] == NULL) {/* Check if string is set */
            hs->dyn_hdr_pos = 0;                /* Reset header position */
            ++hs->dyn_hdr_idx;                  /* Go to next string */
            continue;
        }
        rem_len = strlen(&hs->dyn_hdr_strs[hs->dyn_hdr_idx][hs->dyn_hdr_pos]);  /* Get remaining length of string to write */
        to_write = LWESP_MIN(hs->conn_mem_available, rem_len);  /* Calculate remaining maximal number of bytes we can write */

        /* Write data to connection output buffer */
        lwesp_conn_write(hs->conn, &hs->dyn_hdr_strs[hs->dyn_hdr_idx][hs->dyn_hdr_pos], to_write, 0, &hs->conn_mem_available);
        hs->written_total += to_write;

        hs->dyn_hdr_pos += to_write;            /* Advance for written position */
        if (to_write == rem_len) {              /* Did we write everything? */
            hs->dyn_hdr_pos = 0;                /* Reset output variable position */
            ++hs->dyn_hdr_idx;                  /* Increase header index string */
        }
    }
    /*
     * Flush data to output
     *
     * TODO: Do not flush it now and try to write more data together with user output?
     */
    //lwesp_conn_write(hs->conn, NULL, 0, 1, &hs->conn_mem_available);  /* Flush data to output */
}
#endif

/**
 * \brief           Get file from uri in format /folder/file?param1=value1&...
 * \param[in]       hs: HTTP state
 * \param[in]       uri: Input URI to get file for
 * \return          `1` on success, `0` otherwise
 */
uint8_t
http_get_file_from_uri(http_state_t* hs, const char* uri) {
    size_t uri_len;

    LWESP_MEMSET(&hs->rlwesp_file, 0x00, sizeof(hs->rlwesp_file));
    uri_len = strlen(uri);                      /* Get URI total length */
    if ((uri_len == 1 && uri[0] == '/') ||      /* Index file only requested */
        (uri_len > 1 && uri[0] == '/' && uri[1] == '?')) {  /* Index file + parameters */
        size_t i;
        /*
         * Scan index files and check if there is one from user
         * available to return as main file
         */
        for (i = 0; i < LWESP_ARRAYSIZE(http_index_filenames); ++i) {
            hs->rlwesp_file_opened = http_fs_data_open_file(hi, &hs->rlwesp_file, http_index_filenames[i]); /* Give me a file with desired path */
            if (hs->rlwesp_file_opened) {       /* Do we have a file? */
                uri = http_index_filenames[i];  /* Set new URI for next of this func */
                break;
            }
        }
    }

    /*
     * We still don't have a file,
     * maybe there was a request for specific file and possible parameters
     */
    if (!hs->rlwesp_file_opened) {
        char* req_params;
        size_t params_len;
        req_params = strchr(uri, '?');          /* Search for params delimiter */
        if (req_params != NULL) {               /* We found parameters? They should not exists in static strings or we may have buf! */
            req_params[0] = 0;                  /* Reset everything at this point */
            ++req_params;                       /* Skip NULL part and go to next one */
        }

        params_len = http_get_params(req_params);   /* Get request params from request */
        if (hi != NULL && hi->cgi != NULL) {    /* Check if any user specific controls to process */
            for (size_t i = 0; i < hi->cgi_count; ++i) {
                if (!strcmp(hi->cgi[i].uri, uri)) {
                    uri = hi->cgi[i].fn(http_params, params_len);
                    break;
                }
            }
        }
        hs->rlwesp_file_opened = http_fs_data_open_file(hi, &hs->rlwesp_file, uri); /* Give me a new file now */
    }

    /*
     * We still don't have a file!
     * Try with 404 error page if available by user
     */
    if (!hs->rlwesp_file_opened) {
        for (size_t i = 0; i < LWESP_ARRAYSIZE(http_404_uris); ++i) {
            uri = http_404_uris[i];
            hs->rlwesp_file_opened = http_fs_data_open_file(hi, &hs->rlwesp_file, uri); /* Get 404 error page */
            if (hs->rlwesp_file_opened) {
                break;
            }
        }
    }

    /*
     * Check if SSI should be supported on this file
     */
    hs->is_ssi = 0;                             /* By default no SSI is supported */
    if (hs->rlwesp_file_opened) {
        size_t uri_len, suffix_len;
        const char* suffix;

        uri_len = strlen(uri);                  /* Get length of URI */
        for (size_t i = 0; i < LWESP_ARRAYSIZE(http_ssi_suffixes); ++i) {
            suffix = http_ssi_suffixes[i];      /* Get suffix */
            suffix_len = strlen(suffix);        /* Get length of suffix */

            if (suffix_len < uri_len && !strcmpa(suffix, &uri[uri_len - suffix_len])) {
                hs->is_ssi = 1;                 /* We have a SSI tag */
                break;
            }
        }
    }

#if HTTP_DYNAMIC_HEADERS
    /*
     * Process with dynamic headers response only
     * if file is opened for response.
     *
     * If file is not opened, send default 404 resp or close connection
     */
    prepare_dynamic_headers(hs, uri);
#endif /* HTTP_DYNAMIC_HEADERS */

    return hs->rlwesp_file_opened;
}

#if HTTP_SUPPORT_POST
/**
 * \brief           Send the received pbuf to user space
 * \param[in]       hs: HTTP state context
 * \param[in]       pbuf: Pbuf with received data
 * \param[in]       offset: Offset in pbuf where to start reading the buffer
 */
static void
http_post_send_to_user(http_state_t* hs, lwesp_pbuf_p pbuf, size_t offset) {
    lwesp_pbuf_p new_pbuf;

    if (hi == NULL || hi->post_data_fn == NULL) {
        return;
    }

    new_pbuf = lwesp_pbuf_skip(pbuf, offset, &offset);  /* Skip pbufs and create this one */
    if (new_pbuf != NULL) {
        lwesp_pbuf_advance(new_pbuf, offset);   /* Advance pbuf for remaining bytes */

        hi->post_data_fn(hs, new_pbuf);         /* Notify user with data */
    }
}
#endif /* HTTP_SUPPORT_POST */

/**
 * \brief           Read next part of response file
 * \param[in]       hs: HTTP state
 */
static uint32_t
read_rlwesp_file(http_state_t* hs) {
    uint32_t len = 0;

    if (!hs->rlwesp_file_opened) {              /* File should be opened at this point! */
        return 0;
    }

    hs->buff_ptr = 0;                           /* Reset buffer pointer at this point */

    /* Is our memory set for some reason? */
    if (hs->buff != NULL) {                     /* Do we have already something in our buffer? */
        if (!hs->rlwesp_file.is_static) {       /* If file is not static... */
            lwesp_mem_free_s((void**)&hs->buff);/* ...free the memory... */
        }
        hs->buff = NULL;                        /* ...and reset pointer */
    }

    /*
     * Is buffer set to NULL?
     * In this case set a pointer to static memory in case of static file or
     * allocate memory for dynamic file and read it
     */
    if (hs->buff == NULL) {                     /* Do we have a buffer empty? */
        len = http_fs_data_read_file(hi, &hs->rlwesp_file, NULL, 0, NULL);  /* Get number of remaining bytes to read in file */
        if (len > 0) {                          /* Is there anything to read? On static files, this should be valid only once */
            if (hs->rlwesp_file.is_static) {    /* On static files... */
                len = http_fs_data_read_file(hi, &hs->rlwesp_file, (void**)&hs->buff, len, NULL);   /* ...simply set file pointer */
                hs->buff_len = len;             /* Set buffer length */
                if (len == 0) {                 /* Empty read? */
                    hs->buff = NULL;            /* Reset buffer */
                }
            } else {
                if (len > LWESP_CFG_CONN_MAX_DATA_LEN) {/* Limit to maximal length */
                    len = LWESP_CFG_CONN_MAX_DATA_LEN;
                }
                hs->buff_ptr = 0;               /* Reset read pointer */
                do {
                    hs->buff_len = len;
                    hs->buff = (const void*)lwesp_mem_malloc(sizeof(*hs->buff) * hs->buff_len);
                    if (hs->buff != NULL) {     /* Is memory ready? */
                        /* Read file directly and stop everything */
                        if (!http_fs_data_read_file(hi, &hs->rlwesp_file, (void**)&hs->buff, hs->buff_len, NULL)) {
                            lwesp_mem_free_s((void**)&hs->buff);
                        }
                        break;
                    }
                } while ((len >>= 1) > 64);
            }
        }
    }

    return hs->buff != NULL;                    /* Do we have our memory ready? */
}

/**
 * \brief           Send response using SSI processing
 * \param[in]       hs: HTTP state
 */
static void
send_response_ssi(http_state_t* hs) {
    uint8_t reset = 0;
    uint8_t ch;

    LWESP_DEBUGF(LWESP_CFG_DBG_SERVER_TRACE, "[HTTP SERVER] processing with SSI\r\n");

    /* First get available memory in output buffer */
    lwesp_conn_write(hs->conn, NULL, 0, 0, &hs->conn_mem_available);/* Get available memory and/or create a new buffer if possible */

    /*
     * Check if we have to send temporary buffer,
     * because of wrong TAG format set by user
     */
    if (hs->ssi_tag_buff_written < hs->ssi_tag_buff_ptr) {  /* Do we have to send something from SSI buffer? */
        size_t len;
        len = LWESP_MIN(hs->ssi_tag_buff_ptr - hs->ssi_tag_buff_written, hs->conn_mem_available);
        if (len > 0) {                          /* More data to send? */
            lwesp_conn_write(hs->conn, &hs->ssi_tag_buff[hs->ssi_tag_buff_written], len, 0, &hs->conn_mem_available);
            hs->written_total += len;           /* Increase total number of written elements */
            hs->ssi_tag_buff_written += len;    /* Increase total number of written SSI buffer */

            if (hs->ssi_tag_buff_written == hs->ssi_tag_buff_ptr) {
                hs->ssi_tag_buff_ptr = 0;       /* Reset pointer */
            }
        }
    }

    /* Are we ready to read more data? */
    if (hs->buff == NULL || hs->buff_ptr == hs->buff_len) {
        read_rlwesp_file(hs);                   /* Read more file at this point */
    }

    /*
     * Process remaining SSI tag buffer
     * Buffer should be ready from response file function call
     */
    if (hs->buff != NULL) {
        while (hs->buff_ptr < hs->buff_len && hs->conn_mem_available) { /* Process entire buffer if possible */
            ch = hs->buff[hs->buff_ptr];        /* Get next character */
            switch (hs->ssi_state) {
                case HTTP_SSI_STATE_WAIT_BEGIN: {
                    if (ch == HTTP_SSI_TAG_START[0]) {
                        hs->ssi_tag_buff[0] = ch;
                        hs->ssi_tag_buff_ptr = 1;
                        hs->ssi_state = HTTP_SSI_STATE_BEGIN;
                    } else {
                        reset = 1;
                    }
                    break;
                }
                case HTTP_SSI_STATE_BEGIN: {
                    if (hs->ssi_tag_buff_ptr < HTTP_SSI_TAG_START_LEN
                        && ch == HTTP_SSI_TAG_START[hs->ssi_tag_buff_ptr]) {
                        hs->ssi_tag_buff[hs->ssi_tag_buff_ptr] = ch;
                        ++hs->ssi_tag_buff_ptr;

                        if (hs->ssi_tag_buff_ptr == HTTP_SSI_TAG_START_LEN) {
                            hs->ssi_state = HTTP_SSI_STATE_TAG;
                            hs->ssi_tag_len = 0;
                        }
                    } else {
                        reset = 1;
                    }
                    break;
                }
                case HTTP_SSI_STATE_TAG: {
                    if (ch == HTTP_SSI_TAG_END[0]) {
                        hs->ssi_tag_buff[hs->ssi_tag_buff_ptr++] = ch;
                        hs->ssi_state = HTTP_SSI_STATE_END;
                    } else {
                        if ((hs->ssi_tag_buff_ptr - HTTP_SSI_TAG_START_LEN) < HTTP_SSI_TAG_MAX_LEN) {
                            hs->ssi_tag_buff[hs->ssi_tag_buff_ptr] = ch;
                            ++hs->ssi_tag_buff_ptr;
                            ++hs->ssi_tag_len;
                        } else {
                            reset = 1;
                        }
                    }
                    break;
                }
                case HTTP_SSI_STATE_END: {
                    if ((hs->ssi_tag_buff_ptr - HTTP_SSI_TAG_START_LEN - hs->ssi_tag_len) < HTTP_SSI_TAG_END_LEN
                        && ch == HTTP_SSI_TAG_END[(hs->ssi_tag_buff_ptr - HTTP_SSI_TAG_START_LEN - hs->ssi_tag_len)]) {

                        hs->ssi_tag_buff[hs->ssi_tag_buff_ptr++] = ch;

                        /* Did we reach end of tag and are ready to get replacement from user? */
                        if (hs->ssi_tag_buff_ptr == (HTTP_SSI_TAG_START_LEN + hs->ssi_tag_len + HTTP_SSI_TAG_END_LEN)) {
                            hs->ssi_tag_buff[HTTP_SSI_TAG_START_LEN + hs->ssi_tag_len] = 0;

                            hs->ssi_tag_process_more = 0;
                            if (hi != NULL && hi->ssi_fn != NULL) {
                                /* Call user function */
                                hs->ssi_tag_process_more = !hi->ssi_fn(hs, &hs->ssi_tag_buff[HTTP_SSI_TAG_START_LEN], hs->ssi_tag_len);
                            }
                            hs->ssi_state = HTTP_SSI_STATE_WAIT_BEGIN;
                            hs->ssi_tag_len = 0;
                            hs->ssi_tag_buff_ptr = 0;   /* Manually reset everything to prevent anything to be sent */
                        }
                    } else {
                        reset = 1;
                    }
                    break;
                }
                default:
                    break;
            }

            if (reset) {
                reset = 0;
                if (hs->ssi_tag_buff_ptr > 0) { /* Do we have to send something from temporary TAG buffer? */
                    size_t len;

                    len = LWESP_MIN(hs->ssi_tag_buff_ptr, hs->conn_mem_available);
                    lwesp_conn_write(hs->conn, hs->ssi_tag_buff, len, 0, &hs->conn_mem_available);
                    hs->written_total += len;   /* Increase total written length */
                    hs->ssi_tag_buff_written = len; /* Set length of number of written buffer */
                    if (len == hs->ssi_tag_buff_ptr) {
                        hs->ssi_tag_buff_ptr = 0;
                    }
                }
                if (hs->conn_mem_available > 0) {   /* Is there memory to write a current byte? */
                    lwesp_conn_write(hs->conn, &ch, 1, 0, &hs->conn_mem_available);
                    ++hs->written_total;
                    ++hs->buff_ptr;
                }
                hs->ssi_state = HTTP_SSI_STATE_WAIT_BEGIN;
            } else {
                ++hs->buff_ptr;
            }
        }
    }
    lwesp_conn_write(hs->conn, NULL, 0, 1, &hs->conn_mem_available);/* Flush to output if possible */
}

/**
 * \brief           Send more data without SSI tags parsing
 * \param[in]       hs: HTTP state
 */
static void
send_response_no_ssi(http_state_t* hs) {
    LWESP_DEBUGF(LWESP_CFG_DBG_SERVER_TRACE, "[HTTP SERVER] processing NO SSI\r\n");

    /* Are we ready to read more? */
    if (hs->buff == NULL || hs->written_total == hs->sent_total) {
        read_rlwesp_file(hs);                   /* Try to read response file */
    }

    /*
     * Do we have a file?
     * Static file should be processed only once at the end
     * as entire memory can be send at a time
     */
    if (hs->buff != NULL) {
        const uint8_t* b = hs->buff;
        size_t blen = hs->buff_len;

#if HTTP_DYNAMIC_HEADERS
        /*
         * In case we still have remaining memory from dynamic headers write,
         * try to write more to fill packet to send as much data as possible at single time
         */
        if (hs->conn_mem_available > 0) {
            size_t to_write;
            to_write = LWESP_MIN(hs->buff_len, hs->conn_mem_available);
            lwesp_conn_write(hs->conn, b, to_write, 0, &hs->conn_mem_available);
            hs->written_total += to_write;
            blen -= to_write;
            b += to_write;
        }
#endif /* HTTP_DYNAMIC_HEADERS */

        if (blen > 0) {
            if (lwesp_conn_send(hs->conn, b, blen, NULL, 0) == lwespOK) {
                hs->written_total += blen;      /* Set written total length */
            }
        }
    }
}

/**
 * \brief           Send response back to connection
 * \param[in]       hs: HTTP state
 * \param[in]       ft: Flag indicating function was called first time to send the response
 */
static void
send_response(http_state_t* hs, uint8_t ft) {
    uint8_t close = 0;

    if (!hs->process_resp ||                    /* Not yet ready to process response? */
        (hs->written_total > 0 && hs->written_total != hs->sent_total)) {   /* Did we wrote something but didn't send yet? */
        return;
    }

    /*
     * Do we have a file ready to be send?
     * At this point it should be opened already if request method is valid
     */
    if (hs->rlwesp_file_opened) {
#if HTTP_DYNAMIC_HEADERS
        uint8_t send_dyn_head = 0;
        /*
         * Before processing actual output, make sure
         * dynamic headers were sent to client output
         */
        if (hs->dyn_hdr_idx < HTTP_MAX_HEADERS) {
            send_dynamic_headers(hs);           /* Send dynamic headers to output */
            send_dyn_head = 1;
        }
        if (hs->dyn_hdr_idx >= HTTP_MAX_HEADERS)
#endif /* HTTP_DYNAMIC_HEADERS */
        {
            /* Process and send more data to output */
            if (hs->is_ssi) {                   /* In case of SSI request, process data using SSI */
                send_response_ssi(hs);          /* Send response using SSI parsing */
            } else {
                send_response_no_ssi(hs);       /* Send response without SSI parsing */
            }

            /*
             * Shall we hare directly close a connection if buff is NULL?
             * Maybe check first if problem was memory and try next time again
             *
             * Currently this is a solution to close the file
             */
            if (hs->buff == NULL) {             /* Sent everything or problem somehow? */
                close = 1;
            }
        }
#if HTTP_DYNAMIC_HEADERS
        /*
         * If we were sending header data,
         * force flush, no matter on a fact if response
         * functions did flush it already.
         *
         * Maybe response functions didn't response anything?
         */
        if (send_dyn_head) {
            lwesp_conn_write(hs->conn, NULL, 0, 1, &hs->conn_mem_available);
        }
#endif /* HTTP_DYNAMIC_HEADERS */
    } else  {
#if HTTP_USE_METHOD_NOTALLOWED_RESP
        if (hs->req_method == HTTP_METHOD_NOTALLOWED) { /* Is request method not allowed? */
            lwesp_conn_send(hs->conn, http_data_method_not_allowed, sizeof(http_data_method_not_allowed) - 1, NULL, 0);
            /* Don't set number of bytes written to prevent recursion */
        } else
#endif /* HTTP_USE_METHOD_NOT_ALLOWED_RESPONSE */
        {
            /*
             * Since user should include _fs.c file and 404 response is part of it,
             * we should never enter here. However in case user does
             * hard modifications, we may enter here.
             *
             * Think about "hard" 404 response before closing?
             */
        }
        close = 1;                              /* Close connection, file is not opened */
    }

    if (close) {
        lwesp_conn_close(hs->conn, 0);          /* Close the connection as no file opened in this case */
    }
}

/**
 * \brief           Server connection callback
 * \param[in]       evt: Pointer to callback data
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
static lwespr_t
http_evt(lwesp_evt_t* evt) {
    uint8_t close = 0;
    lwesp_conn_p conn;
    http_state_t* hs = NULL;

    conn = lwesp_conn_get_from_evt(evt);        /* Get connection from event */
    if (conn != NULL) {
        hs = lwesp_conn_get_arg(conn);          /* Get connection argument */
    }
    switch (lwesp_evt_get_type(evt)) {
        /* A new connection just became active */
        case LWESP_EVT_CONN_ACTIVE: {
            LWESP_DEBUGF(LWESP_CFG_DBG_SERVER_TRACE_WARNING, "[HTTP SERVER] Conn %d active\r\n",
                         (int)lwesp_conn_getnum(conn));
            hs = lwesp_mem_calloc(1, sizeof(*hs));
            if (hs != NULL) {
                hs->conn = conn;                /* Save connection handle */
                lwesp_conn_set_arg(conn, hs);   /* Set argument for connection */
            } else {
                LWESP_DEBUGF(LWESP_CFG_DBG_SERVER_TRACE_WARNING,
                             "[HTTP SERVER] Cannot allocate memory for http state\r\n");
                close = 1;                      /* No memory, close the connection */
            }
            break;
        }

        /* Data received on connection */
        case LWESP_EVT_CONN_RECV: {
            lwesp_pbuf_p p;
            size_t pos;

            p = lwesp_evt_conn_recv_get_buff(evt);  /* Get received buffer */
            if (hs != NULL) {                   /* Do we have a valid http state? */
                /*
                 * Check if we have to receive headers data first
                 * before we can proceed with everything else
                 */
                if (!hs->headers_received) {    /* Are we still waiting for headers data? */
                    if (hs->p == NULL) {
                        hs->p = p;              /* This is a first received packet */
                    } else {
                        lwesp_pbuf_cat(hs->p, p);   /* Add new packet to the end of linked list of recieved data */
                    }
                    lwesp_pbuf_ref(p);          /* Increase reference counter */

                    /*
                     * Check if headers are fully received.
                     * To know this, search for "\r\n\r\n" sequence in received data
                     */
                    if ((pos = lwesp_pbuf_strfind(hs->p, CRLF CRLF, 0)) != LWESP_SIZET_MAX) {
                        uint8_t http_uri_parsed;
                        LWESP_DEBUGF(LWESP_CFG_DBG_SERVER_TRACE, "[HTTP SERVER] HTTP headers received!\r\n");
                        hs->headers_received = 1;   /* Flag received headers */

                        /* Parse the URI, process request and open response file */
                        http_uri_parsed = http_parse_uri(hs->p) == lwespOK;

#if HTTP_SUPPORT_POST
                        /* Check for request method used on this connection */
                        if (!lwesp_pbuf_strcmp(hs->p, "POST ", 0)) {
                            size_t data_pos, pbuf_total_len;

                            hs->req_method = HTTP_METHOD_POST;  /* Save a new value as POST method */

                            /*
                             * At this point, all headers are received
                             * We can start process them into something useful
                             */
                            data_pos = pos + 4; /* Ignore 4 bytes of CRLF sequence */

                            /*
                             * Try to find content length on this request
                             * search for 2 possible values "Content-Length" or "content-length" parameters
                             */
                            hs->content_length = 0;
                            if (((pos = lwesp_pbuf_strfind(hs->p, "Content-Length:", 0)) != LWESP_SIZET_MAX) ||
                                (pos = lwesp_pbuf_strfind(hs->p, "content-length:", 0)) != LWESP_SIZET_MAX) {
                                uint8_t ch;

                                pos += 15;      /* Skip this part */
                                if (lwesp_pbuf_get_at(hs->p, pos, &ch) && ch == ' ') {
                                    ++pos;
                                }
                                lwesp_pbuf_get_at(hs->p, pos, &ch);
                                while (ch >= '0' && ch <= '9') {
                                    hs->content_length = 10 * hs->content_length + (ch - '0');
                                    ++pos;
                                    if (!lwesp_pbuf_get_at(hs->p, pos, &ch)) {
                                        break;
                                    }
                                }
                            }

                            /* Check if we are expecting any data on POST request */
                            if (hs->content_length > 0) {
                                /*
                                 * Call user POST start method here
                                 * to notify him to prepare himself to receive POST data
                                 */
                                if (hi != NULL && hi->post_start_fn != NULL) {
                                    hi->post_start_fn(hs, http_uri, hs->content_length);
                                }

                                /*
                                 * Check if there is anything to send already
                                 * to user from data part of request
                                 */
                                pbuf_total_len = lwesp_pbuf_length(hs->p, 1);   /* Get total length of current received pbuf */
                                if ((pbuf_total_len - data_pos) > 0) {
                                    hs->content_received = pbuf_total_len - data_pos;

                                    /* Send data to user */
                                    http_post_send_to_user(hs, hs->p, data_pos);

                                    /*
                                     * Did we receive everything in single packet?
                                     * Close POST loop at this point and notify user
                                     */
                                    if (hs->content_received >= hs->content_length) {
                                        hs->process_resp = 1;   /* Process with response to user */
                                        if (hi != NULL && hi->post_end_fn != NULL) {
                                            hi->post_end_fn(hs);
                                        }
                                    }
                                }
                            } else {
                                hs->process_resp = 1;
                            }
                        } else
#else /* HTTP_SUPPORT_POST */
                        LWESP_UNUSED(pos);
#endif /* !HTTP_SUPPORT_POST */
                        {
                            if (!lwesp_pbuf_strcmp(hs->p, "GET ", 0)) {
                                hs->req_method = HTTP_METHOD_GET;
                                hs->process_resp = 1;   /* Process with response to user */
                            } else {
                                hs->req_method = HTTP_METHOD_NOTALLOWED;
                                hs->process_resp = 1;
                            }
                        }

                        /*
                         * If uri was parsed succssfully and if method is allowed,
                         * then open and prepare file for future response
                         */
                        if (http_uri_parsed && hs->req_method != HTTP_METHOD_NOTALLOWED) {
                            http_get_file_from_uri(hs, http_uri);   /* Open file */
                        }
                    }
                } else {
#if HTTP_SUPPORT_POST
                    /*
                     * We are receiving request data now
                     * as headers are already received
                     */
                    if (hs->req_method == HTTP_METHOD_POST) {
                        /* Did we receive all the data on POST? */
                        if (hs->content_received < hs->content_length) {
                            size_t tot_len;

                            tot_len = lwesp_pbuf_length(p, 1);  /* Get length of pbuf */
                            hs->content_received += tot_len;

                            http_post_send_to_user(hs, p, 0);   /* Send data directly to user */

                            /* Check if everything received */
                            if (hs->content_received >= hs->content_length) {
                                hs->process_resp = 1;   /* Process with response to user */

                                /* Stop the response part here! */
                                if (hi != NULL && hi->post_end_fn) {
                                    hi->post_end_fn(hs);
                                }
                            }
                        }
                    } else
#endif /* HTTP_SUPPORT_POST */
                    {
                        /* Protocol violation at this point! */
                    }
                }

                /* Do the processing on response */
                if (hs->process_resp) {
                    send_response(hs, 1);       /* Send the response data */
                }
            } else {
                close = 1;
            }
            lwesp_conn_recved(conn, p);         /* Notify stack about received data */
            break;
        }

        /* Data send event */
        case LWESP_EVT_CONN_SEND: {
            size_t len;
            lwespr_t res;
            res = lwesp_evt_conn_send_get_result(evt);
            if (res == lwespOK && hs != NULL) {
                len = lwesp_evt_conn_send_get_length(evt);  /* Get length */
                LWESP_DEBUGF(LWESP_CFG_DBG_SERVER_TRACE,
                             "[HTTP SERVER] data sent with %d bytes\r\n", (int)len);
                hs->sent_total += len;          /* Increase number of bytes sent */
                send_response(hs, 0);           /* Send more data if possible */
            } else {
                LWESP_DEBUGW(LWESP_CFG_DBG_SERVER_TRACE_DANGER, res != lwespOK,
                             "[HTTP SERVER] data send error. Closing connection..\r\n");
                close = 1;
            }
            break;
        }

        /* Connection was just closed, either forced by user or by remote side */
        case LWESP_EVT_CONN_CLOSE: {
            LWESP_DEBUGF(LWESP_CFG_DBG_SERVER_TRACE, "[HTTP SERVER] connection closed\r\n");
            if (hs != NULL) {
#if HTTP_SUPPORT_POST
                if (hs->req_method == HTTP_METHOD_POST) {
                    if (hs->content_received < hs->content_length) {
                        if (hi != NULL && hi->post_end_fn) {
                            hi->post_end_fn(hs);
                        }
                    }
                }
#endif /* HTTP_SUPPORT_POST */
                if (hs->p != NULL) {
                    lwesp_pbuf_free(hs->p);     /* Free packet buffer */
                    hs->p = NULL;
                }
                if (hs->rlwesp_file_opened) {   /* Is file opened? */
                    uint8_t is_static = hs->rlwesp_file.is_static;
                    http_fs_data_close_file(hi, &hs->rlwesp_file);  /* Close file at this point */
                    if (!is_static && hs->buff != NULL) {
                        lwesp_mem_free_s((void**)&hs->buff);
                    }
                    hs->rlwesp_file_opened = 0; /* File is not opened anymore */
                }
                lwesp_mem_free_s((void**)&hs);
            }
            break;
        }

        /* Poll the connection */
        case LWESP_EVT_CONN_POLL: {
            if (hs != NULL) {
                send_response(hs, 0);           /* Send more data if possible */
            } else {
                close = 1;
            }
            break;
        }
        default:
            break;
    }

    if (close) {                                /* Do we have to close a connection? */
        lwesp_conn_close(conn, 0);              /* Close a connection */
    }

    return lwespOK;
}

/**
 * \brief           Initialize HTTP server at specific port
 * \param[in]       init: Initialization structure for server
 * \param[in]       port: Port for HTTP server, usually 80
 * \return          \ref lwespOK on success, member of \ref lwespr_t otherwise
 */
lwespr_t
lwesp_http_server_init(const http_init_t* init, lwesp_port_t port) {
    lwespr_t res;
    if ((res = lwesp_set_server(1, port, LWESP_CFG_MAX_CONNS, 80, http_evt, NULL, NULL, 1)) == lwespOK) {
        hi = init;
    }
    return res;
}

/**
 * \brief           Write data directly to connection from callback
 * \note            This function may only be called from SSI callback function for HTTP server
 * \param[in]       hs: HTTP state
 * \param[in]       data: Data to write
 * \param[in]       len: Length of bytes to write
 * \return          Number of bytes written
 */
size_t
lwesp_http_server_write(http_state_t* hs, const void* data, size_t len) {
    lwesp_conn_write(hs->conn, data, len, 0, &hs->conn_mem_available);
    hs->written_total += len;                   /* Increase total length */
    return len;
}
