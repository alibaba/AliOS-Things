#include "mmf_source.h"
#include "h264_encode.h"

void* h264_unit_open(void)
{
    struct h264_context *h264_ctx = (struct h264_context *) malloc(sizeof(struct h264_context));
    if (h264_ctx == NULL) {
        return NULL;
    }
    memset(h264_ctx, 0, sizeof(struct h264_context));

    h264_stream_init(h264_ctx);
    h264_enc_init(h264_ctx);

    //voe_init(h264_ctx);
    //isp_init(h264_ctx);

    return h264_ctx;
}

void h264_unit_close(void* ctx)
{
    free((struct h264_context *)ctx);
}

int h264_unit_set_param(void* ctx, int cmd, int arg)
{
    return 0;
}

int h264_unit_handle(void* ctx, void* b)
{
    int ret = 0;

	ret = h264_stream_get_frame(ctx, b);

    return 0;
}

msrc_module_t h264_unit_module =
{
	.create = h264_unit_open,
	.destroy = h264_unit_close,
	.set_param = h264_unit_set_param,
	.handle = h264_unit_handle,
};
