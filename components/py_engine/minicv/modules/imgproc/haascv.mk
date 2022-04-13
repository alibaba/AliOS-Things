
#add include file
include_directories(${IMGPROCDIR}/include)

#add source file
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/HaasImageProc.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/HaasImageProcCpu.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/HaasImageProcHal.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageCvtColorCpu.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageCvtColorHal.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageDrawCpu.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageDrawHal.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageTransformCpu.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageTransformHal.cpp")

IF(USE_OPENCV)
add_definitions(-DENABLE_OPENCV)
include_directories(${OPENCVDIR}/include)
include_directories(${OPENCVDIR}/buildroot-build)
include_directories(${OPENCVDIR}/modules/core/include)
include_directories(${OPENCVDIR}/modules/imgcodecs/include)
include_directories(${OPENCVDIR}/modules/imgproc/include)
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/HaasImageProcOpenCV.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageCvtColorOpenCV.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageDrawOpenCV.cpp")
list(APPEND SOURCEFILE "${IMGPROCDIR}/src/ImageTransformOpenCV.cpp")
ENDIF()
