NAME := tinyengine_app

$(NAME)_SOURCES := tinyengine_app.c

GLOBAL_DEFINES += VCALL_RHINO

GLOBAL_DEFINES += BE_OS_AOS
GLOBAL_DEFINES += TINY_ENGINE

$(NAME)_COMPONENTS := log yloop cli protocol.linkkit.iotkit  connectivity.mqtt cjson netmgr framework.common fota

$(NAME)_COMPONENTS += 3rdparty.experimental.spiffs

$(NAME)_COMPONENTS += framework.tinyengine

