/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "xz/decompress.h"

static struct xz_dec *s;

int xz_uncompress_init(struct xz_buf *stream)
{
	xz_crc32_init();
    /*
         * Support up to 32 KiB dictionary. The actually needed memory
         * is allocated once the headers have been parsed.
         */
    s = xz_dec_init(XZ_DYNALLOC, 1 << 15);
    if (s == NULL) {
    	return -1;
    }

	stream->in_pos = 0;
	stream->in_size = 0;
	stream->out_pos = 0;
	stream->out_size = 0;

	return 1;
}

int xz_uncompress_stream(struct xz_buf *stream, uint8_t *sbuf, uint32_t slen,
		uint8_t *dbuf, uint32_t dlen, uint32_t *decomp_len)
{
	int status;
	*decomp_len = 0;

	stream->in = sbuf;
	stream->out = dbuf;

	if (stream->in_pos == stream->in_size) {
		stream->in_size = slen;
	    stream->in_pos = 0;
	}

	stream->out_size = dlen;
	stream->out_pos = 0;

	status = xz_dec_run(s, stream);

	*decomp_len = stream->out_pos;

	return status;
}

void xz_uncompress_end()
{
	xz_dec_end(s);
}

static size_t xz_len_decode(const uint8_t buf[], size_t size_max, uint32_t *num)
{
    if (size_max == 0)
        return 0;

    if (size_max > 9)
        size_max = 9;

    *num = buf[0] & 0x7F;
    size_t i = 0;

    while (buf[i++] & 0x80) {
        if (i >= size_max || buf[i] == 0x00)
            return 0;

        *num |= (uint32_t)(buf[i] & 0x7F) << (i * 7);
    }

    return i;
}

uint32_t xz_index_len(uint8_t *stream_footer)
{

	uint32_t len = (stream_footer[7] << 24)
		          +  (stream_footer[6] << 16)
		          +  (stream_footer[5] << 8)
		          +  stream_footer[4];
	return (len + 1) * 4;
}

uint32_t xz_file_uncompress_size(uint8_t *index, uint32_t len)
{
	uint32_t num_len = 0;
	uint32_t uncompress_size = 0;
	uint8_t *temp = &index[1];

	num_len += xz_len_decode(temp, len, &uncompress_size);
	num_len += xz_len_decode(&temp[num_len], len - num_len, &uncompress_size);
	xz_len_decode(&temp[num_len], len - num_len, &uncompress_size);
	return uncompress_size;
}
