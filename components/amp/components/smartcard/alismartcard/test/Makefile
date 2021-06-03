#--------------------------------------------------------------
#               Quectel OpenLinux
#--------------------------------------------------------------
QL_EXP_TARGETS = mn_manual_switch_test
QL_C_FILE      = mn_manual_switch_test.c
LDFLAGS +=  -L ../modules -lali_smartcard

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
CFLAGS =
CPPFLAGS += -I./                     \
            -I../exports             \
            -I../libalilog           \
            -Wall -g

LDFLAGS +=  -pthread

all: $(QL_EXP_TARGETS)
.PHPNY: all

$(QL_EXP_TARGETS):$(QL_C_FILE) $(QL_EXP_DEP_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(QL_EXP_TARGETS) *.o

install:
	@if test -f $(QL_EXP_TARGETS) ; then \
		cp $(QL_EXP_TARGETS) ../output; \
	fi;


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
