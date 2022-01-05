set(COMPONENT_ADD_INCLUDEDIRS cloud_services/include include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_PRIV_REQUIRES esp_http_client jsmn mbedtls audio_sal)

set(COMPONENT_SRCS ./json_utils.c cloud_services/aws_sig_v4_signing.c cloud_services/baidu_access_token.c)

register_component()
