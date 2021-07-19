
#add include file
include_directories(${MLDIR}/include)

#add source file
list(APPEND SOURCEFILE "${MLDIR}/src/HaasML.cpp")
list(APPEND SOURCEFILE "${MLDIR}/src/HaasMLCloud.cpp")
list(APPEND SOURCEFILE "${MLDIR}/src/HaasMLMnn.cpp")
#list(APPEND SOURCEFILE "${MLDIR}/src/HaasMLOlda.cpp")
