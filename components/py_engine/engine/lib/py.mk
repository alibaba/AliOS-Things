#add include
include_directories(${MPYENGINEDIR}/lib)
include_directories(${MPYENGINEDIR}/lib/libm_dbl)

#add src
list(APPEND SOURCEFILE "${MPYENGINEDIR}/shared/runtime/interrupt_char.c")
list(APPEND SOURCEFILE "${MPYENGINEDIR}/shared/runtime/printf.c")
list(APPEND SOURCEFILE "${MPYENGINEDIR}/shared/runtime/pyexec.c")
list(APPEND SOURCEFILE "${MPYENGINEDIR}/shared/runtime/stdout_helpers.c")
# list(APPEND SOURCEFILE "${MPYENGINEDIR}/shared/runtime/sys_stdio_mphal.c")
