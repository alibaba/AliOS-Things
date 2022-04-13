NAME := v_pwm_drv

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board haas100
MODULE             := pwm

$(NAME)_INCLUDES += ../../drivers/platform/drivers/ana
$(NAME)_INCLUDES += ../../drivers/config/haas1000_normalization/

$(NAME)_SOURCES += pwm.c
#$(NAME)_SOURCES += ../../drivers/platform/hal/hal_pwm.c

CPU_FLAGS := -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -DCORTEX_DEBUG_V8M

COM_FLAGS := -g -O2 -Wall #-Werror

COM_MACROS := -DCHIP=haas1000 -DKERNEL=RHINO -DCHIP_HAAS1000_ARCH_ENHANCE -DCHIP_HAAS1000 -DMIC_SAMPLERATE=48000 -DMIC_PERIOD_MS=48

#CWARN_FLAGS := \
        -Wno-int-conversion -Wno-unused-variable -Wno-unused-parameter -Wno-type-limits \
        -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value \
        -Wno-strict-aliasing -Wno-pointer-sign -Wno-discarded-qualifiers -Wno-overflow \
        -Wno-comment -Wno-maybe-uninitialized -Wno-float-conversion -Wno-incompatible-pointer-types \
        -Wno-implicit-function-declaration

$(NAME)_CFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(CWARN_FLAGS) $(COM_MACROS) $(MACROS) -std=gnu11 -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer

$(NAME)_CXXFLAGS += $(CPU_FLAGS) $(COM_MACROS) $(MACROS)

GLOBAL_CFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(CWARN_FLAGS) $(COM_MACROS) -DCHIP_HAS_TRANSQ -DCHIP_HAS_UART=3 -DRTOS=1 -DDISABLE_CLI_MULTI_CONSOLE=0 \
        -std=gnu11 -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer

GLOBAL_CXXFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(COM_MACROS) -D_GLIBCXX_HAS_GTHREADS \
        -std=gnu++11 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fno-rtti -fno-exceptions -fpermissive

GLOBAL_ASMFLAGS += $(CPU_FLAGS) $(COM_FLAGS) $(COM_MACROS) -c -x assembler-with-cpp

GLOBAL_LDFLAGS += $(CPU_FLAGS) -u _printf_float -lstdc++ -lsupc++ -lm -lc -lgcc  -nostartfiles
