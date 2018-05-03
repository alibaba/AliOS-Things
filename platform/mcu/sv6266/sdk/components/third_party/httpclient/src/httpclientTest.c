/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "httpclient.h"

#define BUF_SIZE        (1024 * 1)
#define HTTP_GET_URL    "http://www.aliyun.com/"

static void httpclient_test_get(void)
{
    char *get_url = HTTP_GET_URL;    

    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf, *header;
    HTTPCLIENT_RESULT ret;
    int val_pos, val_len;
    
    printf( "\nhttpclient_test_get()\n");
    OS_MsDelay(3000);
    buf = OS_MemAlloc(BUF_SIZE);    
    header = OS_MemAlloc(BUF_SIZE);
    if (buf == NULL || header == NULL) {        
        printf("\nmemory malloc failed\n");
        return;
    }
    
    // Http "get"
    client_data.header_buf = header;    
    client_data.header_buf_len = BUF_SIZE;
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;    
    client_data.response_buf[0] = '\0';
    ret = httpclient_get(&client, get_url, &client_data);
    if (ret < 0)
        goto fail;   
   printf("\ndata received: %s\n", client_data.response_buf);

    // get response header
    if(0 == httpclient_get_response_header_value(client_data.header_buf, "Content-length", &val_pos, &val_len))
        printf("Content-length: %.*s", val_len, client_data.header_buf + val_pos);

fail:
    OS_MemFree(buf);
    OS_MemFree(header);

    // Print final log
    if (ret >= 0)    
        printf("example project test success.");
    else        
        printf("httpclient_get fail, reason:%d.", ret);
}

int httpclient_test()
{
    httpclient_test_get();
    OS_TaskDelete(NULL);
    return 0;
}

