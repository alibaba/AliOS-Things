#./demo_prj_v1.2/democfg.mak
 
#---------------------------------------------------------
# tool chain define
#---------------------------------------------------------

AMP_VERSION  = "2.0.0"
GIT_COMMIT   = "$(shell git show -s --pretty=format:%h)"
COMPILE_TIME = "$(shell date +"%Y-%m-%d %H:%M:%S")"

CC 	= gcc
CXX = g++
AR 	= ar
LD 	= ld
OBJCOPY = objcopy
CCFLAGS = -DAMP_VERSION=\"$(AMP_VERSION)\" -DCOMPILE_TIME=\"$(COMPILE_TIME)\" -DGIT_COMMIT=\"$(GIT_COMMIT)\"

#---------------------------------------------------------
# shell command 
#---------------------------------------------------------
ECHO = @echo
MKDIR = mkdir -p
MV = @mv -r
RM = @rm -rf
MAKE = @make -j8
 
#---------------------------------------------------------
# complier flags
# v for make debug 
#---------------------------------------------------------

ARFLAGS =  rcs
 
#---------------------------------------------------------
# link libs 
#---------------------------------------------------------
LIBS = -ln -lrt -lpthread -lutil