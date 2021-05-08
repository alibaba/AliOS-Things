
#add include file
include_directories(${MPYADAPTORDIR})

set(PORTDIR ${MPYADAPTORDIR})

#add class code
include(${MPYADAPTORDIR}/component/component.cmake)
include(${MPYADAPTORDIR}/system/system.cmake)

list(APPEND SOURCEFILE "${MPYADAPTORDIR}/mpy_main.c")
