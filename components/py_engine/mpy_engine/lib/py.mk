#add include
include_directories(${MPYENGINEDIR}/lib)
include_directories(${MPYENGINEDIR}/lib/libm_dbl)

#add src
list(APPEND SOURCEFILE "${MPYENGINEDIR}/lib/utils/interrupt_char.c")
list(APPEND SOURCEFILE "${MPYENGINEDIR}/lib/utils/printf.c")
list(APPEND SOURCEFILE "${MPYENGINEDIR}/lib/utils/pyexec.c")
list(APPEND SOURCEFILE "${MPYENGINEDIR}/lib/utils/stdout_helpers.c")
# list(APPEND SOURCEFILE "${MPYENGINEDIR}/lib/utils/sys_stdio_mphal.c")

list(APPEND SOURCEFILE "${MPYENGINEDIR}/lib/netutils/netutils.c")

list(APPEND SOURCEFILE "${MPYENGINEDIR}/lib/mp-readline/readline.c")
list(APPEND SOURCEFILE "${MPYENGINEDIR}/lib/timeutils/timeutils.c")


