# duktape_engine/jse
${NAME}_SOURCES +=  \
										duktape_engine/duktape/duktape.c \
										duktape_engine/be_module_node.c \
										duktape_engine/bone_engine_refs.c \
										duktape_engine/bone_engine.c

# C includes
$(NAME)_INCLUDES += duktape_engine

# including addon
include $(JSE_ROOT)/engine/duktape_engine/addons/addons.mk