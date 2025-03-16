cmake_minimum_required(VERSION 3.0)
project(StackManager LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()


file(GLOB_RECURSE SRC_FILES INC_FILES src/*.c include/*.h)
add_library(StackManager SHARED ${SRC_FILES} ${INC_FILES})

add_subdirectory(clitools)


