
#add include file
include_directories(${UIDIR}/include)

#add source file
list(APPEND SOURCEFILE "${UIDIR}/src/HaasUI.cpp")
list(APPEND SOURCEFILE "${UIDIR}/src/HaasUIFB.cpp")
list(APPEND SOURCEFILE "${UIDIR}/src/HaasUILvgl.cpp")
list(APPEND SOURCEFILE "${UIDIR}/src/HaasUISdl.cpp")

if (SYSTEM_OS_NAME MATCHES "linux")
list(APPEND SOURCEFILE "${UIDIR}/src/HaasUIGreenui.cpp")
endif()
