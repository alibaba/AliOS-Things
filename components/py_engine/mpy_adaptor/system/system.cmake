#add include
include_directories(${MPYADAPTORDIR}/system)
include_directories(${MPYENGINEDIR}/system)

#add source code
#driver
include(${MPYADAPTORDIR}/system/driver/py.mk)
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/moddriver.c")

#system-manager
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/modbattery.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/modcharger.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/modpm.c")

#utils
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/modutime.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/modsdcard.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/modsysteminfo.c")

if(${PY_BUILD_USOCKET})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/modsocket.c")
	add_definitions(-DPY_BUILD_USOCKET)
endif()


if(${PY_BUILD_UOS})
	list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/moduos.c")
endif()

#port
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/_frozen_mpy.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/mphalport.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/mphalport.c")
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/mpthreadport.c")

#utility
list(APPEND SOURCEFILE "${MPYADAPTORDIR}/system/utility.c")