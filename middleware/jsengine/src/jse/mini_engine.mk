# mini_engine/jse 内核
$(NAME)_SOURCES +=  \
										src/jse/mini_engine/core/be_jse_executor.c \
										src/jse/mini_engine/core/be_jse_lex.c      \
										src/jse/mini_engine/core/be_jse_module.c   \
										src/jse/mini_engine/core/be_jse_symbol.c   \
										src/jse/mini_engine/core/be_jse_utils.c    \
										src/jse/mini_engine/core/bone_engine.c

# C includes
$(NAME)_INCLUDES += src/jse/mini_engine/include

# 引用components的编译配置文件
include $(JSE_ROOT)/components/components.mk

# 引用addon的编译配置文件
include $(JSE_ROOT)/jse/mini_engine/addons/addons.mk