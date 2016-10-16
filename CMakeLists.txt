cmake_minimum_required(VERSION 3.3)
project(cvlab1)

set(CMAKE_PREFIX_PATH C:/Qt/5.5/mingw492_32)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(GSL REQUIRED)
find_package(Qt5Widgets)

#include_directories(${GSL_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp Image.cpp Image.h KernelFactory.cpp KernelFactory.h Pyramid.cpp Pyramid.h InterestingPoints.cpp InterestingPoints.h DescriptorsKit.cpp DescriptorsKit.h Transform.cpp Transform.h)
add_executable(cvlab1 ${SOURCE_FILES})

target_link_libraries(cvlab1 Qt5::Widgets ${GSL_LIBRARIES})