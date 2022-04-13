#-------------------------------------------------------------------------------
# Configure environment path
#-------------------------------------------------------------------------------
TOP_DIR:=$(TOP_DIR)
ROOT_DIR:=$(TOP_DIR)/../..
BUILD_DIR:=build
IMAGE_BUILD_DIR:=build
OBJ_DIR:=$(subst $(strip \),/,$(BUILD_DIR)/obj)
CUR_MAKEFILE := $(abspath $(firstword $(MAKEFILE_LIST)))
PACKAGE_PATH:=$(firstword $(dir $(CUR_MAKEFILE)))
PACKAGE_PARENT_PATH_TMP:=$(dir $(realpath $(PACKAGE_PATH)))
PACKAGE_PARENT_PATH:=$(patsubst $(subst $(strip \),/,$(PACKAGE_PATH))%,%,$(PACKAGE_PARENT_PATH_TMP))
PACKAGE_NAME_TMP1=$(lastword $(subst $(strip /), ,$(PACKAGE_PATH)))
PACKAGE_NAME_TMP2=$(lastword $(subst $(strip /), ,$(PACKAGE_PARENT_PATH)))

PACKAGE_NAME:=$(PACKAGE_NAME_TMP1)
CREATE_BIN_TOOLS_DIR:=$(TOP_DIR)/tools
IMAGE_NAME:=$(IMAGE_GENERAL_INFO)$(IMAGE_BUILD_MARK).bin
COMLIED_LIB_LIST_FILE:=$(TOP_DIR)/$(IMAGE_BUILD_DIR)/complied_lib.txt
COMLIED_PP_LIST_FILE:=$(TOP_DIR)/$(IMAGE_BUILD_DIR)/complied_pp.txt
COMMON_PP_LIST_FILE:=$(TOP_DIR)/adapter/platform/build_config/common/common_pp_list.txt
TXT_MDB_FILE_NAME:=KERNEL_MDB.txt
DAIGDB_C_FILE_NAME:=diagDB.c

#-------------------------------------------------------------------------------
# Configure source code files to compile in the source code directories
#-------------------------------------------------------------------------------
COMMPILE_DIRS:=$(subst $(strip \),/,$(COMMPILE_DIRS))
TARGET_COMMPILE_DIRS:=$(patsubst %,%/target_compile_dir,$(COMMPILE_DIRS))
TARGET_CLEAN_DIRS:=$(patsubst %,%/target_clean_dir,$(COMMPILE_DIRS))

SRC_FILES:=$(subst $(strip \),/,$(SRC_FILES))
SRC_DIRS:=$(dir $(SRC_FILES))

OBJS_TMP:= $(SRC_FILES:.c=.o)
OBJS_TMP:=$(OBJS_TMP:.cpp=.o)
OBJS_TMP:=$(OBJS_TMP:.cc=.o)
OBJS_TMP:=$(OBJS_TMP:.cxx=.o)
OBJS_TMP:=$(OBJS_TMP:.S=.o)
OBJS_TMP:=$(OBJS_TMP:.s=.o)
OBJS_TMP:=$(patsubst %,$(subst $(strip \),/,$(OBJ_DIR))/%,$(OBJS_TMP))
OBJS=$(OBJS_TMP)

DEF_FILES_TMP:= $(SRC_FILES:.c=.d)
DEF_FILES_TMP:=$(DEF_FILES_TMP:.cpp=.d)
DEF_FILES_TMP:=$(patsubst %,$(subst $(strip \),/,$(OBJ_DIR))/%,$(DEF_FILES_TMP))
DEF_FILES:=$(DEF_FILES_TMP)

#-------------------------------------------------------------------------------
# Configure compiling utilities
#-------------------------------------------------------------------------------
CC:=armcc.exe
ASM:=armasm.exe
CPP:=armcc.exe
LD:=armlink.exe
AR:=armar.exe
FROMELF:=fromelf.exe
MAKE:=gnumake.exe
PPC:=$(TOP_DIR)\tools\prepass\Hermon\Release\PPC.exe
PPL:=$(TOP_DIR)\tools\prepass\Hermon\Release\PPL.exe
PPU:=$(TOP_DIR)\tools\prepass\Hermon\Release\PPU.exe
PPMAN:=$(TOP_DIR)\tools\prepass\Hermon\Release\PPManCmd.exe

