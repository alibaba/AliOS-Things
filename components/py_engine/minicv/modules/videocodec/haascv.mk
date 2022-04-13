
#add include file
include_directories(${VIDEOCODECDIR}/include)

#add source file
list(APPEND SOURCEFILE "${VIDEOCODECDIR}/src/HaasVideoCodec.cpp")
if (SYSTEM_OS_NAME MATCHES "linux")
list(APPEND SOURCEFILE "${VIDEOCODECDIR}/src/HaasVideoCodecFFmpeg.cpp")
endif()
list(APPEND SOURCEFILE "${VIDEOCODECDIR}/src/HaasVideoCodecHal.cpp")
