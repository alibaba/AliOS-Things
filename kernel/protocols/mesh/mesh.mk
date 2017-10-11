NAME := mesh

GLOBAL_INCLUDES += include

$(NAME)_INCLUDES += include

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
$(NAME)_PREBUILT_LIBRARY := lib/linuxhost/libmesh.a 
else ifeq ($(findstring mk3060, $(BUILD_STRING)), mk3060)
$(NAME)_PREBUILT_LIBRARY := lib/mk3060/libmesh.a 
endif

MESHDEBUG ?= 1
ifeq ($(MESHDEBUG), 1)
GLOBAL_DEFINES += CONFIG_YOS_MESH_DEBUG
endif

MESHSUPER ?= 1
ifeq ($(MESHSUPER), 1)
GLOBAL_DEFINES += CONFIG_YOS_MESH_SUPER
endif

$(NAME)_CFLAGS += -Wall -Werror
GLOBAL_DEFINES += CONFIG_YOS_MESH
