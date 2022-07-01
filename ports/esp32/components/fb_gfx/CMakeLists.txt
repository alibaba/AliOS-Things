set(src_dirs .)
set(include_dirs .
                ./include)
set(requires  esp32-camera)
idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})