##
## Auto Generated makefile by CDK
## Do not modify this file, and any manual changes will be erased!!!   
##
## BuildSet
ProjectName            :=yocprj
ConfigurationName      :=BuildSet
WorkspacePath          :=C:/Users/hgewo/AppData/Roaming/cdk/default_workspace19/
ProjectPath            :=E:/cskygit/newyoc/tools/cdk/hobbit1_2_evb_images_programmer/
IntermediateDirectory  :=Obj
OutDir                 :=$(IntermediateDirectory)
User                   :=hgewo
Date                   :=09/01/2018
CDKPath                :=D:/Program Files (x86)/C-Sky/C-Sky Development Kit
LinkerName             :=csky-abiv2-elf-gcc
LinkerNameoption       :=
SIZE                   :=csky-abiv2-elf-size
READELF                :=csky-abiv2-elf-readelf
CHECKSUM               :=crc32
SharedObjectLinkerName :=
ObjectSuffix           :=.o
DependSuffix           :=.d
PreprocessSuffix       :=.i
DisassemSuffix         :=.asm
IHexSuffix             :=.ihex
ExeSuffix              :=.elf
LibSuffix              :=.a
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
ElfInfoSwitch          :=-hlS
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
UnPreprocessorSwitch   :=-U
SourceSwitch           :=-c 
ObjdumpSwitch          :=-S
ObjcopySwitch          :=-O ihex
OutputFile             :=$(ProjectName)
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="yocprj.txt"
MakeDirCommand         :=mkdir
LinkOptions            :=-mcpu=ck802  -nostartfiles -Wl,--gc-sections  -T""
IncludeCPath           :=
IncludeAPath           :=
Libs                   :=
ArLibs                 := 
LibPath                :=

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       :=csky-abiv2-elf-ar rcu
CXX      :=csky-abiv2-elf-g++
CC       :=csky-abiv2-elf-gcc
AS       :=csky-abiv2-elf-gcc
OBJDUMP  :=csky-abiv2-elf-objdump
OBJCOPY  :=csky-abiv2-elf-objcopy
CXXFLAGS :=-mcpu=ck802   $(PreprocessorSwitch)CONFIG_CSKY_MMU=0   -Os  -g  -Wall -ffunction-sections -fdata-sections 
CFLAGS   :=-mcpu=ck802   $(PreprocessorSwitch)CONFIG_CSKY_MMU=0   -Os  -g  -Wall -ffunction-sections -fdata-sections 
ASFLAGS  :=-mcpu=ck802   $(PreprocessorSwitch)CONFIG_CKCPU_MMU=0   -Wa,--gdwarf2    




Objects=

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PostBuild MakeIntermediateDirs
all: $(IntermediateDirectory)/$(OutputFile)

$(IntermediateDirectory)/$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@echo "" > $(IntermediateDirectory)/.d
	@echo linking...
	@$(LinkerName) $(OutputSwitch)"$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)" $(LinkerNameoption) @$(ObjectsFileList)  $(LibPath) $(Libs) $(LinkOptions)
	@echo size of target:
	@$(SIZE) "$(ProjectPath)$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)" 
	@echo -n "checksum value of target:  "
	@$(CHECKSUM) "$(ProjectPath)/$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)" 
	
MakeIntermediateDirs:
	@test -d Obj || $(MakeDirCommand) Obj

	@test -d Lst || $(MakeDirCommand) Lst

$(IntermediateDirectory)/.d:
	@test -d Obj || $(MakeDirCommand) Obj
	@test -d Lst || $(MakeDirCommand) Lst


PreBuild:


##
## Objects
##

$(IntermediateDirectory)/__rt_entry$(ObjectSuffix): $(IntermediateDirectory)/__rt_entry$(DependSuffix)
	@$(AS) $(SourceSwitch) "$(ProjectPath)/$(IntermediateDirectory)/__rt_entry.S" $(ASFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/__rt_entry$(ObjectSuffix) $(IncludeAPath)
	@rm -f $(IntermediateDirectory)/__rt_entry.S
$(IntermediateDirectory)/__rt_entry$(DependSuffix):
	@$(CC) $(CFLAGS) $(IncludeAPath) -MG -MP -MT$(IntermediateDirectory)/__rt_entry$(ObjectSuffix) -MF$(IntermediateDirectory)/__rt_entry$(DependSuffix) -MM "$(ProjectPath)/$(IntermediateDirectory)/__rt_entry.S"

-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	@echo Cleaning target...
	@rm -rf $(IntermediateDirectory)/ $(ObjectsFileList) yocprj.mk Lst/

clean_internal:
	@rm -rf $(IntermediateDirectory)/*.o $(IntermediateDirectory)/.d $(IntermediateDirectory)/*.d $(IntermediateDirectory)/*.a $(IntermediateDirectory)/*.elf $(IntermediateDirectory)/*.ihex Lst/

