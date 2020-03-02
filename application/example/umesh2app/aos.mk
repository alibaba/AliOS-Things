NAME := umesh2app

no_with_lwip := 0
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uMesh2 sample application

$(NAME)_SOURCES     :=aos_start.c mesh_example.c

$(NAME)_COMPONENTS  += umesh2 cli netmgr ulog lwip
GLOBAL_DEFINES      += TAPIF_DEFAULT_OFF DEBUG 

$(NAME)_INCLUDES += ./
