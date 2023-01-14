cmake_minimum_required(VERSION 3.0)

project(Test)

if(UNIX)
    set(LIB pthread)
endif()

include_directories(../../)

add_library(logging OBJECT ../logging.c)

add_executable(basic ../basic.c)
target_link_libraries(basic ${LIB})
add_executable(basic_e ../basic.c)
target_link_libraries(basic_e ${LIB} logging)
target_compile_definitions(basic_e PRIVATE -DLOGGING_AS_HEADER)

add_executable(extend ../extend.c)
target_link_libraries(extend ${LIB})
add_executable(extend_e ../extend.c)
target_link_libraries(extend_e ${LIB} logging)
target_compile_definitions(extend_e PRIVATE -DLOGGING_AS_HEADER)

add_executable(format ../format.c)
target_link_libraries(format ${LIB})
add_executable(format_e ../format.c)
target_link_libraries(format_e ${LIB} logging)
target_compile_definitions(format_e PRIVATE -DLOGGING_AS_HEADER)

add_executable(color ../color.cpp)
target_link_libraries(color ${LIB})
add_executable(color_e ../color.cpp)
target_link_libraries(color_e ${LIB} logging)
target_compile_definitions(color_e PRIVATE -DLOGGING_AS_HEADER)

add_executable(logfile ../logfile.cpp)
target_link_libraries(logfile ${LIB})
add_executable(logfile_e ../logfile.cpp)
target_link_libraries(logfile_e ${LIB} logging)
target_compile_definitions(logfile_e PRIVATE -DLOGGING_AS_HEADER)

add_executable(threading ../threading.cpp)
target_link_libraries(threading ${LIB})
add_executable(threading_e ../threading.cpp)
target_link_libraries(threading_e ${LIB} logging)
target_compile_definitions(threading_e PRIVATE -DLOGGING_AS_HEADER)
