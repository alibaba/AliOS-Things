NAME := micropython

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := a lean and efficient Python 3 implementation for MCU

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES += \
    py/argcheck.c \
    py/asmarm.c \
    py/asmbase.c \
    py/asmthumb.c \
    py/asmx64.c \
    py/asmx86.c \
    py/asmxtensa.c \
    py/bc.c \
    py/binary.c \
    py/builtinevex.c \
    py/builtinhelp.c \
    py/builtinimport.c \
    py/compile.c \
    py/emitbc.c \
    py/emitcommon.c \
    py/emitglue.c \
    py/emitinlinethumb.c \
    py/emitinlinextensa.c \
    py/emitnative.c \
    py/formatfloat.c \
    py/frozenmod.c \
    py/gc.c \
    py/lexer.c \
    py/malloc.c \
    py/map.c \
    py/modarray.c \
    py/modbuiltins.c \
    py/modcmath.c \
    py/modcollections.c \
    py/modgc.c \
    py/modio.c \
    py/modmath.c \
    py/modmicropython.c \
    py/modstruct.c \
    py/modsys.c \
    py/modthread.c \
    py/moduerrno.c \
    py/mpprint.c \
    py/mpstate.c \
    py/scheduler.c \
    py/mpz.c \
    py/nativeglue.c \
    py/nlr.c \
    py/nlrsetjmp.c \
    py/nlrthumb.c \
    py/nlrx64.c \
    py/nlrx86.c \
    py/nlrxtensa.c \
    py/objarray.c \
    py/objattrtuple.c \
    py/objbool.c \
    py/objboundmeth.c \
    py/obj.c \
    py/objcell.c \
    py/objclosure.c \
    py/objcomplex.c \
    py/objdeque.c \
    py/objdict.c \
    py/objenumerate.c \
    py/objexcept.c \
    py/objfilter.c \
    py/objfloat.c \
    py/objfun.c \
    py/objgenerator.c \
    py/objgetitemiter.c \
    py/objint.c \
    py/objint_longlong.c \
    py/objint_mpz.c \
    py/objlist.c \
    py/objmap.c \
    py/objmodule.c \
    py/objnamedtuple.c \
    py/objnone.c \
    py/objobject.c \
    py/objpolyiter.c \
    py/objproperty.c \
    py/objrange.c \
    py/objreversed.c \
    py/objset.c \
    py/objsingleton.c \
    py/objslice.c \
    py/objstr.c \
    py/objstringio.c \
    py/objstrunicode.c \
    py/objtuple.c \
    py/objtype.c \
    py/objzip.c \
    py/opmethods.c \
    py/parse.c \
    py/parsenumbase.c \
    py/parsenum.c \
    py/persistentcode.c \
    py/pystack.c \
    py/qstr.c \
    py/reader.c \
    py/repl.c \
    py/runtime.c \
    py/runtime_utils.c \
    py/scope.c \
    py/sequence.c \
    py/showbc.c \
    py/smallint.c \
    py/stackctrl.c \
    py/stream.c \
    py/unicode.c \
    py/vm.c \
    py/vstr.c \
    py/warning.c

$(NAME)_SOURCES += \
    lib/utils/interrupt_char.c \
    lib/utils/printf.c \
    lib/utils/pyexec.c \
    lib/utils/stdout_helpers.c \
    lib/mp-readline/readline.c \
    lib/timeutils/timeutils.c

$(NAME)_SOURCES += \
    port/_frozen_mpy.c \
    port/mphalport.c \
    port/mpy_main.c \
    port/mphalport.c \
    port/modutime.c \
    port/modmachine.c \
    port/ipc.c \
    port/gccollect.c \
    port/mpthreadport.c \

$(NAME)_SOURCES += \
    extmod/crypto-algorithms/sha256.c \
    extmod/machine_i2c.c \
    extmod/machine_mem.c \
    extmod/machine_pinbase.c \
    extmod/machine_pulse.c \
    extmod/machine_signal.c \
    extmod/machine_spi.c \
    extmod/modbtree.c \
    extmod/modframebuf.c \
    extmod/modubinascii.c \
    extmod/moductypes.c \
    extmod/moduhashlib.c \
    extmod/moduheapq.c \
    extmod/modujson.c \
    extmod/modurandom.c \
    extmod/modure.c \
    extmod/moduselect.c \
    extmod/modussl_axtls.c \
    extmod/modussl_mbedtls.c \
    extmod/modutimeq.c \
    extmod/moduzlib.c \
    extmod/modwebrepl.c \
    extmod/moduwebsocket.c \
    extmod/uos_dupterm.c \
    extmod/vfs.c \
    extmod/vfs_fat.c \
    extmod/vfs_fat_diskio.c \
    extmod/vfs_fat_file.c \
    extmod/vfs_reader.c \
    extmod/virtpin.c \
    extmod/re1.5/charclass.c \
    extmod/re1.5/compilecode.c \
    extmod/re1.5/dumpcode.c \
    extmod/re1.5/recursiveloop.c \
    extmod/uzlib/adler32.c \
    extmod/uzlib/crc32.c \
    extmod/uzlib/tinfgzip.c \
    extmod/uzlib/tinflate.c \
    extmod/uzlib/tinfzlib.c \
    extmod/utime_mphal.c

$(NAME)_SOURCES += \
    drivers/bus/softqspi.c \
    drivers/bus/softspi.c

$(NAME)_INCLUDES += \
    port \
    py \
    lib \
    lib/libm_dbl \
    extmod \
    extmod/crypto-algorithms \
    extmod/re1.5 \
    extmod/uzlib \
    drivers/bus \
    .

GLOBAL_INCLUDES += ./include

$(NAME)_CFLAGS += -DMICROPY_HW_BOARD_NAME=\"$(PLATFORM)\"
$(NAME)_CFLAGS += -DMICROPY_HW_MCU_NAME=\"$(HOST_ARCH)\"
