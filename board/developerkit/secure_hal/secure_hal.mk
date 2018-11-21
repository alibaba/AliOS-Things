NAME := secure_hal

$(NAME)_PREBUILT_LIBRARY := nuoxing_id2.lib

$(NAME)_SOURCES := id2_client_demo.c secure_demo.c

GLOBAL_INCLUDES += include

GLOBAL_DEFINES += DEVELOPERKIT_SECURE