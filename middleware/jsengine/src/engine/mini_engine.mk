# mini_engine/jse
$(NAME)_SOURCES +=  \
										src/engine/mini_engine/core/be_jse_executor.c \
										src/engine/mini_engine/core/be_jse_lex.c      \
										src/engine/mini_engine/core/be_jse_module.c   \
										src/engine/mini_engine/core/be_jse_symbol.c   \
										src/engine/mini_engine/core/be_jse_utils.c    \
										src/engine/mini_engine/core/bone_engine.c

# C includes
$(NAME)_INCLUDES += src/engine/mini_engine/include

# including components
include $(JSE_ROOT)/components/components.mk

# including addon
include $(JSE_ROOT)/engine/mini_engine/addons/addons.mk