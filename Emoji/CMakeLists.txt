# write a cmakelists to build the project

cmake_minimum_required(VERSION 3.0)

set( CMAKE_CXX_FLAGS "-std=c++17" )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(cvt cvt.cpp)
target_link_libraries( cvt ${OpenCV_LIBS} )
