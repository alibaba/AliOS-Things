///////////////////////////////////////////////////////////////////////////////
//
/// \file       xzdec.c
/// \brief      deCompress from stdin to stdout in multi-call mode
///
//
//  Author:     ting.guit
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////
#include "xzdec.h"

#if (defined BOARD_ESP8266)
#define DICT_SIZE (1<<12)
#else
#define DICT_SIZE (1<<14)
#endif

/* same with "enum xz_ret" */
static const char *errorinfo[] = 
   {"XZ_OK\n",
	"XZ_STREAM_END\n",
	"XZ_UNSUPPORTED_CHECK\n",
	"XZ_MEM_ERROR\n",
	"XZ_MEMLIMIT_ERROR\n",
	"XZ_FORMAT_ERROR\n",
	"XZ_OPTIONS_ERROR\n",
	"XZ_DATA_ERROR\n",
	"XZ_BUF_ERROR\n",
    "BUG!\n"
    };

int geterror(enum xz_ret ret)
{
    if ( ret == XZ_OK || ret == XZ_STREAM_END )
    {
        return 0;
    }

    ret = ret > sizeof(errorinfo)/sizeof(errorinfo[0]) ?
          sizeof(errorinfo)/sizeof(errorinfo[0]) - 1 : ret;

    LOG("err: %d\n", ret);
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



size_t xz_read(xzReadHandler *handler, struct xz_buf *b,struct xz_dec *s, unsigned long in_file, u_char *buf, int buf_len) {

    if (handler == NULL || b == NULL || s == NULL || in_file == 0 || buf == NULL || buf_len < 0)
    {  return 0; };

    if(buf_len == 0) {
         return 0;
    }

    enum xz_ret ret;
    b->out = buf;
    b->out_pos = 0;
    b->out_size = buf_len;

    while(true) {
        if (b->in_pos == b->in_size && !nbpatch_eof(in_file) && handler->avail_size > 0) {
            size_t readsize = IN_BUF_MAX >  handler->avail_size ? handler->avail_size:IN_BUF_MAX;
            b->in_size = nbpatch_read(in_file, handler->in_buf, handler->read_pos, readsize,0);

            handler->avail_size -= readsize;
            handler->read_pos += readsize;
            b->in_pos = 0;
            b->in = handler->in_buf;
        }

        ret = xz_dec_run(s, b);

        if(geterror(ret)) {
            return 0;
        }

        //LOG("\nhandler->avail_size %ld b->in_pos :%ld,b->in_size :%ld buflen %ld, %ld\n", handler->avail_size,b->in_pos, b->in_size, buf_len, b->out_pos);
        if (handler->avail_size <= 0 && b->in_pos == b->in_size
                && b->out_pos < buf_len) {
            return b->out_pos;
        }

        if (b->out_pos == buf_len) {
            return buf_len;
        }
    }
}

#if LZMA
size_t xz_Read(xzReadHandler *handler, lzma_stream *strm, unsigned long in_file, u_char *buf, int buf_len)
{
    if (handler == NULL || strm == NULL || in_file == 0 || buf == NULL || buf_len < 0)
    {  return 0; };

    if(buf_len == 0) {
        LOG("read: 0\n");
        return 0;
    }
    lzma_action action;
    lzma_ret ret_xz;
    size_t n;
    strm->next_out = buf;
    strm->avail_out = buf_len;

    int in_finished = 0;

    static unsigned long in_end = 0l;

    while (true) {
        if (strm->avail_in == 0 && !nbpatch_eof(in_file))
        {
            size_t readsize = IN_BUF_MAX >  handler->avail_size ? handler->avail_size:IN_BUF_MAX;
            n = nbpatch_read(in_file, handler->in_buf, handler->read_pos, readsize);
            if (nbpatch_error(in_file))
            {
                return 0;
            }
            handler->avail_size -= readsize;
            handler->read_pos   += readsize;
            in_end = in_file + handler->read_pos;
            //LOG("in_end %ld, %ld\n", in_end, handler->avail_size);
            strm->next_in = handler->in_buf;
            strm->avail_in = n;
        }

        if (nbpatch_eof(in_end)) {
            in_finished = 1;
        }

        action = in_finished ? LZMA_FINISH : LZMA_RUN;

        /* decompress data */
        ret_xz = lzma_code(strm, action);

        if (!get_error(ret_xz)) {
            return 0;
        }
        //LOG("\n strm->avail_in :%ld,strm->avail_out :%ld\n",  strm->avail_in, strm->avail_out);
        if (nbpatch_eof(in_file) && strm->avail_in == 0 && strm->avail_out > 0) {
            LOG("in: %ld,out:%ld\n", strm->avail_in, strm->avail_out);
            return buf_len - strm->avail_out;
        }

        if (strm->avail_out == 0) {
            return buf_len;
        }
    }
    return 0;
}
#endif

