idf_build_get_property(target IDF_TARGET)

execute_process(
    COMMAND python ${COMPONENT_PATH}/../../tools/convert_tool/convert.py -t ${IDF_TARGET} -i ${COMPONENT_PATH}/../model/npy -n mnist_coefficient -o ${COMPONENT_PATH}/../model
)

set(srcs            app_main.cpp
                    ../model/mnist_coefficient.cpp)

set(src_dirs        ../model)

set(include_dirs    ../model
                    ../../include
                    ../../include/tool
                    ../../include/typedef
                    ../../include/nn
                    ../../include/layer)

idf_component_register(SRCS ${srcs} SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

set(lib     libdl.a)

if(${IDF_TARGET} STREQUAL "esp32")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/esp32")

elseif(${IDF_TARGET} STREQUAL "esp32s2")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/esp32s2")

elseif(${IDF_TARGET} STREQUAL "esp32s3")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/esp32s3")

elseif(${IDF_TARGET} STREQUAL "esp32c3")    
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/esp32c3")

endif()


target_link_libraries(${COMPONENT_TARGET} ${links})
target_link_libraries(${COMPONENT_TARGET} ${lib})