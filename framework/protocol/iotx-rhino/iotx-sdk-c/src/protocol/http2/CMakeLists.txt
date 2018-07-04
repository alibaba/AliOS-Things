INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/http2/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_http2 STATIC
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_http.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_outbound_item.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_version.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_hd_huffman.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_session.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_net.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/iotx_http2_api.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_stream.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_helper.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_priority_spec.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_buf.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_frame.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_hd_huffman_data.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_debug.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_queue.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_submit.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_mem.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_callbacks.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_hd.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_option.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_npn.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_rcbuf.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_pq.c
    ${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2_map.c
    ${EXTRA_C_SOURCES}
)

