NAME := tinyengine_app

$(NAME)_SOURCES := tinyengine_app.c

GLOBAL_DEFINES += OSAL_RHINO

GLOBAL_DEFINES += BE_OS_AOS
GLOBAL_DEFINES += TINY_ENGINE

$(NAME)_COMPONENTS := log yloop cli middleware.linkkit.iotkit  connectivity.mqtt cjson netmgr middleware.common middleware.uagent.uota

$(NAME)_COMPONENTS += 3rdparty.experimental.fs.spiffs

$(NAME)_COMPONENTS += middleware.tinyengine

GLOBAL_INCLUDES += ./