#-------------------------------------------------------------------------------
# Configure compiling options
#-------------------------------------------------------------------------------
SFLAGS:=-g --cpu Cortex-R4 --no_unaligned_access --keep --fpu None --apcs /inter --diag_suppress 1658
CFLAGS:=-c --cpu Cortex-R4 --no_unaligned_access -g -O2 --apcs /inter --diag_suppress 2084,1,2,177,550,6319 --diag_error 47 --gnu --thumb --loose_implicit_cast
CPPFLAGS:=-c --cpu Cortex-R4 --no_unaligned_access -g -O2 --apcs /inter --diag_suppress 2084,1,2,177,550,6319 --diag_error 47 --gnu --thumb --loose_implicit_cast
ARFLAG:=-v --create -r
LDFLAGS:=--via config/common/common_lib_list.txt \
         --via $(IMAGE_BUILD_DIR)/complied_lib.txt \
	 --elf --scatter config/ram/Crane_DS_16M_Ram_16M_Flash_XIP_CIPSRAM_Common.sct \
	 --predefine="-DENABLE_CAT1_LG" \
	 --predefine="-DL1_WIFI_LOCATION" \
	 --predefine="-DCRANE_MODULE_SUPPORT" \
	 --map --symbols --info sizes,totals  \
	 --keep init.o(Header) \
	 --keep init.o(Vectors) \
	 --diag_suppress 6312,6314,6319,6329



#-------------------------------------------------------------------------------
# Definition for compiling procedure
#-------------------------------------------------------------------------------
.PHONY: all clean CreateDir compile_lib_list


ifeq ($(strip $(PACKAGE_PATH)),$(subst $(strip \),/,$(strip $(TOP_DIR))))
#-------------------------------------------------------------------------------
# Top makefile
#------------------------------------------------------------------------------- 

PREPARE:=CreateDir
IAMGE:=kernel

all:$(PREPARE) $(IAMGE) 

CreateDir:
	@if exist $(COMLIED_LIB_LIST_FILE) (@echo clean $(COMLIED_LIB_LIST_FILE) & del /s/q $(subst $(strip /),\,$(COMLIED_LIB_LIST_FILE)))
	@if exist $(COMLIED_PP_LIST_FILE) (@echo clean $(COMLIED_PP_LIST_FILE) & del /s/q $(subst $(strip /),\,$(COMLIED_PP_LIST_FILE)))
	@if not exist $(IMAGE_BUILD_DIR) ( \
		md $(subst $(strip /),\,$(IMAGE_BUILD_DIR)) && \
		xcopy /E/Y/I $(subst $(strip /),\,$(CREATE_BIN_TOOLS_DIR)) $(subst $(strip /),\,$(IMAGE_BUILD_DIR)\) \
	) else ( \
		(@if exist $(IMAGE_BUILD_DIR)/$(IAMGE).bin (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IAMGE).bin))) & \
		(@if exist $(IMAGE_BUILD_DIR)/$(IAMGE).axf (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IAMGE).axf))) & \
		(@if exist $(IMAGE_BUILD_DIR)/$(IAMGE).map (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IAMGE).map))) & \
		(@if exist $(IMAGE_BUILD_DIR)/TIM_$(IAMGE).bin (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/TIM_$(IAMGE).bin))) & \
		(@if exist $(IMAGE_BUILD_DIR)/release_$(IAMGE).bin (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/release_$(IAMGE).bin))) & \
		(@if exist $(IMAGE_BUILD_DIR)/$(IAMGE).init (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IAMGE).init))) & \
		(@if exist $(IMAGE_BUILD_DIR)/$(IAMGE)_no_tim.bin (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IAMGE)_no_tim.bin))) & \
		(@if exist $(IMAGE_BUILD_DIR)/$(IMAGE_NAME) (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IMAGE_NAME)))) \
	)

clean: $(TARGET_CLEAN_DIRS)
	@if exist $(IMAGE_BUILD_DIR) (@echo clean $(IMAGE_BUILD_DIR) & rd /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)))

%/target_clean_dir:%
	gnumake -C $< -f Makefile clean TOP_DIR=$(TOP_DIR)

#-------------------------------------------------------------------------------
# Top makefile end
#-------------------------------------------------------------------------------  

else

#-------------------------------------------------------------------------------
# Package makefile
#------------------------------------------------------------------------------- 

PREPARE:=CreateDir
TARGET:=$(BUILD_DIR)/$(subst $(strip /),-,$(PACKAGE_NAME)).lib

all:$(PREPARE) compile_lib_list

CreateDir:
	@if not exist $(BUILD_DIR) (md $(subst $(strip /),\,$(BUILD_DIR)))
	@if not exist $(OBJ_DIR) (md $(subst $(strip /),\,$(OBJ_DIR)))
	@for /d %%y in ($(subst $(strip /),\,$(SRC_DIRS))) do ( \
		@if not exist $(OBJ_DIR)\%%y ( \
			(@echo creating direcotry $(OBJ_DIR)\%%y) & (md $(subst $(strip /),\,$(OBJ_DIR)\%%y)) \
		) \
	)

