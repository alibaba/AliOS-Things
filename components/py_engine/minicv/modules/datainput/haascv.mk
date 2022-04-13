
#add include file
include_directories(${DATAINPUTDIR}/include)

#add source file
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInput.cpp")

if (SYSTEM_OS_NAME MATCHES "linux")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputCameraUsb.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/DataInputVideoFileFFmpeg.cpp")
endif()

list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputCameraIp.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputCameraLocal.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputCameraUsb.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputImageBmp.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputImageGif.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputImageJpg.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputImagePng.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputVideoFile.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputVideoRtmp.cpp")
list(APPEND SOURCEFILE "${DATAINPUTDIR}/src/HaasDataInputVideoRtsp.cpp")
