ifneq ($(GEN_SDK_LIB),1)
ifeq ($(KERNEL),RTX)
rtos_lib-y := rtx/TARGET_CORTEX_M/
else ifeq ($(KERNEL),RTX5)
rtos_lib-y := rtx5/
else ifeq ($(KERNEL),FREERTOS)
rtos_lib-y := freertos/
else ifeq ($(KERNEL),RHINO)
ifeq ($(KERNEL_INTO_LIB),1)
rtos_lib-y := rhino/
else
rtos_lib-y :=
endif
ifeq ($(KERNEL),RHINO)
ifeq ($(CMSIS_AOS),1)
rtos_lib-y += rhino/cmsis/cmsis_aos.o
rtos_lib-y += rhino/cmsis/aos_stub.o
else
rtos_lib-y += rhino/cmsis/cmsis_os.o
endif
rtos_lib-y += rhino/cmsis/cmsis_init.o
rtos_lib-y += rhino/cmsis/k_membox.o
endif
endif

ifeq ($(MBED),1)
rtos_lib-y += rtos/
endif

obj-y := rtos_lib.o
ifeq ($(KERNEL), RHINO)
ifneq ($(KERNEL_INTO_LIB),1)
obj-y += rhino/
endif
endif #rhino


else

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := librtos
ifneq ($(SDK),1)

ifeq ($(KERNEL),RTX)
rtos_lib-y := rtx/TARGET_CORTEX_M/
else ifeq ($(KERNEL),RTX5)
rtos_lib-y := rtx5/
else ifeq ($(KERNEL),FREERTOS)
rtos_lib-y := freertos/
else ifeq ($(KERNEL),RHINO)
ifeq ($(KERNEL_INTO_LIB),1)
rtos_lib-y := rhino/
else
rtos_lib-y :=
endif
ifeq ($(KERNEL),RHINO)
ifeq ($(CMSIS_AOS),1)
rtos_lib-y += rhino/cmsis/cmsis_aos.o
rtos_lib-y += rhino/cmsis/aos_stub.o
else
rtos_lib-y += rhino/cmsis/cmsis_os.o
endif
rtos_lib-y += rhino/cmsis/cmsis_init.o
rtos_lib-y += rhino/cmsis/k_membox.o
endif
endif

ifeq ($(MBED),1)
rtos_lib-y += rtos/
endif


$(LIB_NAME)-y := $(rtos_lib-y)
endif

obj-y := $(LIB_NAME).a
ifeq ($(KERNEL), RHINO)
ifneq ($(KERNEL_INTO_LIB),1)
obj-y += rhino/
endif
endif #rhino
endif

ccflags-y += -Irtos/rhino/include
ccflags-y += -Irtos/rhino/cmsis
subdir-ccflags-y += -Iplatform/cmsis/inc -Iplatform/hal -Iutils/hwtimer_list -DRHINO_CONFIG_CLI_AS_NMI=1
subdir-asflags-y += -Iplatform/cmsis/inc -DRHINO_CONFIG_CLI_AS_NMI=1

