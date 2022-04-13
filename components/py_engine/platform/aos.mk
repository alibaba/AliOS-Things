
#add include file
include_directories(${DRIVERDIR}/include)
#include_directories(${DRIVERDIR}/include/jquick)
include_directories(${DRIVERDIR}/include/peripheral)
include_directories(${DRIVERDIR}/platforms/aos-haas100)
include_directories(${DRIVERDIR}/platforms/aos-haas100/jquick)
include_directories(${DRIVERDIR}/platforms/aos-haas100/network)
include_directories(${DRIVERDIR}/platforms/aos-haas100/peripheral)

include_directories(${DRIVERDIR}/services/board_mgr)
include_directories(${DRIVERDIR}/services/amp_utils)

include(${DRIVERDIR}/platforms/aos-haas100/aos.mk)

#amp_utils
list(APPEND SOURCEFILE "${DRIVERDIR}/services/amp_utils/amp_utils.c")

#board_mgr
list(APPEND SOURCEFILE "${DRIVERDIR}/services/board_mgr/board_info.c")
list(APPEND SOURCEFILE "${DRIVERDIR}/services/board_mgr/board_mgr.c")
