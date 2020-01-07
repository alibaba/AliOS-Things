

#include <string.h>
#include <stdlib.h>

#include "buffer.h"
#include "osal.h"
#include "define.h"
#include "utils.h"


struct buf *buf_new_owned(int len) {
	if (len < 0)
		return NULL;

	struct buf *buf = (struct buf *)umesh_malloc(sizeof(struct buf));
	if(buf == NULL) {
		return NULL;
	}
	memset(buf, 0, sizeof(struct buf));
	buf->data = (uint8_t *)umesh_malloc(len);
	buf->orig = buf->data;
	buf->len = len;
	buf->owned = 1;
	return buf;
}

const struct buf *buf_new_const(const uint8_t *data, int len) {
	if (len < 0)
		return NULL;
	struct buf *buf = (struct buf *)umesh_malloc(sizeof(struct buf));
	if(buf == NULL) {
		return NULL;
	}
	memset(buf, 0, sizeof(struct buf));
	buf->data = (uint8_t *) data;
	buf->orig = data;
	buf->len = len;
	buf->owned = 0;
	return buf;
}

void buf_free(const struct buf *buf) {
	if (buf->owned)
		umesh_free((void *) buf->orig);
	umesh_free((struct buf *) buf);
}

const uint8_t *buf_data(const struct buf *buf) {
	return buf->data;
}

int buf_len(const struct buf *buf) {
	return buf->len;
}

int buf_strip(const struct buf *buf, int len) {
	if (len > buf->len || len < 0)
		return UMESH_ERR_OUT_OF_BOUNDS;
	((struct buf *) buf)->data += len;
	((struct buf *) buf)->len -= len;
	return len;
}

int buf_take(const struct buf *buf, int len) {
	if (len > buf->len || len < 0)
		return UMESH_ERR_OUT_OF_BOUNDS;
	((struct buf *) buf)->len -= len;
	return len;
}

int write_u8(struct buf *buf, int offset, uint8_t value) {
	if (offset < 0 || offset + 1 > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;
	*(buf->data + offset) = value;
	return 1;
}

int write_le16(struct buf *buf, int offset, uint16_t value) {
	union array_uint16 temp16;
	
	if (offset < 0 || offset + 2 > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;

	temp16.value16 = umesh_htole16(value);
	memcpy(buf->data + offset,temp16.array, 2);
	return 2;
}

int write_be16(struct buf *buf, int offset, uint16_t value) {
	union array_uint16 temp16;
	
	if (offset < 0 || offset + 2 > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;

	temp16.value16 = umesh_htobe16(value);
	memcpy(buf->data + offset,temp16.array, 2);
	return 2;
}



int write_le32(struct buf *buf, int offset, uint32_t value) {
	union array_uint32 temp32;
	if (offset < 0 || offset + 4 > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;
		temp32.value32 = umesh_htole32(value);
	memcpy(buf->data + offset,temp32.array, sizeof(temp32.value32));
	return 4;
}

int write_be32(struct buf *buf, int offset, uint32_t value) {
	union array_uint32 temp32;
	if (offset < 0 || offset + 4 > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;
	temp32.value32 = umesh_htobe32(value);
	memcpy(buf->data + offset,temp32.array, sizeof(temp32.value32));
	return 4;
}

int write_bytes(struct buf *buf, int offset, const uint8_t *bytes, int length) {
	if (offset < 0 || offset + length > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;
	memcpy(buf->data + offset, bytes, length);
	return length;
}

int read_u8(const struct buf *buf, int offset, uint8_t *value) {
	if (offset < 0 || offset + 1 > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;
	if (value)
		*value = *(buf->data + offset);
	return 1;
}

int read_le16(const struct buf *buf, int offset, uint16_t *value) {
	union array_uint16 temp16;
	if (offset < 0 || offset + 2 > buf->len || value == NULL)
		return UMESH_ERR_OUT_OF_BOUNDS;
        memcpy(temp16.array,buf->data + offset, 2);
		*value = umesh_le16toh(temp16.value16);
	return 2;
}

int read_be16(const struct buf *buf, int offset, uint16_t *value) {
	union array_uint16 temp16;
	if (offset < 0 || offset + 2 > buf->len || value == NULL)
		return UMESH_ERR_OUT_OF_BOUNDS;
        memcpy(temp16.array, buf->data + offset, 2);
		*value = umesh_be16toh(temp16.value16);
	return 2;
}

int read_le32(const struct buf *buf, int offset, uint32_t *value) {
	union array_uint32 temp32;
	if (offset < 0 || offset + 4 > buf->len || value == NULL)
		return UMESH_ERR_OUT_OF_BOUNDS;
	memcpy(temp32.array, buf->data + offset, 4);

	*value = umesh_le32toh(temp32.value32);
	return 4;
}

int read_be32(const struct buf *buf, int offset, uint32_t *value) {
	union array_uint32 temp32;
	if (offset < 0 || offset + 4 > buf->len || value == NULL)
		return UMESH_ERR_OUT_OF_BOUNDS;
	memcpy(temp32.array, buf->data + offset, 4);

	*value = umesh_be32toh(temp32.value32);
	return 4;
}



int read_bytes(const struct buf *buf, int offset, const uint8_t **bytes, int length) {
	if (offset < 0 || offset + length > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;
	if (bytes)
		*bytes = buf->data + offset;
	return length;
}

int read_bytes_copy(const struct buf *buf, int offset, uint8_t *bytes, int length) {
	if (offset < 0 || offset + length > buf->len)
		return UMESH_ERR_OUT_OF_BOUNDS;
	if (bytes)
		memcpy(bytes, buf->data + offset, length);
	return length;
}

int read_int_string(const struct buf *buf, int offset, char *str, int length) {
	uint8_t _len;
	READ_U8(buf, offset, &_len);
	if (_len > length)
		_len = length;
	READ_BYTES_COPY(buf, offset + 1, (uint8_t *) str, _len);
	if (str)
		str[_len] = 0; /* add trailing zero */
	return _len + 1;
buffer_error:
	return UMESH_ERR_OUT_OF_BOUNDS;
}

int read_tlv(const struct buf *buf, int offset, uint8_t *type, uint16_t *len, const uint8_t **val) {
	uint8_t _type;
	uint16_t _len;
	READ_U8(buf, offset, &_type);
	READ_LE16(buf, offset + 1, &_len);
	READ_BYTES(buf, offset + 3, val, _len);
	if (type)
		*type = _type;
	if (len)
		*len = _len;
	return _len + 3;
buffer_error:
	return UMESH_ERR_OUT_OF_BOUNDS;
}
