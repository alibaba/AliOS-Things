#include "xzdec.h"

#if (defined BOARD_ESP8266)
#define DICT_SIZE (1<<12)
#else
#define DICT_SIZE (1<<14)
#endif

int geterror(enum xz_ret ret)
{
    if ( ret == XZ_OK || ret == XZ_STREAM_END )
    {
        return 0;
    }

    OTA_LOG_I("xzerr: %d\n", ret);
    return -1;
}

bool xz_init(struct xz_dec **s, struct xz_buf *b)
{
    if(!b || !s) {
        return false;
    }

    *s = xz_dec_init(XZ_DYNALLOC, DICT_SIZE);
    if (*s == NULL) {
        return false;
    }
    b->in_pos = 0;
    b->in_size = 0;
    return true;
}

bool xz_end(struct xz_dec *s)
{
    xz_dec_end(s);
    return true;
}

size_t xz_read(xzReadHandler *handler, struct xz_buf *b,struct xz_dec *s, unsigned long patch, unsigned char *buf, int buf_len) {

    if (handler == NULL || b == NULL || s == NULL || patch == 0 || buf == NULL || buf_len < 0)
    {  return 0; };

    if(buf_len == 0) {
         return 0;
    }

    enum xz_ret ret;
    b->out = buf;
    b->out_pos = 0;
    b->out_size = buf_len;

    while(true) {
        if (b->in_pos == b->in_size && handler->avail_size > 0) {
            size_t readsize = IN_BUF_MAX >  handler->avail_size ? handler->avail_size:IN_BUF_MAX;
            b->in_size = ota_patch_read(patch, handler->in_buf, handler->read_pos, readsize);
            handler->avail_size -= readsize;
            handler->read_pos += readsize;
            b->in_pos = 0;
            b->in = handler->in_buf;
        }

        ret = xz_dec_run(s, b);
        if(geterror(ret)) {
            return 0;
        }
        //OTA_LOG_I("\nhandler->avail_size %ld b->in_pos :%ld,b->in_size :%ld buflen %ld, %ld\n", handler->avail_size,b->in_pos, b->in_size, buf_len, b->out_pos);
        if (handler->avail_size <= 0 && b->in_pos == b->in_size
                && b->out_pos < buf_len) {
            return b->out_pos;
        }

        if (b->out_pos == buf_len) {
            return buf_len;
        }
    }
}
