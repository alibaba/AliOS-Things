# entry
${NAME}_SOURCES += src/main/jse_main.c
${NAME}_SOURCES += src/main/jse_task.c

# utils
$(NAME)_SOURCES += src/utils/be_utils.c

# port
${NAME}_SOURCES += \
										src/port/aos/jse_osal.c \
										src/port/aos/jse_system.c

# C includes
$(NAME)_INCLUDES += \
										src/include \
										src/include/common \
										src/port/

ifeq ($(JSE_ENGINE_MINI),y)
include ${JSE_ROOT}/engine/mini_engine.mk
else ifeq ($(JSE_ENGINE_DUKTAPE),y)
include ${JSE_ROOT}/engine/duktape_engine.mk
endif

# including components
include $(JSE_ROOT)/components/components.mk

# including services
include $(JSE_ROOT)/services/services.mk

