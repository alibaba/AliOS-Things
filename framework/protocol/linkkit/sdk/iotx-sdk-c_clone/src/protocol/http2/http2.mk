NAME := libiot_http2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := ./nghttp2_version.c \
./nghttp2_hd_huffman.c \
./nghttp2_debug.c \
./nghttp2_http.c \
./nghttp2_hd.c \
./nghttp2_npn.c \
./nghttp2_map.c \
./nghttp2_priority_spec.c \
./nghttp2_hd_huffman_data.c \
./nghttp2_net.c \
./nghttp2_queue.c \
./nghttp2_buf.c \
./nghttp2_option.c \
./iotx_http2_api.c \
./nghttp2_outbound_item.c \
./nghttp2_frame.c \
./nghttp2_stream.c \
./nghttp2_helper.c \
./nghttp2_submit.c \
./nghttp2_callbacks.c \
./nghttp2_pq.c \
./nghttp2_session.c \
./nghttp2_rcbuf.c \
./nghttp2_mem.c \
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \


$(NAME)_INCLUDES := ../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/log \
../../../src/protocol/http2 \
../../../src/protocol/http2/nghttp2 \
./

GLOBAL_INCLUDES += ./nghttp2 

