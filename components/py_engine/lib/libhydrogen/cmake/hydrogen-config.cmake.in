get_filename_component(hydrogen_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET hydrogen::hydrogen)
  include("${hydrogen_CMAKE_DIR}/hydrogen-targets.cmake")
endif()

set(hydrogen_LIBRARIES hydrogen::hydrogen)
