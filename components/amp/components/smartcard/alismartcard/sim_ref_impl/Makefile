QL_EXP_TARGETS = libsim.a

QL_C_FILE += sim.c
QL_C_FILE += common_sim.c
QL_C_FILE += std_ali_card.c

#--------------------------------------------------------
# TOOLS BASIC CONFIG
# Note: No Need to change them
#--------------------------------------------------------
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
CFLAGS =
CPPFLAGS += -I./                     \
            -I./inc                  \
            -I../exports             \
            -I../imports             \
            -I../modules             \
            -I../libalilog           \
            -Wall -g

all: $(QL_EXP_TARGETS)
.PHPNY: all

$(QL_EXP_TARGETS):$(QL_C_FILE)
	$(COMPILE.c) $(CPPFLAGS) $(QL_C_FILE) -fPIC
	$(AR) rcv $(QL_EXP_TARGETS) *.o

clean:
	rm -rf $(QL_EXP_TARGETS) *.o

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
