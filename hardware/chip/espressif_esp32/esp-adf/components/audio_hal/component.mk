#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_ADD_INCLUDEDIRS := ./include ./driver/include
COMPONENT_SRCDIRS := .
COMPONENT_PRIV_INCLUDEDIRS := ./driver/include

COMPONENT_ADD_INCLUDEDIRS += ./driver/es8388 ./driver/es8374
COMPONENT_SRCDIRS += ./driver/es8388 ./driver/es8374

COMPONENT_ADD_INCLUDEDIRS += ./driver/es8311 ./driver/es7243
COMPONENT_SRCDIRS += ./driver/es8311 ./driver/es7243

COMPONENT_ADD_INCLUDEDIRS += ./driver/zl38063 ./driver/zl38063/api_lib ./driver/zl38063/example_apps ./driver/zl38063/firmware
COMPONENT_SRCDIRS += ./driver/zl38063 ./driver/zl38063/api_lib ./driver/zl38063/example_apps ./driver/zl38063/firmware
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/driver/zl38063/firmware -lfirmware

COMPONENT_ADD_INCLUDEDIRS += ./driver/tas5805m ./driver/es7148
COMPONENT_SRCDIRS += ./driver/tas5805m ./driver/es7148

COMPONENT_ADD_INCLUDEDIRS += ./driver/es7210
COMPONENT_SRCDIRS += ./driver/es7210
