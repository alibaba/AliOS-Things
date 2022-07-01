/**
 * \file            lwesp_http_server_fs.c
 * \brief           HTTP server file system wrapper
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
#include "lwesp/apps/lwesp_http_server.h"
#include "lwesp/lwesp_mem.h"

/* Number of opened files in system */
extern uint16_t http_fs_opened_files_cnt;

#if HTTP_USE_DEFAULT_STATIC_FILES
/**
 * \brief           Default index.html file including response headers
 */
static const uint8_t
responseData[] = ""
                 "HTTP/1.1 200 OK\r\n"
                 "Server: " HTTP_SERVER_NAME "\r\n"
                 "Content-Type: text/html\r\n"
                 "\r\n"
                 "<html>\n"
                 "   <head>\n"
                 "       <title><!--#title--></title>\n"
                 "       <meta http-equiv=\"Refresh\" content=\"1\" />\n"
                 "       <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js\"></script>\n"
                 "       <script src=\"/js/js.js\" type=\"text/javascript\"></script>\n"
                 "       <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.2/css/bootstrap.min.css\" integrity=\"sha384-PsH8R72JQ3SOdhVi3uxftmaW6Vc51MKb0q5P2rRUpPvrszuE4W1povHYgTpBfshb\" crossorigin=\"anonymous\" />\n"
                 "       <link rel=\"stylesheet\" type=\"text/css\" href=\"/css/style.css\" />\n"
                 "   </head>\n"
                 "   <body>\n"
                 "       <div class=\"container\">\n"
                 "           <h1>Welcome to web server hosted by ESP8266 Wi-Fi module!</h1>\n"
                 "       </div>\n"
                 "       <div id=\"maindiv\"></div>"
                 "       <footer>\n"
                 "           <div class=\"container\">\n"
                 "               Copyright &copy; 2017. All rights reserved. Webserver is hosted on ESP8266.\n"
                 "           </div>\n"
                 "       </footer>\n"
                 "   </body>\n"
                 "</html>\n";

/**
 * \brief           Default CSS response besides bootstrap response
 */
static const uint8_t
responseData_css[] = ""
                     "HTTP/1.1 200 OK\r\n"
                     "Server: " HTTP_SERVER_NAME "\r\n"
                     "Content-Type: text/css\r\n"
                     "\r\n"
                     "html, body { margin: 0; padding: 0; color: blue; font-family: Arial, Tahoma; }\r\n"
                     "h1 { font-size: 22px; }\n"
                     "footer .container { width: 1000px; padding: 6px 3px; border: 1px solid #000000; font-size: 11px; }\n"
                     "footer { position: fixed; bottom: 0; width: 100%; background: brown; color: #DDDDDD; }\n"
                     "";

/**
 * \brief           Default static javascript file
 *
 *                  Script will write a paragraph using jQuery to main div
 */
static const uint8_t
responseData_js1[] = ""
                     "HTTP/1.1 200 OK\r\n"
                     "Server: " HTTP_SERVER_NAME "\r\n"
                     "Content-Type: text/javascript\r\n"
                     "\r\n"
                     "jQuery(document).ready(function() {\n"
                     "   jQuery(\"#maindiv\").append(\"<p>This paragraphs was written using jQuery</p>\");\n"
                     "})\n";
#endif /* HTTP_USE_DEFAULT_STATIC_FILES */

/**
 * \brief           Default 404 file
 */
static const uint8_t
responseData_404[] = ""
                     "HTTP/1.1 404 Not Found\r\n"
                     "Server: " HTTP_SERVER_NAME "\r\n"
                     "Content-Type: text/html\r\n"
                     "\r\n"
                     "<html><body><h1>404 Page not found!</h1></body></html>\n";

/**
 * \brief           List of dummy files for output on user request
 */
