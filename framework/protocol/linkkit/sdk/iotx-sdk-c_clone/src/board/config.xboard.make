# xboard support 3 kind of toolchains, this file take STM32L496 for example, you can change it
ifeq ($(COMPILER),armcc)
OVERRIDE_CC:=armcc
CONFIG_ENV_CFLAGS += -DSTM32L496xx \
					 --cpu=Cortex-M4 \
					 --apcs=interwork \
					 --split_sections
CONFIG_ENV_CFLAGS += -c --c99 -D__MICROLIB -g --gnu --library_type=microlib -W -Ospace -O3 -Iprebuilt/extra/stdc/armlibc
OVERRIDE_AR:=armar -rcs
				 
else ifeq ($(COMPILER),iar)
OVERRIDE_CC:=iccarm
CONFIG_ENV_CFLAGS += -DSTM32L496xx \
					--cpu=Cortex-M4 \
                    --cpu_mode=thumb \
                    --endian=little 		    
CONFIG_ENV_CFLAGS += --vla -e --dlib_config=full -D_TIMESPEC_DEFINED --silent --only_stdout --no_warnings --diag_warning=Pe167,Pe144,Pe513 -Ohz -Iprebuilt/extra/stdc/iarlibc
OVERRIDE_AR:=iarchive --create

else
CROSS_PREFIX:=arm-none-eabi-
CONFIG_ENV_CFLAGS += -DSTM32L496xx \
				     -mcpu=cortex-m4 \
					 -mlittle-endian \
					 -mthumb \
					 -mthumb-interwork
CONFIG_ENV_CFLAGS += -c -Os
endif


CONFIG_src/ref-impl/tls         :=
CONFIG_src/ref-impl/hal         :=
CONFIG_examples                 :=
CONFIG_tests                    :=
CONFIG_src/tools/linkkit_tsl_convert :=

