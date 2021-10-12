CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

.PHONY:startup
startup: all

all:
	@echo "Build Solution by $(BOARD) "
	$(CPRE) scons $(VERB) --board=$(BOARD) $(MULTITHREADS) $(MAKEFLAGS)
	@echo AOS SDK Done

sdk:
	$(CPRE) aos sdk

.PHONY:clean
clean:
	$(CPRE) scons -c --board=$(BOARD)
ifeq ($(OS), Windows_NT)
	$(CPRE) if exist aos_sdk rmdir /s /q aos_sdk
	$(CPRE) if exist binary rmdir /s /q binary
	$(CPRE) if exist out rmdir /s /q out
	$(CPRE) if exist aos.elf del /f /q aos.elf
	$(CPRE) if exist aos.map del /f /q aos.map
else
	$(CPRE) rm -rf aos_sdk binary out aos.elf aos.map
endif
