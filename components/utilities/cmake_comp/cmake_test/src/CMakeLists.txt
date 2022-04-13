
link_directories(../build/lib/)
ADD_EXECUTABLE(main main.c)
INCLUDE_DIRECTORIES(../lib/)
target_link_libraries(main -lhello)