clean:
	@if exist $(BUILD_DIR) (@echo clean $(BUILD_DIR) & rd /s/q $(BUILD_DIR))
	@if exist $(TARGET) (@echo clean $(TARGET) & rd /s/q $(TARGET))

#-------------------------------------------------------------------------------
# Package makefile end
#------------------------------------------------------------------------------- 
endif


$(IAMGE):$(TARGET_COMMPILE_DIRS) $(TOP_DIR)/$(IMAGE_BUILD_DIR)/$(TXT_MDB_FILE_NAME)
	@if exist $(IMAGE_BUILD_DIR)/$(IAMGE).map (del /s/q $(subst $(strip /),\,$(IAMGE).map))
	$(LD) $(LDFLAGS) -o $(IMAGE_BUILD_DIR)/$(IAMGE).axf --list $(IMAGE_BUILD_DIR)/$(IAMGE).map
	@if exist $(IMAGE_BUILD_DIR)/TIM_$(IAMGE).bin (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/TIM_$(IAMGE).bin))
	@if exist $(IMAGE_BUILD_DIR)/release_$(IAMGE).bin (del /s/q $(subst $(strip /),\$(IMAGE_BUILD_DIR)/,release_$(IAMGE).bin))
	@if exist $(IMAGE_BUILD_DIR)/$(IAMGE).init (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IAMGE).init))
	@if exist $(IMAGE_BUILD_DIR)/$(IAMGE)_no_tim.bin (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IAMGE)_no_tim.bin))
	@if exist $(IMAGE_BUILD_DIR)/$(IMAGE_NAME) (del /s/q $(subst $(strip /),\,$(IMAGE_BUILD_DIR)/$(IMAGE_NAME)))
	cd $(IMAGE_BUILD_DIR) & ( \
	$(FROMELF) --bin --output $(IAMGE).bin $(IAMGE).axf && \
	rspTIM.exe -image $(IAMGE).bin -param_db COMM && \
	ren $(IAMGE).bin $(IAMGE)_no_tim.bin && \
	ren TIM_$(IAMGE).bin $(IAMGE).bin && \
	tavor_ddr_bin2init.exe $(IAMGE).bin $(IAMGE).init 32 && \
	platform -p DKB_SS -t EVB_3 -i NONE  -b $(IAMGE)_no_tim.bin -o $(IMAGE_NAME) && \
	perl external_rw_region_compress.pl $(IMAGE_NAME) && \
	perl external_loadtable_update.pl $(IMAGE_NAME) $(IMAGE_GENERAL_INFO) && \
	copy $(IMAGE_NAME) release_$(IAMGE).bin )

$(TOP_DIR)/$(IMAGE_BUILD_DIR)/$(TXT_MDB_FILE_NAME):$(COMLIED_PP_LIST_FILE)
	cd $(TOP_DIR)/$(IMAGE_BUILD_DIR) &\
	SETLOCAL ENABLEDELAYEDEXPANSION & \
	(for /f "delims=" %%i in ('type $(subst $(strip /),\, $(COMMON_PP_LIST_FILE))') do (set common_pp_ret=!common_pp_ret!..\%%i )) & \
	(for /f "delims=" %%i in ('type $(subst $(strip /),\, $(COMLIED_PP_LIST_FILE))') do (set compiled_pp_ret=!compiled_pp_ret!%%i )) & \
	(if exist $(TXT_MDB_FILE_NAME) (del /s/q $(TXT_MDB_FILE_NAME))) &\
	(del /s/q diagDB*) &\
	($(PPL) $(TXT_MDB_FILE_NAME) $(DAIGDB_C_FILE_NAME) !common_pp_ret! !compiled_pp_ret! -f $(TOP_DIR)\config\mdb\3G_PS_Filter_file.txt -f $(TOP_DIR)\config\mdb\APLP_Filter_file.txt -f $(TOP_DIR)\config\mdb\diag_list.txt 1>nul) &\
	ENDLOCAL &\
	($(PPMAN) -S..\config\mdb\hsiupdlibdev.i -N$(TXT_MDB_FILE_NAME))&\
	($(PPU) $(TXT_MDB_FILE_NAME) $(DAIGDB_C_FILE_NAME) ..\config\mdb\diagDB.txt) &\
	(if exist $(patsubst %.c,%.o,$(DAIGDB_C_FILE_NAME)) (del /s/q $(patsubst %.c,%.o,$(DAIGDB_C_FILE_NAME)))) &\
	($(CC) $(CFLAGS) -DENV_XSCALE -DPLAT_USE_THREADX -I$(TOP_DIR)/common/include -o $(patsubst %.c,%.o,$(DAIGDB_C_FILE_NAME)) $(DAIGDB_C_FILE_NAME)) &\
	(if exist $(patsubst %.c,%.o,$(DAIGDB_C_FILE_NAME)) (copy /y $(patsubst %.c,%.o,$(DAIGDB_C_FILE_NAME)) $(subst $(strip /),\,$(TOP_DIR)\common\lib\))) &\
	cd ..
	

