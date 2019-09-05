NAME := arch_armv7a-svc

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := arch for armv7a

ifeq ($(ENABLE_USPACE),1)

$(NAME)_SOURCES := common/k_vector.c
$(NAME)_SOURCES += common/k_asid.c
$(NAME)_SOURCES += common/k_cache.c
$(NAME)_SOURCES += common/k_load.c
$(NAME)_SOURCES += common/k_mmu_sd.c
$(NAME)_SOURCES += common/panic.c
$(NAME)_SOURCES += common/port_c.c

$(NAME)_INCLUDES += common/include

GLOBAL_INCLUDES += include

ifeq ($(COMPILER),armcc)

GLOBAL_ASMFLAGS += --cpreproc
$(NAME)_SOURCES += common/k_cache_armcc.S

ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_SOURCES += armcc/a5/port_s.S
$(NAME)_SOURCES += armcc/a5/vector_s.S
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_SOURCES += armcc/a7/port_s.S
$(NAME)_SOURCES += armcc/a7/vector_s.S
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_SOURCES += armcc/a9/port_s.S
$(NAME)_SOURCES += armcc/a9/vector_s.S
endif

$(NAME)_PREBUILT_LIBRARY := lib/armcc/libarchmk.a

else ifeq ($(COMPILER),iar)

$(NAME)_SOURCES += common/k_cache_iccarm.S

ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_SOURCES += iccarm/a5/port_s.S
$(NAME)_SOURCES += iccarm/a5/vector_s.S
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_SOURCES += iccarm/a7/port_s.S
$(NAME)_SOURCES += iccarm/a7/vector_s.S
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_SOURCES += iccarm/a9/port_s.S
$(NAME)_SOURCES += iccarm/a9/vector_s.S
endif

$(NAME)_PREBUILT_LIBRARY := lib/iccarm/libarchmk.a

else

$(NAME)_SOURCES += common/k_cache_gcc.S
$(NAME)_SOURCES += common/k_svc_gcc.S

ifeq ($(HOST_ARCH),Cortex-A5)
$(NAME)_SOURCES += gcc/a5/port_s.S
$(NAME)_SOURCES += gcc/a5/vector_s.S
else ifeq ($(HOST_ARCH),Cortex-A7)
$(NAME)_SOURCES += gcc/a7/port_s.S
$(NAME)_SOURCES += gcc/a7/vector_s.S
else ifeq ($(HOST_ARCH),Cortex-A9)
$(NAME)_SOURCES += gcc/a9/port_s.S
$(NAME)_SOURCES += gcc/a9/vector_s.S
endif

$(NAME)_PREBUILT_LIBRARY := lib/gcc/libarchmk.a

endif

endif # ENABLE_USPACE

