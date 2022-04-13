
#add include file
include_directories(${IMGCODECDIR}/include)

#add source file
list(APPEND SOURCEFILE "${IMGCODECDIR}/src/HaasImageCodec.cpp")
list(APPEND SOURCEFILE "${IMGCODECDIR}/src/HaasImageCodecBmp.cpp")
list(APPEND SOURCEFILE "${IMGCODECDIR}/src/HaasImageCodecGif.cpp")
list(APPEND SOURCEFILE "${IMGCODECDIR}/src/HaasImageCodecJpeg.cpp")
list(APPEND SOURCEFILE "${IMGCODECDIR}/src/HaasImageCodecPng.cpp")
