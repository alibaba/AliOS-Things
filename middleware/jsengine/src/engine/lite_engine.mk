# lite_engine
$(NAME)_SOURCES +=  \
										src/engine/lite_engine/core/be_executor.c \
										src/engine/lite_engine/core/be_lex.c      \
										src/engine/lite_engine/core/be_module.c   \
										src/engine/lite_engine/core/be_symbol.c   \
										src/engine/lite_engine/core/be_utils.c    \
										src/engine/lite_engine/core/be.c

# C includes
$(NAME)_INCLUDES += src/engine/lite_engine/include

# including components
include $(JSE_ROOT)/components/components.mk

# including addon
include $(JSE_ROOT)/engine/lite_engine/addons/addons.mk