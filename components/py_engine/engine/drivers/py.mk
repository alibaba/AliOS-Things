
#add include
include_directories(${MPYENGINEDIR}/drivers/bus)

#add src
list(APPEND SOURCEFILE "${MPYENGINEDIR}/drivers/bus/softqspi.c")
list(APPEND SOURCEFILE "${MPYENGINEDIR}/drivers/bus/softspi.c")
