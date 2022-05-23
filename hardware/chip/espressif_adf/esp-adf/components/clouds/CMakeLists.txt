set(COMPONENT_ADD_INCLUDEDIRS ./dueros/lightduer/include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_SRCS )


register_component()
IF (NOT (CONFIG_IDF_TARGET STREQUAL "esp32c3"))

IF (IDF_VERSION_MAJOR GREATER 3)
target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L${CMAKE_CURRENT_LIST_DIR}/dueros/lightduer")
target_link_libraries(${COMPONENT_TARGET} INTERFACE duer-device-v4x)
ELSE ()
target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L${CMAKE_CURRENT_LIST_DIR}/dueros/lightduer")
target_link_libraries(${COMPONENT_TARGET} INTERFACE duer-device)
ENDIF (IDF_VERSION_MAJOR GREATER 3)

ENDIF()