%/target_compile_dir:%
	@echo ---------------compile [$<]-------------------
	gnumake -C $< -f Makefile  TOP_DIR=$(TOP_DIR)

compile_lib_list:$(TARGET)
	@echo $(PACKAGE_PATH)/$(TARGET) >> $(COMLIED_LIB_LIST_FILE)
	@echo $(patsubst %.lib,%.pp,$(subst $(strip /),\,$(PACKAGE_PATH)/$(TARGET)))  >> $(COMLIED_PP_LIST_FILE)
ifneq ($(U_LIBS),)
	@for /d %%y in ($(subst $(strip /),\,$(U_LIBS))) do ( \
		(@echo %%y >> $(COMLIED_LIB_LIST_FILE)) \
	)
endif

$(TARGET):$(OBJS)
	@echo ---------------create lib [$@]-------------------
	$(AR) $(ARFLAG) $@ $^
	type $(patsubst %.o,%.pp,$(subst $(strip /),\,$^)) > $(patsubst %.lib,%.pp,$(subst $(strip /),\,$@))


$(OBJ_DIR)/%.o: %.S
	$(ASM)  $(SFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: %.s
	$(ASM)  $(SFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: %.c
	@ ($(CC) -E -C -D_PPC_ $(subst $(strip /),\,$<) $(CFLAGS) $(DFLAGS) $(U_FLAGS) $(INC_DIRS) -o $(patsubst %.o,%.ppp,$(subst $(strip /),\,$@)) && $(PPC) $(patsubst %.o,%.ppp,$(subst $(strip /),\,$@)) $(patsubst %.o,%.pp,$(subst $(strip /),\,$@)) ) 
	@ ($(CC) $(CFLAGS) $(DFLAGS) $(U_FLAGS) $(INC_DIRS) -o $@ $< --depend=$(patsubst %.c,$(subst $(strip \),/,$(OBJ_DIR))/%.d,$<))
	@ $(LD) --edit=${TOP_DIR}\amp\adapter\platform\ThreadX-ec100y\build_config\common\rename.txt --partial $@ -o $@

$(OBJ_DIR)/%.o: %.cpp
	($(CPP) -E -C -D_PPC_ $(subst $(strip /),\,$<) $(CPPFLAGS) $(DFLAGS) $(U_CPPFLAGS) $(INC_DIRS) -o $(patsubst %.o,%.ppp,$(subst $(strip /),\,$@)) && $(PPC) $(patsubst %.o,%.ppp,$(subst $(strip /),\,$@)) $(patsubst %.o,%.pp,$(subst $(strip /),\,$@)) )
	($(CPP) $(CPPFLAGS) $(DFLAGS) $(U_CPPFLAGS) $(INC_DIRS) -o $@ $< --depend=$(patsubst %.cpp,$(subst $(strip \),/,$(OBJ_DIR))/%.d,$<))

$(OBJ_DIR)/%.o: %.cc
	($(CPP) -E -C -D_PPC_ $(subst $(strip /),\,$<) $(CPPFLAGS) $(DFLAGS) $(U_CPPFLAGS) $(INC_DIRS) -o $(patsubst %.o,%.ppp,$(subst $(strip /),\,$@)) && $(PPC) $(patsubst %.o,%.ppp,$(subst $(strip /),\,$@)) $(patsubst %.o,%.pp,$(subst $(strip /),\,$@)) )
	$(CPP) $(CPPFLAGS) $(DFLAGS) $(U_CPPFLAGS) $(INC_DIRS) -o $@ $< --depend=$(patsubst %.cc,$(subst $(strip \),/,$(OBJ_DIR))/%.d,$<)

$(OBJ_DIR)/%.o: %.cxx
	($(CPP) -E -C -D_PPC_ $(subst $(strip /),\,$<) $(CPPFLAGS) $(DFLAGS) $(U_CPPFLAGS) $(INC_DIRS) -o $(patsubst %.o,%.ppp,$(subst $(strip /),\,$@)) && $(PPC) $(patsubst %.o,%.ppp,$(subst $(strip /),\,$@)) $(patsubst %.o,%.pp,$(subst $(strip /),\,$@)) )
	$(CPP) $(CPPFLAGS) $(DFLAGS) $(U_CPPFLAGS) $(INC_DIRS) -o $@ $< --depend=$(patsubst %.cxx,$(subst $(strip \),/,$(OBJ_DIR))/%.d,$<)

-include $(DEF_FILES)