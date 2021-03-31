#c interface
#add include file

set(CINTERFACEDIR ${CMAKE_SOURCE_DIR}/minicv/modules/c)
set(CORE ${CMAKE_SOURCE_DIR}/minicv/modules/core)
set(MLDIR ${CMAKE_SOURCE_DIR}/minicv/modules/ml)

include_directories(${MINICVDIR}/include)
include_directories(${MINICVDIR}/include/base)
include(${CINTERFACEDIR}/aos.mk)
include(${CORE}/aos.mk)
include(${MLDIR}/aos.mk)

#add src file

