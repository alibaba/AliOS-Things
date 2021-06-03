#--------------------------------------------------------------
#               Quectel OpenLinux
#--------------------------------------------------------------
QL_SDK_PATH   ?= $(shell pwd)/../../..


ifeq (y, $(strip $(BUILD_RTOS)))
QL_EXP_TARGETS = libali_smartcard.a

else

ifeq (y, $(strip $(BUILD_WITH_NO_SERIAL_OPS)))
QL_EXP_TARGETS = libali_smartcard_no_serial_ops.so
else
QL_EXP_TARGETS = libali_smartcard.so
endif

endif

ifeq (y, $(strip $(BUILD_RTOS)))
QL_EXP_DEP_LIB = ../rtos_hal_impl/*.o
QL_EXP_DEP_LIB += ../libalilog/*.o
QL_EXP_DEP_LIB += ../sim_ref_impl/*.o
QL_C_FILE      = mn_export.c mn_common.c mn_dummy_config.c
else
QL_EXP_DEP_LIB = ../linux_hal_impl/lib_hal_impl.a
QL_EXP_DEP_LIB += ../utils/libparson.a
QL_EXP_DEP_LIB += ../libalilog/libalilog.a
QL_EXP_DEP_LIB += ../sim_ref_impl/libsim.a
QL_C_FILE      = mn_export.c mn_common.c mn_config.c
endif

#--------------------------------------------------------
# TOOLS BASIC CONFIG
# Note: No Need to change them
#--------------------------------------------------------
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
CFLAGS =
CPPFLAGS += -I./                     \
            -I./inc                  \
            -I../exports             \
            -I../imports             \
            -I../sim_ref_impl        \
            -I../rtos_hal_impl       \
	    -I../libalilog           \
	    -I../utils               \
	    -Wall -g

ifeq (y,$(strip $(ANDROID_TOOLCHAIN_ENABLED)))
LDFLAGS += -L./ -L$(QL_SDK_PATH)/lib -fopenmp -pthread
CPPFLAGS += -DHOST_ANDROID_ENABLED
else
LDFLAGS += -L./ -L$(QL_SDK_PATH)/lib -pthread
endif

all: $(QL_EXP_TARGETS)
.PHPNY: all

ifeq (y, $(strip $(BUILD_RTOS)))
$(QL_EXP_TARGETS):$(QL_C_FILE) $(QL_EXP_DEP_LIB)
	$(COMPILE.c) $(CPPFLAGS) $(QL_C_FILE) -fPIC
	$(AR) rcv $(QL_EXP_TARGETS) *.o $(QL_EXP_DEP_LIB)
else
$(QL_EXP_TARGETS):$(QL_C_FILE) $(QL_EXP_DEP_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(QL_EXP_DEP_LIB) -shared -fPIC
endif

clean:
	rm -rf $(QL_EXP_TARGETS) *.o *.a *.so

install:
	$(SIZE) -t $(QL_EXP_TARGETS)
ifeq (y, $(strip $(BUILD_RTOS)))
	$(STRIP) -S $(QL_EXP_TARGETS)
else
	$(STRIP) -s $(QL_EXP_TARGETS)
endif
	@cp $(QL_EXP_TARGETS) ../output

.PHONY:checkmake
checkmake:
	@echo -e "CURDIR =		\n	${CURDIR}"
	@echo -e "\nMAKE_VERSION =	\n	${MAKE_VERSION}"
	@echo -e "\nMAKEFILE_LIST =	\n	${MAKEFILE_LIST}"
	@echo -e "\nCOMPILE.c =		\n	${COMPILE.c}"
	@echo -e "\nCOMPILE.cc =	\n	${COMPILE.cc}"
	@echo -e "\nCOMPILE.cpp =	\n	${COMPILE.cpp}"
	@echo -e "\nLINK.cc =		\n	${LINK.cc}"
	@echo -e "\nLINK.o =		\n	${LINK.o}"
	@echo -e "\nCPPFLAGS =		\n	${CPPFLAGS}"
	@echo -e "\nCFLAGS =		\n	${CFLAGS}"
	@echo -e "\nCXXFLAGS =		\n	${CXXFLAGS}"
	@echo -e "\nLDFLAGS =		\n	${LDFLAGS}"
	@echo -e "\nLDLIBS =		\n	${LDLIBS}"
