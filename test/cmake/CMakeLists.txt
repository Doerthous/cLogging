cmake_minimum_required(VERSION 3.0)

project(Test)

if(UNIX)
    set(LIB pthread)
endif()

include_directories(../../)

add_executable(basic ../basic.c)
target_link_libraries(basic ${LIB})
add_executable(extend ../extend.c)
target_link_libraries(extend ${LIB})
add_executable(format ../format.c)
target_link_libraries(format ${LIB})
add_executable(color ../color.cpp)
target_link_libraries(color ${LIB})
add_executable(logfile ../logfile.cpp)
target_link_libraries(logfile ${LIB})
add_executable(threading ../threading.cpp)
target_link_libraries(threading ${LIB})