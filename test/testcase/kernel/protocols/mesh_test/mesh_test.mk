NAME := mesh_test

$(NAME)_COMPONENTS  += protocols.mesh dda 

$(NAME)_INCLUDES    += include
$(NAME)_INCLUDES    += ../../../../../tools/dda

include test/testcase/kernel/protocols/mesh_test/filelists.mk
$(NAME)_SOURCES     += $(MESHYTSFILE)

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

