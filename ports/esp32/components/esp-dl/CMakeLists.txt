idf_build_get_property(target IDF_TARGET)

if(${IDF_TARGET} STREQUAL "esp32")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32")

elseif(${IDF_TARGET} STREQUAL "esp32s2")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s2")

elseif(${IDF_TARGET} STREQUAL "esp32s3")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3")

elseif(${IDF_TARGET} STREQUAL "esp32c3")    
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32c3")

endif()

set(COMPONENT_ADD_INCLUDEDIRS   include/
                                include/tool
                                include/typedef
                                include/image
                                include/math
                                include/nn
                                include/layer
                                include/detect
                                include/model_zoo)

register_component()
target_link_libraries(${COMPONENT_TARGET}   INTERFACE   ${links})

set(lib     libcat_face_detect.a
            libhuman_face_detect.a
            libcolor_detect.a
            libmfn.a
            libdl.a)
target_link_libraries(${COMPONENT_TARGET}   INTERFACE   ${lib})
