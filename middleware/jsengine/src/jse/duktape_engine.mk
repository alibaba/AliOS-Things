# duktape_engine/jse 内核
${NAME}_SOURCES +=  \
										duktape_engine/duktape/duktape.c \
										duktape_engine/be_module_node.c \
										duktape_engine/bone_engine_refs.c \
										duktape_engine/bone_engine.c

# C includes
$(NAME)_INCLUDES += duktape_engine

# 引用addon的编译配置文件
include $(JSE_ROOT)/jse/duktape_engine/addons/addons.mk