NAME := httpc_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := HTTP app to IoT cloud

$(NAME)_SOURCES := app_entry.c     \
                   httpc_example.c \
				   maintask.c

$(NAME)_COMPONENTS += libiot_http netmgr cjson cli

GLOBAL_DEFINES += CONFIG_AOS_CLI DISABLE_SECURE_STORAGE 
