#add include
include_directories(${MICROPYTHONDIR}/lib)
include_directories(${MICROPYTHONDIR}/lib/libm_dbl)

#add src
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/lib/utils/interrupt_char.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/lib/utils/printf.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/lib/utils/pyexec.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/lib/utils/stdout_helpers.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/lib/mp-readline/readline.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/lib/timeutils/timeutils.c")
