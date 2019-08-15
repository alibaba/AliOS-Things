# duktape_engine
${NAME}_SOURCES +=  \
										src/engine/duktape_engine/duktape/duktape.c \
										src/engine/duktape_engine/be_module_node.c \
										src/engine/duktape_engine/be_refs.c \
										src/engine/duktape_engine/be.c

# C includes
$(NAME)_INCLUDES += \
										src/engine/duktape_engine \
										src/engine/duktape_engine/include

# including addon
include $(JSE_ROOT)/engine/duktape_engine/addons/addons.mk