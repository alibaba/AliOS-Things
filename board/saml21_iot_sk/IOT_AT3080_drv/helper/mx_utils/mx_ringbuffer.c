#include "mx_ringbuffer.h"

/**
 * \brief Ringbuffer init
 */
int32_t ringbuffer_init(struct ringbuffer *const rb, void *buf, uint32_t size)
{
	mx_status err = kNoErr;
	require_action(rb && buf && size, exit, err = kParamErr);

	/*
	 * buf size must be aligned to power of 2
	 */
	require_action((size & (size - 1))==0, exit, err = kParamErr);

	/* size - 1 is faster in calculation */
	rb->size        = size - 1;
	rb->read_index  = 0;
	rb->write_index = rb->read_index;
	rb->buf         = (uint8_t *)buf;

exit:
	return err;
}

/**
 * \brief Get one byte from ringbuffer
 *
 */
int32_t ringbuffer_get(struct ringbuffer *const rb, uint8_t *data)
{
	mx_status err = kNotFoundErr;
	require_action(rb && data, exit, err = kParamErr);

	if (rb->write_index != rb->read_index) {
		*data = rb->buf[rb->read_index & rb->size];
		rb->read_index++;
		return kNoErr;
	}

exit:
	return err;
}

/**
 * \brief Put one byte to ringbuffer
 *
 */
int32_t ringbuffer_put(struct ringbuffer *const rb, uint8_t data)
{
	mx_status err = kNoErr;
	require_action(rb, exit, err = kParamErr);

	rb->buf[rb->write_index & rb->size] = data;

	/*
	 * buffer full strategy: new data will overwrite the oldest data in
	 * the buffer
	 */
	if ((rb->write_index - rb->read_index) > rb->size) {
		rb->read_index = rb->write_index - rb->size;
	}

	rb->write_index++;

exit:
	return err;
}

/**
 * \brief Return the element number of ringbuffer
 */
uint32_t ringbuffer_num(const struct ringbuffer *const rb)
{
	return rb->write_index - rb->read_index;
}

/**
 * \brief Flush ringbuffer
 */
uint32_t ringbuffer_flush(struct ringbuffer *const rb)
{
	rb->read_index = rb->write_index;
	return kNoErr;
}
