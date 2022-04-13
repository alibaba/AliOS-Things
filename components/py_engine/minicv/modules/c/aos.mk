#c interface
#add include file
include_directories(${CINTERFACEDIR}/include)

#add src file
list(APPEND SOURCEFILE "${CINTERFACEDIR}/src/WrapperIHaasDataInput.cpp")
list(APPEND SOURCEFILE "${CINTERFACEDIR}/src/WrapperIHaasImageCodec.cpp")
list(APPEND SOURCEFILE "${CINTERFACEDIR}/src/WrapperIHaasImageProc.cpp")
list(APPEND SOURCEFILE "${CINTERFACEDIR}/src/WrapperIHaasML.cpp")
list(APPEND SOURCEFILE "${CINTERFACEDIR}/src/WrapperIHaasUI.cpp")
list(APPEND SOURCEFILE "${CINTERFACEDIR}/src/WrapperIHaasVideoCodec.cpp")

