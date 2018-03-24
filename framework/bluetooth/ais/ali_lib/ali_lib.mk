NAME := ali_lib

$(NAME)_SOURCES := ali_auth/ali_auth.c ali_auth/sha256.c
$(NAME)_SOURCES += ali_core/ali_core.c
$(NAME)_SOURCES += ali_gap/ali_gap.c
#$(NAME)_SOURCES += ali_ota/ali_ota.c
$(NAME)_SOURCES += ali_transport/aes.c ali_transport/ali_transport.c
$(NAME)_SOURCES += ble_ais/ble_ais.c

GLOBAL_INCLUDES += ali_core common ble_ais ali_auth ali_gap ali_ota ali_transport

$(NAME)_COMPONENTS := protocols.bluetooth digest_algorithm
