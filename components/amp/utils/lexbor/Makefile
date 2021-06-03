TARGET = liblexbor.a
MODULE = lexbor

MOD_SOURCES += \
		core/array.c \
		core/array_obj.c \
		core/avl.c \
		core/bst.c \
		core/bst_map.c \
		core/conv.c \
		core/diyfp.c \
		core/dobject.c \
		core/dtoa.c \
		core/hash.c \
		core/in.c \
		core/mem.c \
		core/mraw.c \
		core/plog.c \
		core/shs.c \
		core/str.c \
		core/strtod.c \
		core/utils.c

MOD_SOURCES += \
		css/syntax/consume.c \
		css/syntax/state.c \
		css/syntax/token.c \
		css/syntax/tokenizer.c \
		css/syntax/tokenizer/error.c

MOD_SOURCES += \
		ports/posix/lexbor/core/fs.c \
		ports/posix/lexbor/core/memory.c

MOD_INCLUDES := \
		./ \
		./css \
		./css/syntax \
		./css/syntax/tokenizer \
		./core \
		./ports/posix/lexbor/core \
                ../../adapter/include

include $(TOOLS_DIR)/rules.mk
