NAME := tinyengine_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := an engine for javascript application development
$(NAME)_SOURCES := tinyengine_app.c

#choose JSE_TINYENGINE OR JSE_DUKTAPE here
#export AOS_CONFIG_TINYENGINE_TYPE=JSE_DUKTAPE
export AOS_CONFIG_TINYENGINE_TYPE=JSE_TINYENGINE

$(NAME)_COMPONENTS := log yloop cli feature.linkkit-mqtt cjson netmgr middleware.uagent.uota
$(NAME)_COMPONENTS += 3rdparty.experimental.fs.spiffs
$(NAME)_COMPONENTS += middleware.tinyengine

GLOBAL_INCLUDES += ./
