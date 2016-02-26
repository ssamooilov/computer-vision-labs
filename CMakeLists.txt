cmake_minimum_required(VERSION 3.3)
project(cvlab1)

set(CMAKE_PREFIX_PATH C:/Qt/5.5/mingw492_32)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Qt5Widgets)

set(SOURCE_FILES main.cpp Convolution.cpp Convolution.h BlackWhiteConverter.cpp BlackWhiteConverter.h)
add_executable(cvlab1 ${SOURCE_FILES})

target_link_libraries(cvlab1 Qt5::Widgets)