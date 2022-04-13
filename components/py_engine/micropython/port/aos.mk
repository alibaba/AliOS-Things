
#add include file
include_directories(${MICROPYTHONDIR}/port)

set(PORTDIR ${MICROPYTHONDIR}/port)
#add class code
include(${MICROPYTHONDIR}/port/minicv/aos.mk)
include(${MICROPYTHONDIR}/port/driver/aos.mk)
include(${MICROPYTHONDIR}/port/network/aos.mk)
include(${MICROPYTHONDIR}/port/power/aos.mk)
include(${MICROPYTHONDIR}/port/system/aos.mk)
include(${PORTDIR}/advanced/advanced.cmake)

#add modules
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/modminicv.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/moddriver.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/modnetwork.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/modpower.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/modsystem.c")

list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/modutime.c")

list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/modlinkkit.c")
#list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/modnetmgr.c")

#port
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/_frozen_mpy.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/mphalport.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/mpy_main.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/mphalport.c")
list(APPEND SOURCEFILE "${MICROPYTHONDIR}/port/mpthreadport.c")

 