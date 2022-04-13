
#add include
include_directories(${MICROPYTHONDIR}/drivers/bus)

#add src
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/drivers/bus/softqspi.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/drivers/bus/softspi.c")
