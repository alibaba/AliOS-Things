ifndef spiffs
$(warn defaulting path to generic spiffs module, spiffs variable not set)
spiffs = ../generic/spiffs
endif
FLAGS	+= -DCONFIG_BUILD_SPIFFS
INC	+= -I${spiffs}/src
CPATH	+= ${spiffs}/src
CFILES	+= spiffs_nucleus.c
CFILES	+= spiffs_gc.c
CFILES	+= spiffs_hydrogen.c
CFILES	+= spiffs_cache.c
CFILES	+= spiffs_check.c
