#c interface
#add include file

set(CINTERFACEDIR ${MPYADAPTORDIR}/component/minicv/base/modules/c)
set(CORE ${MPYADAPTORDIR}/component/minicv/base/modules/core)
set(MLDIR ${MPYADAPTORDIR}/component/minicv/base/modules/ml)

include_directories(${MPYADAPTORDIR}/component/minicv/base/include)
include_directories(${MPYADAPTORDIR}/component/minicv/base/include/base)
include(${CINTERFACEDIR}/cv.mk)
include(${CORE}/cv.mk)
if(${PY_BUILD_AI})
include(${MLDIR}/cv.mk)
endif()

#add src file