const http_fs_file_table_t
http_fs_static_files[] = {
#if HTTP_USE_DEFAULT_STATIC_FILES
    {"/index.html",         responseData,       sizeof(responseData) - 1},
    {"/index.shtml",        responseData,       sizeof(responseData) - 1},
    {"/css/style.css",      responseData_css,   sizeof(responseData_css) - 1},
    {"/js/js.js",           responseData_js1,   sizeof(responseData_js1) - 1},
#endif /* HTTP_USE_DEFAULT_STATIC_FILES */
    {"/404.html",           responseData_404,   sizeof(responseData_404) - 1},
};

/**
 * \brief           Open file from file system
 * \param[in]       hi: HTTP init structure
 * \param[in]       file: Pointer to file structure
 * \param[in]       path: File path to open
 * \return          `1` on success, `0` otherwise
 */
uint8_t
http_fs_data_open_file(const http_init_t* hi, http_fs_file_t* file, const char* path) {
    uint8_t i, res;

    file->fptr = 0;
    if (hi != NULL && hi->fs_open != NULL) {    /* Is user defined file system ready? */
        file->rem_open_files = &http_fs_opened_files_cnt;   /* Set pointer to opened files */
        res = hi->fs_open(file, path);          /* Try to read file from user file system */
        if (res) {
            ++http_fs_opened_files_cnt;         /* Increase number of opened files */

            file->is_static = 0;                /* File is not static */
            return 1;                           /* File is opened! */
        }
    }

    /*
     * Try to open static file if available
     */
    for (i = 0; i < LWESP_ARRAYSIZE(http_fs_static_files); ++i) {
        if (path != NULL && !strcmp(http_fs_static_files[i].path, path)) {
            LWESP_MEMSET(file, 0x00, sizeof(*file));

            file->size = http_fs_static_files[i].size;
            file->data = (uint8_t*)http_fs_static_files[i].data;
            file->is_static = 1;                /* Set to 0 for testing purposes */
            return 1;
        }
    }
    return 0;
}

/**
 * \brief           Read part of file or check if we have more data to read
 * \param[in]       hi: HTTP init structure
 * \param[in]       file: File handle
 * \param[in]       buff: Pointer to buffer to save read data
 * \param[in]       btr: Number of bytes to read and write to buffer
 * \param[out]      br: Pointer to save number of bytes read to buffer
 * \return          Number of bytes read or number of remaining bytes ready to be read
 */
uint32_t
http_fs_data_read_file(const http_init_t* hi, http_fs_file_t* file, void** buff, size_t btr, size_t* br) {
    uint32_t len;

    len = file->size - file->fptr;              /* Calculate remaining length */
    if (buff == NULL) {                         /* If there is no buffer */
        if (file->is_static) {                  /* Check static file */
            return len;                         /* Simply return difference */
        } else if (hi != NULL && hi->fs_read != NULL) { /* Check for read function */
            return hi->fs_read(file, NULL, 0);  /* Call a function for dynamic file check */
        }
        return 0;                               /* No bytes to read */
    }

    len = LWESP_MIN(btr, len);                  /* Get number of bytes we can read */
    if (file->is_static) {                      /* Is file static? */
        *buff = (void*)&file->data[file->fptr]; /* Set a new address pointer only */
    } else if (hi != NULL && hi->fs_read != NULL) {
        len = hi->fs_read(file, *buff, len);    /* Read and return number of bytes read */
    } else {
        return 0;
    }
    file->fptr += len;                          /* Incrase current file pointer */
    if (br != NULL) {
        *br = len;
    }
    return len;
}

/**
 * \brief           Close file handle
 * \param[in]       hi: HTTP init structure
 * \param[in]       file: Pointer to file handle to close
 */
void
http_fs_data_close_file(const http_init_t* hi, http_fs_file_t* file) {
    if (!file->is_static && hi != NULL && hi->fs_close != NULL) {
        if (hi->fs_close(file)) {               /* Close file handle */
            --http_fs_opened_files_cnt;         /* Decrease number of files opened */
        }
    }
}
