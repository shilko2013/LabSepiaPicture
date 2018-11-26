
cmake_minimum_required(VERSION 3.6)

project(LabBlurPicture)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(SOURCE_EXE main.c)

set(SOURCE_LIB bmp.c)

add_library(bmp STATIC ${SOURCE_LIB})

add_executable(main ${SOURCE_EXE})

target_link_libraries(main bmp m)