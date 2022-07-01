idf_build_get_property(target IDF_TARGET)


set(srcs            app_main.cpp)

set(src_dirs        .)

set(include_dirs    .
                    ../../../include
                    ../../../include/tool
                    ../../../include/math
                    ../../../include/nn
                    ../../../include/layer
                    ../../../include/image
                    ../../../include/typedef
                    ../../../include/detect
                    ../../../include/model_zoo)

idf_component_register(SRCS ${srcs} SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

set(lib     libmfn.a
            libhuman_face_detect.a
            libdl.a)

if(${IDF_TARGET} STREQUAL "esp32")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/esp32")

elseif(${IDF_TARGET} STREQUAL "esp32s2")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/esp32s2")

elseif(${IDF_TARGET} STREQUAL "esp32s3")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/esp32s3")

elseif(${IDF_TARGET} STREQUAL "esp32c3")    
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/esp32c3")

endif()


target_link_libraries(${COMPONENT_TARGET} ${links})
target_link_libraries(${COMPONENT_TARGET} ${lib})